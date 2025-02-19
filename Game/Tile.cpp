#include "stdafx.h"
#include "Tile.h"
#include "../DDrawLib/DDrawDevice.h"

Tile::Tile()
    : m_tileID(0), m_collidable(false)
{
}

Tile::~Tile()
{
}

void Tile::SetTileID(int id)
{
    m_tileID = id;
}

int Tile::GetTileID() const
{
    return m_tileID;
}


void Tile::Render(CDDrawDevice* pDevice)
{

}
