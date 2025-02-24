#pragma once
#include "GameObject.h"

enum class ETileLayer
{
    BackGround,
    Collider
};

class Tile : public GameObject {
public:
    Tile(int id);
    Tile(Tile& tile);

public:

    int m_id;

    ETileLayer m_tileLayer;

};