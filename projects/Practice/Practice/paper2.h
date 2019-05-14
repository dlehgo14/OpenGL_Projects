// paper.h
//
// Drawing by primitive GL_TRIANGLES
//
// Vertex shader: the location (0: position attrib (vec3), 1: normal (vec3), 2: color (vec3), 3: texture (vec2))
// Fragment shader

#ifndef PAPER2_H
#define PAPER2_H

#include <cmath>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "MyUtils.h"

class Paper2 {
public:
	const static int WIDTH = 100, HEIGHT = 100;
	const static int NUM_OF_TOTAL_TRIANGLES = WIDTH * HEIGHT * 4;
	const static int NUM_OF_TOTAL_VERTICES = (HEIGHT * WIDTH) + (HEIGHT + 1)*(WIDTH + 1);
	float width, height, box_width, box_height;
	bool colorMode, flatNormals;

	Paper2(int width, int height) {
		this->width = width; this->height = height;
		this->box_width = width / (float)WIDTH;
		this->box_height = height / (float)HEIGHT;
		forceMode = false;
		colorMode = false;
		flatNormals = true;
		initCoord();
		createBuffers();
		initBuffers();
	}

	void draw(Shader *shader) {
		if(forceMode) update_status();
		shader->use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, WIDTH * HEIGHT * 4 * 3);
		glBindVertexArray(0);
	}

	void set_force(int w, int h, float x, float y, float z, float force) {
		// normalize force vector
		float total = sqrt(pow(x, 2.0f) + pow(y, 2.0f) + pow(z, 2.0f));
		x = x / total * force; y = y / total * force; z = z / total * force;

		// merge new force with original force
		x = status[h][w][0] * status[h][w][3] + x; y = status[h][w][1] * status[h][w][3] + y; z = status[h][w][2] * status[h][w][3] + z;

		// set force vector
		force = sqrt(pow(x, 2.0f) + pow(y, 2.0f) + pow(z, 2.0f));
		status[h][w][0] = x / force; status[h][w][1] = y / force; status[h][w][2] = z / force; status[h][w][3] = force;
		/*
		// force spreading
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
		*/
	}

	void forceModeSwitch() {
		forceMode = !forceMode;
		pastTime = glfwGetTime();
		if (forceMode) std::cout << "force mode ON" << std::endl;
		else std::cout << "force mode OFF" << std::endl;
	}
