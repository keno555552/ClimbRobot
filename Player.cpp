#define NOMINMAX ///<<<必ずすべでのファイルの先頭に書くこと
#include "Player.h"
#include "GameScene.h"
#include <algorithm>
#include <numbers>

void Player::Initialize(const Camera* camera, const Vector3& position) {
	/// モデルの設定
	model_ = Model::CreateFromOBJ("player4", true);
	attackEffectModel_ = Model::CreateFromOBJ("attackEffect", true);
	aimerModel_ = Model::CreateFromOBJ("aimer", true);

	/// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	attackEffectWorldTransform_.Initialize();
	attackEffectWorldTransform_.scale_ = {1.4f, 0.8f, 1.0f};
	attackEffectWorldTransform_.translation_ = worldTransform_.translation_;
	aimerWorldTransform_.Initialize();
	aimerWorldTransform_.scale_ = {1.2f, 1.5f, 1.2f};
	aimerWorldTransform_.translation_ = position;

	input_ = Input::GetInstance();

	/// カメラの設定
	camera_ = camera;
	/// テクスチャーハンドルの設定
	// textureHandle_ = textureHandle;

	/// ワールドトランスフォームの初期化
	// 初期回転
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	turnFirstRotationY_ = worldTransform_.rotation_.y;
}

Player::~Player() {
	delete model_, model_ = nullptr;
	delete attackEffectModel_, attackEffectModel_ = nullptr;
	delete aimerModel_, aimerModel_ = nullptr;

	// delete modelPlayer_;
}

void Player::UpdateForTitle() {
	titleWalkTimer_++;
	worldTransform_.translation_.y += 0.01f * std::sin(2.0f * 3.14f * titleWalkTimer_ / 180.0f);
	worldTransform_.rotation_.y += 1.5f * (3.14f / 180.0f);

	{
		// アフィン変換
		Matrix4x4 mS = MakeScaleMatrixM(worldTransform_.scale_);
		Matrix4x4 mR = MakeRotateMatrixM(MakeRotateXMatrixM(worldTransform_.rotation_.x), MakeRotateYMatrixM(worldTransform_.rotation_.y), MakeRotateZMatrixM(worldTransform_.rotation_.z));

		Matrix4x4 mT = MakeTranslateMatrixM(worldTransform_.translation_);

		worldTransform_.matWorld_ = MultM(mS, MultM(mR, mT));
	}

	// ワールドトランスフォームの更新
	worldTransform_.TransferMatrix();

	///// ImGuiのデバッグウィンドウ
	// ImGui::Begin("Debug");
	//// ImGui::Checkbox("DebugCamera", &useDebugCamera);
	// ImGui::SliderFloat3("worldTransform_,translation_", &worldTransform_.translation_.x, -50, 50);
	// ImGui::SliderFloat3("worldTransform_,rotation_", &worldTransform_.rotation_.x, 0, 10);
	// ImGui::End();
}

