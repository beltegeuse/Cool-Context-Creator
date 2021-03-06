////////////
/// Includes
////////////
// *** OpenGL
#ifdef WIN32
#include <GL/glew.h>
#else
#define GL3_PROTOTYPES 1
#include "gl3.h"
#endif
#ifdef MSVC
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
// *** STD
#include <iostream>
#include <stdarg.h>
#include <stdio.h>
// *** C3
#include <C3/Window.h>
// *** Others
#include <vectormath.h>
#include <glsw.h>



static void CreateIcosahedron();
static void LoadEffect();

typedef struct {
    GLuint Projection;
    GLuint Modelview;
    GLuint NormalMatrix;
    GLuint LightPosition;
    GLuint AmbientMaterial;
    GLuint DiffuseMaterial;
    GLuint TessLevelInner;
    GLuint TessLevelOuter;
} ShaderUniforms;

static GLuint vao;
static GLsizei IndexCount;
static const GLuint PositionSlot = 0;
static GLuint ProgramHandle;
static Matrix4 ProjectionMatrix;
static Matrix4 ModelviewMatrix;
static Matrix3 NormalMatrix;
static ShaderUniforms Uniforms;
static float TessLevelInner;
static float TessLevelOuter;

void PezCheckCondition(bool condition, const char* fmt, ...)
{
	va_list a;
	if (condition)
		return;

	va_start(a, fmt);
	vprintf(fmt, a);
}

void PezRender(GLuint fbo)
{
    glUniform1f(Uniforms.TessLevelInner, TessLevelInner);
    glUniform1f(Uniforms.TessLevelOuter, TessLevelOuter);
    
    Vector4 lightPosition = V4MakeFromElems(0.25, 0.25, 1, 0);
    glUniform3fv(Uniforms.LightPosition, 1, &lightPosition.x);
    
    glUniformMatrix4fv(Uniforms.Projection, 1, 0, &ProjectionMatrix.col0.x);
    glUniformMatrix4fv(Uniforms.Modelview, 1, 0, &ModelviewMatrix.col0.x);

    Matrix3 nm = M3Transpose(NormalMatrix);
    float packed[9] = { nm.col0.x, nm.col1.x, nm.col2.x,
                        nm.col0.y, nm.col1.y, nm.col2.y,
                        nm.col0.z, nm.col1.z, nm.col2.z };
    glUniformMatrix3fv(Uniforms.NormalMatrix, 1, 0, &packed[0]);

    // Render the scene:
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPatchParameteri(GL_PATCH_VERTICES, 3);
    glUniform3f(Uniforms.AmbientMaterial, 0.04f, 0.04f, 0.04f);
    glUniform3f(Uniforms.DiffuseMaterial, 0, 0.75, 0.75);
    glDrawElements(GL_PATCHES, IndexCount, GL_UNSIGNED_INT, 0);
}

void PezInitialize(int width, int height)
{
    TessLevelInner = 3;
    TessLevelOuter = 2;

    CreateIcosahedron();
    LoadEffect();

    Uniforms.Projection = glGetUniformLocation(ProgramHandle, "Projection");
    Uniforms.Modelview = glGetUniformLocation(ProgramHandle, "Modelview");
    Uniforms.NormalMatrix = glGetUniformLocation(ProgramHandle, "NormalMatrix");
    Uniforms.LightPosition = glGetUniformLocation(ProgramHandle, "LightPosition");
    Uniforms.AmbientMaterial = glGetUniformLocation(ProgramHandle, "AmbientMaterial");
    Uniforms.DiffuseMaterial = glGetUniformLocation(ProgramHandle, "DiffuseMaterial");
    Uniforms.TessLevelInner = glGetUniformLocation(ProgramHandle, "TessLevelInner");
    Uniforms.TessLevelOuter = glGetUniformLocation(ProgramHandle, "TessLevelOuter");

    // Set up the projection matrix:
    const float HalfWidth = 0.6f;
    const float HalfHeight = HalfWidth * 600.f / 600.f;
    ProjectionMatrix = M4MakeFrustum(-HalfWidth, +HalfWidth, -HalfHeight, +HalfHeight, 5, 150);

    // Initialize various state:
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(0.7f, 0.6f, 0.5f, 1.0f);
}

