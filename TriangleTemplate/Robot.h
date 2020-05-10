#pragma once
#include "Common.h"
#include "Model.h"
#include "Constant.h"
#include "Shader.h"

#include <vector>
#include<chrono>
#include<iostream>
using namespace glm;
using namespace std;

enum RobotState {
	DEFAULT,
	WALK,
	JUMP,
	BOWDOWN,
	ORZ,
	ATTACK,
	SHOOT,
};

enum ShaderMode {

};

class Robot
{
public:
	Robot();
	// call after glewInit is called
	void InitModels();

	// draw all models
	void Draw(Shader shader);

	// self update
	void Update();
	// model mat update
	void ModelMatUpdate();
	// action update
	void ActionUpdate();

	// state transition
	void setState(RobotState toState);

	// shader transition
	void setShader(ShaderMode shaderMode);

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

	// robot state
	RobotState robotState;

	

	// timer
	chrono::steady_clock::time_point timer;

	// helper function
	// determine which state currently by the action
	int stateDetermination(vector<float>timeNeededPerState, double& passSeconds, int repeatStartIndex);
	// reset all translate, rotation to default
	void reset();

	// actions functions
	// keep doing it very frame
	void DoWalkAction();
	// walk action needed var
	double previousPassTime;
	double previousStateIndex;
};

