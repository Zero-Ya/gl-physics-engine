#pragma once

#include <chrono>

class Time {
public:
	using TimePoint = std::chrono::steady_clock::time_point;

	Time();

	void update();
	float getDeltaTime() const { return m_DeltaTime; }

	TimePoint getCurrentTimePoint() const { return std::chrono::steady_clock::now(); }

	float getElapsedTimeMs(const TimePoint& start) const {
		return std::chrono::duration<float, std::milli>(std::chrono::steady_clock::now() - start).count();
	}

private:
	TimePoint m_StartTime;
	TimePoint m_CurrentFrameTime;
	TimePoint m_LastFrameTime;

	float m_DeltaTime = 0.0f;
};