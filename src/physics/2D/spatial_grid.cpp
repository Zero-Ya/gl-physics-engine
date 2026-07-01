#include "spatial_grid.h"
#include "spatial_grid.h"

#include <imgui.h>
#include <string>

SpatialGrid::SpatialGrid(float totalViewWidth, float totalViewHeight, float cellSize)
	: m_CellSize(cellSize),
	  m_Cols(static_cast<int>(totalViewWidth / cellSize) + 1),
	  m_Rows(static_cast<int>(totalViewHeight / cellSize) + 1)
{
	m_InverseCellSize = 1.0f / m_CellSize;
	m_Cells.resize(m_Cols * m_Rows);
}

void SpatialGrid::clear() {
	for (auto& cell : m_Cells) {
		cell.entityIndices.clear();
	}
}

void SpatialGrid::insert(size_t entityIndex, const glm::vec2& position, float totalViewWidth, float totalViewHeight) {
	int cellX = static_cast<int>((position.x + totalViewWidth) * m_InverseCellSize);
	int cellY = static_cast<int>((position.y + totalViewHeight) * m_InverseCellSize);

	cellX = std::max(0, std::min(cellX, m_Cols - 1));
	cellY = std::max(0, std::min(cellY, m_Rows - 1));
	
	size_t cellIndex = cellX + (cellY * m_Cols);
	m_Cells[cellIndex].entityIndices.push_back(entityIndex);
}

void SpatialGrid::resize(float newViewWidth, float newViewHeight) {
	m_InverseCellSize = 1.0f / m_CellSize;

	m_Cols = static_cast<int>(newViewWidth * m_InverseCellSize) + 1;
	m_Rows = static_cast<int>(newViewHeight * m_InverseCellSize) + 1;

	clear();
	m_Cells.resize(m_Cols * m_Rows);
}

void SpatialGrid::getNeighborCells(const glm::vec2& position, std::vector<size_t>& outCellIndices, float totalViewWidth, float totalViewHeight) const {
	outCellIndices.clear();

	int cellX = static_cast<int>((position.x + totalViewWidth) * m_InverseCellSize);
	int cellY = static_cast<int>((position.y + totalViewHeight) * m_InverseCellSize);

	// Basically go through each cells (if they exist) around the center cell (including said center cell)
	for (int r = -1; r <= 1; ++r) {
		for (int c = -1; c <= 1; ++c) {
			int col = cellX + c;
			int row = cellY + r;

			if (col >= 0 && col < m_Cols && row >= 0 && row < m_Rows) {
				outCellIndices.push_back(col + (row * m_Cols));
			}
		}
	}
}

void SpatialGrid::drawDebugGrid(float dynaWidth, float dynaHeight, float viewWidth, float viewHeight) const {
	ImDrawList* drawList = ImGui::GetForegroundDrawList();
	if (!drawList) return;

	float totalViewWidth = viewWidth * 2.0f;
	float totalViewHeight = viewHeight * 2.0f;

	auto worldToScreen = [&](float worldX, float worldY) -> ImVec2 {
		float normX = (worldX + viewWidth) / totalViewWidth;
		float normY = (worldY + viewHeight) / totalViewHeight;

		normY = 1.0f - normY;
		return ImVec2(normX * dynaWidth, normY * dynaHeight);
	};

    ImU32 gridColor = IM_COL32(100, 100, 100, 70);
    ImU32 textBackingColor = IM_COL32(0, 0, 0, 200);
    ImU32 textColor = IM_COL32(0, 255, 0, 255);

    // Column lines
	for (int c = 0; c <= m_Cols; ++c) {
		float worldX = -viewWidth + (c * m_CellSize);

		ImVec2 topPoint = worldToScreen(worldX, viewHeight);
		ImVec2 bottomPoint = worldToScreen(worldX, -viewHeight);

		drawList->AddLine(topPoint, bottomPoint, gridColor, 1.0f);
	}

    // Row lines
	for (int r = 0; r <= m_Rows; ++r) {
		float worldY = -viewHeight + (r * m_CellSize);

		ImVec2 leftPoint = worldToScreen(-viewWidth, worldY);
		ImVec2 rightPoint = worldToScreen(viewHeight, worldY);

		drawList->AddLine(leftPoint, rightPoint, gridColor, 1.0f);
	}

	// Highlight and display circle counter for each cells
	for (int r = 0; r < m_Rows; ++r) {
		for (int c = 0; c < m_Cols; ++c) {
			// Get the 1D index of the current cell
			size_t cellIndex = c + (r * m_Cols);
			size_t circleCount = m_Cells[cellIndex].entityIndices.size();

			// Calculate the physical center of this cell in world coordinates
			float worldCenterX = -viewWidth + (c * m_CellSize) + (m_CellSize * 0.5f);
			float worldCenterY = -viewHeight + (r * m_CellSize) + (m_CellSize * 0.5f);

			// Convert world center to screen pixels
			ImVec2 screenCenter = worldToScreen(worldCenterX, worldCenterY);

			// Highlight cells that actually contain circles
			if (circleCount > 0) {
				ImVec2 topLeft = worldToScreen(-viewWidth + (c * m_CellSize), -viewHeight + ((r + 1) * m_CellSize));
				ImVec2 bottomRight = worldToScreen(-viewWidth + ((c + 1) * m_CellSize), -viewHeight + (r * m_CellSize));
				drawList->AddRectFilled(topLeft, bottomRight, IM_COL32(255, 0, 0, 100));
			}

			// Convert the numerical count into a string
			std::string countStr = std::to_string(circleCount);

			// Center the text layout alignment slightly based on font size estimation
			ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, countStr.c_str());
			ImVec2 textPos = ImVec2(screenCenter.x - (textSize.x * 0.5f), screenCenter.y - (textSize.y * 0.5f));

			// Draw the text string into the layout frame
			drawList->AddText(textPos, textColor, countStr.c_str());
		}
	}
}
