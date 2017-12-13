#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <iostream>

#include <shader.hpp>
#include <display.h>


struct Cart_point {
	glm::vec3 pos;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	Cart_point() {}

	Cart_point(glm::vec3 p, glm::vec3 normal, float tx, float ty) {
		pos = p;
		Normal = normal;
		TexCoords = glm::vec2(tx, ty);
	}
};

struct Cart_object {
	//top outside squre
	Cart_point tolf;	//left front
	Cart_point torf;	//right front
	Cart_point torb;	//right back
	Cart_point tolb;	//left back

	//top inside squre
	Cart_point tilf;	//left front
	Cart_point tirf;	//right front
	Cart_point tirb;	//right back
	Cart_point tilb;	//left back

	//bottom  squre
	Cart_point blf;	//left front
	Cart_point brf;	//right front
	Cart_point brb;	//right back
	Cart_point blb;	//left back
	
	Cart_object() {}

	Cart_object(glm::vec3 position, glm::vec3 front, glm::vec3 normal){
		
		front = glm::normalize(front);
		float bottom_h = (track_width - radius_column) * sqrt(3.0) / 2.0f;
		float top_h = bottom_h;
		float bottom_d = (track_width + radius_column) * sqrt(2.0) / 2.0f;
		float top_out = ((track_width - radius_column) * 2.0 - bottom_d) * sqrt(2.0);
		float top_in = ((track_width - radius_column - cart_thick) * 2.0 - bottom_d) * sqrt(2.0);
		float top_radio = cart_thick / (track_width - radius_column) / 2.0f;
		float bottom_radio = (top_out - bottom_d) / top_out / 2.0f;



		glm::vec3 direction = glm::normalize(rotate(front, normal, glm::radians(45.0f)));

		tolf = Cart_point(position + top_out*direction + normal*top_h, normal, 0.0f, 1.0f);
		tilf = Cart_point(position + top_in*direction + normal*top_h, normal, top_radio, 1.0f - top_radio);
		blf = Cart_point(position + bottom_d*direction - bottom_h * normal, rotate(front, normal, 45.0f), 
			bottom_radio, 1.0f - bottom_radio);

		direction = glm::normalize(rotate(front, normal, glm::radians(-45.0f)));
		torf = Cart_point(position + top_out*direction + normal*top_h, normal, 1.0f, 1.0f);
		tirf = Cart_point(position + top_in*direction + normal*top_h, normal, 1.0f - top_radio, 1.0f - top_radio);
		brf= Cart_point(position + bottom_d*direction - bottom_h * normal, rotate(front, normal, -45.0f),
			1.0f - bottom_radio, 1.0f - bottom_radio);


		direction = glm::normalize(rotate(front, normal, glm::radians(-135.0f)));
		torb = Cart_point(position + top_out*direction + normal*top_h, normal, 1.0f, 0.0f);
		tirb = Cart_point(position + top_in*direction + normal*top_h, normal, 1.0f - top_radio, top_radio);
		brb = Cart_point(position + bottom_d*direction - bottom_h * normal, rotate(front, normal, -135.0f),
			1.0f - bottom_radio, bottom_radio);


		direction = glm::normalize(rotate(front, normal, glm::radians(-225.0f)));
		tolb = Cart_point(position + top_out*direction + normal*top_h, normal, 0.0f, 0.0f);
		tilb = Cart_point(position + top_in*direction + normal*top_h, normal, top_radio, top_radio);
		blb = Cart_point(position + bottom_d*direction - bottom_h * normal, rotate(front, normal, 135.0f),
			bottom_radio, bottom_radio);

	}
};

class Cart
{
public:

	// VAO
	unsigned int VAO;
	
	//cart object
	Cart_object model;
	std::vector<Cart_point> cart_points;
	
	// indices for EBO
	std::vector<unsigned int> indices;

	// constructor, just use same VBO as before, 
	Cart(){
		creat_indices();
		make_cart(track_position, track_position+glm::vec3(1.0f, 0.0f, 0.0f), Yaxes);

	}
	
	void make_cart(glm::vec3 cur, glm::vec3 next, glm::vec3 normal){

		model = Cart_object(cur, next - cur, normal);
		cart_points.clear();
		cart_points.push_back(model.tolf);
		cart_points.push_back(model.torf);
		cart_points.push_back(model.torb);
		cart_points.push_back(model.tolb);

		cart_points.push_back(model.tilf);
		cart_points.push_back(model.tirf);
		cart_points.push_back(model.tirb);
		cart_points.push_back(model.tilb);

		cart_points.push_back(model.blf);
		cart_points.push_back(model.brf);
		cart_points.push_back(model.brb);
		cart_points.push_back(model.blb);

		// create buffers/arrays
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/3/2 array which
		// again translates to 3/3/2 floats which translates to a byte array.
		glBufferData(GL_ARRAY_BUFFER, cart_points.size() * sizeof(Cart_point), &cart_points[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Cart_point), (void*)0);
		// vertex normal coords
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Cart_point), (void*)offsetof(Cart_point, Normal));

		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Cart_point), (void*)offsetof(Cart_point, TexCoords));

		glBindVertexArray(0);
	}

	// render the mesh
	void Draw(Shader shader, unsigned int textureID)
	{
		// Set the shader properties
		shader.use();
		glm::mat4 cart_model;
		shader.setMat4("model", cart_model);
		
		// active proper texture unit before binding
		glActiveTexture(GL_TEXTURE0);
		// and finally bind the textures
		glBindTexture(GL_TEXTURE_2D, textureID);

		// draw mesh
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// always good practice to set everything back to defaults once configured.
		glActiveTexture(GL_TEXTURE0);
	}

	void delete_buffers()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

private:
	/*  Render data  */
	unsigned int VBO, EBO;
	
	void creat_indices() {
		for (size_t i = 0; i < 4; i++)
		{
			//top surface
			//first triangle
			indices.push_back(i);
			indices.push_back((i + 1) % 4);
			indices.push_back((i + 1) % 4 + 4);

			//second triangle
			indices.push_back(i);
			indices.push_back(i + 4);
			indices.push_back((i + 1) % 4 + 4);

			//outside
			//first triangle
			indices.push_back(i);
			indices.push_back((i + 1) % 4);
			indices.push_back((i + 1) % 4 + 8);

			//second triangle
			indices.push_back(i);
			indices.push_back(i + 8);
			indices.push_back((i + 1) % 4 + 8);

			//inside
			//outside
			//first triangle
			indices.push_back(i + 4);
			indices.push_back((i + 1) % 4 + 4);
			indices.push_back((i + 1) % 4 + 8);

			//second triangle
			indices.push_back(i + 4);
			indices.push_back(i + 8);
			indices.push_back((i + 1) % 4 + 8);
		}
		//bottom
		//first triangle
		indices.push_back(4);
		indices.push_back(5);
		indices.push_back(6);

		//second triangle
		indices.push_back(4);
		indices.push_back(6);
		indices.push_back(7);
	}
};
