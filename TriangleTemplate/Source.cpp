#include"stdio.h"
#include"Common.h"
#include "ViewManager.h"
#include"Shader.h"
#include"Model.h"
#include "Robot.h"
#include "stb_image.h"

#define MENU_Entry1 1
#define MENU_Entry2 2
#define MENU_EXIT   3

using namespace glm;
using namespace std;


float			aspect;
ViewManager		m_camera;

// shader part
Shader robotShader;
Shader particleShader;
Shader skyboxShader;
Shader screenShader;

// robot class
Robot robot;

// some define var
#define RED 0
#define GREEN 1
#define BLUE 2

#define NORMAL_AMBIENT 0
#define GLOWING 1
#define GLOWING_RED 2
#define GLOWING_GREEN 3
#define GLOWING_BLUE 4
#define GLOWING_GOLD 5

// different mode's global var
int ambientAnim;
int lightAnim;
int shadermode;
int postmode;

// robot shader location
GLuint AmbientColor;
GLuint shaderMode;
GLuint LightPos;
GLuint fraction_val;
GLuint skyProjection;
GLuint skyView;
GLuint uniformSkybox;
GLuint cameraPosition;

// var used for robot shader
float ambientValue[3] = { 0.1,0.1,0.1 };
float LightPosition[3] = { 1.0,1.0,1.0 };

// some global var used by skybox shader
GLuint UBO;
GLint MatricesIdx;
// skybox location
GLuint _PostMode;
GLuint _DeltaTime;
// skybox vao, vbo
GLuint skyboxVAO, skyboxVBO;
// skybox cube map texture
unsigned int cubemapTexture;
// skybox vertices
float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

// quad part(I think is used for screen shader)
float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
};
GLuint quadVAO, quadVBO;

// fbo??? (IDK)
GLuint framebuffer;
GLuint textureColorbuffer;
GLuint rbo;

// function declaration
unsigned int loadCubemap(vector<std::string> faces);

//recursion of glut timer func changing the uniform ambient of the shader language
void AmbientChange(int loop) {
	if (loop >= 50) {
		loop = 0;
	}

	if (ambientAnim == NORMAL_AMBIENT) {
		ambientValue[0] = 0.1;
		ambientValue[1] = 0.1;
		ambientValue[2] = 0.1;
	}
	else if (ambientAnim == GLOWING) {
		ambientValue[0] = ((double)loop - (loop * loop * 0.02)) / 25.0;//quadratic formula
		ambientValue[1] = ((double)loop - (loop * loop * 0.02)) / 25.0;
		ambientValue[2] = ((double)loop - (loop * loop * 0.02)) / 25.0;
	}
	else if (ambientAnim == GLOWING_RED) {
		ambientValue[0] = ((double)loop - (loop * loop * 0.02)) / 25.0;
		ambientValue[1] = 0.1;
		ambientValue[2] = 0.1;
	}
	else if (ambientAnim == GLOWING_GREEN) {
		ambientValue[0] = 0.1;
		ambientValue[1] = ((double)loop - (loop * loop * 0.02)) / 25.0;
		ambientValue[2] = 0.1;
	}
	else if (ambientAnim == GLOWING_BLUE) {
		ambientValue[0] = 0.1;
		ambientValue[1] = 0.1;
		ambientValue[2] = ((double)loop - (loop * loop * 0.02)) / 25.0;
	}
	else if (ambientAnim == GLOWING_GOLD) {
		ambientValue[0] = ((double)loop - (loop * loop * 0.02)) / 25.0;
		ambientValue[1] = ((double)loop - (loop * loop * 0.02)) / 27.0;
		ambientValue[2] = 0.1;
	}

	glutPostRedisplay();
	glutTimerFunc(40, AmbientChange, loop + 1);
}

void LightChange(int loop2) {
	if (loop2 >= 360) {
		loop2 = 0;
	}

	if (lightAnim == 0) {
		LightPosition[0] = 1.0;
		LightPosition[1] = 1.0;
		LightPosition[2] = 1.0;
	}
	else if (lightAnim == 1) {
		LightPosition[0] = 50 * cos(loop2 * 3.14f / 180);
		LightPosition[1] = 1.0;
		LightPosition[2] = 50 * sin(loop2 * 3.14f / 180);
	}
	else if (lightAnim == 2) {
		LightPosition[0] = 50 * cos(loop2 * 3.14f / 180);
		LightPosition[1] = 50 * sin(loop2 * 3.14f / 180);
		LightPosition[2] = 10.0;
	}
	else if (lightAnim == 3) {
		LightPosition[0] = 10.0;
		LightPosition[1] = 50 * cos(loop2 * 3.14f / 180);
		LightPosition[2] = 50 * sin(loop2 * 3.14f / 180);
	}

	glutPostRedisplay();
	glutTimerFunc(10, LightChange, loop2 + 1);
}

