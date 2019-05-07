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
#include "MyUtils.h"

class Paper {
public:
	const static int WIDTH = 100;
	const static int HEIGHT = 100;
	const static int NUM_OF_TOTAL_TRIANGLES = WIDTH * HEIGHT * 4;
	float width, height, box_width, box_height;
	bool colorMode, flatNormals;

	Paper(int width, int height) {
		this->width = width; this->height = height;
		this->box_width = width / (float)WIDTH;
		this->box_height = height / (float)HEIGHT;
		colorMode = false;
		flatNormals = true;
		createBuffers();
		updateBuffers();
	}

	void set_force(int w, int h, float x, float y, float z, float force) {
		int index = w * HEIGHT + h;
		float total = sqrt(pow(x, 2.0f) + pow(y, 2.0f) + pow(z, 2.0f));
		x = x / total; y = y / total; z = z / total;
		status[index][0] = x; status[index][1] = y; status[index][2] = z; status[index][3] = force;
		float next_force = force * spreading_force;
		if (force > 0.1f) {
			for (int horizontal = -1; horizontal < 2; horizontal += 2) {
				for (int vertical = -1; vertical < 2; vertical += 2) {
					if (((h + vertical) > -1) && ((h + vertical) < HEIGHT)) { // 0 <= h < vertical
						if (((w + horizontal) > -1) && ((w + horizontal) < WIDTH)) { // 0 <= w < horizontal
							set_force(w + horizontal, h + vertical, x, y, z, next_force);
						}
					}
				}
			}
		}
	}

