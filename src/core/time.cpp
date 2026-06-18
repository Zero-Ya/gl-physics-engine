#include "time.h"

Time::Time() {
	m_StartTime = std::chrono::steady_clock::now();
	m_CurrentFrameTime = m_StartTime;
	m_LastFrameTime = m_StartTime;
}

void Time::update() {
	m_LastFrameTime = m_CurrentFrameTime;

	m_CurrentFrameTime = std::chrono::steady_clock::now();

	std::chrono::duration<float> frameDuration = m_CurrentFrameTime - m_LastFrameTime;

	m_DeltaTime = frameDuration.count();
}