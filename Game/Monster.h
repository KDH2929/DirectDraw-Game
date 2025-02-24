#pragma once
#include "GameObject.h"

// Monster 클래스
// 현재로썬 추상 클래스 역할을 하며, 모든 몬스터의 공통 인터페이스를 정의함

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

};
