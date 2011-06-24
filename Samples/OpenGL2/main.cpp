#include <iostream>

#include <Window.h>
#include <windows.h>

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

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	std::cout << "Lancement ..." << std::endl;

	// Object Creation
	PCM::Window win;
	PCM::WindowMode mode(800,600);
	PCM::OpenGLContextSettings openGLSettings(2,1);

	// Create the window
	std::cout << "Creation ..." << std::endl;
	win.Create(mode,"OpenGL2",openGLSettings);

	// Initialise OpenGL
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective(70,(double)640/480,1,1000);

	glEnable(GL_DEPTH_TEST);

	// Draw loop
	std::cout << "Affichage ..." << std::endl;
	while(win.IsOpened())
	{
		// Events
		PCM::Event event;
		while(win.PoolEvent(event))
		{
			std::cout << "Event !" << std::endl;
			if(event.Type == PCM::Event::Closed)
			{
				std::cout << "Close ... " << std::endl;
				win.Close();
			}
			else if(event.Type == PCM::Event::KeyPressed)
			{
				if(event.Key.Code == PCM::Key::Escape)
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
