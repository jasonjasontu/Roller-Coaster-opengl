#pragma once

#include <glm/glm.hpp>

const glm::vec3 Yaxes = glm::vec3(0.0f, 1.0f, 0.0f);	//just the Y axies

const float coaster_height = 2.0f;	//camera from the track on unit normal vector

const float track_width = 0.4f; // the half width of the track

const float cart_thick = 0.02f;	//how thick is the board of the cart

const float Init_h = 54.0f; // initial height of the car, determine how fast your will run

const float g = 9.8f;	//gravity

const float time_unit = 0.001f; //the unit for velocity and accelaration

const float animation_speed = 0.5f;	//the speed of animation

const float radius_column = 0.1f;	//the size of the track column

const glm::vec3 track_position = glm::vec3(0.0f, 34.0f, 0.0f);	//the location that track starts to be drawed.

glm::vec3 rotate(const glm::vec3& direction, const glm::vec3& center, float theta) {
	glm::mat3 i = glm::mat3(1.0f);
	glm::mat3 L = glm::mat3();
	L[0][0] = 0.0f;
	L[0][1] = center[2];
	L[0][2] = -1.0f*center[1];
	L[1][0] = -1.0f*center[2];
	L[1][1] = 0.0f;
	L[1][2] = center[0];
	L[2][0] = center[1];
	L[2][1] = -1.0f*center[0];
	L[2][2] = 0.0f;
	float d = sqrt(pow(center[0], 2.0) + pow(center[1], 2.0) + pow(center[2], 2.0));

	glm::mat3 temp = i + sin(theta) / d*L + (1 - cos(theta)) / (d*d)*(L*L);
	return direction * temp;
}


/*
glm::vec3 rotate(const glm::vec3& direction, const glm::vec3& center, float theta) {
	float cos_theta = cos(theta);
	float sin_theta = sin(theta);

	glm::vec3 rotated = (direction * cos_theta) + (glm::cross(center, direction) * sin_theta)
		+ (center * glm::dot(center, direction)) * (1 - cos_theta);

	return rotated;
}
*/

