#include "sphere_renderer3d.h"

#include <stb_image.h>

void SphereRenderer3D::init(unsigned int rings, unsigned int sectors) {
    m_SphereBatch.reserve(MAX_SPHERES);

	// Create the shader
    m_SphereShader = std::make_unique<Shader>("assets/shaders/3D/sphere_renderer3d.vert", "assets/shaders/3D/sphere_renderer3d.frag");

    std::vector<SphereVertex> vertices;
    std::vector<unsigned int> indices;
	generateUVSphere(1.0f, rings, sectors, vertices, indices);

    m_IndexCount = static_cast<unsigned int>(indices.size());

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_SphereVBO);
    glGenBuffers(1, &m_EBO);
    glGenBuffers(1, &m_InstanceVBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_SphereVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SphereVertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Divisor = 0
    // Position attribute (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SphereVertex), (void*)offsetof(SphereVertex, position));
    glEnableVertexAttribArray(0);

    // Normal attribute (location 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SphereVertex), (void*)offsetof(SphereVertex, normal));
    glEnableVertexAttribArray(1);

    // TexCoords attribute (location 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SphereVertex), (void*)offsetof(SphereVertex, texCoords));
    glEnableVertexAttribArray(2);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Divisor = 1
    glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);
    glBufferData(GL_ARRAY_BUFFER, MAX_SPHERES * sizeof(SphereInstanceData), nullptr, GL_DYNAMIC_DRAW);

    // Instance position (location 3)
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(SphereInstanceData), (void*)offsetof(SphereInstanceData, position));
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    // Instance radius (location 4)
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(SphereInstanceData), (void*)offsetof(SphereInstanceData, radius));
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);

    // Instance color (location 5)
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(SphereInstanceData), (void*)offsetof(SphereInstanceData, color));
    glEnableVertexAttribArray(5);
    glVertexAttribDivisor(5, 1);

    // Instance orientation (location 6)
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(SphereInstanceData), (void*)offsetof(SphereInstanceData, orientation));
    glEnableVertexAttribArray(6);
    glVertexAttribDivisor(6, 1);

    glBindVertexArray(0);
}

void SphereRenderer3D::domeInit() {
    // Shader
    m_DomeShader = std::make_unique<Shader>("assets/shaders/3D/dome_renderer3d.vert", "assets/shaders/3D/dome_renderer3d.frag");

    // Generate vertices and indices
    std::vector<SphereVertex> vertices;
    std::vector<unsigned int> indices;
    generateUVSphere(1.0f, 32, 32, vertices, indices);

    domeIndexCount = static_cast<unsigned int>(indices.size());

    glGenVertexArrays(1, &domeVAO);
    glGenBuffers(1, &domeEBO);
    glGenBuffers(1, &domeVBO);

    glBindVertexArray(domeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, domeVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SphereVertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, domeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SphereVertex), (void*)offsetof(SphereVertex, position));
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SphereVertex), (void*)offsetof(SphereVertex, normal));
    glEnableVertexAttribArray(1);

    // TexCoords attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SphereVertex), (void*)offsetof(SphereVertex, texCoords));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // Load texture
    glGenTextures(1, &domeTexture);
    glBindTexture(GL_TEXTURE_2D, domeTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
  
    unsigned char* data = stbi_load("assets/textures/vaporwave_grid.jpg", &width, &height, &nrChannels, 4);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}

void SphereRenderer3D::beginScene(const glm::mat4& projection) {
    m_SphereShader->use();
    m_SphereShader->setMat4("u_Projection", projection);

    glBindVertexArray(m_VAO);
}

void SphereRenderer3D::endScene() {

}

void SphereRenderer3D::shutdown() {
    if (m_InstanceVBO) glDeleteBuffers(1, &m_InstanceVBO);
    if (m_SphereVBO)   glDeleteBuffers(1, &m_SphereVBO);
    if (m_EBO)         glDeleteBuffers(1, &m_EBO);
    if (m_VAO)         glDeleteVertexArrays(1, &m_VAO);
    m_SphereBatch.clear();

    if (domeVBO)       glDeleteBuffers(1, &domeVBO);
    if (domeEBO)       glDeleteBuffers(1, &domeEBO);
    if (domeVAO)       glDeleteVertexArrays(1, &domeVAO);
}

void SphereRenderer3D::drawDome(const glm::vec3& pos, const glm::vec3& color, float radius, const glm::mat4& projection) {
    // Enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE); // Prevents transparent faces from occluding each other

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    model = glm::scale(model, glm::vec3(radius));

    m_DomeShader->use();
    m_DomeShader->setVec3("u_Color", color);
    m_DomeShader->setMat4("u_Model", model);
    m_DomeShader->setMat4("u_Projection", projection);
    m_DomeShader->setInt("texture1", 0);

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, domeTexture);

    glBindVertexArray(domeVAO);

    glDrawElements(GL_TRIANGLES, domeIndexCount, GL_UNSIGNED_INT, nullptr);

    // Cleanup pipeline state
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glBindVertexArray(0);
}

void SphereRenderer3D::drawSphere(const glm::vec3& position, const glm::vec3& color, float radius, const glm::quat& orientation) {
    if (m_SphereBatch.size() >= MAX_SPHERES) {
        std::cerr << "[SphereSubRenderer] Warning: Sphere batch capacity reached!\n";
        return;
    }

    m_SphereBatch.push_back({ position, radius, color, orientation});
}

void SphereRenderer3D::flush(const glm::mat4& projection) {
    if (m_SphereBatch.empty()) return;

    m_SphereShader->use();
    m_SphereShader->setMat4("u_Projection", projection);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_SphereBatch.size() * sizeof(SphereInstanceData), m_SphereBatch.data());
    glDrawElementsInstanced(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(m_SphereBatch.size()));

    glBindVertexArray(0);
    m_SphereBatch.clear();
}

void SphereRenderer3D::generateUVSphere(float radius, unsigned int rings, unsigned int sectors, std::vector<SphereVertex>& outVertices, std::vector<unsigned int>& outIndices) {
    const float PI = 3.14159265359f;

    // Generate vertices and normals
    for (unsigned int r = 0; r <= rings; ++r) {
        float phi = PI * static_cast<float>(r) / static_cast<float>(rings); // 0 to PI (Top to bottom)
        float y = std::cos(phi);
        float sinPhi = std::sin(phi);

        for (unsigned int s = 0; s <= sectors; ++s) {
            float theta = 2.0f * PI * static_cast<float>(s) / static_cast<float>(sectors); // 0 to 2 * PI
            float x = std::cos(theta) * sinPhi;
            float z = std::sin(theta) * sinPhi;

            SphereVertex v {};
            // Position scaled by radius
            v.position = glm::vec3(x, y, z) * radius;

            // For a sphere centered at origin, the normalized position vector IS the surface normal
            v.normal = glm::vec3(x, y, z);

            // UV texture mapping
            v.texCoords = glm::vec2(static_cast<float>(s) / sectors, static_cast<float>(r) / rings);

            outVertices.push_back(v);
        }
    }

    // Generate triangle indices (Connecting adjacent ring quads)
    for (unsigned int r = 0; r < rings; ++r) {
        for (unsigned int s = 0; s < sectors; ++s) {
            unsigned int current = r * (sectors + 1) + s;
            unsigned int next = current + sectors + 1;

            // Triangle 1
            outIndices.push_back(current);
            outIndices.push_back(next);
            outIndices.push_back(current + 1);

            // Triangle 2
            outIndices.push_back(current + 1);
            outIndices.push_back(next);
            outIndices.push_back(next + 1);
        }
    }
}