#pragma once
#include "KamataEngine.h"
using namespace KamataEngine;
#include <vector>

enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロック
};



struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

class MapChipField {
public:
	struct IndexSet {
		int xIndex;
		int yIndex;
	};

	struct Rect {
		float left;   // 右下
		float right;  // 左下
		float bottom; // 右上
		float top;    // 左上
	};

public:

	/// リセット
	void ResetMapChipData();

	/// 読み込み
	void LoadMapChipCsv(const std::string& filePath);

	/// マップチップ種類を取得
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	/// マップチップ座標を取得
	Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

	/// 座標からマップチップの番号を計算
	IndexSet GetMapChipIndexByPosition(const Vector3& position);

	/// ブロックの範囲取得
	Rect GetRectByIndex(int xIndex, int yIndex);

	uint32_t GetNumBlockVirtical() { return kNumBlockVirtical; }
	uint32_t GetNumBlockHorizontal() { return kNumBlockHorizontal; }

private:
	// 1ブロックのサイズ
	static inline const float kBlockWidth = 2.0f;
	static inline const float kBlockHeight = 2.0f;
	// ブロックの個数
	static inline const uint32_t kNumBlockVirtical = 35;
	static inline const uint32_t kNumBlockHorizontal = 21;

	MapChipData mapChipData_;
};

// void GenerateBlocks(std::vector<std::vector<WorldTransform*>>& worldTransformBlocks);