	void draw(Shader *shader) {
		update_status();
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
	float reducing_force = 0.01f;
	float spreading_force = 0.3f;
	float currentTime;
	float pastTime;
	float status[WIDTH*HEIGHT][4] = { 0.0f }; // [x, y, z, force]

	GLfloat vertices[NUM_OF_TOTAL_TRIANGLES * 3 * 3];	// 4 triangles per box
	GLfloat normals[NUM_OF_TOTAL_TRIANGLES * 3 * 3];		// same as vertices
	GLfloat colors[NUM_OF_TOTAL_TRIANGLES * 3 * 3];		// same as vertices
	GLfloat texcoords[NUM_OF_TOTAL_TRIANGLES * 3 * 2];		// 2(u,v) per vertices

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
				vertices[w_base + h_base + 9] = (w + 1.0f) * box_width - width * 0.5f;
				vertices[w_base + h_base + 10] = h * box_height - height * 0.5f;
				vertices[w_base + h_base + 11] = 0.0f;
				vertices[w_base + h_base + 12] = (w + 1.0f) * box_width - width * 0.5f;
				vertices[w_base + h_base + 13] = (h + 1.0f) * box_height - height * 0.5f;
				vertices[w_base + h_base + 14] = 0.0f;
				vertices[w_base + h_base + 15] = (w + 0.5f) * box_width - width * 0.5f;
				vertices[w_base + h_base + 16] = (h + 0.5f) * box_height - height * 0.5f;
				vertices[w_base + h_base + 17] = 0.0f;
				// top triangle
				vertices[w_base + h_base + 18] = (w + 1.0f) * box_width - width * 0.5f;
				vertices[w_base + h_base + 19] = (h + 1.0f) * box_height - height * 0.5f;
				vertices[w_base + h_base + 20] = 0.0f;
				vertices[w_base + h_base + 21] = w * box_width - width * 0.5f;
				vertices[w_base + h_base + 22] = (h + 1.0f) * box_height - height * 0.5f;
				vertices[w_base + h_base + 23] = 0.0f;
				vertices[w_base + h_base + 24] = (w + 0.5f) * box_width - width * 0.5f;
				vertices[w_base + h_base + 25] = (h + 0.5f) * box_height - height * 0.5f;
				vertices[w_base + h_base + 26] = 0.0f;
				// left triangle
				vertices[w_base + h_base + 27] = w * box_width - width * 0.5f;
				vertices[w_base + h_base + 28] = (h + 1.0f) * box_height - height * 0.5f;
				vertices[w_base + h_base + 29] = 0.0f;
				vertices[w_base + h_base + 30] = w * box_width - width * 0.5f;
				vertices[w_base + h_base + 31] = h * box_height - height * 0.5f;
				vertices[w_base + h_base + 32] = 0.0f;
				vertices[w_base + h_base + 33] = (w + 0.5f) * box_width - width * 0.5f;
				vertices[w_base + h_base + 34] = (h + 0.5f) * box_height - height * 0.5f;
				vertices[w_base + h_base + 35] = 0.0f;
			}
		}
		// -----------------------------
		// normals
		if (flatNormals) {
			for (int i = 0; i < NUM_OF_TOTAL_TRIANGLES; i++) {
				float* normal = get_normal(vertices[i * 9], vertices[i * 9 + 1], vertices[i * 9 + 2], vertices[i * 9 + 3], vertices[i * 9 + 4], vertices[i * 9 + 5], vertices[i * 9 + 6], vertices[i * 9 + 7], vertices[i * 9 + 8]);
				normals[i * 9] = normal[0]; normals[i * 9 + 1] = normal[1]; normals[i * 9 + 2] = normal[2];
				normals[i * 9 + 3] = normal[0]; normals[i * 9 + 4] = normal[1]; normals[i * 9 + 5] = normal[2];
				normals[i * 9 + 6] = normal[0]; normals[i * 9 + 7] = normal[1]; normals[i * 9 + 8] = normal[2];
			}
		}
		
		// -----------------------------
		// colors
		if (colorMode) {
			for (int i = 0; i < NUM_OF_TOTAL_TRIANGLES; i++) {
				int r = rand() % 2;
				int g = rand() % 2;
				int b = rand() % 2;

				colors[i * 9] = r;
				colors[i * 9 + 1] = g;
				colors[i * 9 + 2] = b;
				colors[i * 9 + 3] = r;
				colors[i * 9 + 4] = g;
				colors[i * 9 + 5] = b;
				colors[i * 9 + 6] = r;
				colors[i * 9 + 7] = g;
				colors[i * 9 + 8] = b;
			}
		}
		else {
			for (int i = 0; i < NUM_OF_TOTAL_TRIANGLES; i++) {
				colors[i * 9] = 1.0f;
				colors[i * 9 + 1] = 1.0f;
				colors[i * 9 + 2] = 1.0f;
				colors[i * 9 + 3] = 1.0f;
				colors[i * 9 + 4] = 1.0f;
				colors[i * 9 + 5] = 1.0f;
				colors[i * 9 + 6] = 1.0f;
				colors[i * 9 + 7] = 1.0f;
				colors[i * 9 + 8] = 1.0f;
			}
		}

		// -----------------------------
		// textures
		for (int i = 0; i < NUM_OF_TOTAL_TRIANGLES * 3; i++) {
			texcoords[i * 2] = (vertices[i * 3] + width * 0.5f) / width;
			texcoords[i * 2 + 1] = -(vertices[i * 3 + 1] + height * 0.5f) / height + 1.0f;
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


	void update_status() {
		float currentTime = glfwGetTime();
		float diff = currentTime - pastTime;
		// verices
		for (int i = 0; i < WIDTH * HEIGHT; i++) {
			if (status[i][3] > 0.0f) {
				status[i][3] = (status[i][3] - diff * reducing_force) > 0.0f ? (status[i][3] - diff * reducing_force) : 0.0f;
				for (int k = 0; k < 4; k++) {
					vertices[i * 4 * 9 + k * 9] += status[i][0] * status[i][3];
					vertices[i * 4 * 9 + 1 + k * 9] += status[i][1] * status[i][3];
					vertices[i * 4 * 9 + 2 + k * 9] += status[i][2] * status[i][3];
					vertices[i * 4 * 9 + 3 + k * 9] += status[i][0] * status[i][3];
					vertices[i * 4 * 9 + 4 + k * 9] += status[i][1] * status[i][3];
					vertices[i * 4 * 9 + 5 + k * 9] += status[i][2] * status[i][3];
					vertices[i * 4 * 9 + 6 + k * 9] += status[i][0] * status[i][3];
					vertices[i * 4 * 9 + 7 + k * 9] += status[i][1] * status[i][3];
					vertices[i * 4 * 9 + 8 + k * 9] += status[i][2] * status[i][3];
				}
			}
		}
		// normals
		if (flatNormals) {
			for (int i = 0; i < NUM_OF_TOTAL_TRIANGLES; i++) {
				float* normal = get_normal(vertices[i * 9], vertices[i * 9 + 1], vertices[i * 9 + 2], vertices[i * 9 + 3], vertices[i * 9 + 4], vertices[i * 9 + 5], vertices[i * 9 + 6], vertices[i * 9 + 7], vertices[i * 9 + 8]);
				normals[i * 9] = normal[0]; normals[i * 9 + 1] = normal[1]; normals[i * 9 + 2] = normal[2];
				normals[i * 9 + 3] = normal[0]; normals[i * 9 + 4] = normal[1]; normals[i * 9 + 5] = normal[2];
				normals[i * 9 + 6] = normal[0]; normals[i * 9 + 7] = normal[1]; normals[i * 9 + 8] = normal[2];
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

		pastTime = currentTime;
	}
	
};


#endif // !FIGHTER_PLANE_H