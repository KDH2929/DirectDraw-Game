#pragma once
#include "MathUtils.h"
#include <string>
#include <Windows.h>

// 충돌체 종류
enum class ColliderType {
    AABB,
    OBB,
};

// 충돌 반응 타입
enum class CollisionResponse {
    Block,
    Overlap,
    Ignore
};

// 전방 선언
class GameObject;


// 기본 Collider 클래스 (추상 클래스)
class Collider {
public:
    // owner: 이 Collider를 소유한 GameObject  
    // response: 기본 충돌 반응 (기본적으로 Block)
    Collider(ColliderType type, GameObject* owner, CollisionResponse response = CollisionResponse::Block)
        : m_type(type), m_owner(owner), m_response(response) {}
    virtual ~Collider() {}

    ColliderType GetType() const { return m_type; }
    GameObject* GetOwner() const { return m_owner; }
    CollisionResponse GetCollisionResponse() const { return m_response; }

    // 두 Collider 간의 충돌 여부를 검사하는 추상 함수
    virtual bool CheckCollision(const Collider* other) const = 0;
    virtual void Update(const Transform& transform) = 0;
    virtual void Render(HDC hDC, int renderX, int renderY) const = 0;

    // 디버그용 문자열 반환
    virtual std::string GetDebugString() const = 0;


    // 렌더링 위치 관련 함수
    // 렌더링 위치와 월드 좌표계 상의 위치는 게임설계에 따라 같을 수도 다를 수도 있음
    virtual void SetRenderPosition(const Vector2<float>& pos) = 0;
    virtual Vector2<float> GetRenderPosition() const = 0;


protected:
    ColliderType m_type;
    GameObject* m_owner;
    CollisionResponse m_response;

    // 렌더링 위치 
    // DirectX 스크린 좌표계상에서의 위치
    Vector2<float> m_RenderPosition;
};
