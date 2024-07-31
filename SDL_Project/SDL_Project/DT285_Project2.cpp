// cs250project1.empty.cpp
// -- template for first draft of the final project
// cs250 5/15

#include <sstream>
#include <cstdlib>
#include <SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>

#include "SnubDodecMesh.h"
#include "CubeMesh.h"
#include "Projection.h"
#include "DT285_DrawingCam.h"
#include "HalfSpace.h"

using namespace std;


int width = 700, height = 700;
const char* name = "Final Project First Draft";
double time_last;
double current_time;
int frame_count;
double frame_time;
bool should_close = false;
SDL_Window* window;

const Point O(0, 0, 0);
const Vector ex(1, 0, 0),
ey(0, 1, 0),
ez(0, 0, 1);
const float PI = 4.0f * atan(1.0f);

std::vector<Camera> cameras;

Camera cam2_0, cam3_0;
int currentCameraIndex = 0;

Point cam1_Distance(0, 0, 4.0f);

SnubDodecMesh snub; //earth and moon mesh
Affine snub2worldEarth, snub2worldMoon;

float earth_rot_rate = 2 * PI / 15.0f;
Vector earth_rot_axis(0, 1, 0);
Point earth_center(0, 0, 0);

float moon_rot_rate = 2 * PI / 5.0f;
Vector moon_rot_axis(0, 1, 0);
Point moon_center(0, 0, -6);

vector<Point> temp_world_verts,
temp_cam_verts,
temp_ndc_verts;

Vector snub_move_rate;
float snub_rot_rate;
Affine snub_to_world;

bool draw_solid = true;

float moon_scale;
float earth_scale;

float aspect = float(width) / float(height);

Vector cam2LookVector;

//CUBES
CubeMesh cube;

std::vector<Affine> cube2world;

float cubeDimensions = 0.3f; //uniform cube
float cubeSpacing = 0.03f;

//dimensions including spacing
float effectiveDimensions = cubeDimensions + cubeSpacing;

float wallDistance = 2.5f; //distance from the center to each wall
float wallHeight = effectiveDimensions * 5; //total height of wall

//OBJECT COLORS

Vector earthColor = Vector(0.4f, 0.8f, 0.4f);
Vector moonColor = Vector(0.5f, 0.6f, 0.7f);

Vector leftCubeClr = Vector(0.68f, 0.85f, 0.9f);
Vector rightCubeClr = Vector(0.7f, 0.6f, 0.9f);

void Resized(int W, int H) {
	width = W;
	height = H;
	glViewport(0, 0, W, H);
}

float frand(float a = 0, float b = 1) {
	return a + (b - a) * float(rand()) / float(RAND_MAX);
}

void Init(void) {
	srand(unsigned(time(0)));
	glEnable(GL_DEPTH_TEST);
	time_last = float(SDL_GetTicks() / 1000.0f);
	frame_count = 0;
	frame_time = 0;
	current_time = 0;
	Resized(width, height);


	//Full view camera - Cam 1

	Vector pairVector = moon_center - earth_center;
	float pairDistance = abs(pairVector);

	float camDistance = pairDistance * 0.7f;

	Vector cameraPositionVector = camDistance * ez;
	Point cameraPosition = earth_center + cameraPositionVector; 

	Vector lookVector = -ez;

	cameras.push_back(Camera(cameraPosition, lookVector, Vector(0, 1, 0), 0.5f * PI, aspect, 0.01f, 20.0f));

	//CAM 2
	cam2LookVector = earth_center - snub.Center();
	
	cam2_0 = Camera(snub.Center(), cam2LookVector, ey, 0.5f * PI, aspect, 0.01f, 20.0f);
	cameras.push_back(cam2_0);

	//CAM 3
	cam3_0 = Camera(snub.Center(), -cam2LookVector, ey, 0.5f * PI, aspect, 0.01f, 20.0f);
	cameras.push_back(cam3_0);

	//EARTH & MOON TRANSFORMATIONS

	earth_scale = 3.0f;

	snub2worldEarth = Trans(earth_center - O) 
					* Scale(2.0f / earth_scale) 
					* Trans(O - snub.Center());

	moon_scale = earth_scale * 2.5f;

	snub_move_rate = 2 * PI * Vector(1 / frand(5, 15), 1 / frand(5, 15), 1 / frand(5, 15));
	snub_rot_rate = 2 * PI / frand(5, 15);

	// LEFT Wall
	for (int i = 0; i < 5; ++i) { 
		for (int j = 0; j < 5; ++j) {

			Point cubePosition = Point(-wallDistance - cubeDimensions / 2,
				effectiveDimensions * i - wallHeight / 2,
				effectiveDimensions * j - (cubeDimensions * 5 + cubeSpacing * 4) / 2);

			cube2world.push_back(Trans(cubePosition - O)
				* Scale(0.1f / cube.Dimensions().x,
					cubeDimensions / cube.Dimensions().y,
					cubeDimensions / cube.Dimensions().z)
				* Trans(O - cube.Center()));

		}
	}

	// RIGHT WALL
	for (int i = 0; i < 5; ++i) {
		for (int j = 0; j < 5; ++j) { 

			Point cubePosition = Point(wallDistance + cubeDimensions / 2,
				effectiveDimensions * i - wallHeight / 2,
				effectiveDimensions * j - (cubeDimensions * 5 + cubeSpacing * 4) / 2);

			cube2world.push_back(Trans(cubePosition - O)
				* Scale(0.1f / cube.Dimensions().x,
					cubeDimensions / cube.Dimensions().y,
					cubeDimensions / cube.Dimensions().z)
				* Trans(O - cube.Center()));

		}
	}

	InitBuffer();

}


