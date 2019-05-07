// various utils

#ifndef MYUTILS_H
#define MYUTILS_H

#include <cmath>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"

float *get_normal(float v1_x, float v1_y, float v1_z, float v2_x, float v2_y, float v2_z, float v3_x, float v3_y, float v3_z) {

	float vec1_x = v1_x - v2_x; float vec1_y = v1_y - v2_y; float vec1_z = v1_z - v2_z;
	float vec2_x = v1_x - v3_x; float vec2_y = v1_y - v3_y; float vec2_z = v1_z - v3_z;
	// vec3 * vec1 = 0
	float vec1_vec1_inner_product = pow(vec1_x, 2) + pow(vec1_y, 2) + pow(vec1_z, 2);
	float vec1_vec2_inner_product = vec1_x * vec2_x + vec1_y * vec2_y + vec1_z * vec2_z;
	float vec3_x = vec2_x - (vec1_x*vec1_vec2_inner_product / vec1_vec1_inner_product);
	float vec3_y = vec2_y - (vec1_y*vec1_vec2_inner_product / vec1_vec1_inner_product);
	float vec3_z = vec2_z - (vec1_z*vec1_vec2_inner_product / vec1_vec1_inner_product);
	// vec4: (0, 0, -10) to v1
	float vec4_x = v1_x; float vec4_y = v1_y; float vec4_z = v1_z + 10.0f;
	// vec5: normal vector of the triangle
	float vec3_vec3_inner_product = pow(vec3_x, 2) + pow(vec3_y, 2) + pow(vec3_z, 2);
	float vec1_vec4_inner_product = vec1_x * vec4_x + vec1_y * vec4_y + vec1_z * vec4_z;
	float vec3_vec4_inner_product = vec3_x * vec4_x + vec3_y * vec4_y + vec3_z * vec4_z;

	float vec5_x = vec4_x - (vec1_x*vec1_vec4_inner_product / vec1_vec1_inner_product) - (vec3_x*vec3_vec4_inner_product / vec3_vec3_inner_product);
	float vec5_y = vec4_y - (vec1_y*vec1_vec4_inner_product / vec1_vec1_inner_product) - (vec3_y*vec3_vec4_inner_product / vec3_vec3_inner_product);
	float vec5_z = vec4_z - (vec1_z*vec1_vec4_inner_product / vec1_vec1_inner_product) - (vec3_z*vec3_vec4_inner_product / vec3_vec3_inner_product);

	float *ret = new float[3];
	ret[0] = vec5_x; ret[1] = vec5_y; ret[2] = vec5_z;
	return ret;
}

#endif // !MYUTILS_H