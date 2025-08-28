#include "Enemy.h"
#include "Bullet.h"
#include <algorithm>
#include <numbers>

void Enemy::Initialize(const Camera* camera, const Vector3& position) {
	/// モデルの設定
	model_ = Model::CreateFromOBJ("enemy", true);

	/// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	/// カメラの設定
	camera_ = camera;

	/// ワールドトランスフォームの初期化
	// 初期回転
	worldTransform_.rotation_.y = -std::numbers::pi_v<float>;

	/// 速度を設定する
	velocity_ = {-kWalkSpeed, 0, 0};

	///// アニメーション
	walkTimer_ = 0.0f;
}

void Enemy::Update() {

	if (isDead_ && (deadPhase_ == DeadPhase::none)) {
		behaviorRequest_ = Behavior::kDead;
	}

	if (behaviorRequest_ != Behavior::kUnknown) { // ① 振るまいリクエストが有効なら処理開始
		// 振るまいを変更する
		behavior_ = behaviorRequest_; // ② リクエストされた振るまいに切り替え

		// 各振るまいごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
		default:
			
			break;
		case Behavior::kDead:
			deadEffectParameter_ = 0;
			deadPhase_ = DeadPhase::rotate;
			Audio::GetInstance()->PlayWave(seDieHandle_, false, 1.0f);
			break;
		}
		// 振るまいリクエストをリセット
		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {
	case Behavior::kRoot:
	default:

		Vector3 vector3Right = worldTransform_.translation_;
		vector3Right.x += kEnemyWidth/2;
		Vector3 vector3Left = worldTransform_.translation_;
		vector3Left.x -= kEnemyWidth/2;
		Vector3 vector3RightDown = vector3Right;
		vector3RightDown.y -= kEnemyHeight;
		Vector3 vector3LeftDown = vector3Left;
		vector3LeftDown.y -= kEnemyHeight;
		MapChipField::IndexSet indexRight;
		MapChipField::IndexSet indexLeft;
		MapChipField::IndexSet indexRightDown;
		MapChipField::IndexSet indexLeftDown;
		indexRight = mapChipField_->GetMapChipIndexByPosition(vector3Right);
		indexLeft = mapChipField_->GetMapChipIndexByPosition(vector3Left);
		indexRightDown = mapChipField_->GetMapChipIndexByPosition(vector3RightDown);
		indexLeftDown = mapChipField_->GetMapChipIndexByPosition(vector3LeftDown);
		if ((mapChipField_->GetMapChipTypeByIndex(indexRight.xIndex, indexRight.yIndex) == MapChipType::kBlock) ||
		    (mapChipField_->GetMapChipTypeByIndex(indexRightDown.xIndex, indexRightDown.yIndex) == MapChipType::kBlock)) {
			velocity_.x *= -1.0f;
		}
		if ((mapChipField_->GetMapChipTypeByIndex(indexRight.xIndex, indexRight.yIndex) == MapChipType::kBlock) &&
		    (mapChipField_->GetMapChipTypeByIndex(indexRightDown.xIndex, indexRightDown.yIndex) == MapChipType::kBlock)) {
			velocity_.x *= -1.0f;
		}
		if ((mapChipField_->GetMapChipTypeByIndex(indexLeft.xIndex, indexLeft.yIndex) == MapChipType::kBlock) ||
		    (mapChipField_->GetMapChipTypeByIndex(indexLeftDown.xIndex, indexLeftDown.yIndex) == MapChipType::kBlock)) {
			velocity_.x *= -1.0f;
		}
		if ((mapChipField_->GetMapChipTypeByIndex(indexLeft.xIndex, indexLeft.yIndex) == MapChipType::kBlock) &&
		    (mapChipField_->GetMapChipTypeByIndex(indexLeftDown.xIndex, indexLeftDown.yIndex) == MapChipType::kBlock)) {
			velocity_.x *= -1.0f;
		}
		if (velocity_.x < 0) {
			if (worldTransform_.rotation_.y > -std::numbers::pi_v<float>) {
				worldTransform_.rotation_.y -= 15.0f * (3.14f / 180.0f);
			}
			if (worldTransform_.rotation_.y < -std::numbers::pi_v<float>) {
				worldTransform_.rotation_.y = -std::numbers::pi_v<float>;
			}
		} else {
			if (worldTransform_.rotation_.y < 0) {
				worldTransform_.rotation_.y += 15.0f * (3.14f / 180.0f);
			}
			if (worldTransform_.rotation_.y > 0) {
				worldTransform_.rotation_.y = 0;
			}
		}





		/// 移動
		worldTransform_.translation_.x += velocity_.x;
		worldTransform_.translation_.y += velocity_.y;
		worldTransform_.translation_.z += velocity_.z;

		/// タイマーを加算
		walkTimer_ += 1.0f / 60.0f;

		{
			/// 回転アニメーション
			float param = std::sin(2.0f * 3.14f * walkTimer_ / (kWSalkMotionTime));
			float degree = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
			worldTransform_.rotation_.z = degree * (3.14f / 180.0f);
		}

		break;
	case Behavior::kDead:
		switch (deadPhase_) {
		case DeadPhase::rotate:
		default: {
			deadEffectParameter_++;
			float time = float(kDeadAnimationTime * 60.0f);
			float degree = easyOut(0.0f, kDeadAnimationAnglie, (deadEffectParameter_ / time), 3.0f);
			// float degree = 0.0f + kDeadAnimationAnglie * (deadEffectParameter_ / time);
			worldTransform_.rotation_.z = 0.0f;
			worldTransform_.rotation_.y = degree * (3.14f / 180.0f);

			if (deadEffectParameter_ >= time) {
				deadPhase_ = DeadPhase::finsih;
			}
		}

		break;
		case DeadPhase::finsih:
			deadEffectFinished_ = true;
			break;
		}
		break;
	}

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
	// ImGui::Begin("Debug2");
	//// ImGui::Checkbox("DebugCamera", &useDebugCamera);
	// ImGui::SliderFloat("param", &param, 0,100);
	// ImGui::End();
}

