#pragma once
#include "KamataEngine.h"
using namespace KamataEngine;
#include "MapChipField.h"
#include "Player.h"
#include "crashDecision.h"
#include "myMathForAL.h"

/// キャラクターの当たり判定サイズ
static inline const float kBulletWidth = 0.5f;
static inline const float kBulletHeight = 0.5f;

class Bullet {
public:
	void Initialize(const Vector3& pos, const float angle);
	void Update();
	void Render();

public:
	/// 参照命令
	void SetMpChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	static void SetModel(Model* model) { model_ = model; }
	static void SetCamera(Camera* camera) { camera_ = camera; }
	void SetIsDead(bool dead) { isDead = dead; }

	Vector3 GetWorldPosition()const;
	bool GetIsDead() const { return isDead; }
	AABB GetAABB();

	static Bullet* Create(const Vector3& pos, const float angle);

private:
	void MapCollisionDecide();

private:
	const float kBulletCD = 0.5f; ///(秒)
	const float kBulletSpeed = 1.0f;

private:
	///// 参照
	/// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

private:
	/// モデル
	static Model* model_;
	/// カメラ
	static Camera* camera_;

	/// ワールドトランスフォーム
	WorldTransform worldTransform_;

	ObjectColor color_;

	float angle_ = 0.0f;

	bool isDead = false;
};