void Draw(void) {
	float t = float(SDL_GetTicks() / 1000.0f);
	float dt = t - time_last;
	time_last = t;
	current_time += dt;

	// frame rate
	++frame_count;
	frame_time += dt;
	if (frame_time >= 0.5) {
		double fps = frame_count / frame_time;
		frame_count = 0;
		frame_time = 0;
		stringstream ss;
		ss << name << " [fps=" << int(fps) << "]";
		SDL_SetWindowTitle(window, ss.str().c_str());
	}

	// clear the screen
	glClearColor(1, 1, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	// clear the z-buffer
	glClearDepth(1);
	glClear(GL_DEPTH_BUFFER_BIT);

	// EARTH TRANSFORMATIONS
	snub2worldEarth = Trans(earth_center - O)
		* Rot(earth_rot_rate * dt, earth_rot_axis)
		* Trans(O - earth_center)
		* snub2worldEarth;


	//MOON 2 TRANSFORMATION

	Point P = O + sin(snub_move_rate.x * current_time) * ex
		+ 0.5f * sin(snub_move_rate.y * current_time) * ey
		+ sin(snub_move_rate.z * current_time) * ez;
	Vector vel = snub_move_rate.x * cos(snub_move_rate.x * current_time) * ex
		+ 0.5f * snub_move_rate.y * cos(snub_move_rate.y * current_time) * ey
		+ snub_move_rate.z * cos(snub_move_rate.z * current_time) * ez,
		axis = vel;
	axis.Normalize();

	snub_to_world = Trans(P - O)
		* Rot(acos(axis.y), cross(ey, axis))
		//* Rot(snub_rot_rate*current_time,ey)
		* Scale(1 / snub.Dimensions().x,
			2 / snub.Dimensions().y,
			1 / snub.Dimensions().z)
		* Trans(O - snub.Center());


	Point moon_current_position = Point(snub_to_world[0][3], snub_to_world[1][3], snub_to_world[2][3]);

	cam2LookVector = earth_center - cameras[1].Eye();

	cameras[1].SetEyePosition(moon_current_position);
	cameras[1].SetLookVector(cam2LookVector, ey);

	cameras[2].SetEyePosition(moon_current_position);
	cameras[2].SetLookVector(-cam2LookVector, ey);

	Camera Cam = cameras[currentCameraIndex];


	// test if center of earth is inside moving object
	bool contains = true;
	temp_world_verts.clear();
	for (int i = 0; i < snub.VertexCount(); ++i)
		temp_world_verts.push_back(snub_to_world * snub.GetVertex(i));

	for (int i = 0; contains && i < snub.FaceCount(); ++i) {
		const Mesh::Face& face = snub.GetFace(i);
		const Point& A = temp_world_verts[face.index1],
			B = temp_world_verts[face.index2],
			C = temp_world_verts[face.index3];
		Hcoords h = HalfSpace(A, B, C, P);
		contains = dot(h, O) <= 0;
	}

	Vector moon_color = contains ? Vector(1, 0, 1) : moonColor;

	if (draw_solid) {
		DisplayFaces(snub, snub2worldEarth, Cam, earthColor);
		DisplayFaces(snub, snub_to_world, Cam, moon_color);
	}	
	else {
		DisplayEdges(snub, snub2worldEarth, Cam, earthColor);
		DisplayEdges(snub, snub_to_world, Cam, moonColor);
	}

	for (int i = 0; i < 25; i++) {
		if (draw_solid)
			DisplayFaces(cube, cube2world[i], Cam, leftCubeClr);
		else
			DisplayEdges(cube, cube2world[i], Cam, leftCubeClr);
	}

	for (int i = 25; i < 50; i++) {
		if (draw_solid)
			DisplayFaces(cube, cube2world[i], Cam, rightCubeClr);
		else
			DisplayEdges(cube, cube2world[i], Cam, rightCubeClr);
	}




}

void key_pressed(SDL_Keycode keycode) {
	if (keycode == SDLK_ESCAPE) {
		should_close = true;
	}
	switch (keycode) {
	case '\x1b':
		exit(0);
		break;
	case '3':
		draw_solid = !draw_solid;
		break;
	case ' ':  // KEY 3 SWITCHES CAMERA
		currentCameraIndex = (currentCameraIndex + 1) % 3;
		break;
	}

}


int main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	SDL_GLContext context = SDL_GL_CreateContext(window);
	Resized(width, height);
	// GLEW
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		printf("ERROR: %s\n", glewGetErrorString(err));
	}

	Init();

	while (!should_close) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				should_close = true;
				break;
			case SDL_KEYDOWN:
				key_pressed(event.key.keysym.sym);
				break;
			}
		}
		Draw();
		SDL_GL_SwapWindow(window);
	}

	return 0;
}




