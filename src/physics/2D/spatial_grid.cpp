#include "spatial_grid.h"

#include <imgui.h>
#include <string>

SpatialGrid::SpatialGrid(float virtualWidth, float virtualHeight, float cellSize)
	: m_VirtualWidth(virtualWidth), m_VirtualHeight(virtualHeight), m_CellSize(cellSize)
{
	m_InverseCellSize = 1.0f / m_CellSize;

	m_GridWidth = std::max(1, static_cast<int>(virtualWidth * m_InverseCellSize)); // Columns
	m_GridHeight = std::max(1, static_cast<int>(virtualHeight * m_InverseCellSize)); // Rows

	m_Cells.resize(m_GridWidth * m_GridHeight);
}

void SpatialGrid::clear() {
	for (auto& cell : m_Cells) {
		cell.entityIndices.clear();
	}
}

void SpatialGrid::insert(size_t entityId, const glm::vec2& position) {
	glm::ivec2 cell = worldToCellCoords(position);
	int flatIndex = getFlatCellIndex(cell.x, cell.y);

	if (flatIndex >= 0 && flatIndex < static_cast<int>(m_Cells.size())) {
		m_Cells[flatIndex].entityIndices.push_back(entityId);
	}
}

void SpatialGrid::resize(float newVirtualWidth, float newVirtualHeight, float newCellSize) {
	m_VirtualWidth = newVirtualWidth;
	m_VirtualHeight = newVirtualHeight;

	m_CellSize = newCellSize;
	m_InverseCellSize = 1.0f / m_CellSize;

	m_GridWidth = static_cast<int>(newVirtualWidth * m_InverseCellSize) + 1;
	m_GridHeight = static_cast<int>(newVirtualHeight * m_InverseCellSize) + 1;

	// Force an exact number of columns, then recalculate cellSize to fit height perfectly
	//m_GridWidth = std::max(1, static_cast<int>(std::round(newVirtualWidth / newCellSize)));
	//m_CellSize = newVirtualWidth / m_GridWidth; // Cell size dynamically adjusts
	//m_InverseCellSize = 1.0f / m_CellSize;
	//m_GridHeight = std::max(1, static_cast<int>(std::ceil(newVirtualHeight * m_InverseCellSize)));

	m_Cells.clear();
	m_Cells.resize(m_GridWidth * m_GridHeight);
}

void SpatialGrid::getNeighborCells(const glm::vec2& position, std::vector<size_t>& outCellIndices) const {
	outCellIndices.clear();

	glm::ivec2 cell = worldToCellCoords(position);

	// Basically go through each cells (if they exist) around the center cell (including said center cell)
	for (int r = -1; r <= 1; ++r) {
		for (int c = -1; c <= 1; ++c) {
			int col = cell.x + c;
			int row = cell.y + r;

			if (col >= 0 && col < m_GridWidth && row >= 0 && row < m_GridHeight) {
				outCellIndices.push_back(col + (row * m_GridWidth));
			}
		}
	}
}

glm::ivec2 SpatialGrid::worldToCellCoords(const glm::vec2& worldPos) const {
	int cellX = static_cast<int>(std::floor(worldPos.x * m_InverseCellSize));
	int cellY = static_cast<int>(std::floor(worldPos.y * m_InverseCellSize));

	// Clamp coordinates so particles slightly out of bounds don't crash array lookup
	cellX = std::clamp(cellX, 0, std::max(0, m_GridWidth - 1));
	cellY = std::clamp(cellY, 0, std::max(0, m_GridHeight - 1));

	return glm::ivec2(cellX, cellY);
}

int SpatialGrid::getFlatCellIndex(int cellX, int cellY) const {
	if (cellX < 0 || cellX >= m_GridWidth || cellY < 0 || cellY >= m_GridHeight) {
		return -1; // Invalid index
	}
	return cellX + (cellY * m_GridWidth);
}

bool SpatialGrid::isCellOccupied(int cellX, int cellY) const {
	int flatIndex = getFlatCellIndex(cellX, cellY);
	if (flatIndex < 0 || flatIndex >= static_cast<int>(m_Cells.size())) {
		return false;
	}
	return !m_Cells[flatIndex].entityIndices.empty();
}