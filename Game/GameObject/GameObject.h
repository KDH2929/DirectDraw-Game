#pragma once
#include "Collider.h"
#include "ColliderManager.h"
#include "MathUtils.h"
#include <string>


enum class PhysicsType {
    Static,         // 움직이지 않음 (ex: 벽, 바닥)
    Dynamic,    // 움직임
};


struct DamageInfo {
    float damageAmount;
    GameObject* instigator; 
    Vector2<float> hitLocation;

    DamageInfo(float dmg, GameObject* inst, const Vector2<float>& hitLoc)
        : damageAmount(dmg), instigator(inst), hitLocation(hitLoc)
    {
    }
};



class CDDrawDevice;


class GameObject {
public:
    GameObject();
    virtual ~GameObject();

    // 매 프레임 업데이트 (deltaTime: 밀리초 단위)
    virtual void Update(float deltaTime);
    // 매 프레임 렌더링
    virtual void Render(CDDrawDevice* pDevice);     // 만약 winAPI 렌더기능을 쓰고 싶으면 HDC를 매개변수로 추가

    // Transform 접근자/설정자
    Transform& GetTransform();
    Vector2<float> GetPosition() const;
    void SetTransform(const Transform& newTransform);

    // Collider 관련
    Collider* GetCollider();
    void SetCollider(Collider* collider);
    void SetColliderLocalPosition(const Vector2<float>& pos);
    const Vector2<float>& GetColliderLocalPosition() const;

    // 충돌 이벤트 콜백 (충돌 발생 시 호출)
    // other: 충돌한 다른 GameObject, response: 충돌 반응 타입
    virtual void OnCollision(const CollisionInfo& collisionInfo);
    virtual void TakeDamage(const DamageInfo& damageInfo) {};


    // 보간 관련 함수들
    virtual void SetPosition(const Vector2<float>& newPos);
    void ResetInterpolation();
    void UpdateInterpolation(float alpha);
    Vector2<float> GetInterpolatedPosition() const;


    // 렌더링 위치 관련 함수
    // 렌더링 위치와 월드 좌표계 상의 위치는 게임설계에 따라 같을 수도 다를 수도 있음
    void SetRenderPosition(const Vector2<float>& pos);
    Vector2<float> GetRenderPosition() const;
    

    void SetPhysicsType(PhysicsType type);
    PhysicsType GetPhysicsType() const;


protected:
    Transform m_Transform;
    Collider* m_pCollider;

    Vector2<float> m_ColliderLocalPosition;


    // 보간을 위한 위치 변수
    Vector2<float> m_CurrentPosition;
    Vector2<float> m_PreviousPosition;
    Vector2<float> m_InterpolatedPosition;


    PhysicsType m_physicsType;


    // 렌더링 위치 
    // DirectX 스크린 좌표계상에서의 위치
    Vector2<float> m_RenderPosition;

};
