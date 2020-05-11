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

	// init the sphere

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
			vec3 temp = relativePositions[childID] + translatePos[childID];
			modelMats[childID] = translate(modelMats[i], temp);

			modelMats[childID] = rotate(modelMats[childID], radians(rotations[childID].z), vec3(0.0, 0.0, 1.0));
			modelMats[childID] = rotate(modelMats[childID], radians(rotations[childID].y), vec3(0.0, 1.0, 0.0));
			modelMats[childID] = rotate(modelMats[childID], radians(rotations[childID].x), vec3(1.0, 0.0, 0.0));
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
		case RobotState::CLAP:
			this->DoClapAction();
			break;
		case RobotState::DANCE:
			this->DoDanceAction();
			break;
		case RobotState::SHOOT:
			this->DoShootAction();
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

// clap hand
void Robot::DoClapAction() {
	// state 0 -> 準備動作
	// state 1 -> 雙手準備
	// state 2 -> 雙手擊掌
	// state 3 -> 回去雙手準備的狀態
	// regular procedure
	// 0->1->2->3->2->3->2->3....

	// determine which bow state we are in
	// by using the std::chrono func provided by c++
	vector<float>timerPerState = { 2, 2, 0.1, 0.1};
	double passTime;
	int clapState = stateDetermination(timerPerState, passTime, 2);

	// if the state wasn't same against the previousStateIndex we reset the previousPassTime
	if (clapState != previousStateIndex) {
		previousStateIndex = clapState;
		previousPassTime = 0;
	}

	// recalculate the passTime to be interval between previousPassTime instead of the state time
	double t = passTime;
	passTime -= previousPassTime;
	previousPassTime = t;

	// actionStack[stateIndex]...
	vector<vector<ActionStack>> actionStack = 
	{
		// state 0 -> 準備動作
		{
			ActionStack(LHAND0, vec3(0, -0.038, 0), vec3(0, 0, -36)),
			ActionStack(LHAND1, vec3(0, 0, 0), vec3(0, 0, 0)),

			ActionStack(RHAND0, vec3(0, -0.038, 0), vec3(0, 0, 36)),
			ActionStack(RHAND1, vec3(0, 0, 0), vec3(0, 0, 0)),

			ActionStack(HEAD, vec3(0, 0, 0), vec3(0, 0, 0)),
		},
		// state 1 -> 雙手準備
		{
			ActionStack(LHAND0, vec3(0, -0.085, 0), vec3(-53, -34, -21)),
			ActionStack(LHAND1, vec3(0, 0, 0), vec3(-70, -3, -60)),

			ActionStack(RHAND0, vec3(0, -0.095, 0), vec3(-62, 32, 16)),
			ActionStack(RHAND1, vec3(0, 0, 0), vec3(-43, 72, -22)),

			ActionStack(HEAD, vec3(0, 0, 0), vec3(0, 0, 0)),
		},
		// state 2 -> 雙手擊掌
		{
			ActionStack(LHAND0, vec3(0, -0.085, 0), vec3(-53, -34, -21)),
			ActionStack(LHAND1, vec3(0, 0, 0), vec3(-38, -10, -93)),

			ActionStack(RHAND0, vec3(0, -0.095, 0), vec3(-62, 32, 16)),
			ActionStack(RHAND1, vec3(0, 0, 0), vec3(-43, 88, -25)),

			ActionStack(HEAD, vec3(0, 0, 0.015), vec3(15, 0, 0)),
		},
		// state 3 -> 回去雙手準備的狀態
		{
			ActionStack(LHAND0, vec3(0, -0.085, 0), vec3(-53, -34, -21)),
			ActionStack(LHAND1, vec3(0, 0, 0), vec3(-70, -3, -60)),

			ActionStack(RHAND0, vec3(0, -0.095, 0), vec3(-62, 32, 16)),
			ActionStack(RHAND1, vec3(0, 0, 0), vec3(-43, 72, -22)),

			ActionStack(HEAD, vec3(0, 0, 0), vec3(0, 0, 0)),
		},
	};

	calculateActionStack(actionStack, clapState, passTime, timerPerState);

	// example
	if (false) {
		// state 0 -> 準備動作
		if (clapState == 0) {
			// LHAND0
			this->actionHelper(LHAND0, passTime, timerPerState, clapState, vec3(0, -0.038, 0), vec3(0, 0, -36));
			// RHAND0
			this->actionHelper(RHAND0, passTime, timerPerState, clapState, vec3(0, -0.038, 0), vec3(0, 0, 36));
		}

		// state 1 -> 雙手準備
		else if (clapState == 1) {
			// LHAND0
			this->actionHelper(LHAND0, passTime, timerPerState, clapState, vec3(0, -0.085 + 0.038, 0), vec3(-53, -34, -21 + 36));
			// LHAND1
			this->actionHelper(LHAND1, passTime, timerPerState, clapState, vec3(0, 0, 0), vec3(-70, -3, -60));
			// RHAND0
			this->actionHelper(RHAND0, passTime, timerPerState, clapState, vec3(0, -0.095 + 0.038, 0), vec3(-62, 32, 16 - 36));
			// RHAND1
			this->actionHelper(RHAND1, passTime, timerPerState, clapState, vec3(0, 0, 0), vec3(-43, 72, -22));
		}

		// state 2 -> 雙手擊掌
		else if (clapState == 2) {
			// LHAND0
			this->actionHelper(LHAND0, passTime, timerPerState, clapState, vec3(0, 0, 0), vec3(0, 0, 0));
			// LHAND1
			this->actionHelper(LHAND1, passTime, timerPerState, clapState, vec3(0, 0, 0), vec3(-38 + 70, -10 + 3, -93 + 60));
			// RHAND0
			this->actionHelper(RHAND0, passTime, timerPerState, clapState, vec3(0, 0, 0), vec3(0, 0, 0));
			// RHAND1
			this->actionHelper(RHAND1, passTime, timerPerState, clapState, vec3(0, 0, 0), vec3(-43 + 43, 88 - 72, -25 + 22));

			// HEAD
			this->actionHelper(HEAD, passTime, timerPerState, clapState, vec3(0, 0, 0.015), vec3(15, 0, 0));
		}

		// state 3 -> 回去雙手準備的狀態
		else if (clapState == 3) {
			// LHAND0
			this->actionHelper(LHAND0, passTime, timerPerState, clapState, vec3(0, 0, 0), vec3(0, 0, 0));
			// LHAND1
			this->actionHelper(LHAND1, passTime, timerPerState, clapState, vec3(0, 0, 0), vec3(-1, -1, -1) * vec3(-38 + 70, -10 + 3, -93 + 60));
			// RHAND0
			this->actionHelper(RHAND0, passTime, timerPerState, clapState, vec3(0, 0, 0), vec3(0, 0, 0));
			// RHAND1
			this->actionHelper(RHAND1, passTime, timerPerState, clapState, vec3(0, 0, 0), vec3(-1, -1, -1) * vec3(-43 + 43, 88 - 72, -25 + 22));

			// HEAD
			this->actionHelper(HEAD, passTime, timerPerState, clapState, vec3(0, 0, -0.015), vec3(-15, 0, 0));
		}
	}
}