void Player::Update() {

	if (dushCDParameter_ >= dushCD_ * 60.0f) {
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			if (Input::GetInstance()->PushKey(DIK_A) || Input::GetInstance()->PushKey(DIK_D)) {
				behaviorRequest_ = Behavior::kAttack;
				dushCDParameter_ = 0.0f;
				Audio::GetInstance()->PlayWave(seDushHandle_, false, 0.2f);
			}
		}
	} else {
		dushCDParameter_++;
	}

	if (behaviorRequest_ != Behavior::kUnknown) { // ① 振るまいリクエストが有効なら処理開始
		// 振るまいを変更する
		behavior_ = behaviorRequest_; // ② リクエストされた振るまいに切り替え

		// 各振るまいごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
		default:
			break;
		case Behavior::kAttack:
			attackParameter_ = 0;
			attackPhase_ = AttackPhase::sink;
			break;
		case Behavior::kDamage:
			kDamageParameter_ = 0;
			break;
		}
		// 振るまいリクエストをリセット
		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {
	case Behavior::kRoot:
	default:
		break;
	case Behavior::kAttack:
		attackParameter_++;

		attackEffectWorldTransform_.translation_ = worldTransform_.translation_;

		switch (attackPhase_) {
		case AttackPhase::sink:
		default: {
			float t = static_cast<float>((float)attackParameter_) / 2.0f;
			worldTransform_.scale_.z = Lerp(1.0f, 0.3f, t);
			worldTransform_.scale_.y = Lerp(1.0f, 1.3f, t);
			if (attackParameter_ >= 5.0f) {
				attackPhase_ = AttackPhase::lunge;
				attackParameter_ = 0;
			}
			velocity_.x = 0.0f;
		} break;

		case AttackPhase::lunge: {
			float t = static_cast<float>((float)attackParameter_) / 6.0f;
			worldTransform_.scale_.z = Lerp(0.3f, 1.3f, t);
			worldTransform_.scale_.y = Lerp(1.3f, 0.7f, t);

			if (Input::GetInstance()->PushKey(DIK_A)) {
				velocity_.x = -0.8f;
				attackEffectWorldTransform_.rotation_.z = 180.0f * 3.14f / 180.0f;
			}
			if (Input::GetInstance()->PushKey(DIK_D)) {
				velocity_.x = 0.8f;
				attackEffectWorldTransform_.rotation_.z = 0.0f;
			}

			if (attackParameter_ >= 6.0f) {
				attackPhase_ = AttackPhase::afterglow;
				attackParameter_ = 0;
			}
		} break;

		case AttackPhase::afterglow:
			float t = static_cast<float>((float)attackParameter_) / 2.0f;
			worldTransform_.scale_.z = Lerp(1.3f, 1.0f, t);
			worldTransform_.scale_.y = Lerp(0.7f, 1.0f, t);

			if (attackParameter_ >= 2.0f) {
				behaviorRequest_ = Behavior::kRoot;
			}
			break;
		}
		velocity_.y = 0;

		if (Input::GetInstance()->PushKey(DIK_A)) {
			if (lrDirection_ != LRDirection::kLeft) {
				lrDirection_ = LRDirection::kLeft;
				turnFirstRotationY_ = worldTransform_.rotation_.y;
				turnTimer_ = kTimeTurn;
			}
		}
		if (Input::GetInstance()->PushKey(DIK_D)) {
			if (lrDirection_ != LRDirection::kRight) {
				lrDirection_ = LRDirection::kRight;
				turnFirstRotationY_ = worldTransform_.rotation_.y;
				turnTimer_ = kTimeTurn;
			}
		}

		break;
	}

	/// 射撃CD関連
	if (behavior_ == Behavior::kRoot) {
		if (kBulletParameter_ >= (kBulletCD_ * 60.0f)) {
			if (input_->IsPressMouse(0)) {
				gameScene_->CreateBullet(rotateAimer_);
				kBulletParameter_ = 0.0f;
				Audio::GetInstance()->PlayWave(seShootHandle_, false, 0.5f);
			}
		}
	}

	if (kBulletParameter_ < (kBulletCD_ * 60.0f)) {
		kBulletParameter_++;
	}

	/// 無敵時間関連
	if (kBulletParameter_ >= (kBulletCD_ * 60.0f)) {
		if (behavior_ == Behavior::kDamage) {
			behavior_ = Behavior::kRoot;
		}
	}

	if (kDamageParameter_ < (kDamageCD_ * 60.0f)) {
		behavior_ = Behavior::kDamage;
		kDamageParameter_++;
	}

	BehaviorRootUpdate();

	/// aimer計算
	{
		float deltaX = targetPos_.x - worldTransform_.translation_.x;
		float deltaY = targetPos_.y - worldTransform_.translation_.y; // 注意：Z 軸是前後方向

		rotateAimer_ = atan2(deltaY, deltaX);

		aimerWorldTransform_.translation_ = worldTransform_.translation_;
		aimerWorldTransform_.translation_.x += kAimerDistance * cosf(rotateAimer_);
		aimerWorldTransform_.translation_.y += kAimerDistance * sinf(rotateAimer_);

		aimerWorldTransform_.rotation_.z = rotateAimer_ - 3.14f / 2.0f;
		aimerWorldTransform_.rotation_.y += kAimerRotateSpeed;
	}

	// アフィン変換
	Matrix4x4 mS = MakeScaleMatrixM(worldTransform_.scale_);
	Matrix4x4 mR = MakeRotateMatrixM(MakeRotateXMatrixM(worldTransform_.rotation_.x), MakeRotateYMatrixM(worldTransform_.rotation_.y), MakeRotateZMatrixM(worldTransform_.rotation_.z));
	Matrix4x4 mT = MakeTranslateMatrixM(worldTransform_.translation_);
	worldTransform_.matWorld_ = MultM(mS, MultM(mR, mT));

	// ワールドトランスフォームの更新
	worldTransform_.TransferMatrix();

	/// エフェクト
	// アフィン変換
	mS = MakeScaleMatrixM(attackEffectWorldTransform_.scale_);
	mR = MakeRotateMatrixM(
	    MakeRotateXMatrixM(attackEffectWorldTransform_.rotation_.x), MakeRotateYMatrixM(attackEffectWorldTransform_.rotation_.y), MakeRotateZMatrixM(attackEffectWorldTransform_.rotation_.z));
	mT = MakeTranslateMatrixM(attackEffectWorldTransform_.translation_);
	attackEffectWorldTransform_.matWorld_ = MultM(mS, MultM(mR, mT));

	// ワールドトランスフォームの更新
	attackEffectWorldTransform_.TransferMatrix();

	// アフィン変換
	mS = MakeScaleMatrixM(aimerWorldTransform_.scale_);
	mR = MakeRotateMatrixM(MakeRotateXMatrixM(aimerWorldTransform_.rotation_.x), MakeRotateYMatrixM(aimerWorldTransform_.rotation_.y), MakeRotateZMatrixM(aimerWorldTransform_.rotation_.z));
	mT = MakeTranslateMatrixM(aimerWorldTransform_.translation_);
	aimerWorldTransform_.matWorld_ = MultM(mS, MultM(mR, mT));

	// ワールドトランスフォームの更新
	aimerWorldTransform_.TransferMatrix();

