#pragma once
#include <vector>
#include <algorithm>

// Template Ŭ������ ������Ͼȿ� ���Ǹ� ��
template <typename AnimState>
class SpriteAnim {
public:
    // Sprite �ִϸ��̼ǿ����� Frame�� SpriteSheet �迭�� Index�� ����
    struct AnimSequence {
        AnimState state;   // �ִϸ��̼� ���� (enum)
        int startFrame;
        int endFrame;
        float frameDuration; // ms ����
    };

    SpriteAnim()
        : m_currentState(), m_currentFrame(0), m_timer(0.0f)
    {
    }

    ~SpriteAnim() {}

    // ��ü �ִϸ��̼� ������ ���� ����
    void SetSequences(const std::vector<AnimSequence>& sequences) {
        m_sequences = sequences;
    }

    // ���� �ִϸ��̼� ���� ���� (���� ��ȯ �� �����Ӱ� Ÿ�̸� ����)
    void SetState(AnimState state) {
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

    // deltaTime: ms ������ ���� (Ȥ�� ms�� ��ȯ�Ͽ� ���)
    void Update(float deltaTime) {
        const AnimSequence* currentSeq = nullptr;
        for (const auto& seq : m_sequences) {
            if (seq.state == m_currentState) {
                currentSeq = &seq;
                break;
            }
        }

        // �ִϸ��̼��� endFrame ���� �� startFrame���� �ǵ��ư�
        if (currentSeq != nullptr) {
            m_timer += deltaTime;
            while (m_timer >= currentSeq->frameDuration) {
                m_timer -= currentSeq->frameDuration;
                m_currentFrame++;
                if (m_currentFrame > currentSeq->endFrame)
                    m_currentFrame = currentSeq->startFrame;
            }
        }
    }

    // ���� �ִϸ��̼� ������ �ε��� ��ȯ
    int GetCurrentFrame() const { return m_currentFrame; }

    // ���� �ִϸ��̼� ���� ��ȯ
    AnimState GetState() const { return m_currentState; }

private:
    std::vector<AnimSequence> m_sequences;  // ��ü �ִϸ��̼� ������ ����
    AnimState m_currentState;                       // ���� �ִϸ��̼� ����
    int m_currentFrame;                                // ���� ������ �ε���
    float m_timer;                                         // ���� Ÿ�̸� (ms ����)
};
