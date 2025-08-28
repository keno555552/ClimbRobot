#include "Bullet.h"

KamataEngine::Model* Bullet::model_ = nullptr;
KamataEngine::Camera* Bullet::camera_ = nullptr;

void Bullet::Initialize(const Vector3& pos, const float angle) {
	worldTransform_.Initialize();
	worldTransform_.translation_ = pos;
	worldTransform_.scale_ = {1.5f, 1.5f, 1.0f};

	angle_ = angle;

	color_.Initialize();
	color_.SetColor({1.0f, 1.0f, 0.0f, 1.0f});
}

void Bullet::Update() {

	MapCollisionDecide();

	/// aimer計算
	worldTransform_.translation_.x += kBulletSpeed * cosf(angle_);
	worldTransform_.translation_.y += kBulletSpeed * sinf(angle_);

	worldTransform_.rotation_.z = angle_ - 3.14f / 2.0f;
	worldTransform_.rotation_.y += 0.2f;

	// アフィン変換
	Matrix4x4 mS = MakeScaleMatrixM(worldTransform_.scale_);
	Matrix4x4 mR = MakeRotateMatrixM(MakeRotateXMatrixM(worldTransform_.rotation_.x), MakeRotateYMatrixM(worldTransform_.rotation_.y), MakeRotateZMatrixM(worldTransform_.rotation_.z));
	Matrix4x4 mT = MakeTranslateMatrixM(worldTransform_.translation_);
	worldTransform_.matWorld_ = MultM(mS, MultM(mR, mT));

	// ワールドトランスフォームの更新
	worldTransform_.TransferMatrix();
}

void Bullet::Render() {
	if (!isDead) {
		model_->Draw(worldTransform_, *camera_, &color_);
	}
}

Vector3 Bullet::GetWorldPosition() const {
	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}

Bullet* Bullet::Create(const Vector3& pos, const float angle) {
	/// インスタンス生成
	Bullet* instance = new Bullet;
	/// newの失敗を検出
	//assert(instance);
	/// インスタンスの初期化
	instance->Initialize(pos, angle);
	/// 初期化したインスタンスを返す
	return instance;
}

AABB Bullet::GetAABB() {
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kBulletWidth / 2.0f, worldPos.y - kBulletHeight / 2.0f, worldPos.z - kBulletWidth / 2.0f};
	aabb.max = {worldPos.x + kBulletWidth / 2.0f, worldPos.y + kBulletHeight / 2.0f, worldPos.z + kBulletWidth / 2.0f};

	return aabb;
}

void Bullet::MapCollisionDecide() {
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexByPosition(worldTransform_.translation_);
	if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
		isDead = true;
	}
}
