#pragma once
#include "MathUtils.h"
#include <string>
#include <Windows.h>

// �浹ü ����
enum class ColliderType {
    AABB,
    OBB,
};

// �浹 ���� Ÿ��
enum class CollisionResponse {
    Block,
    Overlap,
    Ignore
};


// 32��Ʈ ��Ʈ����ũ ����� �浹 ���̾ ���� Ÿ�� (����Ƽ�� Layer����)
using CollisionLayer = unsigned int;

// ��� ���̾�� �浹�ϵ��� �ϴ� �⺻ ����ũ
const CollisionLayer DEFAULT_COLLISION_LAYER_MASK = 0xFFFFFFFF;
const CollisionLayer NO_COLLISION_LAYER_MASK = 0x00000000;

// �� ���̾�
const CollisionLayer DEFAULT_COLLISION_LAYER = 1 << 0;         // (0b00...000001)
const CollisionLayer CHARACTER_COLLISION_LAYER = 1 << 1;           // (0b00...000010)
const CollisionLayer TILE_COLLISION_LAYER = 1 << 2;               // (0b00...000100)
const CollisionLayer MONSTER_COLLISION_LAYER = 1 << 3;       // (0b00...001000)



// ���� ����
class GameObject;


// �⺻ Collider Ŭ���� (�߻� Ŭ����)
class Collider {
public:
    // owner: �� Collider�� ������ GameObject  
    // response: �⺻ �浹 ���� (�⺻������ Block)
    // layer: �� Collider�� ���� ���̾� (��: 1 << 0: Default, 1 << 1: Player, 1 << 2: Enemy ��)
    // mask: �� Collider�� �浹�� ����� ���̾���� ��Ʈ����ũ
    
    Collider(ColliderType type, GameObject* owner, CollisionResponse response = CollisionResponse::Block,
        CollisionLayer layer = DEFAULT_COLLISION_LAYER, CollisionLayer mask = DEFAULT_COLLISION_LAYER_MASK)
        : m_type(type), m_owner(owner), m_response(response), m_layer(layer), m_mask(mask) {}
    virtual ~Collider() {}

    ColliderType GetType() const { return m_type; }
    GameObject* GetOwner() const { return m_owner; }

    CollisionResponse GetCollisionResponse() const { return m_response; }
    void SetCollisionResponse(CollisionResponse response) { m_response = response; };


    // �浹 ���̾� �� ����ũ ���� getter/setter
    CollisionLayer GetCollisionLayer() const { return m_layer; }
    void SetCollisionLayer(CollisionLayer layer) { m_layer = layer; }

    CollisionLayer GetCollisionMask() const { return m_mask; }
    void SetCollisionMask(CollisionLayer mask) { m_mask = mask; }


    // �� Collider ���� �浹 ���θ� �˻��ϴ� �߻� �Լ�
    virtual bool CheckCollision(const Collider* other) const = 0;
    virtual void Update(const Transform& transform) = 0;
    virtual void Render(HDC hDC, int renderX, int renderY) const = 0;

    virtual const Vector2<float> GetCenterPosition() const = 0;

    // ����׿� ���ڿ� ��ȯ
    virtual std::string GetDebugString() const = 0;


    // ������ ��ġ ���� �Լ�
    // ������ ��ġ�� ���� ��ǥ�� ���� ��ġ�� ���Ӽ��迡 ���� ���� ���� �ٸ� ���� ����
    virtual void SetRenderPosition(const Vector2<float>& pos) = 0;
    virtual Vector2<float> GetRenderPosition() const = 0;


protected:
    ColliderType m_type;
    GameObject* m_owner;
    CollisionResponse m_response;


    // �浹 ���̾�� ����ũ (��Ʈ����ŷ ���)
    CollisionLayer m_layer;
    CollisionLayer m_mask;


    // ������ ��ġ 
    // DirectX ��ũ�� ��ǥ��󿡼��� ��ġ
    Vector2<float> m_RenderPosition;
};
