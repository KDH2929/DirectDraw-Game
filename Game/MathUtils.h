#pragma once
#include <cmath>

constexpr float PI = 3.14159265f;

// degree�� radian���� ��ȯ�ϴ� �Լ� ����
float degToRad(float degrees);

// 2D ���� ����ü
struct Vector2 {
    float x, y;
};

// 2D Transform ����ü
struct Transform {
    Vector2 position;    // ��ġ
    float rotation;      // ȸ�� (�� ����)
    Vector2 scale;       // ������

    Transform(); // �⺻ ������ ����
};

// ���� ���� �Լ� ����
Vector2 operator+(const Vector2& a, const Vector2& b);
Vector2 operator-(const Vector2& a, const Vector2& b);
Vector2 operator*(const Vector2& a, float s);
float Dot(const Vector2& a, const Vector2& b);
Vector2 Normalize(const Vector2& a);
