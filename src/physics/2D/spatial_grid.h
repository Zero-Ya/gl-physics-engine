#pragma once

#include <glm/glm.hpp>

#include <vector>
#include <algorithm>

struct GridCell {
	std::vector<size_t> entityIndices;
};

class SpatialGrid {
public:
	SpatialGrid(float screenWidth, float screenHeight, float cellSize);

	void clear();
	void insert(size_t entityIndex, const glm::vec2& position);

	void getNeighborCells(const glm::vec2& position, std::vector<size_t>& outCellIndices);

	const GridCell& getCell(size_t cellIndex) const { return m_Cells[cellIndex]; }

private:
	float m_CellSize;
	int m_Cols;
	int m_Rows;
	std::vector<GridCell> m_Cells;
};