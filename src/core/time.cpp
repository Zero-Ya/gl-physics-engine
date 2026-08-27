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

//float currentFrameTime = static_cast<float>(glfwGetTime());
//float dt = currentFrameTime - lastFrameTime;
//lastFrameTime = currentFrameTime;

//Time::TimePoint frameStart = m_Timer->getCurrentTimePoint();
//m_Timer->update();
//float dt = m_Timer->getDeltaTime();
//// To prevent massive physics jumps during lag spikes
//if (dt > 0.1f) dt = 0.1f;
//
//// render/update
//
//lastTotalFrameTimeMs = m_Timer->getElapsedTimeMs(frameStart);