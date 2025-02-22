#pragma once
#include "GameObject.h"

// Monster Ŭ����
// ����ν� �߻� Ŭ���� ������ �ϸ�, ��� ������ ���� �������̽��� ������

enum class MonsterType {
    Unknown,
    Centipede,
};

class Monster : public GameObject {
public:
    Monster();
    virtual ~Monster();

    virtual void Update(float deltaTime) override = 0;
    virtual void Render(CDDrawDevice* pDevice) override = 0;

    virtual MonsterType GetMonsterType() const = 0;
    virtual void SetRenderPosition(const Vector2& pos) = 0;
    virtual Vector2 GetRenderPosition() const = 0;
};
