#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "rendering/utils/shader.h"

struct SphereVertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

struct SphereInstanceData {
	glm::vec3 position;
	float radius;
	glm::vec3 color;
	glm::quat orientation;
};

class SphereRenderer3D {
public:
	SphereRenderer3D() = default;
	~SphereRenderer3D() = default;

	void init(unsigned int rings = 16, unsigned int sectors = 32);
	void domeInit();
	void beginScene(const glm::mat4& projection);
	void endScene();
	void shutdown();

	void drawDome(const glm::vec3& pos, const glm::vec3& color, float radius, const glm::mat4& projection);
	void drawSphere(const glm::vec3& position, const glm::vec3& color, float radius, const glm::quat& orientation);
	void flush(const glm::mat4& projection);

private:
	void generateUVSphere(float radius, unsigned int rings, unsigned int sectors, std::vector<SphereVertex>& outVertices, std::vector<unsigned int>& outIndices);

private:
	std::vector<SphereInstanceData> m_SphereBatch;
	static constexpr size_t MAX_SPHERES = 10000;

	unsigned int m_VAO, m_EBO = 0;
	unsigned int m_SphereVBO = 0;
	unsigned int m_InstanceVBO = 0;
	unsigned int m_IndexCount = 0;
	std::unique_ptr<Shader> m_SphereShader;

	unsigned int domeVAO, domeEBO, domeVBO;
	unsigned int domeIndexCount;
	std::unique_ptr<Shader> m_DomeShader;
	unsigned int domeTexture;
};