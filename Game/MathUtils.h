#pragma once
#include <cmath>
#include <stdexcept>

constexpr float PI = 3.14159265f;

// degree�� radian���� ��ȯ�ϴ� �Լ� ����
float degToRad(float degrees);

class Vector2 {
public:
    float x, y;

    // ������
    Vector2();
    Vector2(float x, float y);

    // ���� ���� ���
    float Length() const;

    // ����ȭ�� ���� ��ȯ (���̰� 0�̸� ���� �߻�)
    Vector2 Normalized() const;

    // ������ �����ε�
    Vector2 operator+(const Vector2& other) const;
    Vector2 operator-(const Vector2& other) const;
    Vector2 operator*(float scalar) const;
    Vector2 operator/(float scalar) const;
    Vector2& operator+=(const Vector2& other);
    Vector2 operator-() const;
};

// �۷ι� ���� ���� �Լ�
float Dot(const Vector2& a, const Vector2& b);
Vector2 Normalize(const Vector2& a);


class Transform {
public:
    Vector2 position;    // ��ġ
    float rotation;      // ȸ�� (�� ����)
    Vector2 scale;       // ������

    Transform();
};
