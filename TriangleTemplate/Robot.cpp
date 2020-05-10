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

	// init timer
	timer = chrono::steady_clock::now();
}

void Robot::InitModels() {
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
	ActionUpdate();
	ModelMatUpdate();
}

// model mat update
void Robot::ModelMatUpdate(){
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

			modelMats[childID] = rotate(modelMats[childID], radians(rotations[childID].x), vec3(1.0, 0.0, 0.0));
			modelMats[childID] = rotate(modelMats[childID], radians(rotations[childID].y), vec3(0.0, 1.0, 0.0));
			modelMats[childID] = rotate(modelMats[childID], radians(rotations[childID].z), vec3(0.0, 0.0, 1.0));
		}
	}
}

// action update
void Robot::ActionUpdate() {
	switch (this->robotState) {
		case RobotState::DEFAULT:
			break;
		case RobotState::WALK:
			this->DoWalkAction();
			break;
		case RobotState::JUMP:
			this->DoJumpAction();
			break;
		default:
			break;
	}
}

// state transition
void Robot::setState(RobotState toState) {
	// reset all position
	if (this->robotState != toState) {
		this->reset();

		// reset timer that used for state
		timer = chrono::steady_clock::now();
		
		// reset state transition used var
		previousPassTime = 0;
		previousStateIndex = 0;
	}

	// set the state
	this->robotState = toState;
}

// shader transition
void Robot::setShader(ShaderMode shaderMode) {

}

// reset
void Robot::reset() {
	for (int i = 0; i < TOTALPART; i++) {
		translatePos[i] = vec3(0.0f);
		rotations[i] = vec3(0.0f);
	}
}

// determine which state currently by the action
int Robot::stateDetermination(vector<float>timeNeededPerState, double& passSeconds, int repeatStartIndex = 0) {
	double elapsedSeconds = ((chrono::duration<double>)(chrono::steady_clock::now() - timer)).count();
	
	// calculated the unrepeat part
	float unrepeatTotalTime = 0;
	for (int i = 0; i < repeatStartIndex; i++) unrepeatTotalTime += timeNeededPerState[i];
	// check whether the time dropped inside the unrepeat range
	if (elapsedSeconds < unrepeatTotalTime) {
		float tempTimer = 0;
		for (int i = 0; i < repeatStartIndex; i++) {
			tempTimer += timeNeededPerState[i];
			if (tempTimer > elapsedSeconds) {
				passSeconds = elapsedSeconds - (tempTimer - timeNeededPerState[i]);
				return i;
			}
		}
		// shouldn't run till here
		throw "The Elapsed Second is smaller than the unrepeatTotalTime, but the state wasn't found in the loop!!!";
	}

	// cut off the unrepeat part
	elapsedSeconds -= unrepeatTotalTime;
	// calculated the repeat part
	float repeatTotalTime = 0;
	for (int i = repeatStartIndex; i < timeNeededPerState.size(); i++) repeatTotalTime += timeNeededPerState[i];
	// find remainder
	while (elapsedSeconds >= repeatTotalTime) elapsedSeconds -= repeatTotalTime;
	// check whether the time dropped inside the repeat range
	float tempTimer = 0;
	for (int i = repeatStartIndex; i < timeNeededPerState.size(); i++) {
		tempTimer += timeNeededPerState[i];
		if (tempTimer > elapsedSeconds) {
			passSeconds = elapsedSeconds - (tempTimer - timeNeededPerState[i]);
			return i;
		}
	}

	// should run till here
	throw "Can't find any state even after repeat part calculated!!!";
}

