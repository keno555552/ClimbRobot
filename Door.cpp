#include "Door.h"

void Door::Initialize(const Camera* camera, const Vector3& position) {

	/// モデルの設定
	model_ = Model::CreateFromOBJ("door", true);

	/// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	/// カメラの設定
	camera_ = camera;
}

void Door::Update() {

	{
		// アフィン変換
		Matrix4x4 mS = MakeScaleMatrixM(worldTransform_.scale_);
		Matrix4x4 mR = MakeRotateMatrixM(MakeRotateXMatrixM(worldTransform_.rotation_.x), MakeRotateYMatrixM(worldTransform_.rotation_.y), MakeRotateZMatrixM(worldTransform_.rotation_.z));

		Matrix4x4 mT = MakeTranslateMatrixM(worldTransform_.translation_);

		worldTransform_.matWorld_ = MultM(mS, MultM(mR, mT));
	}

	// ワールドトランスフォームの更新
	worldTransform_.TransferMatrix();
}

void Door::Render() {

	/// モデル描画
	model_->Draw(worldTransform_, *camera_);
}

Vector3 Door::GetWorldPosition() {
	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
};

AABB Door::GetAABB() {
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - 1.0f, worldPos.y - 1.0f, worldPos.z - 1.0f};
	aabb.max = {worldPos.x + 1.0f, worldPos.y + 1.0f, worldPos.z + 1.0f};

	return aabb;
}