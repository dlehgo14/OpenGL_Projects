// fighter_plane.h
//
// Drawing by primitive GL_TRIANGLES
//
// Vertex shader: the location (0: position attrib (vec3), 1: normal (vec3), 2: color (vec3), 3: texture (vec2))
// Fragment shader

#ifndef FIGHTER_PLANE_H
#define FIGHTER_PLANE_H

#include <cmath>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "bucket.h"

class Fighter_plane {
public:
	Bucket *left_wing, *right_wing, *body, *gun;
	float wing_length, wing_start, wing_end;
	float body_length, body_start, body_end;
	float gun_length, gun_start, gun_end;
	float wing_radian;
	glm::mat4 model;

	Fighter_plane() {
		this->wing_length = 3.0f; this->wing_start = 1.0f; this->wing_end = 0.3f;
		this->body_length = 3.0f; this->body_start = 2.0f; this->body_end = 1.0f;
		this->gun_length = 0.5f; this->gun_start = 0.1f; this->gun_end = 0.1f;

		this->left_wing = new Bucket(12, 3, wing_start, wing_end, 0.3, 0.1, wing_length, false, false);
		this->right_wing = new Bucket(12, 3, wing_start, wing_end, 0.3, 0.1, wing_length, false, false);
		this->body = new Bucket(16, 8, body_start, body_end, 0.3, 0.1, body_length, false, false);
		this->gun = new Bucket(20, 20, gun_start, gun_end, 1.0f, 1.0f, gun_length, false, false);

		this->wing_radian = atan(body_length / (body_start - body_end));
	}

	void draw(Shader *shader, glm::mat4 model) {
		// body
		glm::mat4 body_model = glm::mat4(model);
		shader->setMat4("model", body_model);
		glBindVertexArray(body->getVAO());
		glDrawArrays(GL_TRIANGLES, 0, body->getTriangleNum() * 3);
		glBindVertexArray(0);

		// left wing
		glm::mat4 left_wing_model = glm::mat4(model);
		left_wing_model = glm::translate(left_wing_model, glm::vec3((body_start+body_end)*0.5f, 0.0f, 0.0f));
		left_wing_model = glm::rotate(left_wing_model, wing_radian, glm::vec3(0.0f, 0.0f, 1.0f));
		shader->setMat4("model", left_wing_model);
		glBindVertexArray(left_wing->getVAO());
		glDrawArrays(GL_TRIANGLES, 0, left_wing->getTriangleNum() * 3);
		glBindVertexArray(0);
		
		// right wing
		glm::mat4 right_wing_model = glm::mat4(model);
		right_wing_model = glm::translate(right_wing_model, glm::vec3(-(body_start + body_end)*0.5f, 0.0f, 0.0f));
		right_wing_model = glm::rotate(right_wing_model, -wing_radian, glm::vec3(0.0f, 0.0f, 1.0f));
		shader->setMat4("model", right_wing_model);
		glBindVertexArray(right_wing->getVAO());
		glDrawArrays(GL_TRIANGLES, 0, right_wing->getTriangleNum() * 3);
		glBindVertexArray(0);

		// gun
		glm::mat4 gun_model = glm::mat4(model);
		gun_model = glm::translate(gun_model, glm::vec3(0.0f, -body_length*0.5f, 0.0f));
		shader->setMat4("model", gun_model);
		glBindVertexArray(gun->getVAO());
		glDrawArrays(GL_TRIANGLES, 0, gun->getTriangleNum() * 3);
		glBindVertexArray(0);
	}

};
#endif // !FIGHTER_PLANE_H
