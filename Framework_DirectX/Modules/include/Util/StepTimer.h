//
// StepTimer.h - A simple timer that provides elapsed time information
//

#pragma once

#include <cmath>
#include <cstdint>
#include <exception>


namespace DX
{
    // Helper class for animation and simulation timing.
    class StepTimer
    {
    public:
        StepTimer() noexcept(false) :
            m_ElapsedTicks(0),
            m_TotalTicks(0),
            m_LeftOverTicks(0),
            m_FrameCount(0),
            m_FramesPerSecond(0),
            m_FramesThisSecond(0),
            m_QpcSecondCounter(0),
            m_IsFixedTimeStep(false),
            m_TargetElapsedTicks(m_TicksPerSecond / 60)
        {
            if (!QueryPerformanceFrequency(&m_QpcFrequency))
            {
                throw std::exception();
            }

            if (!QueryPerformanceCounter(&m_QpcLastTime))
            {
                throw std::exception();
            }

            // Initialize max delta to 1/10 of a second.
            m_QpcMaxDelta = static_cast<uint64_t>(m_QpcFrequency.QuadPart / 10);
        }

        // Get elapsed time since the previous Update call.
        uint64_t GetElapsedTicks() const noexcept { return m_ElapsedTicks; }
        double GetElapsedSeconds() const noexcept { return TicksToSeconds(m_ElapsedTicks); }

        // Get total time since the start of the program.
        uint64_t GetTotalTicks() const noexcept { return m_TotalTicks; }
        double GetTotalSeconds() const noexcept { return TicksToSeconds(m_TotalTicks); }

        // Get total number of updates since start of the program.
        uint32_t GetFrameCount() const noexcept { return m_FrameCount; }

        // Get the current framerate.
        uint32_t GetFramesPerSecond() const noexcept { return m_FramesPerSecond; }

        // Set whether to use fixed or variable timestep mode.
        void SetFixedTimeStep(bool isFixedTimestep) noexcept { m_IsFixedTimeStep = isFixedTimestep; }

        // Set how often to call Update when in fixed timestep mode.
        void SetTargetElapsedTicks(uint64_t targetElapsed) noexcept { m_TargetElapsedTicks = targetElapsed; }
        void SetTargetElapsedSeconds(double targetElapsed) noexcept { m_TargetElapsedTicks = SecondsToTicks(targetElapsed); }

        // Integer format represents time using 10,000,000 ticks per second.
        static constexpr uint64_t m_TicksPerSecond = 10000000;

        static constexpr double TicksToSeconds(uint64_t ticks) noexcept { return static_cast<double>(ticks) / m_TicksPerSecond; }
        static constexpr uint64_t SecondsToTicks(double seconds) noexcept { return static_cast<uint64_t>(seconds * m_TicksPerSecond); }

        // After an intentional timing discontinuity (for instance a blocking IO operation)
        // call this to avoid having the fixed timestep logic attempt a set of catch-up
        // Update calls.

        void resetElapsedTime()
        {
            if (!QueryPerformanceCounter(&m_QpcLastTime))
            {
                throw std::exception();
            }

            m_LeftOverTicks = 0;
            m_FramesPerSecond = 0;
            m_FramesThisSecond = 0;
            m_QpcSecondCounter = 0;
        }

        // Update timer state, calling the specified Update function the appropriate number of times.
        template<typename TUpdate>
        void tick(const TUpdate& update)
        {
            // Query the current time.
            LARGE_INTEGER currentTime;

            if (!QueryPerformanceCounter(&currentTime))
            {
                throw std::exception();
            }

            uint64_t timeDelta = static_cast<uint64_t>(currentTime.QuadPart - m_QpcLastTime.QuadPart);

            m_QpcLastTime = currentTime;
            m_QpcSecondCounter += timeDelta;

            // Clamp excessively large time deltas (e.g. after paused in the debugger).
            if (timeDelta > m_QpcMaxDelta)
            {
                timeDelta = m_QpcMaxDelta;
            }

            // Convert QPC units into a canonical tick format. This cannot overflow due to the previous clamp.
            timeDelta *= m_TicksPerSecond;
            timeDelta /= static_cast<uint64_t>(m_QpcFrequency.QuadPart);

            const uint32_t lastFrameCount = m_FrameCount;

            if (m_IsFixedTimeStep)
            {
                // Fixed timestep update logic

                // If the app is running very close to the target elapsed time (within 1/4 of a millisecond) just clamp
                // the clock to exactly match the target value. This prevents tiny and irrelevant errors
                // from accumulating over time. Without this clamping, a game that requested a 60 fps
                // fixed update, running with vsync enabled on a 59.94 NTSC display, would eventually
                // accumulate enough tiny errors that it would drop a frame. It is better to just round
                // small deviations down to zero to leave things running smoothly.

                if (static_cast<uint64_t>(std::abs(static_cast<int64_t>(timeDelta - m_TargetElapsedTicks))) < m_TicksPerSecond / 4000)
                {
                    timeDelta = m_TargetElapsedTicks;
                }

                m_LeftOverTicks += timeDelta;

                while (m_LeftOverTicks >= m_TargetElapsedTicks)
                {
                    m_ElapsedTicks = m_TargetElapsedTicks;
                    m_TotalTicks += m_TargetElapsedTicks;
                    m_LeftOverTicks -= m_TargetElapsedTicks;
                    m_FrameCount++;

                    update();
                }
            }
            else
            {
                // Variable timestep update logic.
                m_ElapsedTicks = timeDelta;
                m_TotalTicks += timeDelta;
                m_LeftOverTicks = 0;
                m_FrameCount++;

                update();
            }

            // Track the current framerate.
            if (m_FrameCount != lastFrameCount)
            {
                m_FramesThisSecond++;
            }

            if (m_QpcSecondCounter >= static_cast<uint64_t>(m_QpcFrequency.QuadPart))
            {
                m_FramesPerSecond = m_FramesThisSecond;
                m_FramesThisSecond = 0;
                m_QpcSecondCounter %= static_cast<uint64_t>(m_QpcFrequency.QuadPart);
            }
        }

    private:
        // Source timing data uses QPC units.
        LARGE_INTEGER m_QpcFrequency;
        LARGE_INTEGER m_QpcLastTime;
        uint64_t m_QpcMaxDelta;

        // Derived timing data uses a canonical tick format.
        uint64_t m_ElapsedTicks;
        uint64_t m_TotalTicks;
        uint64_t m_LeftOverTicks;

        // Members for tracking the framerate.
        uint32_t m_FrameCount;
        uint32_t m_FramesPerSecond;
        uint32_t m_FramesThisSecond;
        uint64_t m_QpcSecondCounter;

        // Members for configuring fixed timestep mode.
        bool m_IsFixedTimeStep;
        uint64_t m_TargetElapsedTicks;
    };
}
