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

struct ActionStack {
	int bodyPartId;
	vec3 translationV;
	vec3 rotationV;

	ActionStack() {
		bodyPartId = -1;
		translationV = vec3();
		rotationV = vec3();
	}

	ActionStack(int b, vec3 t, vec3 r) {
		bodyPartId = b;
		translationV = t;
		rotationV = r;
	}
};

enum RobotState {
	DEFAULT = 0,
	WALK = 1,
	JUMP = 2,
	CLAP = 3,
	ORZ = 4,
	DANCE = 5,
	SHOOT = 6,
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
	// action needed var
	double previousPassTime;
	double previousStateIndex;

	// walk
	void DoWalkAction();
	// jump
	void DoJumpAction();
	// bow down
	void DoClapAction();
	// attack
	void DoDanceAction();
	// shoot
	void DoShootAction();

	// action helper
	void actionHelper(int bodyPart, double passTime, vector<float>timerPerState, int stateIndex, vec3 addedTranslation, vec3 addedRotation);
	// calculate all actionStack depend on actionState
	void calculateActionStack(vector<vector<ActionStack>> actionStack, int actionState, double passTime, vector<float>timerPerState);
};

