#include "GameScene.h"

void GameScene::GenerateBlocks() {
	/// ボックス生成
	// 要素数
	uint32_t kNumBlockVertical = mapChipField_->GetNumBlockVirtical();
	uint32_t kNumBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	// 要素数を変更する
	// 列数を設定(縦方向のブロック数)
	worldTransformBlocks_.resize(kNumBlockVertical);
	for (uint32_t i = 0; i < kNumBlockVertical; i++) {
		// 列数を設定(横方向のブロック数)
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}
	// いざボックス生成
	for (uint32_t i = 0; i < kNumBlockVertical; i++) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}

void GameScene::Initialize() {
#pragma region System
	boxTextureHandle_ = TextureManager::Load("cube/cube.jpg");
	model_ = Model::Create();
	modelBlock_ = Model::Create();
	/// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	/// カメラの初期化
	camera_.Initialize();
	/// デバックカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);
	/// 追従カメラの生成
	cameraController_ = new CameraController;
	cameraController_->Initialize();
	cameraController_->SetMovableArea({20, 20, 10, 57});

	// 軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向表示が参照するビュープロジェクションを指定する（アドレス渡し）
	AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());

	PrimitiveDrawer::GetInstance()->SetCamera(&debugCamera_->GetCamera());

	Camera& nowCamera = cameraController_->GetCamera();

	bgmPlayHandle_ = Audio::GetInstance()->PlayWave(bgmHandle_, true, 0.1f);
#pragma endregion

#pragma region GameObject

	/// スカイドームの生成
	skydome_->Initialize(&nowCamera);

	modelHeart_ = Model::CreateFromOBJ("heart", true);

	/// Player関連
	// 初期配置をマップチップ単位で指定
	player_ = new Player;
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 33);
	player_->Initialize(&nowCamera, playerPosition);
	player_->SetGameScene(this);

	Vector3 doorPosition = mapChipField_->GetMapChipPositionByIndex(5, 2);
	door_->Initialize(&nowCamera, doorPosition);

	/// MouseTracker
	mouseTracker_->Initialize(&nowCamera, player_);

	{
		/// Bullet関連
		Model* newModel = Model::CreateFromOBJ("bullet", true);
		Bullet::SetModel(newModel);
		Bullet::SetCamera(&nowCamera);
	}

	/// Enemy関連
	// 初期配置をマップチップ単位で指定
	Vector3 enemyStartPosition[13];
	enemyStartPosition[0] = mapChipField_->GetMapChipPositionByIndex(11, 33);
	enemyStartPosition[1] = mapChipField_->GetMapChipPositionByIndex(16, 26);
	enemyStartPosition[2] = mapChipField_->GetMapChipPositionByIndex(8, 28);
	enemyStartPosition[3] = mapChipField_->GetMapChipPositionByIndex(16, 30);
	enemyStartPosition[4] = mapChipField_->GetMapChipPositionByIndex(16, 23);
	enemyStartPosition[5] = mapChipField_->GetMapChipPositionByIndex(13, 18);
	enemyStartPosition[6] = mapChipField_->GetMapChipPositionByIndex(2, 21);
	enemyStartPosition[7] = mapChipField_->GetMapChipPositionByIndex(15, 14);
	enemyStartPosition[8] = mapChipField_->GetMapChipPositionByIndex(2, 10);
	enemyStartPosition[9] = mapChipField_->GetMapChipPositionByIndex(6, 6);
	enemyStartPosition[10] = mapChipField_->GetMapChipPositionByIndex(3, 5);
	enemyStartPosition[11] = mapChipField_->GetMapChipPositionByIndex(15, 5);
	enemyStartPosition[12] = mapChipField_->GetMapChipPositionByIndex(12, 2);

	//// Test
	// deathParticles_ = new DeathParticles;
	// deathParticles_->Initialize(&nowCamera, playerPosition);

	/// マップチップの生成
	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipCsv("Resources/stage/blocks1.csv");

	/// マップチップの初期化
	GenerateBlocks();

	for (int i = 0; i < sizeof(enemyStartPosition) / sizeof(enemyStartPosition[0]); i++) {
		Enemy* enemy = new Enemy;
		enemy->Initialize(&nowCamera, enemyStartPosition[i]);
		enemy->SetGameScene(this);
		enemy->SetMpChipField(mapChipField_);
		enemyGroup_.push_back(enemy);
	}

	/// プレイヤーのマップチップ参照
	player_->SetMpChipField(mapChipField_);

	/// 追従カメラの初期化
	cameraController_->SetTarget(player_);

	fade_->Initialze(1280, 720);
	fade_->Start(Fade::Status::FadeIn, 1.0f);

#pragma endregion