void My_Init()
{
	glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// robot shader
	robotShader = Shader("../Assets/shaders/robot.vs.glsl", "../Assets/shaders/robot.fs.glsl");
	robot = Robot();
	robot.InitModels();
	// particle
	particleShader = Shader("../Assets/shaders/particle.vs.glsl", "../Assets/shaders/particle.fs.glsl");
	// skybox
	skyboxShader = Shader("../Assets/shaders/skybox.vp", "../Assets/shaders/skybox.fp");
	// screenshader
	screenShader = Shader("../Assets/shaders/Image_Processing.vs.glsl", "../Assets/shaders/Image_Processing.fs.glsl");

	// some var
	AmbientColor = glGetUniformLocation(robotShader.ID, "ambientColor");
	shaderMode = glGetUniformLocation(robotShader.ID, "mode");
	LightPos = glGetUniformLocation(robotShader.ID, "vLightPosition");
	fraction_val = glGetUniformLocation(robotShader.ID, "fraction");
	uniformSkybox = glGetUniformLocation(robotShader.ID, "skybox");
	cameraPosition = glGetUniformLocation(robotShader.ID, "cameraPos");

	// init for UBO(skybox)
	MatricesIdx = glGetUniformBlockIndex(robotShader.ID, "MatVP");
	// UBO
	glGenBuffers(1, &UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(mat4) * 2, NULL, GL_DYNAMIC_DRAW);
	//get uniform struct size
	int UBOsize = 0;
	glGetActiveUniformBlockiv(robotShader.ID, MatricesIdx, GL_UNIFORM_BLOCK_DATA_SIZE, &UBOsize);
	//bind UBO to its idx
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBO, 0, UBOsize);
	glUniformBlockBinding(robotShader.ID, MatricesIdx, 0);

	// skybox vao
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	vector<std::string> faces
	{
		"../Assets/skybox/right.jpg",
		"../Assets/skybox/left.jpg",
		"../Assets/skybox/top.jpg",
		"../Assets/skybox/bottom.jpg",
		"../Assets/skybox/front.jpg",
		"../Assets/skybox/back.jpg"
	};

	cubemapTexture = loadCubemap(faces);

	glUseProgram(robotShader.ID);
	glUniform1i(uniformSkybox, 0);

	glUseProgram(skyboxShader.ID);

	skyView = glGetUniformLocation(skyboxShader.ID, "view");
	skyProjection = glGetUniformLocation(skyboxShader.ID, "projection");

	// screen quad VAO
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// create a color attachment texture

	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//glUseProgram(screenShaderProgram);//uniformm use shader
	_PostMode = glGetUniformLocation(screenShader.ID, "shader_now");
	_DeltaTime = glGetUniformLocation(screenShader.ID, "iTime");
	glUniform1i(_PostMode, postmode);

	glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
}

// GLUT callback. Called to draw the scene.
void My_Display()
{
	int timeSinceStart = glutGet(GLUT_ELAPSED_TIME);

	// frame buffer used
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)
	glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	glDepthMask(GL_FALSE);
	glUseProgram(skyboxShader.ID);

	glUniformMatrix4fv(skyView, 1, GL_FALSE,  &(m_camera.GetViewMatrix() * m_camera.GetModelMatrix())[0][0]);
	glUniformMatrix4fv(skyProjection, 1, GL_FALSE, &m_camera.GetProjectionMatrix(aspect)[0][0]);

	// skybox cube
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glDepthFunc(GL_LESS); // set depth function back to default
	glDepthMask(GL_TRUE);
	glUseProgram(0);

	robotShader.use();
	//update data to UBO for MVP
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4), &(m_camera.GetViewMatrix() * m_camera.GetModelMatrix()));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4), sizeof(mat4), &m_camera.GetProjectionMatrix(aspect));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// set robot uniform var
	glUniform4f(AmbientColor, ambientValue[RED], ambientValue[GREEN], ambientValue[BLUE], 1);
	glUniform1f(fraction_val, .5f);
	glUniform1i(shaderMode, shadermode);
	glUniform3f(LightPos, LightPosition[0], LightPosition[1], LightPosition[2]);
	glUniform3fv(cameraPosition, 1, glm::value_ptr(m_camera.GetEyePosition()));
	
	// set view matrix
	robotShader.setUniformMatrix4fv("view", m_camera.GetViewMatrix() * m_camera.GetModelMatrix());
	// set projection matrix
	robotShader.setUniformMatrix4fv("projection", m_camera.GetProjectionMatrix(aspect));
	// draw robot
	robot.Draw(robotShader);
	// call robot update function
	robot.Update();

	// set view matrix
	particleShader.setUniformMatrix4fv("view", m_camera.GetViewMatrix() * m_camera.GetModelMatrix());
	// set projection matrix
	particleShader.setUniformMatrix4fv("projection", m_camera.GetProjectionMatrix(aspect));
	// play particle
	ParticleManager::getParticleManager()->Draw(particleShader);


	// get back to the default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
	// clear all relevant buffers
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(screenShader.ID);
	glUniform1i(_PostMode, postmode);
	glUniform1f(_DeltaTime, (float)timeSinceStart);

	glBindVertexArray(quadVAO);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);	// use the color attachment texture as the texture of the quad plane
	glDrawArrays(GL_TRIANGLES, 0, 6);

	///////////////////////////	

	glFlush();
	glutSwapBuffers();
}

