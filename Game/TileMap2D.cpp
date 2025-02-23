

#include "stdafx.h"
#include "TileMap2D.h"
#include "SpriteManager.h"
#include "../Util/TGAImage.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include "../DDrawLib/DDrawDevice.h"
#include "AABBCollider.h"

#pragma warning(disable:4996)

TileMap2D::TileMap2D(int tileWidth, int tileHeight)
	: m_tileHeight(tileHeight), m_tileWidth(tileWidth)
{
	m_spriteManager = nullptr;
	m_worldposX = 0;
	m_worldposY = 0;
	m_mapWidth = -1.0;
	m_mapHeight = -1.0;
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

	if (m_spriteManager != nullptr)
	{
		delete m_spriteManager;
	}
}

void TileMap2D::Render(CDDrawDevice* pDevice)
{
	
	for (int i = 0; i < m_BackGroundLayer.size(); i++)
	{
		int currentFrame = m_BackGroundLayer[i]->m_id;
		const RECT& srcRect = m_spriteManager->GetFrameRect(currentFrame);
		Vector2 renderPos = m_BackGroundLayer[i]->GetTransform().position;
		renderPos.x = renderPos.x + m_offsetX + m_worldposX;
		renderPos.y = renderPos.y + m_offsetY + m_worldposY;

		pDevice->DrawSprite(static_cast<int>(renderPos.x), static_cast<int>(renderPos.y), m_spriteManager->GetSpriteSheet(), srcRect);
	}
	
	
	for (int i = 0; i < m_BlockLayer.size(); i++)
	{
		int currentFrame = m_BlockLayer[i]->m_id;
		const RECT& srcRect = m_spriteManager->GetFrameRect(currentFrame);
		Vector2 renderPos = m_BlockLayer[i]->GetTransform().position;
		renderPos.x = renderPos.x + m_offsetX + m_worldposX;
		renderPos.y = renderPos.y + m_offsetY + m_worldposY;

		pDevice->DrawSprite(static_cast<int>(renderPos.x), static_cast<int>(renderPos.y), m_spriteManager->GetSpriteSheet(), srcRect);
	}
	

}

void TileMap2D::InitSpriteManager(CImageData* spriteSheet, int frameWidth, int frameHeight)
{
	if (m_spriteManager)
	{
		delete m_spriteManager;
	}

	m_spriteManager = new SpriteManager;
	m_spriteManager->SetSpriteSheet(spriteSheet, frameWidth, frameHeight);
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

		if (strcmp(lineHeader, "tile_set:") == 0)
		{
			char tilesourcename[128];
			res = fscanf(file, "%s", tilesourcename);
			char openname[128] = "./data/";

			char newname[128];
			std::string temp = tilesourcename;
			temp = temp.substr(1, temp.size() - 2);
			strcpy(newname, temp.c_str());

			strcat(openname, newname);

			ReadTileSource(openname);

		}
		else if (strcmp(lineHeader, "layers") == 0)
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

	m_mapWidth = m_tileRawwidth * m_mapWidth;
	m_mapHeight = m_tileRawheight * m_mapHeight;
	m_worldposY = m_mapHeight;
}

float TileMap2D::GetTileMapWidth()
{
	return m_mapWidth;
}

float TileMap2D::GetTileMapHeight()
{
	return m_mapHeight;
}

void TileMap2D::SetOffset(float x, float y)
{
	m_offsetX = -x;
	m_offsetY = -y;
}

void TileMap2D::ReadTileSource(const char* filename)
{
	FILE* file = fopen(filename, "r");

	int res;
	char imagename[128];
	char skip[128];
	

	res = fscanf(file, "%s %s", skip, imagename);
	res = fscanf(file, "%s %d", skip, &m_tileRawwidth);
	res = fscanf(file, "%s %d", skip, &m_tileRawheight);

	char openname[128] = "./data/";

	char newname[128];
	std::string temp = imagename;
	temp = temp.substr(1, temp.size() - 2);
	strcpy(newname, temp.c_str());

	strcat(openname, newname);

	
	CTGAImage* sourceImage = new CTGAImage;
	CImageData* TileImgData = nullptr;

	if (sourceImage->Load24BitsTGA(openname, 4))
	{
		DWORD dwColorKey = sourceImage->GetPixel(0, 0);
		TileImgData = new CImageData;
		TileImgData->Create(sourceImage->GetRawImage(), sourceImage->GetWidth(), sourceImage->GetHeight(), dwColorKey);
	}

	delete sourceImage;

	InitSpriteManager(TileImgData, m_tileRawwidth, m_tileRawheight);


	fclose(file);

	
}

bool TileMap2D::ReadLayer(FILE* file, std::string layername)
{
	int res;

	
	if (layername == "\"Block\"")
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

				if (m_mapWidth < x)
				{
					m_mapWidth = x;
				}
				if (m_mapHeight < y)
				{
					m_mapHeight = y;
				}

				Tile* newTile = new Tile(tile);
				Vector2 pos;
				pos.x = x * m_tileRawwidth;
				pos.y = -y * m_tileRawheight;
				newTile->SetPosition(pos);

				Collider* tileCollider = new AABBCollider(newTile, pos.x, pos.y, m_tileRawwidth, m_tileRawheight);
				newTile->SetCollider(tileCollider);

				ColliderManager::GetInstance().AddCollider(newTile->GetCollider());

				m_BlockLayer.push_back(newTile);
			}
			else
			{
				return true;
			}
		}
	}
	else
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

				if (m_mapWidth < x)
				{
					m_mapWidth = x;
				}
				if (m_mapHeight < y)
				{
					m_mapHeight = y;
				}

				Tile* newTile = new Tile(tile);
				Vector2 pos;
				pos.x = x * m_tileRawwidth;
				pos.y = -y * m_tileRawheight;
				newTile->SetPosition(pos);

				m_BackGroundLayer.push_back(newTile);

				std::cout << "22" << '\n';
			}
			else
			{
				return true;
			}
		}
	}

	return false;
}