#ifdef _DEBUG

	MapChipField::IndexSet index = mapChipField_->GetMapChipIndexByPosition(worldTransform_.translation_);
	Vector2 indexV2 = {(float)index.xIndex, (float)index.yIndex};

	/// ImGuiのデバッグウィンドウ
	ImGui::Begin("Debug");
	ImGui::InputFloat("kBulletParameter_", &HP_);
	ImGui::InputFloat2("Index", &indexV2.x);
	ImGui::End();
#endif
}

void Player::Render() {
	// assert(camera_ != nullptr);
	// assert(model_ != nullptr);
	// assert(textureHandle_ != 0u);
	/// モデルの描画
	// model_->Draw(worldTransform_, *camera_, textureHandle_);
	if (!isDead_) {
		if (int(kDamageParameter_) % 4 == 0) {
			model_->Draw(worldTransform_, *camera_);
		}
		aimerModel_->Draw(aimerWorldTransform_, *camera_);
	}
	if (behavior_ == Behavior::kAttack) {
		if (attackPhase_ == AttackPhase::lunge)
			attackEffectModel_->Draw(attackEffectWorldTransform_, *camera_);
		if (attackPhase_ == AttackPhase::afterglow)
			attackEffectModel_->Draw(attackEffectWorldTransform_, *camera_);
	}
}

void Player::RenderForTitle() {
	/// モデルの描画
	if (!isDead_) {
		model_->Draw(worldTransform_, *camera_);
	}
}

void Player::BehaviorRootInitialize() {}

void Player::BehaviorAttackInitialize() { attackParameter_ = 0; }

void Player::BehaviorRootUpdate() {
	/// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	/// 移動量に速度の値をコピー
	collisionMapInfo.moveVector = velocity_;

	/// マップ衝突チェック
	Move();
	MapCollisionDecide(collisionMapInfo);
	MovePlayerByResult(collisionMapInfo);
	OnGroundChanger(collisionMapInfo);
	ceilingCollistionResult(collisionMapInfo);
	WallCollistionAction(collisionMapInfo);
}

