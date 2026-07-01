#include "profiler_panel.h"

#include <imgui.h>
#include <numeric>

ProfilerPanel::ProfilerPanel(size_t maxSamples)
	: m_MaxSamples(maxSamples), m_FrameTimeHistory(maxSamples, 0.0f), m_PhysicsTimeHistory(maxSamples, 0.0f)
{}

void ProfilerPanel::onImGuiRender(float frameTimeMs, float physicsTimeMs, size_t entityCount) {
	if (!m_IsOpen) return;

	m_FrameTimeHistory[m_HistoryOffset] = frameTimeMs;
	m_PhysicsTimeHistory[m_HistoryOffset] = physicsTimeMs;
	m_HistoryOffset = (m_HistoryOffset + 1) % m_MaxSamples;

    if (ImGui::Begin("Engine Performance Profiler", &m_IsOpen)) {

        ImGui::Text("Simulation State:");
        ImGui::Text("  Active Entities: %zu", entityCount);
        ImGui::Separator();
        ImGui::Spacing();

        // 1. Total frame time tracking
        float avgFrameTime = std::accumulate(m_FrameTimeHistory.begin(), m_FrameTimeHistory.end(), 0.0f) / m_MaxSamples;

        ImGui::Text("Total Frame Time");
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "  Current: %.2f ms", frameTimeMs);
        ImGui::Text("  Average: %.2f ms (%.1f FPS)", avgFrameTime, 1000.0f / (avgFrameTime + 0.001f));

        // Render the rolling plot graph
        ImGui::PlotLines("##TotalFrameTimeGraph", m_FrameTimeHistory.data(), static_cast<int>(m_MaxSamples),
            static_cast<int>(m_HistoryOffset), nullptr, 0.0f, 33.33f, ImVec2(0, 80));

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // 2: Physics solver cost breakdown
        float avgPhysicsTime = std::accumulate(m_PhysicsTimeHistory.begin(), m_PhysicsTimeHistory.end(), 0.0f) / m_MaxSamples;
        float physicsLoadPercent = (avgPhysicsTime / (avgFrameTime + 0.0001f)) * 100.0f;

        ImGui::Text("Physics System Workload");
        ImGui::TextColored(ImVec4(0.2f, 0.6f, 1.0f, 1.0f), "  Current: %.2f ms", physicsTimeMs);
        ImGui::Text("  CPU Budget Share: %.1f%%", physicsLoadPercent);

        // Render a secondary overlay graph specifically for physics math tracking
        ImGui::PlotHistogram("##PhysicsTimeGraph", m_PhysicsTimeHistory.data(), static_cast<int>(m_MaxSamples),
            static_cast<int>(m_HistoryOffset), nullptr, 0.0f, 16.67f, ImVec2(0, 80));

        // Warning prompt if you are spilling past your frame budget
        if (frameTimeMs > 16.67f) {
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "WARNING: Performance dropped below 60 FPS!");
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Checkbox("Show Spatial Grid", &showDebugGrid);
    }
    ImGui::End();
}
