#pragma warning(disable:4996)
#include "stdafx.h"
#include "TileMap2D.h"
#include "SpriteManager.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>

TileMap2D::TileMap2D(int tileWidth, int tileHeight)
	: m_tileHeight(tileHeight), m_tileWidth(tileWidth)
{
	m_SpriteManager = nullptr;
}

TileMap2D::~TileMap2D()
{
	for (int i = 0; i < m_BackGroundLayer.size(); i++)
	{
		Tile* temp = m_BackGroundLayer[i];
		delete temp;
	}

	for (int i = 0; i < m_BlockLayer.size(); i++)
	{
		Tile* temp = m_BlockLayer[i];
		delete temp;
	}

	if (m_SpriteManager != nullptr)
	{
		delete m_SpriteManager;
	}
}

void TileMap2D::Render()
{
	
}

void TileMap2D::InitSpriteManager(CImageData* spriteSheet, int frameWidth, int frameHeight)
{
	if (m_SpriteManager != nullptr)
	{
		delete m_SpriteManager;
	}

	SpriteManager* spriteManager = new SpriteManager;
	spriteManager->SetSpriteSheet(spriteSheet, frameWidth, frameHeight);
}

void TileMap2D::AddTile(Tile* tile)
{
	m_arrTile.push_back(tile);
}

void TileMap2D::ReadTileMap(const char* filename)
{
	FILE* file = fopen(filename, "r");

	int res;

	while (1)
	{
		char lineHeader[128];

		res = fscanf(file, "%s", lineHeader);

		if (res == EOF)
			break;

		if (strcmp(lineHeader, "layers") == 0)
		{
			char layername[128];
			char skip[128];

			res = fscanf(file, "%s", skip);
			res = fscanf(file, "%s", skip);
			res = fscanf(file, "%s", layername);

			ReadLayer(file, layername);

		}
	}

	fclose(file);
}

void TileMap2D::ReadTileSource(const char* filename)
{
	FILE* file = fopen(filename, "r");

	int res;
	char imagename[128];
	char skip[128];
	int tilewidth, tileheight;

	res = fscanf(file, "%s %s", skip, imagename);
	res = fscanf(file, "%s %d", skip, &tilewidth);
	res = fscanf(file, "%s %d", skip, &tileheight);


	fclose(file);
}

bool TileMap2D::ReadLayer(FILE* file, std::string layername)
{
	int res;
	if (layername == "\"BackGround\"")
	{
		char skip[128];
		res = fscanf(file, "%s", skip);
		res = fscanf(file, "%s", skip);
		res = fscanf(file, "%s", skip);
		res = fscanf(file, "%s", skip);

		while (1)
		{
			char lineHeader[128];
			res = fscanf(file, "%s", lineHeader);

			if (strcmp(lineHeader, "cell") == 0)
			{
				char start[128], end[128];
				char xHeader[128], yHeader[128], tileHeader[128], h_filpileHeader[128], v_filpileHeader[128];
				int x, y, tile, h_filp, v_filp;
				res = fscanf(file, "%s %s %d %s %d %s %d %s %d %s %d %s", 
					start,
					xHeader, &x, yHeader, &y, tileHeader, &tile, h_filpileHeader, &h_filp, v_filpileHeader, &v_filp,
					end);

				Tile* newTile = new Tile(tile);


			}
			else
			{
				return true;
			}
		}
	}

	return false;
}