static void CreateIcosahedron()
{
    const int Faces[] = {
        2, 1, 0,
        3, 2, 0,
        4, 3, 0,
        5, 4, 0,
        1, 5, 0,

        11, 6,  7,
        11, 7,  8,
        11, 8,  9,
        11, 9,  10,
        11, 10, 6,

        1, 2, 6,
        2, 3, 7,
        3, 4, 8,
        4, 5, 9,
        5, 1, 10,

        2,  7, 6,
        3,  8, 7,
        4,  9, 8,
        5, 10, 9,
        1, 6, 10 };

    const float Verts[] = {
         0.000f,  0.000f,  1.000f,
         0.894f,  0.000f,  0.447f,
         0.276f,  0.851f,  0.447f,
        -0.724f,  0.526f,  0.447f,
        -0.724f, -0.526f,  0.447f,
         0.276f, -0.851f,  0.447f,
         0.724f,  0.526f, -0.447f,
        -0.276f,  0.851f, -0.447f,
        -0.894f,  0.000f, -0.447f,
        -0.276f, -0.851f, -0.447f,
         0.724f, -0.526f, -0.447f,
         0.000f,  0.000f, -1.000f };

    IndexCount = sizeof(Faces) / sizeof(Faces[0]);

    std::cout << "     - Generate VAO" << std::endl;
    // Create the VAO:
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    std::cout << "     - Position buffer" << std::endl;
    // Create the VBO for positions:
    GLuint positions;
    GLsizei stride = 3 * sizeof(float);
    glGenBuffers(1, &positions);
    glBindBuffer(GL_ARRAY_BUFFER, positions);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Verts), Verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(PositionSlot);
    glVertexAttribPointer(PositionSlot, 3, GL_FLOAT, GL_FALSE, stride, 0);

    std::cout << "     - Indice buffer" << std::endl;
    // Create the VBO for indices:
    GLuint indices;
    glGenBuffers(1, &indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Faces), Faces, GL_STATIC_DRAW);
}

static void LoadEffect()
{
    GLint compileSuccess, linkSuccess;
    GLchar compilerSpew[256];

    glswInit();
    glswSetPath("./", ".glsl");
    glswAddDirectiveToken("*", "#version 400");

    const char* vsSource = glswGetShader("Geodesic.Vertex");
    const char* tcsSource = glswGetShader("Geodesic.TessControl");
    const char* tesSource = glswGetShader("Geodesic.TessEval");
    const char* gsSource = glswGetShader("Geodesic.Geometry");
    const char* fsSource = glswGetShader("Geodesic.Fragment");
    const char* msg = "Can't find %s shader.  Does '../Geodesic.glsl' exist?\n";
    PezCheckCondition(vsSource != 0, msg, "vertex");
    PezCheckCondition(tcsSource != 0, msg, "tess control");
    PezCheckCondition(tesSource != 0, msg, "tess eval");
    PezCheckCondition(gsSource != 0, msg, "geometry");
    PezCheckCondition(fsSource != 0, msg, "fragment");

    GLuint vsHandle = glCreateShader(GL_VERTEX_SHADER);
    GLuint tcsHandle = glCreateShader(GL_TESS_CONTROL_SHADER);
    GLuint tesHandle = glCreateShader(GL_TESS_EVALUATION_SHADER);
    GLuint gsHandle = glCreateShader(GL_GEOMETRY_SHADER);
    GLuint fsHandle = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vsHandle, 1, &vsSource, 0);
    glCompileShader(vsHandle);
    glGetShaderiv(vsHandle, GL_COMPILE_STATUS, &compileSuccess);
    glGetShaderInfoLog(vsHandle, sizeof(compilerSpew), 0, compilerSpew);
    PezCheckCondition(compileSuccess, "Vertex Shader Errors:\n%s", compilerSpew);

    glShaderSource(tcsHandle, 1, &tcsSource, 0);
    glCompileShader(tcsHandle);
    glGetShaderiv(tcsHandle, GL_COMPILE_STATUS, &compileSuccess);
    glGetShaderInfoLog(tcsHandle, sizeof(compilerSpew), 0, compilerSpew);
    PezCheckCondition(compileSuccess, "Tess Control Shader Errors:\n%s", compilerSpew);

    glShaderSource(tesHandle, 1, &tesSource, 0);
    glCompileShader(tesHandle);
    glGetShaderiv(tesHandle, GL_COMPILE_STATUS, &compileSuccess);
    glGetShaderInfoLog(tesHandle, sizeof(compilerSpew), 0, compilerSpew);
    PezCheckCondition(compileSuccess, "Tess Eval Shader Errors:\n%s", compilerSpew);

    glShaderSource(gsHandle, 1, &gsSource, 0);
    glCompileShader(gsHandle);
    glGetShaderiv(gsHandle, GL_COMPILE_STATUS, &compileSuccess);
    glGetShaderInfoLog(gsHandle, sizeof(compilerSpew), 0, compilerSpew);
    PezCheckCondition(compileSuccess, "Geometry Shader Errors:\n%s", compilerSpew);

    glShaderSource(fsHandle, 1, &fsSource, 0);
    glCompileShader(fsHandle);
    glGetShaderiv(fsHandle, GL_COMPILE_STATUS, &compileSuccess);
    glGetShaderInfoLog(fsHandle, sizeof(compilerSpew), 0, compilerSpew);
    PezCheckCondition(compileSuccess, "Fragment Shader Errors:\n%s", compilerSpew);

    ProgramHandle = glCreateProgram();
    glAttachShader(ProgramHandle, vsHandle);
    glAttachShader(ProgramHandle, tcsHandle);
    glAttachShader(ProgramHandle, tesHandle);
    glAttachShader(ProgramHandle, gsHandle);
    glAttachShader(ProgramHandle, fsHandle);
    glBindAttribLocation(ProgramHandle, PositionSlot, "Position");
    glLinkProgram(ProgramHandle);
    glGetProgramiv(ProgramHandle, GL_LINK_STATUS, &linkSuccess);
    glGetProgramInfoLog(ProgramHandle, sizeof(compilerSpew), 0, compilerSpew);
    PezCheckCondition(linkSuccess, "Shader Link Errors:\n%s", compilerSpew);

    glUseProgram(ProgramHandle);
}

