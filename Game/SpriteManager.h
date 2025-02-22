#pragma once
#include <vector>
#include <Windows.h>
#include "../ImageData/ImageData.h"

struct SpriteFrame {
    RECT srcRect;           // 스프라이트 시트 내에서 해당 프레임의 영역
};

class SpriteManager {
public:
    SpriteManager();
    SpriteManager(SpriteManager& spriteManager);
    ~SpriteManager();

    bool SetSpriteSheet(CImageData* spriteSheet, int frameWidth, int frameHeight);
    const CImageData* GetSpriteSheet() const;


    // 전체 프레임 수 반환
    int GetFrameCount() const;

    // 지정한 인덱스의 프레임 소스 사각형 반환
    const RECT& GetFrameRect(int index) const;

    int GetFrameWidth() const;
    int GetFrameHeight() const;

    CImageData* CopyImageData();
    std::vector<RECT> GetFrames();
private:
    void GenerateFrames();

    CImageData* m_spriteSheet;      // 스프라이트 시트 이미지 데이터
    int m_frameWidth;               // 각 프레임의 너비
    int m_frameHeight;              // 각 프레임의 높이
    std::vector<RECT> m_frames;     // 계산된 프레임의 소스 사각형 목록
};