// dance
void Robot::DoDanceAction() {
	// state 0 -> 準備動作
	// state 1 -> 雙手1
	// state 2 -> 雙手2
	// state 3 -> 雙手3
	// state 4 -> 雙手4
	// state 5 -> 雙手1
	// regular procedure
	// 0->1->2->3->4->5->2->3->4->5->2->3->4->5....

	// determine which bow state we are in
	// by using the std::chrono func provided by c++
	vector<float>timerPerState = { 2, 0.3, 0.3, 0.3, 0.3, 0.3 };
	double passTime;
	int danceState = stateDetermination(timerPerState, passTime, 2);

	// if the state wasn't same against the previousStateIndex we reset the previousPassTime
	if (danceState != previousStateIndex) {
		previousStateIndex = danceState;
		previousPassTime = 0;
	}

	// recalculate the passTime to be interval between previousPassTime instead of the state time
	double t = passTime;
	passTime -= previousPassTime;
	previousPassTime = t;

	// actionStack[stateIndex]...
	vector<vector<ActionStack>> actionStack =
	{
		// state 0 -> 準備動作
		{
			ActionStack(LHAND0, vec3(0, -0.038, 0), vec3(0, 0, -36)),
			ActionStack(LHAND1, vec3(0, 0, 0), vec3(0, 0, 0)),

			ActionStack(RHAND0, vec3(0, -0.038, 0), vec3(0, 0, 36)),
			ActionStack(RHAND1, vec3(0, 0, 0), vec3(0, 0, 0)),

			ActionStack(BODY, vec3(0, 0, 0), vec3(0, 0, 0)),
			ActionStack(LBODY, vec3(0, 0, 0), vec3(0, 0, 0)),

			// leg 
			ActionStack(LLEG0, vec3(0, 0, 0), vec3(0, 0, 0)),
			ActionStack(LLEG1, vec3(0, 0, 0), vec3(0, 0, 0)),

			ActionStack(RLEG0, vec3(0, 0, 0), vec3(0, 0, 0)),
			ActionStack(RLEG1, vec3(0, 0, 0), vec3(0, 0, 0)),
		},

		// state 1 -> 雙手1
		{
			ActionStack(LHAND0, vec3(0, -0.038, 0), vec3(-93.348, -65.284, 35.294)),
			ActionStack(LHAND1, vec3(0, 0, 0), vec3(-62.43, -14.762, -40.297)),

			ActionStack(RHAND0, vec3(0.018, -0.097, -0.045), vec3(-85.224, 22.581, 12.621)),
			ActionStack(RHAND1, vec3(0, 0, 0), vec3(-20.815, 8.708, 101.014)),

			ActionStack(BODY, vec3(0.3, 0, 0), vec3(0, 0, 20)),
			ActionStack(LBODY, vec3(-0.048, 0.082, -0.021), vec3(0, 0, -20)),

			// leg 
			ActionStack(LLEG0, vec3(0, 0, 0), vec3(0, -15, 0)),
			ActionStack(LLEG1, vec3(0.011, 0.069, 0.023), vec3(28.543, 3.185, -11.362)),

			ActionStack(RLEG0, vec3(0, 0, 0), vec3(0, 15, 0)),
			ActionStack(RLEG1, vec3(-0.012, 0.048, 0.02), vec3(24.782, -2.931, 11.642)),
		},

		// state 2 -> 雙手2
		{
			ActionStack(LHAND0, vec3(0, -0.038, -0.029), vec3(-78.338, -50.737, 22.44)),
			ActionStack(LHAND1, vec3(0, 0, 0), vec3(-98.338, -24.87, -28.651)),

			ActionStack(RHAND0, vec3(0.027, -0.112, -0.065), vec3(-84.575, 25.204, 12.884)),
			ActionStack(RHAND1, vec3(0, 0, 0), vec3(-53.73, 30.308, 88.686)),

			ActionStack(BODY, vec3(0, 0, 0), vec3(0, 0, 0)),
			ActionStack(LBODY, vec3(0, 0, 0), vec3(0, 0, 0)),

			// leg 
			ActionStack(LLEG0, vec3(-0.071, 0.066, 0.014), vec3(8.095, 36.095, 27.733)),
			ActionStack(LLEG1, vec3(0.011, 0.069, 0.023), vec3(28.543, 3.185, -11.362)),

			ActionStack(RLEG0, vec3(0.043, 0.052, 0.004), vec3(1.822, -15.067, -24.098)),
			ActionStack(RLEG1, vec3(-0.012, 0.048, 0.02), vec3(24.782, -2.931, 11.642)),
		},

		// state 3 -> 雙手3
		{
			ActionStack(LHAND0, vec3(0, -0.038, 0), vec3(-72.452, -39.511, 18.248)),
			ActionStack(LHAND1, vec3(0, 0, 0), vec3(-67.196, -29.053, -63.75)),

			ActionStack(RHAND0, vec3(0.018, -0.097, -0.045), vec3(-75.895, 50.064, 18.318)),
			ActionStack(RHAND1, vec3(0, 0, 0), vec3(-21.191, 14.836, 70.762)),

			ActionStack(BODY, vec3(-0.3, 0, 0), vec3(0, 0, -20)),
			ActionStack(LBODY, vec3(0.04, 0.1, 0.005), vec3(0, 0, 20)),

			// leg 
			ActionStack(LLEG0, vec3(0, 0, 0), vec3(0, -15, 0)),
			ActionStack(LLEG1, vec3(0.011, 0.069, 0.023), vec3(28.543, 3.185, -11.362)),

			ActionStack(RLEG0, vec3(0, 0, 0), vec3(0, 15, 0)),
			ActionStack(RLEG1, vec3(-0.012, 0.048, 0.02), vec3(24.782, -2.931, 11.642)),
		},

		// state 4 -> 雙手4
		{
			ActionStack(LHAND0, vec3(0, -0.038, -0.029), vec3(-78.338, -50.737, 22.44)),
			ActionStack(LHAND1, vec3(0, 0, 0), vec3(-98.338, -24.87, -28.651)),

			ActionStack(RHAND0, vec3(0.027, -0.112, -0.065), vec3(-84.575, 25.204, 12.884)),
			ActionStack(RHAND1, vec3(0, 0, 0), vec3(-53.73, 30.308, 88.686)),

			ActionStack(BODY, vec3(0, 0, 0), vec3(0, 0, 0)),
			ActionStack(LBODY, vec3(0, 0, 0), vec3(0, 0, 0)),

			// leg 
			ActionStack(LLEG0, vec3(-0.071, 0.066, 0.014), vec3(8.095, 36.095, 27.733)),
			ActionStack(LLEG1, vec3(0.011, 0.069, 0.023), vec3(28.543, 3.185, -11.362)),

			ActionStack(RLEG0, vec3(0.043, 0.052, 0.004), vec3(1.822, -15.067, -24.098)),
			ActionStack(RLEG1, vec3(-0.012, 0.048, 0.02), vec3(24.782, -2.931, 11.642)),
		},

		// state 5 -> 雙手1
		{
			ActionStack(LHAND0, vec3(0, -0.038, 0), vec3(-93.348, -65.284, 35.294)),
			ActionStack(LHAND1, vec3(0, 0, 0), vec3(-62.43, -14.762, -40.297)),

			ActionStack(RHAND0, vec3(0.018, -0.097, -0.045), vec3(-85.224, 22.581, 12.621)),
			ActionStack(RHAND1, vec3(0, 0, 0), vec3(-20.815, 8.708, 101.014)),

			ActionStack(BODY, vec3(0.3, 0, 0), vec3(0, 0, 20)),
			ActionStack(LBODY, vec3(-0.048, 0.082, -0.021), vec3(0, 0, -20)),

			// leg 
			ActionStack(LLEG0, vec3(0, 0, 0), vec3(0, -15, 0)),
			ActionStack(LLEG1, vec3(0.011, 0.069, 0.023), vec3(28.543, 3.185, -11.362)),

			ActionStack(RLEG0, vec3(0, 0, 0), vec3(0, 15, 0)),
			ActionStack(RLEG1, vec3(-0.012, 0.048, 0.02), vec3(24.782, -2.931, 11.642)),
		},
	};

	calculateActionStack(actionStack, danceState, passTime, timerPerState);
}

