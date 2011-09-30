#include <iostream>
#include <C3/Window.h>

#ifdef MSVC
#include <windows.h>
#endif
#include <glload/gl_3_2_comp.h>
#include <glload/gll.h>
#include <string>
#include <fstream>
#include <streambuf>


GLuint CreateShader(GLenum eShaderType, const std::string &strShaderFile)
{
	GLuint shader = glCreateShader(eShaderType);
	const char *strFileData = strShaderFile.c_str();
	glShaderSource(shader, 1, &strFileData, NULL);

	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

		const char *strShaderType = NULL;
		switch(eShaderType)
		{
		case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
		case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
		case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
		}

		std::cout << "Compile failure in " << strShaderType<< " shader:\n" << strInfoLog << "\n";
		delete[] strInfoLog;
	}

	return shader;
}

GLuint CreateProgram(const std::vector<GLuint> &shaderList)
{
	GLuint program = glCreateProgram();

	for(size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
		glAttachShader(program, shaderList[iLoop]);

	glLinkProgram(program);

	GLint status;
	glGetProgramiv (program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		std::cout << "Linker failure: " << strInfoLog << "\n";
		delete[] strInfoLog;
	}

	return program;
}

GLuint theProgram;

/*const std::string strVertexShader(
	"#version 330\n"
	"layout(location = 0) in vec4 position;\n"
	"out vec2 pos;\n"
	"void main()\n"
	"{\n"
	"   pos = clamp(position.xy,0,1);"
	"   gl_Position = position;\n"
	"}\n"
);

const std::string strFragmentShader(
	"#version 330\n"
	"out vec4 outputColor;\n"
	"in vec2 pos;\n"
	"void main()\n"
	"{\n"
	"   outputColor = vec4(pos.xy, 1.0f, 1.0f);\n"
	"}\n"
	);*/

void InitializeProgram(const std::string vs, const std::string fs)
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(CreateShader(GL_VERTEX_SHADER, vs));
	shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, fs));

	theProgram = CreateProgram(shaderList);
}

const float vertexPositions[] = {
	0.75f, 0.75f, 0.0f, 1.0f,
	0.75f, -0.75f, 0.0f, 1.0f,
	-0.75f, -0.75f, 0.0f, 1.0f,
};

const float vertexPositionsTest[] = {
    -1.f,  -1.f,  0.f,  1.f,
    -1.f,  1.f,  0.f,  1.f,
    1.f,  1.f,  0.f,  1.f,
    1.f,  -1.f,  0.f,  1.f
};

GLuint positionBufferObject;
GLuint vao;


void InitializeVertexBuffer()
{
	glGenBuffers(1, &positionBufferObject);

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositionsTest), vertexPositionsTest, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//Called after the window and OpenGL are initialized. Called exactly once, before the main loop.
void init(const std::string vs, const std::string fs)
{
  InitializeProgram(vs, fs);
	InitializeVertexBuffer();

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
}

int main(int argc, char ** argv)
{

  //Reading the shader files

  std::ifstream vs("test.vs");
  const std::string strVertexShader((std::istreambuf_iterator<char>(vs)),
		  std::istreambuf_iterator<char>());

  std::cout << "VS : \n" << strVertexShader << std::endl;

  std::ifstream fs("test.fs");
  const std::string strFragmentShader((std::istreambuf_iterator<char>(fs)),
		  std::istreambuf_iterator<char>());


  float time = 0.f;
	// Object Creation
	C3::Window win;
	C3::WindowMode mode(800,600);
	C3::OpenGLContextSettings openGLSettings(3,1);

	// Create the window
	std::cout << "Creation ..." << std::endl;
	win.Create(mode,"OpenGL3",openGLSettings);

	// Init OpenGL 3
	int glloadErr = glload::LoadFunctions();
	if(glloadErr != glload::LS_LOAD_FUNCTIONS_ALL)
	{
		std::cerr << "[Warning] Error on loading OpenGL functions. \n";
	}

	init(strVertexShader, strFragmentShader);

	// Initialise OpenGL
	glViewport(0, 0, 800, 600);

	// Draw loop
	std::cout << "Affichage ..." << std::endl;
	while(win.IsOpened())
	{
		// Events
		C3::Event event;
		while(win.PoolEvent(event))
		{
						//std::cout << "Event !" << std::endl;
			if(event.Type == C3::Event::Closed)
			{
				std::cout << "Close ... " << std::endl;
				win.Close();
			}
			else if(event.Type == C3::Event::KeyPressed)
			{
				if(event.Key.Code == C3::Key::Escape)
				{
					std::cout << "Close ... " << std::endl;
					win.Close();
				}
			}
		}

		time += (win.GetFrameTime())/1000.f;
		//		std::cout << time << std::endl;


		// Draw the scene
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(theProgram);


		GLuint stime = glGetUniformLocation(theProgram, "time");
		glUniform1f(stime, time);

		glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glDrawArrays(GL_QUADS, 0, 4);

		glDisableVertexAttribArray(0);
		glUseProgram(0);

		// Swap buffers
		win.Display();
	}

	std::cout << "... End" << std::endl;
	return 0;
}
