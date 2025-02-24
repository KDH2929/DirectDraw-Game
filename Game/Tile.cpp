#include "stdafx.h"
#include "Tile.h"
#include "../DDrawLib/DDrawDevice.h"

Tile::Tile(int id)
    : m_id(id)
{
}

Tile::Tile(Tile& tile)
{
    m_id = tile.m_id;
    m_tileLayer = tile.m_tileLayer;
}