void Enemy::Render() {
	/// モデルの描画
	model_->Draw(worldTransform_, *camera_);
}

void Enemy::OnCollision(const Player* player) {
	(void)player;
	if (isDead_)
		return;
	if (player->GetStateAttack()) {
		behaviorRequest_ = Behavior::kDead;

		Vector3 effectPos;
		effectPos.x = (GetWorldPosition().x + player->GetWorldPosition().x) / 2.0f;
		effectPos.y = (GetWorldPosition().y + player->GetWorldPosition().y) / 2.0f;
		effectPos.z = (GetWorldPosition().z + player->GetWorldPosition().z) / 2.0f;
		gameScene_->CreateHitEffect(effectPos);
		Audio::GetInstance()->PlayWave(seDushDieHandle_, false, 0.5f);
	}
	isDead_ = true;
}

void Enemy::OnCollision(Bullet* bullet) {
	(void)bullet;
	if (isDead_)
		return;
	if (!bullet->GetIsDead()) {
		HP -= 1.0f;

		Vector3 effectPos;
		effectPos.x = (GetWorldPosition().x + bullet->GetWorldPosition().x) / 2.0f;
		effectPos.y = (GetWorldPosition().y + bullet->GetWorldPosition().y) / 2.0f;
		effectPos.z = (GetWorldPosition().z + bullet->GetWorldPosition().z) / 2.0f;
		gameScene_->CreateHitEffect(effectPos);
		Audio::GetInstance()->PlayWave(seHitHandle_, false, 0.5f);

		bullet->SetIsDead(true);

		if (HP <= 0) {
			behaviorRequest_ = Behavior::kDead;
			isDead_ = true;
		}
	}
}

Vector3 Enemy::GetWorldPosition() {
	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
};

AABB Enemy::GetAABB() {
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kEnemyWidth / 2.0f, worldPos.y - kEnemyHeight / 2.0f, worldPos.z - kEnemyWidth / 2.0f};
	aabb.max = {worldPos.x + kEnemyWidth / 2.0f, worldPos.y + kEnemyHeight / 2.0f, worldPos.z + kEnemyWidth / 2.0f};

	return aabb;
}