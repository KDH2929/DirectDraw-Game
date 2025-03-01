#pragma once
#include "MathUtils.h"
#include "CollisionLayer.h"
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
    // layer: 이 Collider가 속한 레이어 (예: 1 << 0: Default, 1 << 1: Player, 1 << 2: Enemy 등)
    // mask: 이 Collider가 충돌을 허용할 레이어들의 비트마스크
    
    Collider(ColliderType type, GameObject* owner, CollisionResponse response = CollisionResponse::Block,
        CollisionLayer layer = DEFAULT_COLLISION_LAYER, CollisionLayer mask = DEFAULT_COLLISION_LAYER_MASK)
        : m_type(type), m_owner(owner), m_response(response), m_layer(layer), m_mask(mask) {}
    virtual ~Collider() {}

    ColliderType GetType() const { return m_type; }
    GameObject* GetOwner() const { return m_owner; }

    CollisionResponse GetCollisionResponse() const { return m_response; }
    void SetCollisionResponse(CollisionResponse response) { m_response = response; };


    // 충돌 레이어 및 마스크 관련 getter/setter
    CollisionLayer GetCollisionLayer() const { return m_layer; }
    void SetCollisionLayer(CollisionLayer layer) { m_layer = layer; }

    CollisionLayer GetCollisionMask() const { return m_mask; }
    void SetCollisionMask(CollisionLayer mask) { m_mask = mask; }


    // 두 Collider 간의 충돌 여부를 검사하는 추상 함수
    virtual bool CheckCollision(const Collider* other) const = 0;
    virtual void Update(const Transform& transform) = 0;
    virtual void Render(HDC hDC, int renderX, int renderY) const = 0;

    virtual const Vector2<float> GetCenterPosition() const = 0;

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


    // 충돌 레이어와 마스크 (비트마스킹 방식)
    CollisionLayer m_layer;
    CollisionLayer m_mask;


    // 렌더링 위치 
    // DirectX 스크린 좌표계상에서의 위치
    Vector2<float> m_RenderPosition;
};
