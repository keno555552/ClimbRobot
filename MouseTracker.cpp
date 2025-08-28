#include "MouseTracker.h"

void MouseTracker::Initialize(Camera* camera, Player* player) {
	camera_ = camera;
	model_ = Model::CreateFromOBJ("targeter", true);
	model_2_ = Model::CreateFromOBJ("targeterInv", true);

	input_ = Input::GetInstance();

	worldTransform_.Initialize();
	worldTransform_.translation_.z = player->GetWorldPosition().z;

	player_ = player;
}

void MouseTracker::Update() {
	Vector2 mousePos = input_->GetMousePosition();

	//float ndcX = (2.0f * mousePos.x) / 1280.0f - 1.0f;
	//float ndcY = 1.0f - (2.0f * mousePos.y) / 720.0f;

	float ndcX = (2.0f * mousePos.x) / 1280.0f - 1.0f;
	float ndcY = 1.0f - (2.0f * mousePos.y) / 720.0f;
	
	//Vector4 clipNear = {ndcX, ndcY, 0.01f,1.0f};
	Vector4 clipFar = {ndcX, ndcY, 0.99f,1.0f};
	
	Matrix4x4 invVp = Inverse(MultM(camera_->matView,camera_->matProjection));
	//
	//Vector4 worldNear4 = MultV4ByM4(clipNear, invVp);
	//Vector3 worldNear;
	//worldNear.x = worldNear4.x / worldNear4.w;
	//worldNear.y = worldNear4.y / worldNear4.w;
	//worldNear.z = worldNear4.z / worldNear4.w;
	//
	Vector4 worldFar4 = MultV4ByM4(clipFar, invVp);
	Vector3 worldFar;
	worldFar.x = worldFar4.x / worldFar4.w;
	worldFar.y = worldFar4.y / worldFar4.w;
	worldFar.z = worldFar4.z / worldFar4.w;
	
	Vector3 rayOrigin = camera_->translation_;
	Vector3 world;
	//world.x = worldFar4.x - worldNear4.x;
	//world.y = worldFar4.y - worldNear4.y;
	//world.z = worldFar4.z - worldNear4.z;
	world.x = worldFar4.x - rayOrigin.x;
	world.y = worldFar4.y - rayOrigin.y;
	world.z = worldFar4.z - rayOrigin.z;


	Vector3 rayDir = Normalize(world);
	float targetZ = player_->GetWorldPosition().z ;
	float t = 0.0f;
	if (fabs(rayDir.z) > 1e-6f) {
		//t = (targetZ - worldNear.z) / rayDir.z;
		t = (targetZ - rayOrigin.z) / rayDir.z;
	} else {
		t = 0.0f;
	}
	float num = 28.0f;
	//float posX = 0.003f;
	//float posY = 0.005f;
	//float num = 1.0f;
	//worldTransform_.translation_ = {worldNear.x + rayDir.x * t, worldNear.y + rayDir.y * t, worldNear.z + rayDir.z * t};
	worldTransform_.translation_ = {(rayOrigin.x + rayDir.x * t) * num, (rayOrigin.y + rayDir.y * t) * num, (rayOrigin.z + rayDir.z * t - 1.1f)};
	//worldTransform_.translation_ = {camera_->translation_.x + rayDir.x * t, camera_->translation_.y + rayDir.y * t, 0};

	// アフィン変換
	Matrix4x4 mS = MakeScaleMatrixM(worldTransform_.scale_);
	Matrix4x4 mR = MakeRotateMatrixM(MakeRotateXMatrixM(worldTransform_.rotation_.x), MakeRotateYMatrixM(worldTransform_.rotation_.y), MakeRotateZMatrixM(worldTransform_.rotation_.z));
	Matrix4x4 mT = MakeTranslateMatrixM(worldTransform_.translation_);
	worldTransform_.matWorld_ = MultM(mS, MultM(mR, mT));

	// ワールドトランスフォームの更新
	worldTransform_.TransferMatrix();

	///// ImGuiのデバッグウィンドウ
	// ImGui::Begin("Debug");
	//// ImGui::Checkbox("DebugCamera", &useDebugCamera);
	// ImGui::SliderFloat2("mousePos", &mousePos.x, 1, 50);
	// ImGui::SliderFloat3("worldTransform_,translation_", &worldTransform_.translation_.x, 1, 50);
	// ImGui::SliderFloat3("worldPosV3", &world.x, 1, 50);
	// ImGui::SliderFloat("far", &far_, 1, 20000);
	// ImGui::SliderFloat("near", &near_, -1, -20000);
	// ImGui::End();
}

void MouseTracker::Render() { 
	if (input_->IsPressMouse(0)) {
		model_2_->Draw(worldTransform_, *camera_);
	} else {
		model_->Draw(worldTransform_, *camera_);
	}
}