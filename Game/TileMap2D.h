#pragma once

#include "Tile.h"


class CTGAImage;
class SpriteManager;

class TileMap2D
{
public:
	TileMap2D(int tileWidth, int tileHeight);
	~TileMap2D();

	void Render();
	void InitSpriteManager(CImageData* spriteSheet, int frameWidth, int frameHeight);
	void AddTile(Tile* tile); // ���� ��� x

	void ReadTileMap(const char* filename);
	void ReadTileSource(const char* filename);

private:
	bool ReadLayer(FILE* file, std::string layername);

private:
	int m_tileWidth;
	int m_tileHeight;

//	CTGAImage* m_pBackgroundImage;

	std::vector<Tile*> m_BackGroundLayer;
	std::vector<Tile*> m_BlockLayer;
//	std::vector<Tile*> m_EventLayer; // Ư�� Ÿ�� ���� Ÿ���� Render �Լ��� ������� �����Ѵ�.

	SpriteManager* m_SpriteManager;

	std::vector<Tile*> m_arrTile;
};