private:
	// force mode
	bool forceMode;
	// vertices coordinates
	float center_coord[HEIGHT][WIDTH][3];
	float corner_coord[HEIGHT + 1][WIDTH + 1][3];
	unsigned int VAO;
	// VBO[0]: for position
	// VBO[1]: for normals 
	// VBO[2]: for color
	// VBO[3]: texcoords 
	unsigned int VBO[4];
	GLfloat vertices[NUM_OF_TOTAL_TRIANGLES * 3 * 3];	// 4 triangles per box
	GLfloat normals[NUM_OF_TOTAL_TRIANGLES * 3 * 3];		// same as vertices
	GLfloat colors[NUM_OF_TOTAL_TRIANGLES * 3 * 3];		// same as vertices
	GLfloat texcoords[NUM_OF_TOTAL_TRIANGLES * 3 * 2];		// 2(u,v) per vertices
	// for forces
	float reducing_force = 0.1f;
	float spreading_force = 0.3f;
	float currentTime;
	float pastTime;
	float status[HEIGHT][WIDTH][4] = { 0.0f }; // [x, y, z, force]

	void initCoord() {
		// center coordinates
		for (int w = 0; w < WIDTH; w++) {
			for (int h = 0; h < HEIGHT; h++) {
				center_coord[h][w][0] = -(float)width * 0.5f + (0.5f + (float)w)*box_width;
				center_coord[h][w][1] = -(float)height * 0.5f + (0.5f + (float)h)*box_height;
				center_coord[h][w][2] = 0.0f;
			}
		}
		updateCoord();
	}
	void updateCoord() {
		// middle coordinates between center coordinates
		for (int w = 1; w < WIDTH; w++) {
			for (int h = 1; h < HEIGHT; h++) {
				for (int coord = 0; coord < 3; coord++) { // 0:x, 1:y, 2:z
					corner_coord[h][w][coord] = 0.25*(center_coord[h - 1][w - 1][coord] + center_coord[h][w - 1][coord] + center_coord[h - 1][w][coord] + center_coord[h][w][coord]);
				}
			}
		}
		// edge coordinates of corner coordinates
		for (int w = 0; w < WIDTH + 1; w += WIDTH) { // w = [0, WIDTH]
			for (int h = 0; h < HEIGHT; h++) {
				for (int coord = 0; coord < 3; coord++) {
					corner_coord[h][w][coord] = center_coord[h][w][coord];
				}
			}
			for (int coord = 0; coord < 3; coord++) {
				corner_coord[HEIGHT][w][coord] = center_coord[HEIGHT - 1][w][coord];
			}
		}
		for (int h = 0; h < HEIGHT + 1; h += HEIGHT) { // h = [0, HEIGHT]
			for (int w = 0; w < WIDTH; w++) {
				for (int coord = 0; coord < 3; coord++) {
					corner_coord[h][w][coord] = center_coord[h][w][coord];
				}
			}
			for (int coord = 0; coord < 3; coord++) {
				corner_coord[h][WIDTH][coord] = center_coord[h][WIDTH - 1][coord];
			}
		}
	}
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
	void initBuffers() {
		// -----------------------------
		// vertices
		for (int w = 0; w < WIDTH; w++) {
			int w_base = w * HEIGHT * 4 * 3 * 3;
			for (int h = 0; h < HEIGHT; h++) {
				int h_base = h * 4 * 3 * 3;
				// bottom triangle
				vertices[w_base + h_base] = corner_coord[h][w][0];
				vertices[w_base + h_base + 1] = corner_coord[h][w][1];
				vertices[w_base + h_base + 2] = corner_coord[h][w][2];
				vertices[w_base + h_base + 3] = corner_coord[h][w + 1][0];
				vertices[w_base + h_base + 4] = corner_coord[h][w + 1][1];
				vertices[w_base + h_base + 5] = corner_coord[h][w + 1][2];
				vertices[w_base + h_base + 6] = center_coord[h][w][0];
				vertices[w_base + h_base + 7] = center_coord[h][w][1];
				vertices[w_base + h_base + 8] = center_coord[h][w][2];
				//std::cout << (vertices[w_base + h_base + 6] + width * 0.5f) / box_width << std::endl;
				//std::cout << vertices[w_base + h_base] << std::endl;
				// right triangle
				vertices[w_base + h_base + 9] = corner_coord[h][w + 1][0];;
				vertices[w_base + h_base + 10] = corner_coord[h][w + 1][1];;
				vertices[w_base + h_base + 11] = corner_coord[h][w + 1][2];;
				vertices[w_base + h_base + 12] = corner_coord[h + 1][w + 1][0];
				vertices[w_base + h_base + 13] = corner_coord[h + 1][w + 1][1];
				vertices[w_base + h_base + 14] = corner_coord[h + 1][w + 1][2];
				vertices[w_base + h_base + 15] = center_coord[h][w][0];
				vertices[w_base + h_base + 16] = center_coord[h][w][1];
				vertices[w_base + h_base + 17] = center_coord[h][w][2];
				// top triangle
				vertices[w_base + h_base + 18] = corner_coord[h + 1][w + 1][0];
				vertices[w_base + h_base + 19] = corner_coord[h + 1][w + 1][1];
				vertices[w_base + h_base + 20] = corner_coord[h + 1][w + 1][2];
				vertices[w_base + h_base + 21] = corner_coord[h + 1][w][0];
				vertices[w_base + h_base + 22] = corner_coord[h + 1][w][1];
				vertices[w_base + h_base + 23] = corner_coord[h + 1][w][2];
				vertices[w_base + h_base + 24] = center_coord[h][w][0];
				vertices[w_base + h_base + 25] = center_coord[h][w][1];
				vertices[w_base + h_base + 26] = center_coord[h][w][2];
				// left triangle
				vertices[w_base + h_base + 27] = corner_coord[h + 1][w][0];
				vertices[w_base + h_base + 28] = corner_coord[h + 1][w][1];
				vertices[w_base + h_base + 29] = corner_coord[h + 1][w][2];
				vertices[w_base + h_base + 30] = corner_coord[h][w][0];
				vertices[w_base + h_base + 31] = corner_coord[h][w][1];
				vertices[w_base + h_base + 32] = corner_coord[h][w][2];
				vertices[w_base + h_base + 33] = center_coord[h][w][0];
				vertices[w_base + h_base + 34] = center_coord[h][w][1];
				vertices[w_base + h_base + 35] = center_coord[h][w][2];
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
	void updateBuffers() {
		// -----------------------------
		// vertices
		for (int w = 0; w < WIDTH; w++) {
			int w_base = w * HEIGHT * 4 * 3 * 3;
			for (int h = 0; h < HEIGHT; h++) {
				int h_base = h * 4 * 3 * 3;
				// bottom triangle
				vertices[w_base + h_base] = corner_coord[h][w][0];
				vertices[w_base + h_base + 1] = corner_coord[h][w][1];
				vertices[w_base + h_base + 2] = corner_coord[h][w][2];
				vertices[w_base + h_base + 3] = corner_coord[h][w + 1][0];
				vertices[w_base + h_base + 4] = corner_coord[h][w + 1][1];
				vertices[w_base + h_base + 5] = corner_coord[h][w + 1][2];
				vertices[w_base + h_base + 6] = center_coord[h][w][0];
				vertices[w_base + h_base + 7] = center_coord[h][w][1];
				vertices[w_base + h_base + 8] = center_coord[h][w][2];
				// right triangle
				vertices[w_base + h_base + 9] = corner_coord[h][w + 1][0];;
				vertices[w_base + h_base + 10] = corner_coord[h][w + 1][1];;
				vertices[w_base + h_base + 11] = corner_coord[h][w + 1][2];;
				vertices[w_base + h_base + 12] = corner_coord[h + 1][w + 1][0];
				vertices[w_base + h_base + 13] = corner_coord[h + 1][w + 1][1];
				vertices[w_base + h_base + 14] = corner_coord[h + 1][w + 1][2];
				vertices[w_base + h_base + 15] = center_coord[h][w][0];
				vertices[w_base + h_base + 16] = center_coord[h][w][1];
				vertices[w_base + h_base + 17] = center_coord[h][w][2];
				// top triangle
				vertices[w_base + h_base + 18] = corner_coord[h + 1][w + 1][0];
				vertices[w_base + h_base + 19] = corner_coord[h + 1][w + 1][1];
				vertices[w_base + h_base + 20] = corner_coord[h + 1][w + 1][2];
				vertices[w_base + h_base + 21] = corner_coord[h + 1][w][0];
				vertices[w_base + h_base + 22] = corner_coord[h + 1][w][1];
				vertices[w_base + h_base + 23] = corner_coord[h + 1][w][2];
				vertices[w_base + h_base + 24] = center_coord[h][w][0];
				vertices[w_base + h_base + 25] = center_coord[h][w][1];
				vertices[w_base + h_base + 26] = center_coord[h][w][2];
				// left triangle
				vertices[w_base + h_base + 27] = corner_coord[h + 1][w][0];
				vertices[w_base + h_base + 28] = corner_coord[h + 1][w][1];
				vertices[w_base + h_base + 29] = corner_coord[h + 1][w][2];
				vertices[w_base + h_base + 30] = corner_coord[h][w][0];
				vertices[w_base + h_base + 31] = corner_coord[h][w][1];
				vertices[w_base + h_base + 32] = corner_coord[h][w][2];
				vertices[w_base + h_base + 33] = center_coord[h][w][0];
				vertices[w_base + h_base + 34] = center_coord[h][w][1];
				vertices[w_base + h_base + 35] = center_coord[h][w][2];
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

		glBindVertexArray(0);
	}
	void update_status() {
		float currentTime = glfwGetTime();
		float diff = currentTime - pastTime;
		// update center coordinates
		for (int h = 0; h < HEIGHT;h++) {
			for (int w = 0; w < WIDTH; w++) {
				if (status[h][w][3] > 0.0f) {
					for (int coord = 0; coord < 3; coord++) {
						center_coord[h][w][coord] += 0.1f * diff * (status[h][w][coord] * status[h][w][3]);
					}
					status[h][w][3] = (status[h][w][3] - diff * reducing_force) > 0.0f ? (status[h][w][3] - diff * reducing_force) : 0.0f;
				}
			}
		}
		// update corner coordinates
		updateCoord();
		// update buffers
		updateBuffers();
		pastTime = currentTime;
	}
};
#endif // !PAPER2_H