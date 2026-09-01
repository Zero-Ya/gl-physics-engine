#pragma once

#include <glm/glm.hpp>
#include "spatial_grid.h"
#include "rendering/2D/line_renderer2d.h"

class SpatialGridDebugRenderer {
public:
    // Renders spatial grid outlines and highlights occupied cell buckets in virtual world space
    static void draw(SpatialGrid& grid, LineRenderer& lineRenderer,
        bool highlightOccupied = true)
    {
        float cellSize = grid.getCellSize();
        int gridWidth = grid.getGridWidth();
        int gridHeight = grid.getGridHeight();
        float virtualWidth = grid.getVirtualWidth();
        float virtualHeight = grid.getVirtualHeight();

        // Visual Styling (Virtual Space)
        const glm::vec4 gridColor(1.0f, 1.0f, 1.0f, 0.15f);      // Faint white grid
        const glm::vec4 occupiedColor(0.2f, 0.8f, 1.0f, 0.5f);   // Cyan for active cells

        // 1. Draw Vertical Grid Lines
        for (int i = 0; i <= gridWidth; ++i) {
            // Clamp the final line exactly to virtualWidth to flush with the right screen edge
            float x = std::min(i * cellSize, virtualWidth);
            lineRenderer.drawLine(glm::vec2(x, 0.0f), glm::vec2(x, virtualHeight), gridColor);
        }

        // 2. Draw Horizontal Grid Lines
        for (int j = 0; j <= gridHeight; ++j) {
            // Clamp the final line exactly to virtualHeight to flush with the top screen edge
            float y = std::min(j * cellSize, virtualHeight);
            lineRenderer.drawLine(glm::vec2(0.0f, y), glm::vec2(virtualWidth, y), gridColor);
        }

        // 3. Highlight Occupied Cells
        if (!highlightOccupied) return;

        for (int cellX = 0; cellX < gridWidth; ++cellX) {
            for (int cellY = 0; cellY < gridHeight; ++cellY) {

                // Query cell occupancy
                if (grid.isCellOccupied(cellX, cellY)) {
                    // Calculate cell bounds and clamp them strictly to virtual space!
                    float minX = cellX * cellSize;
                    float minY = cellY * cellSize;

                    // Clamping ensures the top/right partial cells don't extend past the screen
                    float maxX = std::min(minX + cellSize, virtualWidth);
                    float maxY = std::min(minY + cellSize, virtualHeight);

                    // Draw a cross-hatch inside the occupied cell
                    lineRenderer.drawLine(glm::vec2(minX, minY), glm::vec2(maxX, maxY), occupiedColor);
                    lineRenderer.drawLine(glm::vec2(minX, maxY), glm::vec2(maxX, minY), occupiedColor);

                    // Draw inner border highlight
                    lineRenderer.drawLine(glm::vec2(minX, minY), glm::vec2(maxX, minY), occupiedColor);
                    lineRenderer.drawLine(glm::vec2(maxX, minY), glm::vec2(maxX, maxY), occupiedColor);
                    lineRenderer.drawLine(glm::vec2(maxX, maxY), glm::vec2(minX, maxY), occupiedColor);
                    lineRenderer.drawLine(glm::vec2(minX, maxY), glm::vec2(minX, minY), occupiedColor);
                }
            }
        }
    }
};