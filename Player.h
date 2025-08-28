#pragma once
#include "KamataEngine.h"
using namespace KamataEngine;
#include "Bullet.h"
#include "MapChipField.h"
#include "crashDecision.h"
#include "myMathForAL.h"

///// 前方宣言
class MapChipField;

/////設定
/// 移動速度
static inline const float kAcceleration = 0.01f;
/// 移動減衰
static inline const float kAttenuation = 0.08f;
/// 移動限界速度
static inline const float kLimitRunSpeed = 0.5f;
/// 回転速度
static inline const float kTimeTurn = 10.0f;
/// 重力加速度
static inline const float kGravityAcceleration = 0.02f;
/// 最大落下速度
static inline const float kLimitFallSpeed = 0.5f;
/// ジャンプ初速
static inline const float kJumpAcceleration = 0.5f;
/// キャラクターの当たり判定サイズ
static inline const float kPlayerWidth = 1.9f;
static inline const float kPlayerHeight = 1.9f;
/// 着地時の速度減衰率
static inline const float kAttenuationLanding = 0.00000001f;

///
static inline const float kAttenuationWall = 0.5f;

///
static inline const float kAimerDistance = 2.0f;
static inline const float kAimerRotateSpeed = 0.1f;

/// マップとの当たり判定情報
struct CollisionMapInfo {
	bool ceilingHit = false; // 天井に当の当たり判定
	bool floorHit = false;   // 床に当の当たり判定
	bool wallHit = false;    // 壁に当の当たり判定
	Vector3 moveVector = {}; // 移動量
};

/// 角
enum Corner {
	kRightBottom, // 右下
	kLeftBottom,  // 左下
	kRightTop,    // 右上
	kLeftTop,     // 左上

	kNumCorner // 要素数
};

enum class LRDirection { kRight, kLeft, None };

class Enemy;
class Bullet;
class GameScene;
class Player {
public:
	~Player();
	/// 初期化
	void Initialize(const Camera* camera, const Vector3& position);
	/// 更新
	void Update();
	/// 更新(title描画用)
	void UpdateForTitle();
	/// 描画
	void Render();
	/// 描画
	void RenderForTitle();

	/// 行動関連
	void BehaviorRootInitialize();
	void BehaviorAttackInitialize();
	void BehaviorRootUpdate();
	void OnCollision(Enemy* enemy);

	/// 参照命令
	void SetMpChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	/// aimerの目標位置セッタ
	void SetTargetPos(const Vector2 pos) { targetPos_ = pos; }
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	/// ワールドトランスフォーム取得
	WorldTransform& GetWorldTransform() { return worldTransform_; }
	Vector3 GetWorldPosition() const;
	const Vector3& GetVelocity() const { return velocity_; }
	AABB GetAABB();
	bool GetStateDead() const { return isDead_; }
	bool GetStateAttack() const { return behavior_ == Behavior::kAttack; }
	Vector2 GetShootingPosition();
	float GetShootingAngle() const { return rotateAimer_; }
	float GetHp() const { return HP_; }

private:
	Vector3 CornerPosition(const Vector3& center, Corner corner);

	void Move();

	void MapCollisionDecide(CollisionMapInfo& info);
	void MapCollisionDecideUp(CollisionMapInfo& info);
	void MapCollisionDecideDown(CollisionMapInfo& info);
	void MapCollisionDecideLeft(CollisionMapInfo& info);
	void MapCollisionDecideRight(CollisionMapInfo& info);

	void WallCollistionAction(CollisionMapInfo& info);
	void OnGroundChanger(const CollisionMapInfo& info);
	void MovePlayerByResult(const CollisionMapInfo& info);
	void ceilingCollistionResult(const CollisionMapInfo& info);

private:
	///// 振る舞い
	enum class Behavior {
		kUnknown = -1, // リクエストのない
		kRoot,         // 通常状態
		kAttack,       // 攻撃中
		kDamage,       // ダメージ中
	};
	Behavior behavior_ = Behavior::kRoot;
	Behavior behaviorRequest_ = Behavior::kUnknown;

	/// 攻撃
	// 攻撃ギミックの経過時間カウンター
	int attackParameter_ = 0;
	enum class AttackPhase {
		none,      // アタックではない
		sink,      // 溜め
		lunge,     // 突進
		afterglow, // 余韻
	};
	AttackPhase attackPhase_ = AttackPhase::none;

private:
	///// モデル
	Model* model_ = nullptr;

	/// ワールドトランスフォーム
	WorldTransform worldTransform_;

	/// アタックエフェクトのモデル
	Model* attackEffectModel_ = nullptr;

	/// アタックエフェクトのモデルのワールドトランスフォーム
	WorldTransform attackEffectWorldTransform_;

	/// カメラ
	const Camera* camera_ = nullptr;
	/// テクスチャーハンドル
	uint32_t textureHandle_ = 0u;

	/// Input
	Input* input_ = nullptr;

	/// ゲームシーン
	GameScene* gameScene_ = nullptr;

private:
	///// aimer計算
	/// aimer
	Model* aimerModel_ = nullptr;
	float rotateAimer_ = 0.0f;
	WorldTransform aimerWorldTransform_;
	Vector2 targetPos_ = {};

private:
	///// 参照
	/// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

private:
	/////// State
	/// HP
	float HP_ = 3.0f;

	/////// CD
	/// dushCD
	float dushCD_ = 1.0f; ///(秒)
	float dushCDParameter_ = dushCD_ * 60.0f;

	/// Bullet
	float kBulletCD_ = 0.25; ///(秒)
	float kBulletParameter_ = kBulletCD_ * 60.0f;

	/// DamageCD
	float kDamageCD_ = 1.0f; ///(秒)
	float kDamageParameter_ = kDamageCD_ * 60.0f;

private:
	//////// Title
	/// 経過時間
	float titleWalkTimer_ = 0.0f;

	//////// プレイヤーデータ
	/// 移動速度
	Vector3 velocity_ = {};
	/// 方向/開始時の方向
	LRDirection lrDirection_ = LRDirection::kRight;
	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;
	// 接地状態フラグ
	bool onGround_ = true;

	float kBlank = 0.0f; // ジャンプ中のブランク時間

private:
	/// 効果音
	uint32_t seJumpHandle_ = Audio::GetInstance()->LoadWave("sound/SE/player_jump.wav");
	uint32_t seShootHandle_ = Audio::GetInstance()->LoadWave("sound/SE/player_attack.wav");
	uint32_t seDushHandle_ = Audio::GetInstance()->LoadWave("sound/SE/player_dush.wav");
	uint32_t seDamageHandle_ = Audio::GetInstance()->LoadWave("sound/SE/playerDamage.wav");
	uint32_t seDeadHandle_ = Audio::GetInstance()->LoadWave("sound/SE/player_dead.wav");

private:
	bool isDamage_ = false;
	// デスフラグ
	bool isDead_ = false;
};
