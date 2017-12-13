#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <iostream>

#include <shader.hpp>
#include <rc_spline.h>
#include <display.h>



struct Track_point {
	glm::vec3 pos;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	Track_point() {}

	Track_point(glm::vec3 p, glm::vec3 normal, float tx, float ty) {
		pos = p;
		Normal = normal;
		TexCoords = glm::vec2(tx, ty);
	}
};

struct Camera_Point {
	glm::vec3 pos;
	glm::vec3 cart_pos;
	glm::vec3 up;
	float dis; //as for distance
	Camera_Point(){}

	Camera_Point(glm::vec3 position, glm::vec3 normal, float distance) {
		pos = position + coaster_height*normal;
		cart_pos = position;
		dis = distance;
		up = normal;
	}
};

struct Track_part {
	Camera_Point cp;

	Track_point left0;
	Track_point left1;
	Track_point left2;
	Track_point left3;
	Track_point left4;
	Track_point left5;
	Track_point left6;


	Track_point right0;
	Track_point right1;
	Track_point right2;
	Track_point right3;
	Track_point right4;
	Track_point right5;
	Track_point right6;


	Track_point bl0;
	Track_point bl1;
	Track_point bl2;
	Track_point bl3;
	Track_point bl4;
	Track_point bl5;
	Track_point bl6;


	Track_point br0;
	Track_point br1;
	Track_point br2;
	Track_point br3;
	Track_point br4;
	Track_point br5;
	Track_point br6;

	Track_part(glm::vec3 pre, glm::vec3 position, glm::vec3 next, float texture){
		//get the unit front vector to describe where the track is going
		glm::vec3 front = next - position;
		float distance = sqrt(pow(front.x, 2.0) + pow(front.y, 2.0) + pow(front.z, 2.0));
		float radio = 1.0f / distance;
		glm::vec3 unit = radio*front;

		//rotate the normal vector to fit the angle
		glm::vec3 normal = glm::cross(glm::cross(front, Yaxes), front);
		normal = glm::normalize(normal);//rotate(normal, unit, glm::radians(angle)));

		//get the left direction for building track parts
		glm::vec3 left_d = glm::cross(normal, front);
		radio = track_width / sqrt(pow(left_d.x, 2.0) + pow(left_d.y, 2.0) + pow(left_d.z, 2.0));
		left_d = radio*left_d;
		
		//build all track point with our data
		glm::vec3 l = position + left_d;
		glm::vec3 r = position - left_d;
		glm::vec3 bl = position - float(sqrt(3.0) / 2.0)*track_width*normal + 0.5f*left_d;
		glm::vec3 br = bl - left_d;

		glm::vec3 n = normal;
		left0 = Track_point(l + radius_column*n, n, 1.0f, texture);
		left1 = Track_point(l + radius_column*n, n, 0.0f, texture);
		right0 = Track_point(r + radius_column*n, n, 1.0f, texture);
		right1 = Track_point(r + radius_column*n, n, 0.0f, texture);
		bl0 = Track_point(bl + radius_column*n, n, 1.0f, texture);
		bl1 = Track_point(bl + radius_column*n, n, 0.0f, texture);
		br0 = Track_point(br + radius_column*n, n, 1.0f, texture);
		br1 = Track_point(br + radius_column*n, n, 0.0f, texture);
		n = glm::normalize(rotate(n, unit, glm::radians(60.0f)));
		left2 = Track_point(l + radius_column*n, n, 0.167f, texture);
		right2 = Track_point(r + radius_column*n, n, 0.167f, texture);
		bl2 = Track_point(bl + radius_column*n, n, 0.167f, texture);
		br2 = Track_point(br + radius_column*n, n, 0.167f, texture);
		n = glm::normalize(rotate(n, unit, glm::radians(60.0f)));
		left3 = Track_point(l + radius_column*n, n, 0.333f, texture);
		right3 = Track_point(r + radius_column*n, n, 0.333f, texture);
		bl3 = Track_point(bl + radius_column*n, n, 0.333f, texture);
		br3 = Track_point(br + radius_column*n, n, 0.333f, texture);
		n = glm::normalize(rotate(n, unit, glm::radians(60.0f)));
		left4 = Track_point(l + radius_column*n, n, 0.5f, texture);
		right4 = Track_point(r + radius_column*n, n, 0.5f, texture);
		bl4 = Track_point(bl + radius_column*n, n, 0.5f, texture);
		br4 = Track_point(br + radius_column*n, n, 0.5f, texture);
		n = glm::normalize(rotate(n, unit, glm::radians(60.0f)));
		left5 = Track_point(l + radius_column*n, n, 0.667f, texture);
		right5 = Track_point(r + radius_column*n, n, 0.667f, texture);
		bl5 = Track_point(bl + radius_column*n, n, 0.667f, texture);
		br5 = Track_point(br + radius_column*n, n, 0.667f, texture);
		n = glm::normalize(rotate(n, unit, glm::radians(60.0f)));
		left6 = Track_point(l + radius_column*n, n, 0.833f, texture);
		right6 = Track_point(r + radius_column*n, n, 0.833f, texture);
		bl6 = Track_point(bl + radius_column*n, n, 0.833f, texture);
		br6 = Track_point(br + radius_column*n, n, 0.833f, texture);

		cp = Camera_Point(position, normal, distance);
	}
};

