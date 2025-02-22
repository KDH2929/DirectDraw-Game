#pragma once
#include "GameObject.h"

enum class ETileLayer
{
    BackGround,
    Collider
};

class Tile : public GameObject {
public:
    Tile(int id = 0);
    Tile(Tile& tile);

    int GetTileID();
    void SetTileID(int tile);

public:
//    int m_spriteID;
//    int m_tileID;
    int m_id;
    
    ETileLayer m_tileLayer;
 //   bool b_IsAnim;
 //   int m_spriteIDEnd;
};