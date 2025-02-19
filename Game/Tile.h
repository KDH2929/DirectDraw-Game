#pragma once
#include "GameObject.h"

class Tile : public GameObject {
public:
    Tile();
    virtual ~Tile();

    // Ÿ�� ��Ʈ ������ ��� Ÿ�� �̹����� ���� �����ϴ� ID
    void SetTileID(int id);
    int GetTileID() const;

    virtual void Render(class CDDrawDevice* pDevice) override;

private:
    int m_tileID;      // ��������Ʈ ��Ʈ ���� Ÿ�� �ε���
    bool m_collidable; // �浹 ����
};