void Player::OnCollision(Enemy* enemy) {
	(void)enemy;
	if (behavior_ != Behavior::kDamage) {
		if (behaviorRequest_ != Behavior::kDamage) {
			behaviorRequest_ = Behavior::kDamage;
			HP_ -= 1.0f;
			Audio::GetInstance()->PlayWave(seDamageHandle_, false, 0.7f);

			if (HP_ <= 0.0f) {
				isDead_ = true;
				Audio::GetInstance()->PlayWave(seDeadHandle_, false, 0.3f);
			}

			float hitPosX = (GetWorldPosition().x - enemy->GetWorldPosition().x);
			if (hitPosX > 0) {
				velocity_.x = 0.5f;
			} else {
				velocity_.x = -0.5f;
			}
			velocity_.y = 0.15f;
		}
	}
}

Vector3 Player::GetWorldPosition() const {
	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}

AABB Player::GetAABB() {
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kPlayerWidth / 2.0f, worldPos.y - kPlayerHeight / 2.0f, worldPos.z - kPlayerWidth / 2.0f};
	aabb.max = {worldPos.x + kPlayerWidth / 2.0f, worldPos.y + kPlayerHeight / 2.0f, worldPos.z + kPlayerWidth / 2.0f};

	return aabb;
}

Vector2 Player::GetShootingPosition() { return {aimerWorldTransform_.translation_.x, aimerWorldTransform_.translation_.y}; }

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
	    {+kPlayerWidth / 2.0f, -kPlayerHeight / 2.0f, 0}, // kRightBottom
	    {-kPlayerWidth / 2.0f, -kPlayerHeight / 2.0f, 0}, // kLeftBottom
	    {+kPlayerWidth / 2.0f, +kPlayerHeight / 2.0f, 0}, // kRightTop
	    {-kPlayerWidth / 2.0f, +kPlayerHeight / 2.0f, 0}, // kLeftTop
	};

	Vector3 result;
	result.x = center.x + offsetTable[static_cast<uint32_t>(corner)].x;
	result.y = center.y + offsetTable[static_cast<uint32_t>(corner)].y;
	result.z = center.z + offsetTable[static_cast<uint32_t>(corner)].z;

	return result;
}

void Player::Move() {

	bool landing = false;
	/// 移動
	// 左右移動捜索
	if (Input::GetInstance()->PushKey(DIK_D) || Input::GetInstance()->PushKey(DIK_A)) {
		Vector3 acceleration = {};
		if (Input::GetInstance()->PushKey(DIK_D)) {
			if (velocity_.x < 0.0f) {
				// 速度と逆方向に入力中に急ブレーキ
				velocity_.x *= (1.0f - kAttenuation);
			}
			acceleration.x += kAcceleration;

			if (lrDirection_ != LRDirection::kRight) {
				lrDirection_ = LRDirection::kRight;
				turnFirstRotationY_ = worldTransform_.rotation_.y;
				turnTimer_ = kTimeTurn;
			}
		}
		if (Input::GetInstance()->PushKey(DIK_A)) {
			if (velocity_.x > 0.0f) {
				// 速度と逆方向に入力中に急ブレーキ
				velocity_.x *= (1.0f - kAttenuation);
			}
			acceleration.x -= kAcceleration;
			if (lrDirection_ != LRDirection::kLeft) {
				lrDirection_ = LRDirection::kLeft;
				turnFirstRotationY_ = worldTransform_.rotation_.y;
				turnTimer_ = kTimeTurn;
			}
		}

		if (Input::GetInstance()->PushKey(DIK_A) && Input::GetInstance()->PushKey(DIK_D)) {
			if (lrDirection_ != LRDirection::None) {
				lrDirection_ = LRDirection::None;
				turnFirstRotationY_ = worldTransform_.rotation_.y;
				turnTimer_ = kTimeTurn;
			}
		}

		// 加速/減速
		velocity_.x += acceleration.x;

		velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
	} else {
		// 非入力時は移動減衰をかける
		velocity_.x *= (1.0f - kAttenuation);
		if (velocity_.x < 0.01f && velocity_.x > -0.01f) {
			velocity_.x = 0.0f;
		}
		if (behavior_ != Behavior::kAttack) {
			if (lrDirection_ != LRDirection::None) {
				lrDirection_ = LRDirection::None;
				turnFirstRotationY_ = worldTransform_.rotation_.y;
				turnTimer_ = kTimeTurn;
			}
		}
	}

	// 回転制御
	if (turnTimer_ > 0.0f) {
		turnTimer_ -= 1.0f;

		float destinationRotationYTable[] = {
		    0.0f,                            // 右
		    std::numbers::pi_v<float>,       // 左
		    std::numbers::pi_v<float> / 2.0f // なし
		};
		// 状態に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		// 自キャラの角度を設定する
		worldTransform_.rotation_.y = turnFirstRotationY_ * (turnTimer_ / kTimeTurn) + destinationRotationY * (1 - turnTimer_ / kTimeTurn);
	}

	if (onGround_) {

		// ジャンプ
		if (Input::GetInstance()->PushKey(DIK_W)) {
			// ジャンプ初速を加える
			velocity_.y += kJumpAcceleration;
			Audio::GetInstance()->PlayWave(seJumpHandle_, false, 0.5);
		}

	} else {
		// 落下速度
		velocity_.y -= kGravityAcceleration;
		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
		// 空中
		if (velocity_.y < 0) {
			if ((worldTransform_.translation_.y + velocity_.y) <= 2.0f) {
				landing = true;
			}
		}
	}
}

