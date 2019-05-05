// Drawing by primitive GL_TRIANGLES
//
//
// Vertex shader: the location (0: position attrib (vec3), 1: normal (vec3), 2: color (vec3), 3: texture (vec2))
// Fragment shader

#ifndef PYRAMID_H
#define PYRAMID_H

#include <cmath>
#include <iostream>
#include "shader.h"

class Pyramid {

public:
	float bottom_line;
	float height;
	float bottom_line_half; // bottom_line/2.0f
	float height_half; // height/2.0f
	float height_side_triangle; // the height of side triangles of the pyramid

	Pyramid() {
		Pyramid(1.0f, 1.0f);
	}

	Pyramid(float bottom_line, float height) {
		if (bottom_line < 0.0f || height < 0.0f) {
			std::cout << "The value of bottom_line or height cannot be a negative float" << std::endl;
			exit(-1);
		}
		this->bottom_line = bottom_line;
		this->height = height;
		this->bottom_line_half = this->bottom_line*0.5f;
		this->height_half = this->height*0.5f;
		this->height_side_triangle = get_height_side_triangles();
		createBuffers();
		updateBuffers();
	}

	void draw(Shader *shader) {
		shader->use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6 * 3);
		glBindVertexArray(0);
	}
private:

	GLfloat vertices[6*3*3];	// 6(num of triagles) * 3(vertices per triangle) * 3(values per vertex)
	GLfloat normals[6*3*3];		// same as vertices
	GLfloat colors[6*3*3];		// same as vertices
	GLfloat texcoords[6*3*2];		// 6 * 3 * 2(values per vertex)

	unsigned int VAO;
	// VBO[0]: for position
	// VBO[1]: for normals 
	// VBO[2]: for color
	// VBO[3]: texcoords 
	unsigned int VBO[4];

	float get_height_side_triangles() {
		return sqrt(pow(bottom_line_half, 2.0f) + pow(height, 2.0f));
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

	void updateBuffers() {
		// -----------------------------
		// vertices
		// 4 side triangles
		int num_of_triangles = 0;
		float sign_x, sign_z;
		for (; num_of_triangles < 4; num_of_triangles++) {
			// top vertex
			vertices[num_of_triangles * 9] = 0;
			vertices[num_of_triangles * 9 + 1] = height_half;
			vertices[num_of_triangles * 9 + 2] = 0;
			// two bottom vertices
			// sign of x, z coord
			switch (num_of_triangles) {
			case 0:
				sign_x = -1.0f;
				sign_z = -1.0f;
				break;
			case 1:
				sign_x = +1.0f;
				sign_z = -1.0f;
				break;
			case 2:
				sign_x = +1.0f;
				sign_z = +1.0f;
				break;
			case 3:
				sign_x = -1.0f;
				sign_z = +1.0f;
				break;
			}
			vertices[num_of_triangles * 9 + 3] = sign_x * bottom_line_half;
			vertices[num_of_triangles * 9 + 4] = -height_half;
			vertices[num_of_triangles * 9 + 5] = sign_z * bottom_line_half;
			// sign of x, z coord
			switch (num_of_triangles) {
			case 3:
				sign_x = -1.0f;
				sign_z = -1.0f;
				break;
			case 0:
				sign_x = +1.0f;
				sign_z = -1.0f;
				break;
			case 1:
				sign_x = +1.0f;
				sign_z = +1.0f;
				break;
			case 2:
				sign_x = -1.0f;
				sign_z = +1.0f;
				break;
			}
			vertices[num_of_triangles * 9 + 6] = sign_x * bottom_line_half;
			vertices[num_of_triangles * 9 + 7] = -height_half;
			vertices[num_of_triangles * 9 + 8] = sign_z * bottom_line_half;
		}
		// bottom square
		for (; num_of_triangles < 6; num_of_triangles++) {
			sign_x = pow(-1.0f, num_of_triangles); // 4: +, 5: -
			sign_z = pow(-1.0f, num_of_triangles + 1); // 4: -, 5: +
			vertices[num_of_triangles * 9] = -bottom_line_half;
			vertices[num_of_triangles * 9 + 1] = -height_half;
			vertices[num_of_triangles * 9 + 2] = -bottom_line_half;

			vertices[num_of_triangles * 9 + 3] = sign_x * bottom_line_half;
			vertices[num_of_triangles * 9 + 4] = -height_half;
			vertices[num_of_triangles * 9 + 5] = sign_z * bottom_line_half;

			vertices[num_of_triangles * 9 + 6] = bottom_line_half;
			vertices[num_of_triangles * 9 + 7] = -height_half;
			vertices[num_of_triangles * 9 + 8] = bottom_line_half;
		}
		// -----------------------------
		// Normals
		// Flat Normals
		num_of_triangles = 0;
		float tangent_value = bottom_line_half / height;
		for (; num_of_triangles < 4; num_of_triangles++) {
			
			switch (num_of_triangles) {
			case 0:
				sign_x = 0.0f;
				sign_z = -1.0f;
				break;
			case 1:
				sign_x = +1.0f;
				sign_z = 0.0f;
				break;
			case 2:
				sign_x = 0.0f;
				sign_z = +1.0f;
				break;
			case 3:
				sign_x = -1.0f;
				sign_z = 0.0f;
				break;
			}
			// top vertex
			normals[num_of_triangles * 9] = sign_x;
			normals[num_of_triangles * 9 + 1] = tangent_value;
			normals[num_of_triangles * 9 + 2] = sign_z;
			// two bottom vertices
			normals[num_of_triangles * 9 + 3] = sign_x;
			normals[num_of_triangles * 9 + 4] = tangent_value;
			normals[num_of_triangles * 9 + 5] = sign_z;

			normals[num_of_triangles * 9 + 6] = sign_x;
			normals[num_of_triangles * 9 + 7] = tangent_value;
			normals[num_of_triangles * 9 + 8] = sign_z;
		}
		// bottom square
		for (; num_of_triangles < 6; num_of_triangles++) {
			normals[num_of_triangles * 9] = 0.0f;
			normals[num_of_triangles * 9 + 1] = -1.0f;
			normals[num_of_triangles * 9 + 2] = 0.0f;

			normals[num_of_triangles * 9 + 3] = 0.0f;
			normals[num_of_triangles * 9 + 4] = -1.0f;
			normals[num_of_triangles * 9 + 5] = 0.0f;

			normals[num_of_triangles * 9 + 6] = 0.0f;
			normals[num_of_triangles * 9 + 7] = -1.0f;
			normals[num_of_triangles * 9 + 8] = 0.0f;
		}
		// -----------------------------
		// Colors
		num_of_triangles = 0;
		for (; num_of_triangles < 6; num_of_triangles++) {
			colors[num_of_triangles * 9] = 0.0f;
			colors[num_of_triangles * 9 + 1] = 1.0f;
			colors[num_of_triangles * 9 + 2] = 0.0f;

			colors[num_of_triangles * 9 + 3] = 0.0f;
			colors[num_of_triangles * 9 + 4] = 1.0f;
			colors[num_of_triangles * 9 + 5] = 0.0f;

			colors[num_of_triangles * 9 + 6] = 0.0f;
			colors[num_of_triangles * 9 + 7] = 1.0f;
			colors[num_of_triangles * 9 + 8] = 0.0f;
		}
		// -----------------------------
		// Textures
		num_of_triangles = 0;
		float tex_u, tex_v;
		// 4 side triangles
		for (; num_of_triangles < 4; num_of_triangles++) {
			// top vertex
			texcoords[num_of_triangles * 6] = 0.5f;
			texcoords[num_of_triangles * 6 + 1] = 0.5f;
			// two bottom vertices
			// sign of x, z coord
			switch (num_of_triangles) {
			case 0:
				tex_u = 0.0f;
				tex_v = 0.0f;
				break;
			case 1:
				tex_u = +1.0f;
				tex_v = 0.0f;
				break;
			case 2:
				tex_u = +1.0f;
				tex_v = +1.0f;
				break;
			case 3:
				tex_u = 0.0f;
				tex_v = +1.0f;
				break;
			}
			texcoords[num_of_triangles * 6 + 2] = tex_u;
			texcoords[num_of_triangles * 6 + 3] = tex_v;
			// sign of x, z coord
			switch (num_of_triangles) {
			case 3:
				tex_u = 0.0f;
				tex_v = 0.0f;
				break;
			case 0:
				tex_u = +1.0f;
				tex_v = 0.0f;
				break;
			case 1:
				tex_u = +1.0f;
				tex_v = +1.0f;
				break;
			case 2:
				tex_u = 0.0f;
				tex_v = +1.0f;
				break;
			}
			texcoords[num_of_triangles * 6 + 4] = tex_u;
			texcoords[num_of_triangles * 6 + 5] = tex_v;
		}
		// bottom square
		for (; num_of_triangles < 6; num_of_triangles++) {
			sign_x = (pow(-1.0f, num_of_triangles) + 1)*0.5; // 4: 1.0f, 5: 0.0f
			sign_z = (pow(-1.0f, num_of_triangles + 1) + 1)*0.5; // 4: 0.0f, 5: 1.0f

			texcoords[num_of_triangles * 6] = 0.0f;
			texcoords[num_of_triangles * 6 + 1] = 0.0f;

			texcoords[num_of_triangles * 6 + 2] = sign_x;
			texcoords[num_of_triangles * 6 + 3] = sign_z;

			texcoords[num_of_triangles * 6 + 4] = 1.0f;
			texcoords[num_of_triangles * 6 + 5] = 1.0f;
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



#endif // !PYRAMID_H