//Call to resize the window
void My_Reshape(int width, int height)
{
	aspect = width * 1.0f / height;
	m_camera.SetWindowSize(width, height);
	glViewport(0, 0, width, height);

	glDeleteRenderbuffers(1, &rbo);
	glDeleteTextures(1, &textureColorbuffer);
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//Timer event
void My_Timer(int val)
{
	glutPostRedisplay();
	glutTimerFunc(16, My_Timer, val);
}

//Mouse event
void My_Mouse(int button, int state, int x, int y)
{
	m_camera.mouseEvents(button, state, x, y);

	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			printf("Mouse %d is pressed at (%d, %d)\n", button, x, y);
		}
		else if (state == GLUT_UP)
		{
			printf("Mouse %d is released at (%d, %d)\n", button, x, y);
		}
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		printf("Mouse %d is pressed\n", button);
	}
	printf("%d %d %d %d\n", button, state, x, y);
}

//Keyboard event
void My_Keyboard(unsigned char key, int x, int y)
{
	m_camera.keyEvents(key);
	printf("Key %c is pressed at (%d, %d)\n", key, x, y);
}

//Special key event
void My_SpecialKeys(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_F1:
		printf("F1 is pressed at (%d, %d)\n", x, y);
		break;
	case GLUT_KEY_PAGE_UP:
		printf("Page up is pressed at (%d, %d)\n", x, y);
		break;
	case GLUT_KEY_LEFT:
		printf("Left arrow is pressed at (%d, %d)\n", x, y);
		break;
	default:
		printf("Other special key is pressed at (%d, %d)\n", x, y);
		break;
	}
}

//Menu event
void mainMenuFunc(int id)
{
	switch (id)
	{
	case MENU_EXIT:
		exit(0);
		break;
	default:
		break;
	}
}

// state menu function
void stateMenuFunc(int id) {
	robot.setState((RobotState)id);
}

// copy from ....
void ModeMenuEvents(int option) {
	switch (option) {
	case 0:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 1:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 2:
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	}
}
void ShaderMenuEvents(int option) {
	switch (option) {
	case 0:
		ambientAnim = NORMAL_AMBIENT;
		break;
	case 1:
		ambientAnim = GLOWING;
		break;
	case 2:
		ambientAnim = GLOWING_RED;
		break;
	case 3:
		ambientAnim = GLOWING_GREEN;
		break;
	case 4:
		ambientAnim = GLOWING_BLUE;
		break;
	case 5:
		ambientAnim = GLOWING_GOLD;
		break;
	}
}
void LightMenuEvents(int option) {
	lightAnim = option;
}
void ShaderAlgorithmEvents(int option) {
	shadermode = option;
}
void PostProcessEvents(int option) {
	postmode = option;
}
void My_Mouse_Moving(int x, int y) {
	m_camera.mouseMoveEvent(x, y);
}

