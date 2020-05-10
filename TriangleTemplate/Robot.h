#pragma once
#include "Common.h"
#include "Model.h"
#include "Constant.h"
#include <vector>
#include "Shader.h"
using namespace glm;

class Robot
{
public:
	Robot();
	// call after glewInit is called
	void initModels();

	// start some action
	void StartWalk();
	// keep doing it very frame
	void DoWalkAction();

	// draw all models
	void Draw(Shader shader);
	// self update
	void Update();

private:
	vector<Model> robot;
	// relation between each part
	// eg:
	// BODY -> LHAND0, RHAND0, NECK, LBODY
	vector<vector<int>> relations;

	// current time modelMatrix
	vector<mat4> modelMats;
	// default relatedPosition
	vector<vec3> relativePositions;
	// translate
	vector<vec3> translatePos;
	// rotationX
	vector<vec3> rotations;

	// reset all translate, rotation to default
	void reset();
};