#pragma region Effect
	{
		Model* newModel_ = Model::CreateFromOBJ("alphaPlane", true);
		HitEffect::SetModel(newModel_);
		HitEffect::SetCamera(&nowCamera);
	}

#pragma endregion
}

GameScene::~GameScene() {
	delete model_, model_ = nullptr;
	delete debugCamera_, debugCamera_ = nullptr;

	delete mouseTracker_, mouseTracker_ = nullptr;

#pragma region GameObject
	delete player_, player_ = nullptr;

	delete door_, door_ = nullptr;

	if (!enemyGroup_.empty()) {
		for (auto ptr : enemyGroup_) {
			delete ptr, ptr = nullptr;
		}
	}

	delete mapChipField_, mapChipField_ = nullptr;

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();

	if (deathParticles_)
		delete deathParticles_, deathParticles_ = nullptr;

	delete fade_, fade_ = nullptr;

	delete skydome_, skydome_ = nullptr;

	if (!bulletGroup_.empty()) {
		for (auto ptr : bulletGroup_) {
			delete ptr, ptr = nullptr;
		}
	}

	// if (!bulletGroup_.empty()) {
	//	for (auto ptr : bulletGroup_) {
	//		delete ptr, ptr = nullptr;
	//	}
	// }

#pragma endregion

#pragma region Effect

	if (!hitEffect_.empty()) {
		for (auto ptr : hitEffect_) {
			delete ptr, ptr = nullptr;
		}
	}

#pragma endregion

	if (Audio::GetInstance()->IsPlaying(bgmPlayHandle_)) {
		Audio::GetInstance()->StopWave(bgmPlayHandle_);
	}
}

void GameScene::Update() {
#pragma region カメラ関連

	debugCamera_->Update();
	/// 追従カメラの更新
	cameraController_->Update();

	fade_->Update();

#pragma endregion

#pragma region ゲームロジック
#pragma endregion

#pragma region GameObject

	door_->Update();

	switch (phase_) {
	case Phase::kPlay:
		///////////////////////////////////////////////////////////////////////////////////
		/// MouseTracker
		mouseTracker_->Update();

		/// スカイドームの更新
		skydome_->Update();

		/// Player関連
		player_->Update();
		player_->SetTargetPos(mouseTracker_->GetOutput());

		/// Enemy関連
		if (!enemyGroup_.empty()) {
			for (auto enemy : enemyGroup_) {
				enemy->Update();
			}
		}

		enemyGroup_.remove_if([](Enemy* enemy) {
			if (enemy->GetStateAttackEffectFinished()) {
				delete enemy, enemy = nullptr;
				return true;
			}
			return false;
		});

		/// ボックスの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock) {
					continue;
				}

				Matrix4x4 mS = MakeScaleMatrixM(worldTransformBlock->scale_);

				Matrix4x4 mR =
				    MakeRotateMatrixM(MakeRotateXMatrixM(worldTransformBlock->rotation_.x), MakeRotateYMatrixM(worldTransformBlock->rotation_.y), MakeRotateZMatrixM(worldTransformBlock->rotation_.z));

				Matrix4x4 mT = MakeTranslateMatrixM(worldTransformBlock->translation_);

				// アフィン変換
				worldTransformBlock->matWorld_ = MultM(mS, MultM(mR, mT));

				// 定数バッファに転送
				worldTransformBlock->TransferMatrix();
			}
		}

		/// 弾丸処理
		if (!bulletGroup_.empty()) {
			bulletGroup_.erase(
			    std::remove_if(
			        bulletGroup_.begin(), bulletGroup_.end(),
			        [](Bullet* ptr) {
				        ptr->Update();
				        if (ptr->GetIsDead()) {
					        delete ptr, ptr = nullptr;
					        return true;
				        }
				        return false;
			        }),
			    bulletGroup_.end());
		}

		CheckAllCollisions();

		if (player_->GetStateDead()) {
			/// 死亡演出フェーズに切り替え
			phase_ = Phase::kDeath;
			/// 自キャラの座標を取得
			const Vector3& deathParticlesPosition = player_->GetWorldPosition();

			deathParticles_ = new DeathParticles;
			deathParticles_->Initialize(&cameraController_->GetCamera(), deathParticlesPosition);
		}

		break;
	case Phase::kDeath:
		///////////////////////////////////////////////////////////////////////////////////
		/// スカイドームの更新
		skydome_->Update();

		/// Enemy関連
		if (!enemyGroup_.empty()) {
			for (auto ptr : enemyGroup_) {
				ptr->Update();
			}
		}

		deathParticles_->Update();

		/// ボックスの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock) {
					continue;
				}

				Matrix4x4 mS = MakeScaleMatrixM(worldTransformBlock->scale_);

				Matrix4x4 mR =
				    MakeRotateMatrixM(MakeRotateXMatrixM(worldTransformBlock->rotation_.x), MakeRotateYMatrixM(worldTransformBlock->rotation_.y), MakeRotateZMatrixM(worldTransformBlock->rotation_.z));

				Matrix4x4 mT = MakeTranslateMatrixM(worldTransformBlock->translation_);

				// アフィン変換
				worldTransformBlock->matWorld_ = MultM(mS, MultM(mR, mT));

				// 定数バッファに転送
				worldTransformBlock->TransferMatrix();
			}
		}

		CheckAllCollisions();

		if (deathParticles_ && deathParticles_->IsFinished()) {
			if (fade_->GetStatus() != Fade::Status::FadeOut) {
				fade_->Start(Fade::Status::FadeOut, 1.0f);
			}
		}

		if (fade_->IsFinished()) {
			finished_ = true;
		}

		///////////////////////////////////////////////////////////////////////////////////
		break;
	}
	if (fade_->IsFinished() && isWin_) {
		finished_ = true;
	}