int main(int argc, char *argv[])
{
#ifdef __APPLE__
	//Change working directory to source code path
	chdir(__FILEPATH__("/../Assets/"));
#endif
	// Initialize GLUT and GLEW, then create a window.
	////////////////////
	glutInit(&argc, argv);
#ifdef _MSC_VER
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#else
	glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif

	glutInitWindowPosition(100, 100);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Framework"); // You cannot use OpenGL functions before this line; The OpenGL context must be created first by glutCreateWindow()!
#ifdef _MSC_VER
	glewInit();
#endif

	//Print debug information 
	ViewManager::DumpInfo();
	////////////////////

	//Call custom initialize function
	My_Init();

	//Define Menu
	////////////////////
	int menu_main = glutCreateMenu(mainMenuFunc);
	int menu_state = glutCreateMenu(stateMenuFunc);
	int ModeMenu, ShaderMenu, LightMenu, ShaderAlgorithmMenu, PostProcessMenu;

	glutSetMenu(menu_main);
	glutAddSubMenu("Action", menu_state);
	glutAddMenuEntry("Exit", MENU_EXIT);

	glutSetMenu(menu_state);
	glutAddMenuEntry("Default", (int)RobotState::DEFAULT);
	glutAddMenuEntry("Walk", (int)RobotState::WALK);
	glutAddMenuEntry("Jump", (int)RobotState::JUMP);
	glutAddMenuEntry("Clap Hand", (int)RobotState::CLAP);
	glutAddMenuEntry("Dance", (int)RobotState::DANCE);
	glutAddMenuEntry("Shoot", (int)RobotState::SHOOT);

	ModeMenu = glutCreateMenu(ModeMenuEvents);//建立右鍵菜單
	//加入右鍵物件
	glutAddMenuEntry("Line", 0);
	glutAddMenuEntry("Fill", 1);
	glutAddMenuEntry("Point", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);	//與右鍵關聯


	ShaderMenu = glutCreateMenu(ShaderMenuEvents);//建立右鍵菜單
	glutAddMenuEntry("Normal", 0);
	glutAddMenuEntry("Glowing", 1);
	glutAddMenuEntry("Glowing Red", 2);
	glutAddMenuEntry("Glowing Green", 3);
	glutAddMenuEntry("Glowing Blue", 4);
	glutAddMenuEntry("Glowing Gold", 5);
	glutAttachMenu(GLUT_RIGHT_BUTTON);	//與右鍵關聯


	LightMenu = glutCreateMenu(LightMenuEvents);//建立右鍵菜單
	glutAddMenuEntry("Normal", 0);
	glutAddMenuEntry("Rotate A", 1);
	glutAddMenuEntry("Rotate B", 2);
	glutAddMenuEntry("Rotate C", 3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);	//與右鍵關聯

	ShaderAlgorithmMenu = glutCreateMenu(ShaderAlgorithmEvents);//建立右鍵菜單
	glutAddMenuEntry("Basic", 0);
	glutAddMenuEntry("Toon Shader", 1);
	glutAddMenuEntry("Reflection", 2);
	glutAddMenuEntry("Glass Refraction", 3);
	glutAddMenuEntry("Air Refraction", 4);
	glutAddMenuEntry("Diamond Refraction", 5);
	glutAttachMenu(GLUT_RIGHT_BUTTON);	//與右鍵關聯

	PostProcessMenu = glutCreateMenu(PostProcessEvents);//建立右鍵菜單
	glutAddMenuEntry("Normal", 0);
	glutAddMenuEntry("Average Blur", 1);
	glutAddMenuEntry("Median Blur", 2);
	glutAddMenuEntry("Triangle Blur", 3);
	glutAddMenuEntry("Gaussian blur", 4);
	glutAddMenuEntry("Quantization", 5);
	glutAddMenuEntry("Difference of Gauss", 6);
	glutAddMenuEntry("Quant and DoG", 7);
	glutAddMenuEntry("Inverted", 8);
	glutAddMenuEntry("Grayscale", 9);
	glutAddMenuEntry("10", 10);
	glutAddMenuEntry("11", 11);
	glutAddMenuEntry("12", 12);
	glutAttachMenu(GLUT_RIGHT_BUTTON);	//與右鍵關聯

	glutCreateMenu(mainMenuFunc);//建立右鍵菜單
	//加入右鍵物件
	glutAddSubMenu("Action", menu_state);
	glutAddSubMenu("Mode", ModeMenu);
	glutAddSubMenu("Model Shader", ShaderMenu);
	glutAddSubMenu("Light Direction", LightMenu);
	glutAddSubMenu("Shader Algorithm", ShaderAlgorithmMenu);
	glutAddSubMenu("Filter", PostProcessMenu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);	//與右鍵關聯
	////////////////////

	//Register GLUT callback functions
	////////////////////
	glutDisplayFunc(My_Display);
	glutReshapeFunc(My_Reshape);
	glutMouseFunc(My_Mouse);
	glutKeyboardFunc(My_Keyboard);
	glutSpecialFunc(My_SpecialKeys);
	glutTimerFunc(16, My_Timer, 0);
	glutPassiveMotionFunc(My_Mouse_Moving);
	glutMotionFunc(My_Mouse_Moving);
	glutTimerFunc(10, AmbientChange, 0);
	glutTimerFunc(10, LightChange, 0);
	////////////////////

	// Enter main event loop.
	glutMainLoop();

	return 0;
}

unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}