void Player::MapCollisionDecide(CollisionMapInfo& info) {
	MapCollisionDecideUp(info);
	MapCollisionDecideDown(info);
	MapCollisionDecideLeft(info);
	MapCollisionDecideRight(info);
}

void Player::MapCollisionDecideUp(CollisionMapInfo& info) {
	/// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); i++) {
		Vector3 translation_ = {};
		translation_.x = worldTransform_.translation_.x + info.moveVector.x;
		translation_.y = worldTransform_.translation_.y + info.moveVector.y;
		translation_.z = worldTransform_.translation_.z + info.moveVector.z;
		positionsNew[i] = CornerPosition(translation_, static_cast<Corner>(i));
	}
	if (info.moveVector.y < 0)
		return;

	// 当たり判定を行う
	bool hit = false;

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	MapChipField::IndexSet indexSet;
	// 左上の判定
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// 右上の判定（kRightTopについて同様に判定する）
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット?
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftTop]);
		MapChipField::IndexSet indexSetNow = mapChipField_->GetMapChipIndexByPosition(worldTransform_.translation_);
		if (indexSetNow.yIndex != indexSet.yIndex) {
			// めり込み先ブロックの矩形取得
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.moveVector.y = std::min(0.0f, rect.bottom - (kPlayerHeight / 2.0f) - worldTransform_.translation_.y);
			// 天井判定であることを記録する
			info.ceilingHit = true;
		}
	}
}

void Player::MapCollisionDecideDown(CollisionMapInfo& info) {
	/// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); i++) {
		Vector3 translation_ = {};
		translation_.x = worldTransform_.translation_.x + info.moveVector.x;
		translation_.y = worldTransform_.translation_.y + info.moveVector.y;
		translation_.z = worldTransform_.translation_.z + info.moveVector.z;
		positionsNew[i] = CornerPosition(translation_, static_cast<Corner>(i));
	}
	if (info.moveVector.y > 0)
		return;

	// 当たり判定を行う
	bool hit = false;

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	MapChipField::IndexSet indexSet;
	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// ブロックにヒット?
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftBottom]);
		MapChipField::IndexSet indexSetNow = mapChipField_->GetMapChipIndexByPosition(worldTransform_.translation_);
		if (indexSetNow.yIndex != indexSet.yIndex) {
			// めり込み先ブロックの矩形取得
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.moveVector.y = std::max(0.0f, rect.top - (kPlayerHeight / 2.0f) - worldTransform_.translation_.y);
			// 床判定であることを記録する
			info.floorHit = true;
		}
	}
}

