// paper.h
//
// Drawing by primitive GL_TRIANGLES
//
// Vertex shader: the location (0: position attrib (vec3), 1: normal (vec3), 2: color (vec3), 3: texture (vec2))
// Fragment shader

#ifndef PAPER_H
#define PAPER_H

#include <cmath>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"

class Paper {
public:
	const static int WIDTH = 100;
	const static int HEIGHT = 100;
	float width, height, box_width, box_height;

	Paper(int width, int height) {
		this->width = width; this->height = height;
		this->box_width = width / (float)WIDTH;
		this->box_height = height / (float)HEIGHT;
		createBuffers();
		updateBuffers();
	}

	void draw(Shader *shader) {
		shader->use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, WIDTH * HEIGHT * 4 * 3);
		glBindVertexArray(0);
	}

private:
	unsigned int VAO;
	// VBO[0]: for position
	// VBO[1]: for normals 
	// VBO[2]: for color
	// VBO[3]: texcoords 
	unsigned int VBO[4];

	GLfloat vertices[WIDTH * HEIGHT * 4 * 3 * 3];	// 4 triangles per box
	GLfloat normals[WIDTH * HEIGHT * 4 * 3 * 3];		// same as vertices
	GLfloat colors[WIDTH * HEIGHT * 4 * 3 * 3];		// same as vertices
	GLfloat texcoords[WIDTH * HEIGHT * 4 * 3 * 2];		// 2(u,v) per vertices

	void createBuffers() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(4, VBO);

		glBindVertexArray(VAO);

		// reserve space for position attributes
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), 0, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// reserve space for normal attributes
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(normals), 0, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// reserve space for color attributes
		glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(colors), 0, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// reserve space for texture coordinates: for InClass06
		glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), 0, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);
	}
	void updateBuffers() {
		// -----------------------------
		// vertices
		for (int w = 0; w < WIDTH; w++) {
			int w_base = w * HEIGHT * 4 * 3 * 3;
			for (int h = 0; h < HEIGHT; h++) {
				int h_base = h * 4 * 3 * 3;
				// bottom triangle
				vertices[w_base + h_base] = w * box_width - width * 0.5f;
				vertices[w_base + h_base + 1] = h * box_height - height * 0.5f;
				vertices[w_base + h_base + 2] = 0.0f;
				vertices[w_base + h_base + 3] = (w + 1.0f) * box_width - width * 0.5f;
				vertices[w_base + h_base + 4] = h * box_height - height * 0.5f;
				vertices[w_base + h_base + 5] = 0.0f;
				vertices[w_base + h_base + 6] = (w + 0.5f) * box_width - width * 0.5f;
				vertices[w_base + h_base + 7] = (h + 0.5f) * box_height - height * 0.5f;
				vertices[w_base + h_base + 8] = 0.0f;
				// right triangle
				vertices[w_base + h_base] = (w + 1.0f) * box_width - width * 0.5f;
				vertices[w_base + h_base + 9] = h * box_height - height * 0.5f;
				vertices[w_base + h_base + 10] = 0.0f;
				vertices[w_base + h_base + 11] = (w + 1.0f) * box_width - width * 0.5f;
				vertices[w_base + h_base + 12] = (h + 1.0f) * box_height - height * 0.5f;
				vertices[w_base + h_base + 13] = 0.0f;
				vertices[w_base + h_base + 14] = (w + 0.5f) * box_width - width * 0.5f;
				vertices[w_base + h_base + 15] = (h + 0.5f) * box_height - height * 0.5f;
				vertices[w_base + h_base + 16] = 0.0f;
				// top triangle
				vertices[w_base + h_base] = (w + 1.0f) * box_width - width * 0.5f;
				vertices[w_base + h_base + 17] = (h + 1.0f) * box_height - height * 0.5f;
				vertices[w_base + h_base + 18] = 0.0f;
				vertices[w_base + h_base + 19] = w * box_width - width * 0.5f;
				vertices[w_base + h_base + 20] = (h + 1.0f) * box_height - height * 0.5f;
				vertices[w_base + h_base + 21] = 0.0f;
				vertices[w_base + h_base + 22] = (w + 0.5f) * box_width - width * 0.5f;
				vertices[w_base + h_base + 23] = (h + 0.5f) * box_height - height * 0.5f;
				vertices[w_base + h_base + 24] = 0.0f;
				// left triangle
				vertices[w_base + h_base] = w * box_width - width * 0.5f;
				vertices[w_base + h_base + 25] = (h + 1.0f) * box_height - height * 0.5f;
				vertices[w_base + h_base + 26] = 0.0f;
				vertices[w_base + h_base + 27] = w * box_width - width * 0.5f;
				vertices[w_base + h_base + 28] = h * box_height - height * 0.5f;
				vertices[w_base + h_base + 29] = 0.0f;
				vertices[w_base + h_base + 30] = (w + 0.5f) * box_width - width * 0.5f;
				vertices[w_base + h_base + 31] = (h + 0.5f) * box_height - height * 0.5f;
				vertices[w_base + h_base + 32] = 0.0f;
			}
		}
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(normals), normals);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(colors), colors);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(texcoords), texcoords);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);
	}
};


#endif // !FIGHTER_PLANE_H