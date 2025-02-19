#include "stdafx.h"
#include "CharacterAnim.h"

CharacterAnim::CharacterAnim()
    : m_currentState(AnimState::Idle),
    m_currentFrame(0),
    m_timer(0.0f)
{
}

CharacterAnim::~CharacterAnim()
{
}

void CharacterAnim::SetSequences(const std::vector<AnimSequence>& sequences) {
    m_sequences = sequences;
}

void CharacterAnim::SetState(AnimState state) {
    if (m_currentState != state) {
        m_currentState = state;

        for (const auto& seq : m_sequences) {
            if (seq.state == state) {
                m_currentFrame = seq.startFrame;
                m_timer = 0.0f;
                break;
            }
        }
    }
}

void CharacterAnim::Update(float deltaTime) {
    const AnimSequence* currentSeq = nullptr;
    for (const auto& seq : m_sequences) {
        if (seq.state == m_currentState) {
            currentSeq = &seq;
            break;
        }
    }

    // 애니메이션은 endFrame 도달 시 startFrame이 되도록 처리함
    // 타이머를 0으로 안하고 빼는 이유는 deltaTime을 더하는 방식이기 때문에, 초과된 시간도 고려해야하기 때문
    // while문을 쓰는 이유는 deltaTime이 매우 커서 한 프레임 지속시간보다 몇 배 정도 클 경우가 생길 수 있음 
    if (currentSeq != nullptr) {
        m_timer += deltaTime;
        while (m_timer >= currentSeq->frameDuration) {
            m_timer -= currentSeq->frameDuration;
            m_currentFrame++;
            if (m_currentFrame > currentSeq->endFrame) {
                m_currentFrame = currentSeq->startFrame;
            }
        }
    }
}

int CharacterAnim::GetCurrentFrame() const {
    return m_currentFrame;
}

CharacterAnim::AnimState CharacterAnim::GetState() const {
    return m_currentState;
}
