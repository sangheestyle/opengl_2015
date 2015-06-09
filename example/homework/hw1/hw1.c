#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


#define NUM_POINTS 200000
float lorenz_points[NUM_POINTS][3];

// view angle
int view_angle_x = 0;
int view_angle_y = 0;
int view_angle_z = 0;

int axes = 0;
int iter = 0;
int step_size = 10;

// Lorenz Parameters
double s  = 10;
double b  = 8.0/3.0;
double r  = 28;

/*
 * Draw continuous line with lorenz_points
 */
void draw_continuous_line(float point_ptr[][3], int* iter,
                          int step_size, int num_points){
	glBegin(GL_LINE_STRIP);
	int i = 0;
  int color = 0;
	while(i < *iter && i < num_points){
    if (i % 1000 == 0)
      color += 1;
    color %= 4;

    if (color == 0)
		  glColor3f(1.0f, 0.0f, 0.0f);
    else if (color == 1)
		  glColor3f(0.5f, 0.0f, 1.0f);
    else if (color == 2)
		  glColor3f(0.0f, 1.0f, 0.0f);
    else if (color == 3)
		  glColor3f(1.0f, 1.0f, 0.0f);

		glVertex3fv(point_ptr[i++]);
	}
	glEnd();

	if(*iter < num_points){
		if(*iter + step_size > num_points)
      *iter = num_points;
    else
      *iter += step_size;
	}
}

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  //  Maximum length of text string
void Print(const char* format , ...)
{
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

/*
 * OpenGL (GLUT) calls this routine to display the scene
*/
void display(){
  const double len = 50;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

  // Set view angle
	glRotatef(view_angle_x, 1, 0, 0);
	glRotatef(view_angle_y, 0, 1, 0);
	glRotatef(view_angle_z, 0, 0, 1);

  draw_continuous_line(lorenz_points, &iter, step_size, NUM_POINTS);

  //  White
  glColor3f(1,1,1);
  //  Draw axes
  if (axes)
  {
     glBegin(GL_LINES);
     glVertex3d(0.0,0.0,0.0);
     glVertex3d(len,0.0,0.0);
     glVertex3d(0.0,0.0,0.0);
    glVertex3d(0.0,len,0.0);
     glVertex3d(0.0,0.0,0.0);
     glVertex3d(0.0,0.0,len);
     glEnd();
     //  Label axes
     glRasterPos3d(len,0.0,0.0);
     Print("X");
     glRasterPos3d(0.0,len,0.0);
     Print("Y");
     glRasterPos3d(0.0,0.0,len);
     Print("Z");
  }

  //  Five pixels from the lower left corner of the window
  glWindowPos2i(5,5);
  //  Print the text string
  Print("Angle=%d,%d,%d p=%lf, Step=%d, Iter=%d",
      view_angle_x, view_angle_y, view_angle_z, r, step_size, iter);

	glFlush();
	glutSwapBuffers();
	glPopMatrix();
}

/*
 * Generate coordinates using the Lorenz Attractor
 */
void generate_lorenz(int num_steps, float point_ptr[][3]) {

	// Coordinates
	float x = point_ptr[0][0] = 1;
	float y = point_ptr[0][1] = 1;
	float z = point_ptr[0][2] = 1;

	/*  Time step  */
	float dt = 0.001;

	for (int i=0; i< num_steps-1; i++){
		float dx = s*(y-x);
		float dy = x*(r-z)-y;
		float dz = x*y - b*z;

		x += dt*dx;
		y += dt*dy;
		z += dt*dz;

		point_ptr[i+1][0] = x;
		point_ptr[i+1][1] = y;
		point_ptr[i+1][2] = z;
	}
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      view_angle_y += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      view_angle_y -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      view_angle_x += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      view_angle_x -= 5;
   //  Keep angles to +/-360 degrees
   view_angle_x %= 360;
   view_angle_y %= 360;
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   // Exit on ESC
   if (ch == 27)
     exit(0);
   // Reset view angle
   else if (ch == '0')
     view_angle_x = view_angle_y = view_angle_z = 0;
   // Toggle axes
   else if (ch == 'a')
     axes = 1 - axes;
   // Restart animation
   else if (ch == 'r')
     iter = 0;
   // Rotate z
   else if (ch == 'Z')
     view_angle_z += 5;
   else if (ch == 'z')
     view_angle_z -= 5;
   // Step size
   else if (ch == 'S')
     step_size += 1;
   else if (ch == 's')
     step_size -= 1;
   // Change lorenz parameter p
   else if (ch == 'P'){
     r += 1;
     generate_lorenz(NUM_POINTS, lorenz_points);
     iter = 0;
   } else if (ch == 'p') {
     r -= 1;
     generate_lorenz(NUM_POINTS, lorenz_points);
     iter = 0;
   }

   view_angle_z %= 360;
   glutPostRedisplay();
}

void reshape(int width, int height) {
  GLfloat h = (GLfloat) height / (GLfloat) width;

  glViewport(0, 0, (GLint) width, (GLint) height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-5.0, 5.0, -h*2, h*2, 1.0, 300.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0, 0.0, -60.0);
}

void idle(void) {
	iter += step_size;
	glutPostRedisplay();
}



int main(int argc,char* argv[]) {
	generate_lorenz(NUM_POINTS, lorenz_points);

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  glutInitWindowSize(500, 500);
	glutCreateWindow("Sanghee Kim");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
  glutSpecialFunc(special);
  glutKeyboardFunc(key);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}
