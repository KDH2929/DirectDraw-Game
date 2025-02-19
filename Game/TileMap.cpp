#include "stdafx.h"
#include "TileMap.h"
#include "Tile.h"
#include "SpriteManager.h"
#include "../DDrawLib/DDrawDevice.h"
#include "../ImageData/ImageData.h"

TileMap::TileMap(int rows, int cols, int tileWidth, int tileHeight)
    : m_rows(rows), m_cols(cols), m_tileWidth(tileWidth), m_tileHeight(tileHeight), m_spriteManager(nullptr)
{
    // 2차원 벡터를 행, 열에 맞게 초기화
    m_tiles.resize(m_rows);
    for (int i = 0; i < m_rows; i++) {
        m_tiles[i].resize(m_cols, nullptr);
        for (int j = 0; j < m_cols; j++) {
            m_tiles[i][j] = new Tile();
            // 각 타일의 위치를 (열 * tileWidth, 행 * tileHeight)로 설정
            m_tiles[i][j]->SetPosition(Vector2{ static_cast<float>(j * m_tileWidth), static_cast<float>(i * m_tileHeight) });
        }
    }
}

TileMap::~TileMap()
{
    for (int i = 0; i < m_rows; i++) {
        for (int j = 0; j < m_cols; j++) {
            if (m_tiles[i][j]) {
                delete m_tiles[i][j];
                m_tiles[i][j] = nullptr;
            }
        }
    }
}

void TileMap::LoadMap(const std::vector<std::vector<int>>& mapData)
{
    // mapData 크기가 m_rows x m_cols라고 가정하고, 각 타일의 ID를 설정
    for (int i = 0; i < m_rows && i < static_cast<int>(mapData.size()); i++) {
        for (int j = 0; j < m_cols && j < static_cast<int>(mapData[i].size()); j++) {
            if (m_tiles[i][j])
                m_tiles[i][j]->SetTileID(mapData[i][j]);
        }
    }
}

void TileMap::SetSpriteManager(SpriteManager* spriteManager)
{
    m_spriteManager = spriteManager;
}

Tile* TileMap::GetTile(int row, int col)
{
    if (row < 0 || row >= m_rows || col < 0 || col >= m_cols)
        return nullptr;
    return m_tiles[row][col];
}

void TileMap::Render(CDDrawDevice* pDevice)
{
    if (!pDevice || !m_spriteManager)
        return;

    // 모든 타일에 대해 렌더링
    for (int i = 0; i < m_rows; i++) {
        for (int j = 0; j < m_cols; j++) {
            Tile* tile = m_tiles[i][j];
            if (tile) {
                // 타일 ID를 이용해 SpriteManager에서 소스 RECT 가져오기
                int tileID = tile->GetTileID();
                const RECT& srcRect = m_spriteManager->GetFrameRect(tileID);

                // GameObject에 구현된 GetPosition() 사용
                Vector2 pos = tile->GetPosition();
                pDevice->DrawSprite(static_cast<int>(pos.x), static_cast<int>(pos.y), m_spriteManager->GetSpriteSheet(), srcRect);
            }
        }
    }
}
