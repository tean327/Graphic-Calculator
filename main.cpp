#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<math.h>
#include<string>
#include<functional>

#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"shaderClass.h"

#define PI 3.14159265358979323846
#define e 2.71828182845904523536


GLuint uniID;	

#pragma region WindowVar
int WindowSizeX = 1000;
int WindowSizeY = 800;
GLFWwindow* window;
#pragma endregion

const long long  iteration = 99999999;
GLfloat vertices[iteration*3];
std::string input;

float (*func)(float);
float (*func2)(float, float);
float param;
double constante;
float x;

#pragma region FunctionDeclarations
int HandleInput(std::string input);
int CreateWindow();
int DrawFunction();	
int DrawGraph();
int AddVertices();
float f(float x);
#pragma endregion


int main(void)
{
	std::cout << "Welcome to my graphics calculator" << std::endl;
	std::cout << "Please enter your function !  ";
	std::cin >> input;
	std::cout << "Press '+' to zoom in and '-' to zoom out" << std::endl;
	HandleInput(input);
	AddVertices();
	//Create the window
    if (CreateWindow() == 1)
	{
		std::cout << "Failed to create window" << std::endl;
		return 1;
	}

	return 0;
}

int HandleInput(std::string input)
{		
	//Need to change this into a parsing algorithm
		int count = 0;
		for (int i = 0; i < input.length(); i++)
		{
			if (!isdigit(input[i])) continue;
			count++;
		}
		if (count == input.length()) constante = stoi(input);


	//Handle functions input
	else if (input == "sin(x)" || input =="sin") func = sin;
	else if (input == "cos(x)" || input == "cos") func = cos;
	else if (input == "tan(x)" || input == "tan") func = tan;
	else if (input == "asin(x)" || input == "asin") func = asin;
	else if (input == "acos(x)" || input == "acos") func = acos;
	else if (input == "atan(x)" || input == "atan") func = atan;
	else if (input == "x^2" || input == "x²") 
	{ 
		func2 = powf; 
		param = 2;
	}
	else if (input == "x^3") 
	{ 
		func2 = powf; 
		param = 3;
	}
	else if (input == "square" || input == "Vx") func = sqrt;
	else if (input == "log(x)") func = log;
	else if (input == "log2(x)") func = log2;
	else if (input == "log10(x)") func = log10;
	else if (input == "e^x") func = exp;

	//Handle consts input
	else if (input == "PI" || input == "pi" || input == "π") constante = PI;
	else if (input == "e" || input == "E") constante = e;
	else if (input == "x") constante = x;
	else if (input.length() == 1 && isdigit(input[0])) constante = input[0];

	return 0;	
}

int CreateWindow()
{
	// Initialize glfw
	glfwInit();	
	//Give hint to glfw which version of opengl we are using (here 3.3)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//Tell glfw we are using the core profile which means we only will have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	//Create the window (parameters are size x then y, name of the window, if you want to put it in full screen and not important)
	window = glfwCreateWindow(WindowSizeX, WindowSizeY, "Graphs", NULL, NULL);

	//Just to make sure that if the window does not render the program does not break
	if (window == NULL)
	{
		glfwTerminate();
		return 1;
	}
	// This is to tell opengl that we want to use this window if we don't tell him it will be bugs with the following lines of code
	glfwMakeContextCurrent(window);
	//Load glad and all of its function/method so it configures Opengl
	gladLoadGL();

	//Tell opengl the area of our window so that we can make changes after 
	glViewport(0, 0, WindowSizeX, WindowSizeY);

	glClearColor(0.85f, 0.85f, 0.85f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	DrawGraph();
	DrawFunction();

	glfwSwapBuffers(window);
	//This is to make sure that the program still runs while the window is open
	float lScale = 0.0f;
	while (!glfwWindowShouldClose(window))
	{
		//Handle zoom
		if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT)) lScale -= 0.1f;
		if (glfwGetKey(window, GLFW_KEY_KP_ADD)) lScale += 0.1f;
	
		glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUniform1f(uniID, lScale);
		glDrawArrays(GL_LINE_STRIP, 0, iteration);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//We need to destroy the window and terminate opengl at the end of the program
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

int DrawGraph()
{
	GLfloat graphVertices[] =
	{
		50.0f,0.0f, 0.0f,
		-50.0f,0.0f,0.0f,
		0.0f,50.0f,0.0f,
		0.0f,-50.0f,0.0f
	};

	Shader shaderProgram("default.vert", "default.frag");

	VAO VAO1;
	VAO1.Bind();

	VBO VBO1(graphVertices, sizeof(graphVertices));

	VAO1.LinkVBO(VBO1, 0);

	VAO1.UnBind();
	VBO1.UnBind();

	glClearColor(0.85f, 0.85f, 0.85f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	shaderProgram.Activate();
	VAO1.Bind();
	glDrawArrays(GL_LINE_STRIP, 0, 1);
	glfwSwapBuffers(window);

	return 0;
}

int DrawFunction()
{
	Shader shaderProgram("default.vert", "default.frag");

	VAO VAO1;
	VAO1.Bind();

	VBO VBO1(vertices, sizeof(vertices));

	VAO1.LinkVBO(VBO1, 0);

	VAO1.UnBind();
	VBO1.UnBind();
	uniID = glGetUniformLocation(shaderProgram.ID, "scale");

	glClearColor(0.85f, 0.85f, 0.85f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	shaderProgram.Activate();
	VAO1.Bind();
	glDrawArrays(GL_LINE_STRIP, 0, 1);
	glfwSwapBuffers(window);

	return 0;
}

int AddVertices()
{
	x = float(-iteration) * 0.000001f;
	//We iterate by three for the three coordonates of the points, we don't have to declare a value for z because we're in 2D
	for ( int i = 0; i <= iteration*3; i+=3)
	{
		float y = f(x);
		vertices[i] = x;
		vertices[i + 1] = y;
		x += 0.1f;
	}
	return 0;
}

float f(float pX)
{
	if (func != nullptr)  return func(pX);
	else if (func2 != nullptr) return func2(pX, param);
	else if (constante = x) return pX;
	return constante;
}