void Player::MapCollisionDecideLeft(CollisionMapInfo& info) {
	/// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); i++) {
		Vector3 translation_ = {};
		translation_.x = worldTransform_.translation_.x + info.moveVector.x;
		translation_.y = worldTransform_.translation_.y + info.moveVector.y;
		translation_.z = worldTransform_.translation_.z + info.moveVector.z;
		positionsNew[i] = CornerPosition(translation_, static_cast<Corner>(i));
	}
	if (info.moveVector.x > 0)
		return;

	// 当たり判定を行う
	bool hit = false;

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	MapChipField::IndexSet indexSet;
	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// ブロックにヒット?
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftTop]);
		MapChipField::IndexSet indexSetNow = mapChipField_->GetMapChipIndexByPosition(worldTransform_.translation_);
		if (indexSetNow.xIndex != indexSet.xIndex) {
			// めり込み先ブロックの矩形取得
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.moveVector.x = std::max(0.0f, rect.left - worldTransform_.translation_.x - (kPlayerWidth / 2.0f));
			// 床判定であることを記録する
			DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
			info.wallHit = true;
		}
	}
}

void Player::MapCollisionDecideRight(CollisionMapInfo& info) {
	/// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); i++) {
		Vector3 translation_ = {};
		translation_.x = worldTransform_.translation_.x + info.moveVector.x;
		translation_.y = worldTransform_.translation_.y + info.moveVector.y;
		translation_.z = worldTransform_.translation_.z + info.moveVector.z;
		positionsNew[i] = CornerPosition(translation_, static_cast<Corner>(i));
	}
	if (info.moveVector.x < 0)
		return;

	// 当たり判定を行う
	bool hit = false;

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	MapChipField::IndexSet indexSet;
	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// ブロックにヒット?
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightTop]);
		MapChipField::IndexSet indexSetNow = mapChipField_->GetMapChipIndexByPosition(worldTransform_.translation_);
		if (indexSetNow.xIndex != indexSet.xIndex) {
			// めり込み先ブロックの矩形取得
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.moveVector.x = std::min(0.0f, rect.right - worldTransform_.translation_.x - (kPlayerWidth / 2.0f));
			// 床判定であることを記録する
			info.wallHit = true;
		}
	}
}

void Player::OnGroundChanger(const CollisionMapInfo& info) {
	if (onGround_) {
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {
			MapChipType mapChipType;
			MapChipField::IndexSet indexSet;
			std::array<Vector3, kNumCorner> positionsNew;
			for (uint32_t i = 0; i < positionsNew.size(); i++) {
				Vector3 translation_ = {};
				translation_.x = worldTransform_.translation_.x + info.moveVector.x;
				translation_.y = worldTransform_.translation_.y + info.moveVector.y - 0.2f;
				translation_.z = worldTransform_.translation_.z + info.moveVector.z;
				positionsNew[i] = CornerPosition(translation_, static_cast<Corner>(i));
			}
			bool hit = false;
			// 左下点の判定
			indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftBottom]);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}
			// 右下点の判定
			indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightBottom]);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}
			if (!hit) {
				onGround_ = false;
			}
		}
	} else {
		if (info.floorHit) {
			onGround_ = true;
			velocity_.x *= (1.0f - kAttenuationLanding);
			velocity_.y = 0.0f;
		}
	}
}

void Player::MovePlayerByResult(const CollisionMapInfo& info) {
	worldTransform_.translation_.x += info.moveVector.x;
	worldTransform_.translation_.y += info.moveVector.y;
	worldTransform_.translation_.z += info.moveVector.z;
}

void Player::ceilingCollistionResult(const CollisionMapInfo& info) {
	// 天井に当たった？
	if (info.ceilingHit) {
		velocity_.y = 0;
	}
}

void Player::WallCollistionAction(CollisionMapInfo& info) {
	if (info.wallHit) {
		// velocity_.x *= (1.0f - kAttenuationWall);
		velocity_.x = 0;
	}
}
