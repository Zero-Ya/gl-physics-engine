#include "spatial_grid.h"

SpatialGrid::SpatialGrid(float screenWidth, float screenHeight, float cellSize)
	: m_CellSize(cellSize),
	  m_Cols(static_cast<int>(screenWidth / cellSize) + 1),
	  m_Rows(static_cast<int>(screenHeight / cellSize) + 1)
{
	m_Cells.resize(m_Cols * m_Rows);
}

void SpatialGrid::clear() {
	for (auto& cell : m_Cells) {
		cell.entityIndices.clear();
	}
}

void SpatialGrid::insert(size_t entityIndex, const glm::vec2& position) {
	int cellX = static_cast<int>(position.x / m_CellSize);
	int cellY = static_cast<int>(position.y / m_CellSize);

	cellX = std::max(0, std::min(cellX, m_Cols - 1));
	cellY = std::max(0, std::min(cellY, m_Rows - 1));
	
	size_t cellIndex = cellX + (cellY * m_Cols);
	m_Cells[cellIndex].entityIndices.push_back(entityIndex);
}

void SpatialGrid::getNeighborCells(const glm::vec2& position, std::vector<size_t>& outCellIndices) {
	outCellIndices.clear();

	int centerCol = static_cast<int>(position.x / m_CellSize);
	int centerRow = static_cast<int>(position.y / m_CellSize);

	for (int r = -1; r <= 1; ++r) {
		for (int c = -1; c <= 1; ++c) {
			int col = centerCol + c;
			int row = centerRow + r;

			if (col >= 0 && col < m_Cols && row >= 0 && row < m_Rows) {
				outCellIndices.push_back(col + (row * m_Cols));
			}
		}
	}
}
