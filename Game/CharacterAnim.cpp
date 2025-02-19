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

    // �ִϸ��̼��� endFrame ���� �� startFrame�� �ǵ��� ó����
    // Ÿ�̸Ӹ� 0���� ���ϰ� ���� ������ deltaTime�� ���ϴ� ����̱� ������, �ʰ��� �ð��� ����ؾ��ϱ� ����
    // while���� ���� ������ deltaTime�� �ſ� Ŀ�� �� ������ ���ӽð����� �� �� ���� Ŭ ��찡 ���� �� ���� 
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
