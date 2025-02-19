#pragma once
#include <cmath>

constexpr float PI = 3.14159265f;

// degree를 radian으로 변환하는 함수 선언
float degToRad(float degrees);

// 2D 벡터 구조체
struct Vector2 {
    float x, y;
};

// 2D Transform 구조체
struct Transform {
    Vector2 position;    // 위치
    float rotation;      // 회전 (도 단위)
    Vector2 scale;       // 스케일

    Transform(); // 기본 생성자 선언
};

// 벡터 연산 함수 선언
Vector2 operator+(const Vector2& a, const Vector2& b);
Vector2 operator-(const Vector2& a, const Vector2& b);
Vector2 operator*(const Vector2& a, float s);
float Dot(const Vector2& a, const Vector2& b);
Vector2 Normalize(const Vector2& a);