#pragma endregion

#pragma region Effect

	/// Enemy関連
	if (!hitEffect_.empty()) {
		hitEffect_.erase(
		    std::remove_if(
		        hitEffect_.begin(), hitEffect_.end(),
		        [](HitEffect* ptr) {
			        ptr->Update();
			        if (ptr->IsFinished()) {
				        delete ptr, ptr = nullptr;
				        return true;
			        }
			        return false;
		        }),
		    hitEffect_.end());
	}

#pragma endregion

	for (int i = 0; i < 3; i++) {

		heartWorldTransform_[i].Initialize();
		heartWorldTransform_[i].translation_ = cameraController_->GetCamera().translation_;
		heartWorldTransform_[i].translation_.x += -19.0f + i * 2.5f;
		heartWorldTransform_[i].translation_.y += 10.0f;
		heartWorldTransform_[i].translation_.z += 30.0f - 1.2f;

		// アフィン変換
		Matrix4x4 mS = MakeScaleMatrixM(heartWorldTransform_[i].scale_);
		Matrix4x4 mR = MakeRotateMatrixM(
		    MakeRotateXMatrixM(heartWorldTransform_[i].rotation_.x), MakeRotateYMatrixM(heartWorldTransform_[i].rotation_.y), MakeRotateZMatrixM(heartWorldTransform_[i].rotation_.z));

		Matrix4x4 mT = MakeTranslateMatrixM(heartWorldTransform_[i].translation_);

		heartWorldTransform_[i].matWorld_ = MultM(mS, MultM(mR, mT));
		heartWorldTransform_[i].TransferMatrix();
	}

#ifdef _DEBUG

	//
	// ImGui::ShowDemoWindow();
	//
	// ImGui::Begin("Debug2");
	////ImGui::Text("Kamata Tarou %d.%d.%d", 2050, 12, 31);
	////ImGui::InputFloat3("InputFloat3", inputFloat3);
	// ImGui::InputFloat3("SliderFloat3", inputFloat3, 0.0f, 1.0f);
	// ImGui::End();

#endif
}

void GameScene::Render() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

#pragma region 3Dモデル描画
	/// 前処理
	Model::PreDraw(dxCommon->GetCommandList());

	door_->Render();

	/// スカイドームの描画
	skydome_->Render();

	/// 自キャラの描画
	player_->Render();

	if (deathParticles_ != nullptr) {
		deathParticles_->Draw();
	}

	/// 敵キャラの描画
	if (!enemyGroup_.empty()) {
		for (auto ptr : enemyGroup_) {
			ptr->Render();
		}
	}

	/// 　ボックスの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}
			// モデルの描画
			modelBlock_->Draw(*worldTransformBlock, cameraController_->GetCamera(), boxTextureHandle_);
		}
	}

	if (!bulletGroup_.empty()) {
		for (auto ptr : bulletGroup_) {
			ptr->Render();
		}
	}

#pragma region Effect

	/// Enemy関連
	if (!hitEffect_.empty()) {
		for (auto ptr : hitEffect_) {
			ptr->Draw();
		}
	}

	/// MouseTracker
	mouseTracker_->Render();

#pragma endregion

	for (int i = 0; i < player_->GetHp(); i++) {
		modelHeart_->Draw(heartWorldTransform_[i], cameraController_->GetCamera());
	}
	/// 後処理
	Model::PostDraw();
#pragma endregion

	// PrimitiveDrawer::GetInstance()->DrawLine3d({}, {0, 10, 0}, {1.0f, 0.0f, 0.0f, 1.0f});
	// for (int i = 0; i <= 10; i++) {
	//	PrimitiveDrawer::GetInstance()->DrawLine3d({float(-5.0f + i), 0, -5}, {float(-5.0f + i), 0, +5}, {1.0f, 0.0f, 0.0f, 1.0f});
	//	PrimitiveDrawer::GetInstance()->DrawLine3d({-5, 0, float(-5.0f + i)}, {+5, 0, float(-5.0f + i)}, {0.0f, 0.0f, 1.0f, 1.0f});
	// }

