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
	void insert(size_t entityIndex, const glm::vec2& position, float orthoWidth, float orthoHeight);
	void resize(float newScreenWidth, float newScreenHeight);

	void getNeighborCells(const glm::vec2& position, std::vector<size_t>& outCellIndices, float orthoWidth, float orthoHeight) const;
	const GridCell& getCell(size_t cellIndex) const { return m_Cells[cellIndex]; }

	void drawDebugGrid(float dynaWidth, float dynaHeight, float orthoWidth, float orthoHeight) const;

private:
	float m_CellSize;
	float m_InverseCellSize; // Apparently this is faster usage
	int m_Cols;
	int m_Rows;
	std::vector<GridCell> m_Cells;
};