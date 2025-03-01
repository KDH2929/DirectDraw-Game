#include "stdafx.h"
#include "SpriteManager.h"
#include "DebugManager.h"

SpriteManager::SpriteManager()
    : m_spriteSheet(nullptr), m_frameWidth(0), m_frameHeight(0)
{
}

SpriteManager::~SpriteManager()
{
    /*
    여기서 할당해제하고 nullptr로 바꾸면 안됨
    if (m_spriteSheet) {
        delete m_spriteSheet;
        m_spriteSheet = nullptr;
    }
    */

}

bool SpriteManager::SetSpriteSheet(CImageData* spriteSheet, int frameWidth, int frameHeight)
{
    if (!spriteSheet || frameWidth <= 0 || frameHeight <= 0)
        return false;

    if (m_spriteSheet) {
        delete m_spriteSheet;
        m_spriteSheet = nullptr;
    }

    m_spriteSheet = spriteSheet;        // 단순 참조만 유지할 것

    m_frameWidth = frameWidth;
    m_frameHeight = frameHeight;

    // 프레임 정보 생성
    GenerateFrames();

    return true;
}

void SpriteManager::GenerateFrames()
{
    m_frames.clear();
    if (!m_spriteSheet)
        return;

    int sheetWidth = static_cast<int>(m_spriteSheet->GetWidth());
    int sheetHeight = static_cast<int>(m_spriteSheet->GetHeight());
    int cols = sheetWidth / m_frameWidth;
    int rows = sheetHeight / m_frameHeight;

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            RECT rect;
            rect.left = col * m_frameWidth;
            rect.top = row * m_frameHeight;
            rect.right = rect.left + m_frameWidth;
            rect.bottom = rect.top + m_frameHeight;
            m_frames.push_back(rect);
        }
    }
}

int SpriteManager::GetFrameCount() const
{
    return static_cast<int>(m_frames.size());
}

const RECT& SpriteManager::GetFrameRect(int index) const
{
    return m_frames[index];
}

const CImageData* SpriteManager::GetSpriteSheet() const
{
    return m_spriteSheet;
}

int SpriteManager::GetFrameWidth() const {
    return m_frameWidth;
}

int SpriteManager::GetFrameHeight() const {
    return m_frameHeight;
}