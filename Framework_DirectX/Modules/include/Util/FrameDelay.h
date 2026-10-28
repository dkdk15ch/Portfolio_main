#pragma once

class FrameDelay {
public:
    explicit FrameDelay(int defaultWaitFrames = 1)
        : m_DefaultWaitFrames(defaultWaitFrames), m_FrameCount(0) {
    }

    void startWait() {
        m_FrameCount = m_DefaultWaitFrames;
    }

    void startWait(int waitFrames) {
        m_FrameCount = waitFrames;
    }

    void startWait1F() {
        m_FrameCount = 1;
    }

    bool update() {
        if (m_FrameCount > 0) {
            m_FrameCount--;
            return true;
        }
        return false;
    }

    bool isWaiting() const {
        return m_FrameCount > 0;
    }

    void clear() {
        m_FrameCount = 0;
    }

private:
    int m_DefaultWaitFrames;
    int m_FrameCount;
};