// shoot
void Robot::DoShootAction() {
	// state 0 -> 準備動作
	// state 1 -> 左手扶額頭
	// state 2 -> 右手轉出來
	// state 3 -> 右手沿著x軸轉0.5
	// state 4 -> 右手沿著x軸轉0.6
	// state 5 -> 右手回去0.5
	// state 6 -> 保持姿勢，右手噴網
	// regular procedure
	// 0->1->2->3->4->4->4->

	// determine which bow state we are in
	// by using the std::chrono func provided by c++
	vector<float>timerPerState = { 2, 1.5, 0.5, 0.5, 2, 0.1, 0.5};
	double passTime;
	int danceState = stateDetermination(timerPerState, passTime, 6);

	// if the state wasn't same against the previousStateIndex we reset the previousPassTime
	if (danceState != previousStateIndex) {
		previousStateIndex = danceState;
		previousPassTime = 0;
	}

	// recalculate the passTime to be interval between previousPassTime instead of the state time
	double t = passTime;
	passTime -= previousPassTime;
	previousPassTime = t;

	// actionStack[stateIndex]...
	vector<vector<ActionStack>> actionStack =
	{
		// state 0 -> 準備動作
		{
			ActionStack(LHAND0, vec3(0, -0.038, 0), vec3(0, 0, -36)),
			ActionStack(LHAND1, vec3(0, 0, 0), vec3(0, 0, 0)),

			ActionStack(RHAND0, vec3(0, -0.038, 0), vec3(0, 0, 36)),
			ActionStack(RHAND1, vec3(0, 0, 0), vec3(0, 0, 0)),

			ActionStack(HEAD, vec3(0, 0, 0), vec3(0, 0, 0)),
		},

		// state 1 -> 左手扶額頭
		{
			ActionStack(LHAND0, vec3(-0.063, -0.05, -0.038), vec3(-54.434, -10.115, -44.54)),
			ActionStack(LHAND1, vec3(0, 0, 0), vec3(26, -51.74, -126)),

			ActionStack(RHAND0, vec3(0, -0.038, 0), vec3(0, 0, 36)),
			ActionStack(RHAND1, vec3(0, 0, 0), vec3(0, 0, 0)),

			ActionStack(HEAD, vec3(0, 0.02, 0.03), vec3(26.235, 22.16, 9.233)),
		},

		// state 2 -> 右手轉出來
		{
			ActionStack(LHAND0, vec3(-0.063, -0.05, -0.038), vec3(-54.434, -10.115, -44.54)),
			ActionStack(LHAND1, vec3(0, 0, 0), vec3(26, -51.74, -126)),

			ActionStack(RHAND0, vec3(0, -0.038, 0), vec3(-17.609, 12.4, 34)),
			ActionStack(RHAND1, vec3(0.027, 0.032, 0.003), vec3(-86.588, -53, 45.153)),

			ActionStack(HEAD, vec3(0, 0.02, 0.03), vec3(26.235, 22.16, 9.233)),
		},

		// state 3 -> 右手沿著x軸轉0.5
		{
			ActionStack(LHAND0, vec3(-0.063, -0.05, -0.038), vec3(-54.434, -10.115, -44.54)),
			ActionStack(LHAND1, vec3(0, 0, 0), vec3(26, -51.74, -126)),

			ActionStack(RHAND0, vec3(0, -0.038, 0), vec3(-7.685, 10.437, 37)),
			ActionStack(RHAND1, vec3(0.027, 0.032, 0.003), vec3(-68.955, 1.874, 40.56)),

			ActionStack(HEAD, vec3(0, 0.02, 0.03), vec3(26.235, 22.16, 9.233)),
		},

		// state 4 -> 右手沿著x軸轉0.6
		{
			ActionStack(LHAND0, vec3(-0.063, -0.05, -0.038), vec3(-54.434, -10.115, -44.54)),
			ActionStack(LHAND1, vec3(0, 0, 0), vec3(26, -51.74, -126)),

			ActionStack(RHAND0, vec3(0, -0.038, 0), vec3(-7.685, 10.437, 37)),
			ActionStack(RHAND1, vec3(0.027, 0.032, 0.003), vec3(-56.114, 59.605, 58.471)),

			ActionStack(HEAD, vec3(0, 0.02, 0.03), vec3(26.235, 22.16, 9.233)),
		},

		// state 5 -> 右手回去0.5
		{
			ActionStack(LHAND0, vec3(-0.063, -0.05, -0.038), vec3(-54.434, -10.115, -44.54)),
			ActionStack(LHAND1, vec3(0, 0, 0), vec3(26, -51.74, -126)),

			ActionStack(RHAND0, vec3(0, -0.038, 0), vec3(-7.685, 10.437, 37)),
			ActionStack(RHAND1, vec3(0.027, 0.032, 0.003), vec3(-68.955, 1.874, 40.56)),

			ActionStack(HEAD, vec3(0, 0.02, 0.03), vec3(26.235, 22.16, 9.233)),
		},

		// state 6 -> 保持姿勢，右手噴網
		{
			ActionStack(LHAND0, vec3(-0.063, -0.05, -0.038), vec3(-54.434, -10.115, -44.54)),
			ActionStack(LHAND1, vec3(0, 0, 0), vec3(26, -51.74, -126)),

			ActionStack(RHAND0, vec3(0, -0.038, 0), vec3(-7.685, 10.437, 37)),
			ActionStack(RHAND1, vec3(0.027, 0.032, 0.003), vec3(-68.955, 1.874, 40.56)),

			ActionStack(HEAD, vec3(0, 0.02, 0.03), vec3(26.235, 22.16, 9.233)),

			// create 3d particle

		},
	};

	calculateActionStack(actionStack, danceState, passTime, timerPerState);
}

