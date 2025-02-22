#pragma once
#include <cmath>
#include <stdexcept>

constexpr float PI = 3.14159265f;

// degree를 radian으로 변환하는 함수 선언
float degToRad(float degrees);

class Vector2 {
public:
    float x, y;

    // 생성자
    Vector2();
    Vector2(float x, float y);

    // 벡터 길이 계산
    float Length() const;

    // 정규화된 벡터 반환 (길이가 0이면 예외 발생)
    Vector2 Normalized() const;

    // 연산자 오버로딩
    Vector2 operator+(const Vector2& other) const;
    Vector2 operator-(const Vector2& other) const;
    Vector2 operator*(float scalar) const;
    Vector2 operator/(float scalar) const;
    Vector2& operator+=(const Vector2& other);
    Vector2 operator-() const;
};

// 글로벌 벡터 연산 함수
float Dot(const Vector2& a, const Vector2& b);
Vector2 Normalize(const Vector2& a);


class Transform {
public:
    Vector2 position;    // 위치
    float rotation;      // 회전 (도 단위)
    Vector2 scale;       // 스케일

    Transform();
};
