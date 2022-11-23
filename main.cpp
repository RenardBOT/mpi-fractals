#include "Mandelbrot.hpp"
#include <vector>
#include <iostream>
#include "GL/glut.h"

typedef std::vector<double> vector1D;
typedef std::vector<std::vector<double>> vector2D;

int width = 1200;
int height = 900;

int maxIterations = 1000;

double xPlotMin = -2;
double xPlotMax = 1;
double yPlotMin = -1;
double yPlotMax = 1;

void printVector2D(vector2D * v){
      for (auto & row : *v) {
            for (auto & val : row) {
                  std::cout << val << " ";
            }
            std::cout << std::endl;
      }
}

bool initialize()
{
	glEnable(GL_DEPTH_TEST);
	return true;
}

void draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
      
      glBegin(GL_POINTS);
      glColor3f(0,1,0);
	for (int y = 0; y < height ; y++) 
	{
            double yval = Mandelbrot::scale(y,-1.0,1.0,0,height-1);
		for (int x = 0; x < width; x++) 
		{ 
                  double xval = Mandelbrot::scale(x,-1.0,1.0,0,width-1);
                  int c = Mandelbrot::calc(xval,yval,maxIterations);
                  if(c == maxIterations)
                        glColor3f(0,1,0);
                  else
                        glColor3f(0,0,0);
			glVertex2f(xval,yval);
		}
	}
      glEnd();
      glutPostRedisplay();
  //begin - rendering a simple triangle
	glutSwapBuffers();
}

int main(int argc, char **argv){
      /* std::cout << "----- DEBUT SIMULATION : Iterations = " << maxIterations << std::endl;
      vector2D vec(height, vector1D(width,0.0));
      for(int y = 0 ; y < height ; y++){
            for(int x = 0 ; x < width ; x++){
                  double scaledX = Mandelbrot::scale(x,xPlotMin,xPlotMax,0,(double)width-1);
                  double scaledY = Mandelbrot::scale(y,yPlotMin,yPlotMax,height-1,0);
                  int c = Mandelbrot::calc(scaledX,scaledY,maxIterations);
                  if(c == maxIterations)
                        std::cout << "■" ;
                  else
                        std::cout << "□" ;
            }
            std::cout << std::endl;
      } */
      glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutCreateWindow("Truc");	
	glutCreateMenu(NULL);
	if(!initialize()) return 1;
	glutDisplayFunc(draw);
	//glutIdleFunc(idle);
	//glutReshapeFunc(reshape);
	//glutKeyboardFunc(keyboardDown);
	//GUI construction
	glutMainLoop();
	return 0;
      //printVector2D(&vec);

}