void PezUpdate(unsigned int elapsedMicroseconds)
{
    const float RadiansPerMicrosecond = 0.0000005f;
    static float Theta = 0;
    Theta += elapsedMicroseconds * RadiansPerMicrosecond;
    Matrix4 rotation = M4MakeRotationX(Theta);
    Point3 eyePosition = P3MakeFromElems(0, 0, -10);
    Point3 targetPosition = P3MakeFromElems(0, 0, 0);
    Vector3 upVector = V3MakeFromElems(0, 1, 0);
    Matrix4 lookAt = M4MakeLookAt(eyePosition, targetPosition, upVector);
    ModelviewMatrix = M4Mul(lookAt, rotation);
    NormalMatrix = M4GetUpper3x3(ModelviewMatrix);
}

void PezHandleMouse(int x, int y, int action)
{
}

int main(int argc, char ** argv)
{
	std::cout << "Lancement ..." << std::endl;

	// Object Creation
	C3::Window win;
	C3::WindowMode mode(600,600);
	C3::OpenGLContextSettings openGLSettings(4,1);

	// Create the window
	std::cout << "Creation ..." << std::endl;
	win.Create(mode,"OpenGL4",openGLSettings);

#ifdef WIN32
	GLenum err = glewInit();
    PezCheckCondition(GLEW_OK == err, "Error: %s\n", glewGetErrorString(err));
#endif

	std::cout << "Load ... " << std::endl;
	std::cout << "   * Effect " << std::endl;
	LoadEffect();
	std::cout << "   * Geometry " << std::endl;
	CreateIcosahedron();
	std::cout << "   * InitPez " << std::endl;
	PezInitialize(800,600);

	// Initialise OpenGL
	//glViewport(0, 0, 800, 600);

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
				switch(event.Key.Code)
				{
					case C3::Key::Escape:
					{
						std::cout << "Close ... " << std::endl;
						win.Close();
						break;
					}
					case C3::Key::Right:
					{
						TessLevelInner++;
						break;
					}
					case C3::Key::Left:
					{
						TessLevelInner = TessLevelInner > 1 ? TessLevelInner - 1 : 1;
						break;
					}
					case C3::Key::Up:
					{
						TessLevelOuter++;
						break;
					}
					case C3::Key::Down:
					{
						TessLevelOuter = TessLevelOuter > 1 ? TessLevelOuter - 1 : 1;
						break;
					}
				}
			}
		}

		// Update
		PezUpdate(win.GetFrameTime()*1000);

		// Draw the scene
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity( );

		gluLookAt(3,4,2,0,0,0,0,0,1);

		PezRender(0);

		// Swap buffers
		win.Display();
	}

	std::cout << "... End" << std::endl;
	return 0;
}
