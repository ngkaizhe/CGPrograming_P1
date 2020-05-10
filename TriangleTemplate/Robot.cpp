#include "Robot.h"

Robot::Robot() {
	// init model(robot part)
	robot = vector<Model>(12);

	// initialize everything
	for (int i = 0; i < TOTALPART; i++) {
		modelMats.push_back(mat4(1.0f));
		relativePositions.push_back(vec3(1.0f));
		translatePos.push_back(vec3(0.0f));
		rotations.push_back(vec3(0.0f));
		relations.push_back(vector<int>{});
	}

	// init relativePosition
	relativePositions[BODY] = vec3(0.0, 1.0f, 0.0);
	relativePositions[LBODY] = vec3(0.0, -.585, 0.066);

	relativePositions[RLEG0] = vec3(-0.176, -0.088, 0.009);
	relativePositions[RLEG1] = vec3(-0.08, -0.828, -0.185);
	relativePositions[LLEG0] = vec3(-1, 1, 1) * relativePositions[RLEG0];
	relativePositions[LLEG1] = vec3(-1, 1, 1) * relativePositions[RLEG1];

	relativePositions[LHAND0] = vec3(0.418, 0.265, -0.102);
	relativePositions[LHAND1] = vec3(0.379, -0.386, 0.001);
	relativePositions[RHAND0] = vec3(-1, 1, 1) * relativePositions[LHAND0];
	relativePositions[RHAND1] = vec3(-1, 1, 1) * relativePositions[LHAND1];
	relativePositions[NECK] = vec3(0.0, 0.453, -0.044);
	relativePositions[HEAD] = vec3(-.001, 0.143, -0.028);
	
	// init relationship between difference part
	relations[BODY].push_back(NECK);
	relations[BODY].push_back(LHAND0);
	relations[BODY].push_back(RHAND0);
	relations[BODY].push_back(LBODY);

	relations[NECK].push_back(HEAD);

	relations[LHAND0].push_back(LHAND1);

	relations[RHAND0].push_back(RHAND1);

	relations[LBODY].push_back(LLEG0);
	relations[LBODY].push_back(RLEG0);

	relations[LLEG0].push_back(LLEG1);
	
	relations[RLEG0].push_back(RLEG1);

	Update();
}

void Robot::initModels() {
	robot[BODY] = Model("../Assets/objects/spiderman/Body.obj");
	robot[HEAD] = Model("../Assets/objects/spiderman/Head.obj");
	robot[LBODY] = Model("../Assets/objects/spiderman/LBody.obj");
	robot[LHAND0] = Model("../Assets/objects/spiderman/LHAND0.obj");
	robot[LHAND1] = Model("../Assets/objects/spiderman/LHAND1.obj");
	robot[LLEG0] = Model("../Assets/objects/spiderman/LLeg0.obj");
	robot[LLEG1] = Model("../Assets/objects/spiderman/LLeg1.obj");
	robot[NECK] = Model("../Assets/objects/spiderman/Neck.obj");
	robot[RHAND0] = Model("../Assets/objects/spiderman/RHand0.obj");
	robot[RHAND1] = Model("../Assets/objects/spiderman/RHand1.obj");
	robot[RLEG0] = Model("../Assets/objects/spiderman/RLeg0.obj");
	robot[RLEG1] = Model("../Assets/objects/spiderman/RLeg1.obj");
}

// start some action
void Robot::StartWalk() {

}

// keep doing it very frame
void Robot::DoWalkAction() {

}

// draw all models
void Robot::Draw(Shader shader) {
	// draw each models after setting the model mat
	for (int i = 0; i < modelMats.size(); i++) {
		shader.setUniformMatrix4fv("model", modelMats[i]);
		robot[i].Draw(shader);
	}
}

// self update
void Robot::Update() {
	// we will update all model mats depends on its parent relationship
	// we should do all operation for body first
	modelMats[BODY] = translate(mat4(1.0f), relativePositions[BODY]);
	modelMats[BODY] = translate(modelMats[BODY], translatePos[BODY]);

	modelMats[BODY] = rotate(modelMats[BODY], radians(rotations[BODY].x), vec3(1.0, 0.0, 0.0));
	modelMats[BODY] = rotate(modelMats[BODY], radians(rotations[BODY].y), vec3(0.0, 1.0, 0.0));
	modelMats[BODY] = rotate(modelMats[BODY], radians(rotations[BODY].z), vec3(0.0, 0.0, 1.0));

	for (int i = 0; i < relations.size(); i++) {
		for (int j = 0; j < relations[i].size(); j++) {
			int childID = relations[i][j];
			// relative
			modelMats[childID] = translate(modelMats[i], relativePositions[childID]);

			// self translation and rotation
			modelMats[childID] = translate(modelMats[childID], translatePos[childID]);

			modelMats[childID] = rotate(modelMats[childID], radians(rotations[childID].x),  vec3(1.0, 0.0, 0.0));
			modelMats[childID] = rotate(modelMats[childID], radians(rotations[childID].y), vec3(0.0, 1.0, 0.0));
			modelMats[childID] = rotate(modelMats[childID], radians(rotations[childID].z), vec3(0.0, 0.0, 1.0));
		}
	}
}

// reset
void Robot::reset() {

}