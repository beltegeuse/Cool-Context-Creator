#include <iostream>
#include <C3/Window.h>
#ifdef MSVC
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>

void DisplayCube()
{
	glBegin(GL_QUADS);

	glColor3ub(255,0,0); //face rouge
	glVertex3d(1,1,1);
	glVertex3d(1,1,-1);
	glVertex3d(-1,1,-1);
	glVertex3d(-1,1,1);

	glColor3ub(0,255,0); //face verte
	glVertex3d(1,-1,1);
	glVertex3d(1,-1,-1);
	glVertex3d(1,1,-1);
	glVertex3d(1,1,1);

	glColor3ub(0,0,255); //face bleue
	glVertex3d(-1,-1,1);
	glVertex3d(-1,-1,-1);
	glVertex3d(1,-1,-1);
	glVertex3d(1,-1,1);

	glColor3ub(255,255,0); //face jaune
	glVertex3d(-1,1,1);
	glVertex3d(-1,1,-1);
	glVertex3d(-1,-1,-1);
	glVertex3d(-1,-1,1);

	glColor3ub(0,255,255); //face cyan
	glVertex3d(1,1,-1);
	glVertex3d(1,-1,-1);
	glVertex3d(-1,-1,-1);
	glVertex3d(-1,1,-1);

	glColor3ub(255,0,255); //face magenta
	glVertex3d(1,-1,1);
	glVertex3d(1,1,1);
	glVertex3d(-1,1,1);
	glVertex3d(-1,-1,1);

	glEnd();
}


int main(int argc, char ** argv)
{
	std::cout << "Lancement ..." << std::endl;

	// Object Creation
	C3::Window win;
	C3::WindowMode mode(800,600);
	mode.Fullscreen = true;
	C3::OpenGLContextSettings openGLSettings(2,1);

	// Create the window
	std::cout << "Creation ..." << std::endl;
	win.Create(mode,"OpenGL2",openGLSettings);

	// Initialise OpenGL
	glViewport(0, 0, 800, 600);

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)800/(GLfloat)600,0.1f,100.0f);

	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

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

		// Draw the scene
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity( );

		gluLookAt(3,4,2,0,0,0,0,0,1);

		DisplayCube();

		// Swap buffers
		win.Display();
	}

	std::cout << "... End" << std::endl;
	return 0;
}
