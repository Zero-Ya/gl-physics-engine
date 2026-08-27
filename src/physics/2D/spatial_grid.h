#pragma once

#include <glm/glm.hpp>

#include <vector>
#include <algorithm>

struct GridCell {
	std::vector<size_t> entityIndices;
};

class SpatialGrid {
public:
	SpatialGrid(float virtualWidth, float virtualHeight, float cellSize);

	void clear();
	void insert(size_t entityIndex, const glm::vec2& position);
	void resize(float newVirtualWidth, float newVirtualHeight, float newCellSize);

	void getNeighborCells(const glm::vec2& position, std::vector<size_t>& outCellIndices) const;
	const GridCell& getCell(size_t cellIndex) const { return m_Cells[cellIndex]; }
	bool isCellOccupied(int cellX, int cellY) const;

	// Utility getters
	float getCellSize() const { return m_CellSize; }
	int getGridWidth() const { return m_GridWidth; }
	int getGridHeight() const { return m_GridHeight; }
	float getVirtualWidth() const { return m_VirtualWidth; }
	float getVirtualHeight() const { return m_VirtualHeight; }

private:
	// Maps continuous world coordinates to 2D discrete cell coordinates
	glm::ivec2 worldToCellCoords(const glm::vec2& worldPos) const;

	// Converts 2D cell coordinates (x, y) into a flat 1D vector index
	int getFlatCellIndex(int cellX, int cellY) const;

private:
	float m_VirtualWidth;
	float m_VirtualHeight;

	float m_CellSize;
	float m_InverseCellSize;
	int m_GridWidth;
	int m_GridHeight;
	std::vector<GridCell> m_Cells;
};