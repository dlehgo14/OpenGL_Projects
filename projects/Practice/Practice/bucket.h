// Drawing by primitive GL_TRIANGLES
//
//
// Vertex shader: the location (0: position attrib (vec3), 1: normal (vec3), 2: color (vec3), 3: texture (vec2))
// Fragment shader

#ifndef BUCKET_H
#define BUCKET_H

#include <cmath>
#include <iostream>
#include <time.h>
#include "shader.h"

class Bucket{
public:
	float top_radius, bottom_radius, top_ratio, bottom_ratio, height;
	int top_n, bottom_n; // number of sides of top and bottom
	int ratio = -1; // top_n / bottom_n
	bool colorMode; // enable colors or 
	bool flatNormals; // flat normals or smooth normals
	int num_of_total_triangles;
	const static int MIN = 3; // minimum number of sides
	const static int MAX = 20; // maximum number of sides
	const static int RATIO_MAX = 10;

	Bucket(int top_n, int bottom_n, float top_radius, float bottom_radius, float top_ratio, float bottom_ratio, float height) {
		Bucket(top_n, bottom_n, top_radius, bottom_radius, top_ratio, bottom_ratio, height, true, true);
	}
	Bucket(int top_n, int bottom_n, float top_radius, float bottom_radius, float top_ratio, float bottom_ratio, float height,
		bool colorMode, bool flatNormals) {
		srand(time(NULL));
		this->top_n = top_n;
		this->bottom_n = bottom_n;
		this->top_radius = top_radius;
		this->bottom_radius = bottom_radius;
		this->top_ratio = top_ratio; // height / width of top
		this->bottom_ratio = bottom_ratio; // height / width of bottom
		this->height = height;
		this->colorMode = colorMode;
		this->flatNormals = flatNormals;
		// MIN <= top_n, bottom_n <= MAX
		if (top_n < MIN || top_n > MAX || bottom_n < MIN || bottom_n > MAX) {
			std::cout << "BUCKET: top_n and bottom_n should not be smaller than " << MIN << " or larger than " << MAX << std::endl;
			error();
		}

		// top_n >= bottom_n
		if (top_n < bottom_n) {
			std::cout << "BUCKET: top_n must be same or larger than bottom_n" << std::endl;
			error();
		}
		// top_n = bottom_n * ratio
		// ratio is a natural number (1 <= ratio <= RATIO_MAX)
		for (int i = 0; i < RATIO_MAX + 1; i++) {
			if (top_n == i * bottom_n) {
				this->ratio = i;
				break;
			}
		}
		if (ratio == -1) {
			std::cout << "BUCKET: top_n should be bottom_n * some natural number (" << RATIO_MAX << ")" << std::endl;
			error();
		}
		this->num_of_total_triangles = top_n + bottom_n + (bottom_n* (ratio + 1));

		createBuffers();
		updateBuffers();
	}

	void draw(Shader *shader) {
		shader->use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, num_of_total_triangles * 3);
		glBindVertexArray(0);
	}

	unsigned int getVAO() {
		return VAO;
	}

	int getTriangleNum() {
		return num_of_total_triangles;
	}

