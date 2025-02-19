#pragma once
#include <vector>
#include "MathUtils.h" // Vector2 등 사용
#include "Tile.h"

class CImageData;
class CDDrawDevice;
class SpriteManager;

class TileMap {
public:
    // rows: 행의 개수, cols: 열의 개수, tileWidth/Height: 각 타일의 크기 (픽셀 단위)
    TileMap(int rows, int cols, int tileWidth, int tileHeight);
    ~TileMap();

    // 외부 데이터(예: 2차원 int 배열)를 이용해 타일 ID들을 설정
    void LoadMap(const std::vector<std::vector<int>>& mapData);

    // 스프라이트 시트 및 프레임 정보를 가진 SpriteManager를 설정
    void SetSpriteManager(SpriteManager* spriteManager);

    // 전체 타일맵 렌더링
    void Render(CDDrawDevice* pDevice);

    // 특정 타일 접근
    Tile* GetTile(int row, int col);

private:
    int m_rows;
    int m_cols;
    int m_tileWidth;
    int m_tileHeight;
    std::vector<std::vector<Tile*>> m_tiles;
    SpriteManager* m_spriteManager;
};

