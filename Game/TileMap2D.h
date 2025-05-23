#pragma once
#include "Tile.h"
#include "string"

class CTGAImage;
class SpriteManager;

class TileMap2D
{
public:
	TileMap2D(int tileWidth, int tileHeight);
	~TileMap2D();

	void Update(float deltaTime);
	void Render(CDDrawDevice* pDevice);
	void AddTile(Tile* tile); // 아직 사용 x
	void ReadTileMap(const char* filename);
	float GetTileMapWidth();
	float GetTileMapHeight();
	void SetCameraOffset(float x, float y);

	const std::vector<Tile*>& GetBlockLayer() const;

private:
	void ReadTileSource(const char* filename);
	void InitSpriteManager(CImageData* spriteSheet, int frameWidth, int frameHeight);
	bool ReadLayer(FILE* file, std::string layername);

private:
	float m_cameraOffsetX, m_cameraOffsetY;			// 카메라 오프셋
	float m_worldposX, m_worldposY;
	int m_mapWidth, m_mapHeight;
	float m_tileWidth;
	float m_tileHeight;

	int m_tileRawwidth, m_tileRawheight;

	//	CTGAImage* m_pBackgroundImage;

	std::vector<Tile*> m_BackGroundLayer;
	std::vector<Tile*> m_BlockLayer;
	//	std::vector<Tile*> m_EventLayer; // 특수 타일 각각 타일의 Render 함수를 실행시켜 렌더한다.
	SpriteManager* m_spriteManager;



	std::vector<Tile*> m_arrTile;

	std::string m_tileMapPath;
};