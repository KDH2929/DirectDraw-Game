#pragma once
#include "GameObject.h"

class Tile : public GameObject {
public:
    Tile();
    virtual ~Tile();

    // 타일 시트 내에서 어느 타일 이미지를 쓸지 지정하는 ID
    void SetTileID(int id);
    int GetTileID() const;

    virtual void Render(class CDDrawDevice* pDevice) override;

private:
    int m_tileID;      // 스프라이트 시트 상의 타일 인덱스
    bool m_collidable; // 충돌 여부
};