// keep doing it very frame
void Robot::DoWalkAction() {
	// state 0 -> 準備動作
	// state 1 -> 右手上升，左右下降
	// state 2 -> 右手下降至中心，左手上升至中心
	// state 3 -> 左手上升，右手下降
	// state 4 -> 左手下降至中心，右手上升至中心
	// regular procedure
	// 0->1->2->3->4->1->2->3->4...

	// determine which walk state we are in
	// by using the std::chrono func provided by c++
	vector<float>timerPerState = { 0.5, 1, 1, 1, 1 };
	double passTime;
	int walkState = stateDetermination(timerPerState, passTime, 1);
	// cout << "Walk State is " << walkState << '\n';

	// if the state wasn't same against the previousStateIndex we reset the previousPassTime
	if (walkState != previousStateIndex) {
		previousStateIndex = walkState;
		previousPassTime = 0;
	}

	// recalculate the passTime to be interval between previousPassTime instead of the state time
	double t = passTime;
	passTime -= previousPassTime;
	previousPassTime = t;

	// state 0 -> 準備動作
	if (walkState == 0) {
		double totalYTranslation = -0.038;
		double totalZRotation = -36;

		// find the current rotation needed to add
		double zRotationNeeded = passTime / timerPerState[walkState] * totalZRotation;
		// find the translation needed to add
		double yTranslatioNeeded = passTime / timerPerState[walkState] * totalYTranslation;

		this->translatePos[LHAND0].y += yTranslatioNeeded;
		this->rotations[LHAND0].z += zRotationNeeded;

		this->translatePos[RHAND0].y += yTranslatioNeeded;
		// there other side, the rotation signed will be different
		this->rotations[RHAND0].z += -zRotationNeeded;
	}

	// state 1 -> 右手上升，左右下降
	else if (walkState == 1) {
		// hand part
		double totalYTranslation = -0.044;
		double totalXRotation = -35;
		double totalYRotation = -22;
		double totalZRotation = 6;

		double xRotationNeeded = passTime / timerPerState[walkState] * totalXRotation;
		double yRotationNeeded = passTime / timerPerState[walkState] * totalYRotation;
		double zRotationNeeded = passTime / timerPerState[walkState] * totalZRotation;
		// find the translation needed to add
		double yTranslatioNeeded = passTime / timerPerState[walkState] * totalYTranslation;

		// left hand part
		this->translatePos[LHAND0].y += yTranslatioNeeded;
		this->rotations[LHAND0].x += xRotationNeeded;
		this->rotations[LHAND0].y += yRotationNeeded;
		this->rotations[LHAND0].z += zRotationNeeded;
		// right hand part
		this->translatePos[RHAND0].y += yTranslatioNeeded;
		this->rotations[RHAND0].x += -xRotationNeeded;
		this->rotations[RHAND0].y += yRotationNeeded;
		this->rotations[RHAND0].z += -zRotationNeeded;


		// leg part
		totalXRotation = -20;
		xRotationNeeded = passTime / timerPerState[walkState] * totalXRotation;

		// left leg
		this->rotations[LLEG0].x += xRotationNeeded;
		// right leg
		this->rotations[RLEG0].x += -xRotationNeeded;
	}

	// state 2 -> 右手下降至中心，左手上升至中心
	else if (walkState == 2) {
		double totalYTranslation = 0.044;
		double totalXRotation = 35;
		double totalYRotation = 22;
		double totalZRotation = -6;

		double xRotationNeeded = passTime / timerPerState[walkState] * totalXRotation;
		double yRotationNeeded = passTime / timerPerState[walkState] * totalYRotation;
		double zRotationNeeded = passTime / timerPerState[walkState] * totalZRotation;
		// find the translation needed to add
		double yTranslatioNeeded = passTime / timerPerState[walkState] * totalYTranslation;

		// left part
		this->translatePos[LHAND0].y += yTranslatioNeeded;
		this->rotations[LHAND0].x += xRotationNeeded;
		this->rotations[LHAND0].y += yRotationNeeded;
		this->rotations[LHAND0].z += zRotationNeeded;
		// right part
		this->translatePos[RHAND0].y += yTranslatioNeeded;
		this->rotations[RHAND0].x += -xRotationNeeded;
		this->rotations[RHAND0].y += yRotationNeeded;
		this->rotations[RHAND0].z += -zRotationNeeded;

		// leg part
		totalXRotation = 20;
		xRotationNeeded = passTime / timerPerState[walkState] * totalXRotation;

		// left leg
		this->rotations[LLEG0].x += xRotationNeeded;
		// right leg
		this->rotations[RLEG0].x += -xRotationNeeded;
	}

	// state 3 -> 左手上升，右手下降
	else if (walkState == 3) {
		double totalYTranslation = -0.044;
		double totalXRotation = 40;
		double totalYRotation = 25;
		double totalZRotation = 11;

		double xRotationNeeded = passTime / timerPerState[walkState] * totalXRotation;
		double yRotationNeeded = passTime / timerPerState[walkState] * totalYRotation;
		double zRotationNeeded = passTime / timerPerState[walkState] * totalZRotation;
		// find the translation needed to add
		double yTranslatioNeeded = passTime / timerPerState[walkState] * totalYTranslation;

		// left part
		this->translatePos[LHAND0].y += yTranslatioNeeded;
		this->rotations[LHAND0].x += xRotationNeeded;
		this->rotations[LHAND0].y += yRotationNeeded;
		this->rotations[LHAND0].z += zRotationNeeded;
		// right part
		this->translatePos[RHAND0].y += yTranslatioNeeded;
		this->rotations[RHAND0].x += -xRotationNeeded;
		this->rotations[RHAND0].y += yRotationNeeded;
		this->rotations[RHAND0].z += -zRotationNeeded;

		// leg part
		totalXRotation = 20;
		xRotationNeeded = passTime / timerPerState[walkState] * totalXRotation;

		// left leg
		this->rotations[LLEG0].x += xRotationNeeded;
		// right leg
		this->rotations[RLEG0].x += -xRotationNeeded;
	}

	// state 4 -> 左手下降至中心，右手上升至中心
	else if (walkState == 4) {
		double totalYTranslation = 0.044;
		double totalXRotation = -40;
		double totalYRotation = -25;
		double totalZRotation = -11;

		double xRotationNeeded = passTime / timerPerState[walkState] * totalXRotation;
		double yRotationNeeded = passTime / timerPerState[walkState] * totalYRotation;
		double zRotationNeeded = passTime / timerPerState[walkState] * totalZRotation;
		// find the translation needed to add
		double yTranslatioNeeded = passTime / timerPerState[walkState] * totalYTranslation;

		// left part
		this->translatePos[LHAND0].y += yTranslatioNeeded;
		this->rotations[LHAND0].x += xRotationNeeded;
		this->rotations[LHAND0].y += yRotationNeeded;
		this->rotations[LHAND0].z += zRotationNeeded;
		// right part
		this->translatePos[RHAND0].y += yTranslatioNeeded;
		this->rotations[RHAND0].x += -xRotationNeeded;
		this->rotations[RHAND0].y += yRotationNeeded;
		this->rotations[RHAND0].z += -zRotationNeeded;

		// leg part
		totalXRotation = -20;
		xRotationNeeded = passTime / timerPerState[walkState] * totalXRotation;

		// left leg
		this->rotations[LLEG0].x += xRotationNeeded;
		// right leg
		this->rotations[RLEG0].x += -xRotationNeeded;
	}
}