// action helper
void Robot::actionHelper(int bodyPart, double passTime, vector<float>timerPerState, int stateIndex, vec3 addedTranslation, vec3 addedRotation) {
	// translation
	double xTranslationNeeded = passTime / timerPerState[stateIndex] * addedTranslation.x;
	double yTranslationNeeded = passTime / timerPerState[stateIndex] * addedTranslation.y;
	double zTranslationNeeded = passTime / timerPerState[stateIndex] * addedTranslation.z;
	// rotation
	double xRotationNeeded = passTime / timerPerState[stateIndex] * addedRotation.x;
	double yRotationNeeded = passTime / timerPerState[stateIndex] * addedRotation.y;
	double zRotationNeeded = passTime / timerPerState[stateIndex] * addedRotation.z;

	// add up rotation and translation for the correct part
	this->translatePos[bodyPart] += vec3(xTranslationNeeded, yTranslationNeeded, zTranslationNeeded);
	this->rotations[bodyPart] += vec3(xRotationNeeded, yRotationNeeded, zRotationNeeded);
}

// calculate all actionStack depend on actionState
void Robot::calculateActionStack(vector<vector<ActionStack>> actionStack, int actionState, double passTime, vector<float>timerPerState) {
	for (int i = 0; i < actionStack[actionState].size(); i++) {
		// make the body part id is all in same format
		int bodyPart = actionStack[actionState][i].bodyPartId;
		vec3 translationR = actionStack[actionState][i].translationV;
		vec3 rotationR = actionStack[actionState][i].rotationV;

		// get relative translation and rotation
		if (actionState != 0) {
			if (actionStack[actionState - 1][i].bodyPartId != bodyPart) {
				cout << "\n\nThe previos state should be the same body part id!!!\n\n";
			}

			else {
				translationR -= actionStack[actionState - 1][i].translationV;
				rotationR -= actionStack[actionState - 1][i].rotationV;
			}
		}

		if (bodyPart == -1) {
			cout << "\n\nInvalid body part used, in action state!!!\n\n";
		}
		else {
			this->actionHelper(bodyPart, passTime, timerPerState, actionState, translationR, rotationR);
		}
	}
}