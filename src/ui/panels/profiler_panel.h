#pragma once

#include <string>
#include <vector>

class ProfilerPanel {
public:
	ProfilerPanel(size_t maxSamples = 100);
	~ProfilerPanel() = default;

	void onImGuiRender(float frameTimeMs, float physicsTimeMs, size_t entityCount);

	bool& isOpen() { return m_IsOpen; }

private:
	bool m_IsOpen = true;
	size_t m_MaxSamples;

	std::vector<float> m_FrameTimeHistory;
	std::vector<float> m_PhysicsTimeHistory;
	size_t m_HistoryOffset = 0;
};