class Track
{
public:

	// VAO
	unsigned int VAO;

	// Control Points Loading Class for loading from File
	rc_Spline g_Track;

	// Vector of control points
	std::vector<glm::vec3> controlPoints;

	//track object
	std::vector<Track_point> trackPoints;

	//camera track
	std::vector<Camera_Point> cameraTrack;

	// indices for EBO
	std::vector<unsigned int> indices;

	// constructor, just use same VBO as before, 
	Track(const char* trackPath)
	{
		
		// load Track data
		load_track(trackPath);

		create_track();

		setup_track();
	}

	// render the mesh
	void Draw(Shader shader, unsigned int textureID)
	{
		// Set the shader properties
		shader.use();
		glm::mat4 track_model;
		shader.setMat4("model", track_model);
		
		
		// Set material properties
		shader.setVec3("material.specular", 0.3f, 0.3f, 0.3f);
		shader.setFloat("material.shininess", 64.0f);
		

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

	void load_track(const char* trackPath)
	{
		// Set folder path for our projects (easier than repeatedly defining it)
		g_Track.folder = "../Project2/Media/";

		// Load the control points
		g_Track.loadSplineFrom(trackPath);

	}

	// Implement the Catmull-Rom Spline here
	//     Given 4 points, a tau and the u value 
	//     Since you can just use linear algebra from glm, just make the vectors and matrices and multiply them.  
	//     This should not be a very complicated function
	glm::vec3 interpolate(glm::vec3 pointA, glm::vec3 pointB, glm::vec3 pointC, glm::vec3 pointD, float tau, float u)
	{
		//vector for u
		glm::vec4 vec_u(1.0f, u, pow(u, 2.0f), pow(u, 3.0f));

		//the mapping matrix
		glm::vec4 r1(0.0f, 1.0f, 0.0f, 0.0f);
		glm::vec4 r2(-tau, 0.0f, tau, 0.0f);
		glm::vec4 r3(2.0f*tau, tau - 3.0f, 3.0f - 2.0f*tau, -tau);
		glm::vec4 r4(-tau, 2.0f - tau, tau - 2.0f, tau);
		glm::mat4x4 mat(r1, r2, r3, r4);

		//vector of control point
		glm::mat4x3 point(pointA, pointB, pointC, pointD);
		glm::mat3x4 p_T = glm::transpose(point);

		// Just returning the first point at the moment, you need to return the interpolated point.  
		return mat*vec_u*p_T;

	}

	void make_track_part(glm::vec3 pre, glm::vec3 cur, glm::vec3 next, float i){
		Track_part p(pre, cur, next, i);
		
		
		if (trackPoints.size() > 1) {
			int s = trackPoints.size();
			for (size_t i = 0; i < 6; i++)
			{
				for (size_t j = i; j < 28; j+=7)
				{

					//first triangle
					indices.push_back(s + j);
					indices.push_back(s + j + 1);
					indices.push_back(s + j + 1 - 28);

					//second triangle
					indices.push_back(s + j);
					indices.push_back(s + j + 1 - 28);
					indices.push_back(s + j - 28);
				}
			}
			
		}

		//store all the data
		trackPoints.push_back(p.left1);
		trackPoints.push_back(p.left2);
		trackPoints.push_back(p.left3);
		trackPoints.push_back(p.left4);
		trackPoints.push_back(p.left5);
		trackPoints.push_back(p.left6);
		trackPoints.push_back(p.left0);
		trackPoints.push_back(p.bl1);
		trackPoints.push_back(p.bl2);
		trackPoints.push_back(p.bl3);
		trackPoints.push_back(p.bl4);
		trackPoints.push_back(p.bl5);
		trackPoints.push_back(p.bl6);
		trackPoints.push_back(p.bl0);
		trackPoints.push_back(p.br1);
		trackPoints.push_back(p.br2);
		trackPoints.push_back(p.br3);
		trackPoints.push_back(p.br4);
		trackPoints.push_back(p.br5);
		trackPoints.push_back(p.br6);
		trackPoints.push_back(p.br0);
		trackPoints.push_back(p.right1);
		trackPoints.push_back(p.right2);
		trackPoints.push_back(p.right3);
		trackPoints.push_back(p.right4);
		trackPoints.push_back(p.right5);
		trackPoints.push_back(p.right6);
		trackPoints.push_back(p.right0);
		cameraTrack.push_back(p.cp);
	}

	void track_connecter() {
		int sp = trackPoints.size() - 28;//stand for the starting point of this track part
		indices.push_back(sp + 2);
		indices.push_back(sp + 3);
		indices.push_back(sp + 13);

		indices.push_back(sp + 3);
		indices.push_back(sp + 13);
		indices.push_back(sp + 12);

		indices.push_back(sp + 8);
		indices.push_back(sp + 9);
		indices.push_back(sp + 19);

		indices.push_back(sp + 9);
		indices.push_back(sp + 19);
		indices.push_back(sp + 18);

		indices.push_back(sp + 14);
		indices.push_back(sp + 15);
		indices.push_back(sp + 25);

		indices.push_back(sp + 15);
		indices.push_back(sp + 25);
		indices.push_back(sp + 24);
	}

	// Here is the class where you will make the vertices or positions of the necessary objects of the track
	//  For example, to make a basic roller coster:
	//    First, make the vertices for each rail here (and indices for the EBO if you do it that way which is highly suggested).  
	//        You need the XYZ world coordinates, the Normal Coordinates, and the texture coordinates.
	//        The normal coordinates are necessary for the lighting to work.  
	//    Second, make vector of transformations for the planks across the rails
	void create_track()
	{
		//initialize a position to locate the track
		glm::vec3 currentpos = track_position;	//the position

		// iterate throught  the points	g_Track.points() returns the vector containing all the control points
		for (pointVectorIter ptsiter = g_Track.points().begin(); ptsiter != g_Track.points().end(); ptsiter++)
		{
			currentpos = glm::vec3(*ptsiter);
 
			controlPoints.push_back(currentpos);
		}


		int A, B, C, D, size = controlPoints.size();
		glm::vec3 pre = interpolate(controlPoints[size - 1], controlPoints[0], controlPoints[1], controlPoints[2], 0.5f, 0.99);
		glm::vec3 cur = interpolate(controlPoints[size - 1], controlPoints[0], controlPoints[1], controlPoints[2], 0.5f, 0.99);
		glm::vec3 next;

		for (size_t p = 0; p < size; p++)
		{
			//in order to connect the end of the track to the beginning, we use the following as control point set.
			A = p % size;
			B = (p + 1) % size;
			C = (p + 2) % size;
			D = (p + 3) % size;
			
			for (float i = 0.00f; i < 0.99f; i += 0.01f)
			{
				next = interpolate(controlPoints[A], controlPoints[B], controlPoints[C], controlPoints[D], 0.5f, i);
				
				//call make track part function to construct a slice of track
				make_track_part(pre, cur, next, i);// angle, i);

				//add connecter within track
				if ((i > 0.11f && i < 0.13f) || (i > 0.36f && i < 0.38f) || (i > 0.61f && i < 0.63f)|| (i > 0.86f && i < 0.88f)) 
					track_connecter();
				
				//record for next iteration
				pre = cur;
				cur = next;
			}
		}
		
	}

	void setup_track()
	{
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
		glBufferData(GL_ARRAY_BUFFER, trackPoints.size() * sizeof(Track_point), &trackPoints[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Track_point), (void*)0);
		// vertex normal coords
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Track_point), (void*)offsetof(Track_point, Normal));

		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Track_point), (void*)offsetof(Track_point, TexCoords));

		glBindVertexArray(0);
	
	}

};
