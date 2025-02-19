#pragma once
#include <vector>
#include "MathUtils.h" // Vector2 �� ���
#include "Tile.h"

class CImageData;
class CDDrawDevice;
class SpriteManager;

class TileMap {
public:
    // rows: ���� ����, cols: ���� ����, tileWidth/Height: �� Ÿ���� ũ�� (�ȼ� ����)
    TileMap(int rows, int cols, int tileWidth, int tileHeight);
    ~TileMap();

    // �ܺ� ������(��: 2���� int �迭)�� �̿��� Ÿ�� ID���� ����
    void LoadMap(const std::vector<std::vector<int>>& mapData);

    // ��������Ʈ ��Ʈ �� ������ ������ ���� SpriteManager�� ����
    void SetSpriteManager(SpriteManager* spriteManager);

    // ��ü Ÿ�ϸ� ������
    void Render(CDDrawDevice* pDevice);

    // Ư�� Ÿ�� ����
    Tile* GetTile(int row, int col);

private:
    int m_rows;
    int m_cols;
    int m_tileWidth;
    int m_tileHeight;
    std::vector<std::vector<Tile*>> m_tiles;
    SpriteManager* m_spriteManager;
};

