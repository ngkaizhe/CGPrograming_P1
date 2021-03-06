#include "Mesh.h"
#include"ParticleManager.h"

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, Material material) {
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	this->material = material;

	this->setupMesh();
}

void Mesh::Draw(Shader shader, bool isParticle) {
	// if the item to draw is particle, we dont need material
	if (!isParticle) {
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normNr = 1;

		// active used mesh's texture
		for (unsigned int i = 0; i < textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i + 1);

			string number;
			string name = textures[i].type;

			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++);
			else if (name == "texture_normal")
				number = std::to_string(normNr++);

			shader.setUniformInt((name + number).c_str(), i + 1);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
		glActiveTexture(GL_TEXTURE1);
		glActiveTexture(GL_TEXTURE2);
		glActiveTexture(GL_TEXTURE3);

		// no texture are used, so might be only used one colour(rgba)
		if (textures.size() == 0) {
			shader.setUniform3fv("material.ambient", material.ambient);
			shader.setUniform3fv("material.diffuse", material.diffuse);
			shader.setUniform3fv("material.specular", material.specular);
			shader.setUniformFloat("material.shininess", 32);
		}

		// draw mesh
		shader.use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	else {
		// it is particle do something
		// use our own vbo to set objPos
		// bind our position here
		ParticleManager* particleManager = ParticleManager::getParticleManager();
		if (particleManager->positions.size() != 0) {
			// draw mesh
			glBindVertexArray(VAO);
			unsigned int instanceVBO;
			glGenBuffers(1, &instanceVBO);

			glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * particleManager->positions.size(), &(particleManager->positions)[0], GL_STATIC_DRAW);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
			glVertexAttribDivisor(3, 1);
			glEnableVertexAttribArray(3);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, particleManager->positions.size());
			glBindVertexArray(0);
		}
	}
}

void Mesh::setupMesh() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// vertex position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// texture
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	// tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	// bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

	// unbind vao
	glBindVertexArray(0);
}