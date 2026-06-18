#pragma once

#include <chrono>

class Time {
public:
	Time();

	void update();
	
	float getDeltaTime() const { return m_DeltaTime; }

private:
	std::chrono::steady_clock::time_point m_StartTime;
	std::chrono::steady_clock::time_point m_CurrentFrameTime;
	std::chrono::steady_clock::time_point m_LastFrameTime;

	float m_DeltaTime = 0.0f;
};