private:

	float pi = 3.141592;
	
	GLfloat vertices[(MAX * 2 * 3 + MAX * 2 * 3) * 3];	// MAX*2*3 -> top and bottom, MAX*2*3 -> sides
	GLfloat normals[(MAX * 2 * 3 + MAX * 2 * 3) * 3];		// same as vertices
	GLfloat colors[(MAX * 2 * 3 + MAX * 2 * 3) * 3];		// same as vertices
	GLfloat texcoords[(MAX * 2 * 3 + MAX * 2 * 3) * 2];		// 2(u,v) per vertices

	unsigned int VAO;
	// VBO[0]: for position
	// VBO[1]: for normals 
	// VBO[2]: for color
	// VBO[3]: texcoords 
	unsigned int VBO[4];

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
		// top
		int num_of_triangles = 0;
		float angle = 2 * pi / (float)top_n;
		float theta = 0.0f;
		for (; num_of_triangles < top_n; num_of_triangles++) {
			float x_1 = sin(theta) * top_radius;
			float z_1 = cos(theta) * top_radius * top_ratio;
			float x_2 = sin(theta + angle) * top_radius;
			float z_2 = cos(theta + angle) * top_radius * top_ratio;
			float y = height / 2.0f;

			vertices[num_of_triangles * 9] = x_1;
			vertices[num_of_triangles * 9 + 1] = y;
			vertices[num_of_triangles * 9 + 2] = z_1;
			vertices[num_of_triangles * 9 + 3] = x_2;
			vertices[num_of_triangles * 9 + 4] = y;
			vertices[num_of_triangles * 9 + 5] = z_2;
			vertices[num_of_triangles * 9 + 6] = 0;
			vertices[num_of_triangles * 9 + 7] = y;
			vertices[num_of_triangles * 9 + 8] = 0;

			theta += angle;
		}

		// bottom
		angle = 2.0f * pi / (float)bottom_n;
		theta = 0.0f;
		for (; num_of_triangles < top_n + bottom_n; num_of_triangles++) {
			float x_1 = sin(theta) * bottom_radius;
			float z_1 = cos(theta) * bottom_radius * bottom_ratio;
			float x_2 = sin(theta + angle) * bottom_radius;
			float z_2 = cos(theta + angle) * bottom_radius * bottom_ratio;
			float y = -height / 2.0f;

			vertices[num_of_triangles * 9] = x_1;
			vertices[num_of_triangles * 9 + 1] = y;
			vertices[num_of_triangles * 9 + 2] = z_1;
			vertices[num_of_triangles * 9 + 3] = x_2;
			vertices[num_of_triangles * 9 + 4] = y;
			vertices[num_of_triangles * 9 + 5] = z_2;
			vertices[num_of_triangles * 9 + 6] = 0;
			vertices[num_of_triangles * 9 + 7] = y;
			vertices[num_of_triangles * 9 + 8] = 0;

			theta += angle;
		}
		
		// sides
		int middle = ratio / 2; // middle side of top (which makes rectangle with bottom side)
		float angle_top = 2 * pi / (float)top_n;
		float angle_bottom = 2 * pi / (float)bottom_n;
		float theta_top = -middle * angle_top;
		float theta_bottom = 0.0f;
		int bottom = 0;
		int top = 0;
		for (; bottom < bottom_n; bottom++) {
			// coordinates of bottom
			float x_1_bottom = sin(theta_bottom) * bottom_radius;
			float z_1_bottom = cos(theta_bottom) * bottom_radius * bottom_ratio;
			float x_2_bottom = sin(theta_bottom + angle_bottom) * bottom_radius;
			float z_2_bottom = cos(theta_bottom + angle_bottom) * bottom_radius * bottom_ratio;
			float y_bottom = -height / 2.0f;
			// top~middle with bottom_1
			for (; top <= middle + bottom * ratio ; top++) {
				float x_1_top = sin(theta_top) * top_radius;
				float z_1_top = cos(theta_top) * top_radius * top_ratio;
				float x_2_top = sin(theta_top + angle_top) * top_radius;
				float z_2_top = cos(theta_top + angle_top) * top_radius * top_ratio;
				float y_top = height / 2.0f;

				vertices[num_of_triangles * 9] = x_1_top;
				vertices[num_of_triangles * 9 + 1] = y_top;
				vertices[num_of_triangles * 9 + 2] = z_1_top;
				vertices[num_of_triangles * 9 + 3] = x_2_top;
				vertices[num_of_triangles * 9 + 4] = y_top;
				vertices[num_of_triangles * 9 + 5] = z_2_top;
				vertices[num_of_triangles * 9 + 6] = x_1_bottom;
				vertices[num_of_triangles * 9 + 7] = y_bottom;
				vertices[num_of_triangles * 9 + 8] = z_1_bottom;

				theta_top += angle_top;
				num_of_triangles++;
			}
			// bottom with top_middle
			float x_top = sin(theta_top) * top_radius;
			float z_top = cos(theta_top) * top_radius * top_ratio;
			float y_top = height / 2.0f;
			vertices[num_of_triangles * 9] = x_top;
			vertices[num_of_triangles * 9 + 1] = y_top;
			vertices[num_of_triangles * 9 + 2] = z_top;
			vertices[num_of_triangles * 9 + 3] = x_1_bottom;
			vertices[num_of_triangles * 9 + 4] = y_bottom;
			vertices[num_of_triangles * 9 + 5] = z_1_bottom;
			vertices[num_of_triangles * 9 + 6] = x_2_bottom;
			vertices[num_of_triangles * 9 + 7] = y_bottom;
			vertices[num_of_triangles * 9 + 8] = z_2_bottom;

			num_of_triangles++;
			// middle~top with bottom_2
			for (; top < ratio + bottom * ratio; top++) {
				float x_1_top = sin(theta_top) * top_radius;
				float z_1_top = cos(theta_top) * top_radius * top_ratio;
				float x_2_top = sin(theta_top + angle_top) * top_radius;
				float z_2_top = cos(theta_top + angle_top) * top_radius * top_ratio;
				float y_top = height / 2.0f;

				vertices[num_of_triangles * 9] = x_1_top;
				vertices[num_of_triangles * 9 + 1] = y_top;
				vertices[num_of_triangles * 9 + 2] = z_1_top;
				vertices[num_of_triangles * 9 + 3] = x_2_top;
				vertices[num_of_triangles * 9 + 4] = y_top;
				vertices[num_of_triangles * 9 + 5] = z_2_top;
				vertices[num_of_triangles * 9 + 6] = x_2_bottom;
				vertices[num_of_triangles * 9 + 7] = y_bottom;
				vertices[num_of_triangles * 9 + 8] = z_2_bottom;

				theta_top += angle_top;
				num_of_triangles++;
			}
			theta_bottom += angle_bottom;
		}
		
		// -----------------------------
		// normals
		// flat normals
		if (flatNormals) {
			for (int i = 0; i < num_of_total_triangles; i++) {
				float v1_x = vertices[i * 9]; float v1_y = vertices[i * 9 + 1]; float v1_z = vertices[i * 9 + 2];
				float v2_x = vertices[i * 9 + 3]; float v2_y = vertices[i * 9 + 4]; float v2_z = vertices[i * 9 + 5];
				float v3_x = vertices[i * 9 + 6]; float v3_y = vertices[i * 9 + 7]; float v3_z = vertices[i * 9 + 8];

				float vec1_x = v1_x - v2_x; float vec1_y = v1_y - v2_y; float vec1_z = v1_z - v2_z;
				float vec2_x = v1_x - v3_x; float vec2_y = v1_y - v3_y; float vec2_z = v1_z - v3_z;
				// vec3 * vec1 = 0
				float vec1_vec1_inner_product = pow(vec1_x, 2) + pow(vec1_y, 2) + pow(vec1_z, 2);
				float vec1_vec2_inner_product = vec1_x * vec2_x + vec1_y * vec2_y + vec1_z * vec2_z;
				float vec3_x = vec2_x - (vec1_x*vec1_vec2_inner_product / vec1_vec1_inner_product);
				float vec3_y = vec2_y - (vec1_y*vec1_vec2_inner_product / vec1_vec1_inner_product);
				float vec3_z = vec2_z - (vec1_z*vec1_vec2_inner_product / vec1_vec1_inner_product);
				// vec4: 0 to v1
				float vec4_x = v1_x; float vec4_y = v1_y; float vec4_z = v1_z;
				// vec5: normal vector of the triangle
				float vec3_vec3_inner_product = pow(vec3_x, 2) + pow(vec3_y, 2) + pow(vec3_z, 2);
				float vec1_vec4_inner_product = vec1_x * vec4_x + vec1_y * vec4_y + vec1_z * vec4_z;
				float vec3_vec4_inner_product = vec3_x * vec4_x + vec3_y * vec4_y + vec3_z * vec4_z;

				float vec5_x = vec4_x - (vec1_x*vec1_vec4_inner_product / vec1_vec1_inner_product) - (vec3_x*vec3_vec4_inner_product / vec3_vec3_inner_product);
				float vec5_y = vec4_y - (vec1_y*vec1_vec4_inner_product / vec1_vec1_inner_product) - (vec3_y*vec3_vec4_inner_product / vec3_vec3_inner_product);
				float vec5_z = vec4_z - (vec1_z*vec1_vec4_inner_product / vec1_vec1_inner_product) - (vec3_z*vec3_vec4_inner_product / vec3_vec3_inner_product);

				normals[i * 9] = vec5_x;
				normals[i * 9 + 1] = vec5_y;
				normals[i * 9 + 2] = vec5_z;
				normals[i * 9 + 3] = vec5_x;
				normals[i * 9 + 4] = vec5_y;
				normals[i * 9 + 5] = vec5_z;
				normals[i * 9 + 6] = vec5_x;
				normals[i * 9 + 7] = vec5_y;
				normals[i * 9 + 8] = vec5_z;
			}
		}
		else { // smooth normals;
			for (int i = 0; i < num_of_total_triangles; i++) {
				float v1_x = vertices[i * 9]; float v1_y = vertices[i * 9 + 1]; float v1_z = vertices[i * 9 + 2];
				float v2_x = vertices[i * 9 + 3]; float v2_y = vertices[i * 9 + 4]; float v2_z = vertices[i * 9 + 5];
				float v3_x = vertices[i * 9 + 6]; float v3_y = vertices[i * 9 + 7]; float v3_z = vertices[i * 9 + 8];

				normals[i * 9] = v1_x;
				normals[i * 9 + 1] = v1_y;
				normals[i * 9 + 2] = v1_z;
				normals[i * 9 + 3] = v2_x;
				normals[i * 9 + 4] = v2_y;
				normals[i * 9 + 5] = v2_z;
				normals[i * 9 + 6] = v3_x;
				normals[i * 9 + 7] = v3_y;
				normals[i * 9 + 8] = v3_z;
			}
		}
		
		// -----------------------------
		// colors
		if (colorMode) {
			for (int i = 0; i < num_of_total_triangles; i++) {
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
			for (int i = 0; i < num_of_total_triangles; i++) {
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
		// top
		num_of_triangles = 0;
		angle = 2 * pi / (float)top_n;
		theta = 0.0f;
		for (; num_of_triangles < top_n; num_of_triangles++) {
			float x_1 = (sin(theta) + 1) * 0.5f;
			float z_1 = (cos(theta) + 1)*0.5f;
			float x_2 = (sin(theta + angle) + 1) * 0.5f;
			float z_2 = (cos(theta + angle) + 1)* 0.5f;

			texcoords[num_of_triangles * 6] = x_1;
			texcoords[num_of_triangles * 6 + 1] = z_1;
			texcoords[num_of_triangles * 6 + 2] = x_2;
			texcoords[num_of_triangles * 6 + 3] = z_2;
			texcoords[num_of_triangles * 6 + 4] = 0.5f;
			texcoords[num_of_triangles * 6 + 5] = 0.5f;

			theta += angle;
		}
		// bottom
		angle = 2.0f * pi / (float)bottom_n;
		theta = 0.0f;
		for (; num_of_triangles < top_n + bottom_n; num_of_triangles++) {
			float x_1 = (sin(theta) + 1) * 0.5f;
			float z_1 = (cos(theta) + 1)*0.5f;
			float x_2 = (sin(theta + angle) + 1) * 0.5f;
			float z_2 = (cos(theta + angle) + 1)* 0.5f;

			texcoords[num_of_triangles * 6] = x_1;
			texcoords[num_of_triangles * 6 + 1] = z_1;
			texcoords[num_of_triangles * 6 + 2] = x_2;
			texcoords[num_of_triangles * 6 + 3] = z_2;
			texcoords[num_of_triangles * 6 + 4] = 0.5f;
			texcoords[num_of_triangles * 6 + 5] = 0.5f;

			theta += angle;
		}

		// sides
		middle = ratio / 2; // middle side of top (which makes rectangle with bottom side)
		bottom = 0;
		top = 0;
		for (; bottom < bottom_n; bottom++) {
			// coordinates of bottom
			float x_1_bottom = (float)bottom/(float)bottom_n;
			float x_2_bottom = (float)(bottom + 1.0f) / (float)bottom_n;
			// top~middle with bottom_1
			for (; top <= middle + bottom * ratio; top++) {
				float x_1_top = (float)top/(float)top_n;
				float x_2_top = (float)(top + 1.0f) / (float)top_n;

				texcoords[num_of_triangles * 6] = x_1_top;
				texcoords[num_of_triangles * 6 + 1] = 1.0f;
				texcoords[num_of_triangles * 6 + 2] = x_2_top;
				texcoords[num_of_triangles * 6 + 3] = 1.0f;
				texcoords[num_of_triangles * 6 + 4] = x_1_bottom;
				texcoords[num_of_triangles * 6 + 5] = 0.0f;

				theta_top += angle_top;
				num_of_triangles++;
			}
			// bottom with top_middle
			float x_top = (float)top / (float)top_n;
			texcoords[num_of_triangles * 6] = x_top;
			texcoords[num_of_triangles * 6 + 1] = 1.0f;
			texcoords[num_of_triangles * 6 + 2] = x_1_bottom;
			texcoords[num_of_triangles * 6 + 3] = 0.0f;
			texcoords[num_of_triangles * 6 + 4] = x_2_bottom;
			texcoords[num_of_triangles * 6 + 5] = 0.0f;

			num_of_triangles++;
			// middle~top with bottom_2
			for (; top < ratio + bottom * ratio; top++) {
				float x_1_top = (float)top / (float)top_n;
				float x_2_top = (float)(top + 1.0f) / (float)top_n;

				texcoords[num_of_triangles * 6] = x_1_top;
				texcoords[num_of_triangles * 6 + 1] = 1.0f;
				texcoords[num_of_triangles * 6 + 2] = x_2_top;
				texcoords[num_of_triangles * 6 + 3] = 1.0f;
				texcoords[num_of_triangles * 6 + 4] = x_2_bottom;
				texcoords[num_of_triangles * 6 + 5] = 0.0f;

				theta_top += angle_top;
				num_of_triangles++;

				theta_top += angle_top;
				num_of_triangles++;
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

	void error() {
		char sth;
		std::cout << "Any key to go out..." << std::endl;
		std::cin >> sth;
		exit(-1);
	}
};



#endif // !BUCKET_H