// jump
void Robot::DoJumpAction() {
	// state 0 -> 準備動作
	// state 1 -> 雙手往後，蹲
	// state 2 -> 跳上去
	// state 3 -> 回去ready pos
	// regular procedure
	// 0->1->2->3->1->2->3...

	// determine which jump state we are in
	// by using the std::chrono func provided by c++
	vector<float>timerPerState = { 0.5, 4, 0.5, 2};
	double passTime;
	int jumpState = stateDetermination(timerPerState, passTime, 1);

	// if the state wasn't same against the previousStateIndex we reset the previousPassTime
	if (jumpState != previousStateIndex) {
		previousStateIndex = jumpState;
		previousPassTime = 0;
	}

	// recalculate the passTime to be interval between previousPassTime instead of the state time
	double t = passTime;
	passTime -= previousPassTime;
	previousPassTime = t;

	// state 0 -> 準備動作
	if (jumpState == 0) {
		double totalYTranslation = -0.038;
		double totalZRotation = -36;

		// find the current rotation needed to add
		double zRotationNeeded = passTime / timerPerState[jumpState] * totalZRotation;
		// find the translation needed to add
		double yTranslatioNeeded = passTime / timerPerState[jumpState] * totalYTranslation;

		this->translatePos[LHAND0].y += yTranslatioNeeded;
		this->rotations[LHAND0].z += zRotationNeeded;

		this->translatePos[RHAND0].y += yTranslatioNeeded;
		// there other side, the rotation signed will be different
		this->rotations[RHAND0].z += -zRotationNeeded;
	}

	// state 1 -> 雙手往後，蹲
	else if (jumpState == 1) {
		// hand0 part
		{
			double totalYTranslation = -0.039;
			double totalXRotation = 60;
			double totalYRotation = 32;
			double totalZRotation = 19;

			// find the current rotation needed to add
			double zRotationNeeded = passTime / timerPerState[jumpState] * totalZRotation;
			double yRotationNeeded = passTime / timerPerState[jumpState] * totalYRotation;
			double xRotationNeeded = passTime / timerPerState[jumpState] * totalXRotation;
			// find the translation needed to add
			double yTranslatioNeeded = passTime / timerPerState[jumpState] * totalYTranslation;

			this->translatePos[LHAND0].y += yTranslatioNeeded;
			this->rotations[LHAND0].x += xRotationNeeded;
			this->rotations[LHAND0].y += yRotationNeeded;
			this->rotations[LHAND0].z += zRotationNeeded;

			this->translatePos[RHAND0].y += yTranslatioNeeded;
			this->rotations[RHAND0].x += xRotationNeeded;
			this->rotations[RHAND0].y += -yRotationNeeded;
			this->rotations[RHAND0].z += -zRotationNeeded;
		}
		

		// hand1 part
		{
			double totalXRotation = -21;
			double totalYRotation = -15;
			double totalZRotation = 3;

			// find the current rotation needed to add
			double zRotationNeeded = passTime / timerPerState[jumpState] * totalZRotation;
			double yRotationNeeded = passTime / timerPerState[jumpState] * totalYRotation;
			double xRotationNeeded = passTime / timerPerState[jumpState] * totalXRotation;

			this->rotations[LHAND1].x += xRotationNeeded;
			this->rotations[LHAND1].y += yRotationNeeded;
			this->rotations[LHAND1].z += zRotationNeeded;

			this->rotations[RHAND1].x += xRotationNeeded;
			this->rotations[RHAND1].y += -yRotationNeeded;
			this->rotations[RHAND1].z += -zRotationNeeded;
		}

		// leg0 part
		{
			double totalYTranslate = 0.068;
			double totalZTranslate = -0.028;
			double totalXRotation = -121;

			// find the current rotation needed to add
			double yTranslationNeeded = passTime / timerPerState[jumpState] * totalYTranslate;
			double zTranslationNeeded = passTime / timerPerState[jumpState] * totalZTranslate;
			double xRotationNeeded = passTime / timerPerState[jumpState] * totalXRotation;

			this->translatePos[LLEG0].y += yTranslationNeeded;
			this->translatePos[LLEG0].z += zTranslationNeeded;
			this->rotations[LLEG0].x += xRotationNeeded;

			this->translatePos[RLEG0].y += yTranslationNeeded;
			this->translatePos[RLEG0].z += zTranslationNeeded;
			this->rotations[RLEG0].x += xRotationNeeded;
		}

		// leg1 part
		{
			double totalYTranslate = 0.15;
			double totalZTranslate = 0.2;
			double totalXRotation = 125;

			// find the current rotation needed to add
			double yTranslationNeeded = passTime / timerPerState[jumpState] * totalYTranslate;
			double zTranslationNeeded = passTime / timerPerState[jumpState] * totalZTranslate;
			double xRotationNeeded = passTime / timerPerState[jumpState] * totalXRotation;

			this->translatePos[LLEG1].y += yTranslationNeeded;
			this->translatePos[LLEG1].z += zTranslationNeeded;
			this->rotations[LLEG1].x += xRotationNeeded;

			this->translatePos[RLEG1].y += yTranslationNeeded;
			this->translatePos[RLEG1].z += zTranslationNeeded;
			this->rotations[RLEG1].x += xRotationNeeded;
		}

		// body
		{
			double totalYTranslation = -1;
			double totalXRotation = 20;

			double xRotationNeeded = passTime / timerPerState[jumpState] * totalXRotation;
			double yTranslationNeeded = passTime / timerPerState[jumpState] * totalYTranslation;

			this->rotations[BODY].x += xRotationNeeded;
			this->translatePos[BODY].y += yTranslationNeeded;
		}
	}

	// state 2 -> 跳上去
	else if (jumpState == 2) {
		// body
		{
			double totalYTranslation = 4;
			double totalXRotation = -20;

			double xRotationNeeded = passTime / timerPerState[jumpState] * totalXRotation;
			double yTranslationNeeded = passTime / timerPerState[jumpState] * totalYTranslation;

			this->rotations[BODY].x += xRotationNeeded;
			this->translatePos[BODY].y += yTranslationNeeded;
		}

		// hand0 part
		{
			double totalYTranslation = 0;
			double totalXRotation = -130;
			double totalYRotation = -62;
			double totalZRotation = 32;

			// find the current rotation needed to add
			double zRotationNeeded = passTime / timerPerState[jumpState] * totalZRotation;
			double yRotationNeeded = passTime / timerPerState[jumpState] * totalYRotation;
			double xRotationNeeded = passTime / timerPerState[jumpState] * totalXRotation;
			// find the translation needed to add
			double yTranslatioNeeded = passTime / timerPerState[jumpState] * totalYTranslation;

			this->translatePos[LHAND0].y += yTranslatioNeeded;
			this->rotations[LHAND0].x += xRotationNeeded;
			this->rotations[LHAND0].y += yRotationNeeded;
			this->rotations[LHAND0].z += zRotationNeeded;

			this->translatePos[RHAND0].y += yTranslatioNeeded;
			this->rotations[RHAND0].x += xRotationNeeded;
			this->rotations[RHAND0].y += -yRotationNeeded;
			this->rotations[RHAND0].z += -zRotationNeeded;
		}


		// hand1 part
		{
			double totalXRotation = -47;
			double totalYRotation = -20;
			double totalZRotation = 22;

			// find the current rotation needed to add
			double zRotationNeeded = passTime / timerPerState[jumpState] * totalZRotation;
			double yRotationNeeded = passTime / timerPerState[jumpState] * totalYRotation;
			double xRotationNeeded = passTime / timerPerState[jumpState] * totalXRotation;

			this->rotations[LHAND1].x += xRotationNeeded;
			this->rotations[LHAND1].y += yRotationNeeded;
			this->rotations[LHAND1].z += zRotationNeeded;

			this->rotations[RHAND1].x += xRotationNeeded;
			this->rotations[RHAND1].y += -yRotationNeeded;
			this->rotations[RHAND1].z += -zRotationNeeded;
		}

		// leg0 part
		{
			double totalYTranslate = -0.068;
			double totalZTranslate = 0.028;
			double totalXRotation = 121;

			// find the current rotation needed to add
			double yTranslationNeeded = passTime / timerPerState[jumpState] * totalYTranslate;
			double zTranslationNeeded = passTime / timerPerState[jumpState] * totalZTranslate;
			double xRotationNeeded = passTime / timerPerState[jumpState] * totalXRotation;

			this->translatePos[LLEG0].y += yTranslationNeeded;
			this->translatePos[LLEG0].z += zTranslationNeeded;
			this->rotations[LLEG0].x += xRotationNeeded;

			this->translatePos[RLEG0].y += yTranslationNeeded;
			this->translatePos[RLEG0].z += zTranslationNeeded;
			this->rotations[RLEG0].x += xRotationNeeded;
		}

		// leg1 part
		{
			double totalYTranslate = -0.15;
			double totalZTranslate = -0.2;
			double totalXRotation = -125;

			// find the current rotation needed to add
			double yTranslationNeeded = passTime / timerPerState[jumpState] * totalYTranslate;
			double zTranslationNeeded = passTime / timerPerState[jumpState] * totalZTranslate;
			double xRotationNeeded = passTime / timerPerState[jumpState] * totalXRotation;

			this->translatePos[LLEG1].y += yTranslationNeeded;
			this->translatePos[LLEG1].z += zTranslationNeeded;
			this->rotations[LLEG1].x += xRotationNeeded;

			this->translatePos[RLEG1].y += yTranslationNeeded;
			this->translatePos[RLEG1].z += zTranslationNeeded;
			this->rotations[RLEG1].x += xRotationNeeded;
		}
	}

	// state 3 -> 回去ready pos
	else if (jumpState == 3) {
		// body
		{
			double totalYTranslation = -3;

			double yTranslationNeeded = passTime / timerPerState[jumpState] * totalYTranslation;

			this->translatePos[BODY].y += yTranslationNeeded;
		}

		// hand0 part
		{
			double totalYTranslation = 0.039;
			double totalXRotation = 70;
			double totalYRotation = 30;
			double totalZRotation = -51;

			// find the current rotation needed to add
			double zRotationNeeded = passTime / timerPerState[jumpState] * totalZRotation;
			double yRotationNeeded = passTime / timerPerState[jumpState] * totalYRotation;
			double xRotationNeeded = passTime / timerPerState[jumpState] * totalXRotation;
			// find the translation needed to add
			double yTranslatioNeeded = passTime / timerPerState[jumpState] * totalYTranslation;

			this->translatePos[LHAND0].y += yTranslatioNeeded;
			this->rotations[LHAND0].x += xRotationNeeded;
			this->rotations[LHAND0].y += yRotationNeeded;
			this->rotations[LHAND0].z += zRotationNeeded;

			this->translatePos[RHAND0].y += yTranslatioNeeded;
			this->rotations[RHAND0].x += xRotationNeeded;
			this->rotations[RHAND0].y += -yRotationNeeded;
			this->rotations[RHAND0].z += -zRotationNeeded;
		}


		// hand1 part
		{
			double totalXRotation = 68;
			double totalYRotation = 35;
			double totalZRotation = -25;

			// find the current rotation needed to add
			double zRotationNeeded = passTime / timerPerState[jumpState] * totalZRotation;
			double yRotationNeeded = passTime / timerPerState[jumpState] * totalYRotation;
			double xRotationNeeded = passTime / timerPerState[jumpState] * totalXRotation;

			this->rotations[LHAND1].x += xRotationNeeded;
			this->rotations[LHAND1].y += yRotationNeeded;
			this->rotations[LHAND1].z += zRotationNeeded;

			this->rotations[RHAND1].x += xRotationNeeded;
			this->rotations[RHAND1].y += -yRotationNeeded;
			this->rotations[RHAND1].z += -zRotationNeeded;
		}
	}

}