#pragma region 2D描画
	//// 2D描画の前処理
	// Sprite::PreDraw(dxCommon->GetCommandList());
	//// 2D描画の後処理
	// Sprite::PostDraw();

	fade_->Draw(dxCommon);

#pragma endregion
}

void GameScene::CheckAllCollisions() {
#pragma region 自キャラと敵キャラの当たり判定
	{
		/// 判定対象1と2の座標
		AABB aabb1, aabb2;

		/// 自キャラの座標
		aabb1 = player_->GetAABB();
		if (!player_->GetStateAttack()) {
			/// 自キャラと敵弾全ての当たり判定
			if (!enemyGroup_.empty()) {
				for (Enemy* enemy : enemyGroup_) {
					if (enemy->GetStateDead())
						continue;
					/// 敵弾の座標
					aabb2 = enemy->GetAABB();

					if (crashDecision(aabb1, aabb2)) {
						/// 自キャラの衝突時関数を呼び出す
						player_->OnCollision(enemy);
					}
				}
			}
		} else {
			/// 自キャラと敵弾全ての当たり判定
			if (!enemyGroup_.empty()) {
				for (Enemy* enemy : enemyGroup_) {
					/// 敵弾の座標
					aabb2 = enemy->GetAABB();

					if (crashDecision(aabb1, aabb2)) {
						/// 自キャラの衝突時関数を呼び出す
						enemy->OnCollision(player_);
					}
				}
			}
		}
	}
#pragma endregion
#pragma region 自キャラとアイテムの当たり判定

	/// 弾丸の自動消滅
	if (!bulletGroup_.empty()) {
		for (Bullet* bullet : bulletGroup_) {
			Vector3 playerPos = player_->GetWorldPosition();
			Vector3 bulletPos = bullet->GetWorldPosition();

			Vector3 vecBetween;
			vecBetween.x = playerPos.x - bulletPos.x;
			vecBetween.y = playerPos.y - bulletPos.y;
			vecBetween.z = playerPos.z - bulletPos.z;

			if (Length(vecBetween) > 45.0f) {
				bullet->SetIsDead(true);
			}
		}
	}

	{
		AABB aabb1, aabb2;

		/// プレイヤーの座標
		aabb1 = player_->GetAABB();
		/// 自キャラと敵弾全ての当たり判定
		
		/// 門の座標
		aabb2 = door_->GetAABB();

		if (crashDecision(aabb1, aabb2)) {
			/// 自キャラの衝突時関数を呼び出す
			if (fade_->GetStatus() != Fade::Status::FadeOut) {
				fade_->Start(Fade::Status::FadeOut, 1.0f);
				isWin_ = true;
			}
		}
	}

#pragma endregion
#pragma region 自弾と敵キャラの当たり判定
	{
		/// 判定対象1と2の座標
		AABB aabb1, aabb2;

		if (!bulletGroup_.empty()) {
			for (auto bullet : bulletGroup_) {
				/// 弾丸の座標
				aabb1 = bullet->GetAABB();
				/// 自キャラと敵弾全ての当たり判定
				if (!enemyGroup_.empty()) {
					for (Enemy* enemy : enemyGroup_) {
						if (enemy->GetStateDead())
							continue;
						/// 敵弾の座標
						aabb2 = enemy->GetAABB();

						if (crashDecision(aabb1, aabb2)) {
							/// 自キャラの衝突時関数を呼び出す
							enemy->OnCollision(bullet);
						}
					}
				}
			}
		}
	}
#pragma endregion
}

void GameScene::ChangePhase() {
	switch (phase_) {
	case Phase::kPlay:
		break;
	case Phase::kDeath:
		break;
	}
}

void GameScene::CreateHitEffect(Vector3 pos) {
	HitEffect* newHitEffect = HitEffect::Create(pos);
	hitEffect_.push_back(newHitEffect);
}

void GameScene::CreateBullet(float shootAngle) {
	// Vector3 startPos = {mouseTracker_->GetOutput().x, mouseTracker_->GetOutput().y, 1.0f};
	float zbuffer = float(bulletGroup_.size() * 0.1);
	Vector3 startPos = {player_->GetShootingPosition().x, player_->GetShootingPosition().y, zbuffer};
	Bullet* newBullet = Bullet::Create(startPos, shootAngle);
	newBullet->SetMpChipField(mapChipField_);
	bulletGroup_.push_back(newBullet);
}
