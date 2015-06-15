/*
 *  Homework 2
 *
 *  Key bindings:
 *  m          Toggle between perspective and orthogonal
 *  +/-        Changes field of view for perspective
 *  a          Toggle axes
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>

//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int axes=0;       //  Display axes
int mode=0;       //  Projection mode
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=5.0;   //  Size of world

// Tree level of green
double leaves_colors[10];

//  Macro for sin & cos in degrees
#define Cos(th) cos(3.1415927/180*(th))
#define Sin(th) sin(3.1415927/180*(th))

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
 *  Set projection
 */
static void Project()
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation
   if (mode)
      gluPerspective(fov,asp,dim/4,4*dim);
   //  Orthogonal projection
   else
      glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

/*
 *  Draw a tree
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the x axis
 *     trunk height and round (trunk_h, trunk_r)
 *     leaves height and round (leaves_h, leaves_r)
 *     type (type)
 */
static void tree(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th,
                 double trunk_h, double trunk_r,
                 double leaves_h, double leaves_r,
                 double green_depth,
                 int type)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,1,0,0);
   glScaled(dx,dy,dz);

   double d = (1.0/32) * (2*M_PI);

   // Draw Trunk
   glBegin(GL_QUAD_STRIP);
   glColor3f(0.8f,0.4f,0.2f);
   for (int i = 0; i <= 32; i++) {
       // Generate a pair of points, on top and bottom of the strip.
       glNormal3d( cos(d*i), 0, sin(d*i));  // Normal for BOTH points.
       glVertex3d( trunk_r*cos(d*i), trunk_h, trunk_r*sin(d*i));  // Top point.
       glVertex3d( trunk_r*cos(d*i), 0, trunk_r*sin(d*i));  // Bottom point.
   }
   glEnd();

   // Draw Leaves (Side)
   glBegin(GL_QUAD_STRIP);
   glColor3f(0,green_depth,0);
   for (int i = 0; i <= 32; i++) {
       glNormal3d( cos(d*i), leaves_h, sin(d*i));
       // Type 2: Upside narrow
       if (type == 2)
         glVertex3d( leaves_r*0.5f*cos(d*i), trunk_h+leaves_h,
                     leaves_r*0.5f*sin(d*i));
       // Type 4: Cone style
       else if (type == 4)
         glVertex3d( 0, trunk_h+leaves_h, 0);
       else
         glVertex3d( leaves_r*cos(d*i), trunk_h+leaves_h,
                     leaves_r*sin(d*i));

       if (type == 3)
         glVertex3d( leaves_r*0.5f*cos(d*i), trunk_h, leaves_r*0.5f*sin(d*i));
       else
         glVertex3d( leaves_r*cos(d*i), trunk_h, leaves_r*sin(d*i));
   }
   glEnd();

   // Type 4, Cone, does not need to draw top of leaves
   if (type != 4) {
       // Draw Leaves (Top)
       glBegin(GL_POLYGON);
       glColor3f(0,green_depth-0.1f,0);
       for (int i = 0; i <= 32; i++) {
           // Type 2: Upside narrow
           if (type == 2)
               glVertex3d( leaves_r*0.5f*cos(d*i), trunk_h+leaves_h,
                           leaves_r*0.5f*sin(d*i));
           else
               glVertex3d( leaves_r*cos(d*i), trunk_h+leaves_h,
                           leaves_r*sin(d*i));  // Top point.
       }
      glEnd();
   }

   // Draw Leaves (Bottom)
   glBegin(GL_POLYGON);
   glColor3f(0,green_depth+0.1f,0);
   for (int i = 0; i <= 32; i++) {
       // Type 2: Upside narrow
       if (type == 3)
           glVertex3d( leaves_r*0.5f*cos(d*i), trunk_h,
                       leaves_r*0.5f*sin(d*i));
       else
           glVertex3d( leaves_r*cos(d*i), trunk_h, leaves_r*sin(d*i));
   }
   glEnd();

   //  Undo transofrmations
   glPopMatrix();
}

static void land(double x, double y, double z,
                 double dx, double dy, double dz,
                 double th)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Cube
   glBegin(GL_QUADS);
   //  Front
   glColor3f(1.0f,0.7f,0.4f);
   glVertex3f(-1,-1, 1);
   glVertex3f(+1,-1, 1);
   glVertex3f(+1,+1, 1);
   glVertex3f(-1,+1, 1);
   //  Back
   glColor3f(1.0f,0.7f,0.4f);
   glVertex3f(+1,-1,-1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,+1,-1);
   glVertex3f(+1,+1,-1);
   //  Right
   glColor3f(0.9f,0.7f,0.4f);
   glVertex3f(+1,-1,+1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+1,+1,+1);
   //  Left
   glColor3f(0.9f,0.7f,0.4f);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,-1,+1);
   glVertex3f(-1,+1,+1);
   glVertex3f(-1,+1,-1);

   //  Top
   glColor3f(0.745f,0.87f,0.420f);
   glVertex3f(-1,+1,+1);
   glVertex3f(+1,+1,+1);
   glVertex3f(+1,+1,-1);
   glVertex3f(-1,+1,-1);
   //  Bottom
   glColor3f(0.93f,0.63f,0.4f);
   glVertex3f(-1,-1,-1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,-1,+1);
   glVertex3f(-1,-1,+1);
   //  End
   glEnd();
   //  Undo transofrmations
   glPopMatrix();
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   const double len=1.5;  //  Length of axes
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective - set eye position
   if (mode)
   {
      double Ex = -2*dim*Sin(th)*Cos(ph);
      double Ey = +2*dim        *Sin(ph);
      double Ez = +2*dim*Cos(th)*Cos(ph);
      gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
   }
   //  Orthogonal - set world orientation
   else
   {
      glRotatef(ph,1,0,0);
      glRotatef(th,0,1,0);
   }

   // Draw land
   land(0, -0.2f, 0,3,0.2, 3.5, 0);

   // Draw forest
   tree(2.5f,-0.1f,0.0f, 0.3,0.3,0.3, 0, 1.0f,0.2f,2.0f,1.0f, leaves_colors[0], 2);
   tree(2.0f,-0.1f,0.0f, 0.3,0.3,0.3, 0, 1.0f,0.2f,2.0f,1.0f, leaves_colors[5], 3);
   tree(0.0f,-0.1f,-2.5f, 0.3,0.5,0.3, 0, 1.2f,0.2f,2.0f,1.0f, leaves_colors[1], 2);
   tree(-1.2f,-0.1f,-1.0f, 0.3,0.5,0.7, 0, 0.9f,0.2f,2.0f,1.0f, leaves_colors[3], 3);
   tree(0,-0.2f,0 ,0.8,0.5,0.8, 30, 1.9f,0.2f,2.0f,1.0f, leaves_colors[4], 1);

   tree(1,0,1 ,0.5,0.5,0.5, 0, 1.0f,0.3f,4.0f,1.0f, leaves_colors[6], 2);
   tree(1.5f,0,3 ,0.5,0.5,0.5, 0, 1.1f,0.3f,3.0f,1.0f, leaves_colors[7], 3);
   tree(-1.2f,0,3 ,0.5,0.5,0.5, 0, 0.3f,0.3f,3.0f,1.0f, leaves_colors[8], 4);
   tree(-1.0f,0,-2.0f ,0.5,0.5,0.5, 0, 1.0f,0.3f,3.0f,0.5f, leaves_colors[9], 2);
   tree(-2.1f,0,-1.0f ,0.5,0.5,0.5, 0, 0.1f,0.3f,2.0f,1.0f, leaves_colors[0], 1);
   tree(-2.0f,0,-2.5f ,0.5,0.5,0.5, 0, 0.2f,0.3f,1.5f,1.0f, leaves_colors[1], 3);
   tree(-2.5f,0,-2.0f ,0.5,0.5,0.5, 0, 0.3f,0.3f,3.0f,1.0f, leaves_colors[2], 4);

   glColor3f(1,1,1);
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
   //  Display parameters
   glWindowPos2i(5,5);
   Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s",th,ph,dim,fov,mode?"Perpective":"Orthogonal");
   //  Render the scene and make it visible
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //  PageUp key - increase dim
   else if (key == GLUT_KEY_PAGE_UP)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key == GLUT_KEY_PAGE_DOWN && dim>1)
      dim -= 0.1;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0')
      th = ph = 0;
   //  Toggle axes
   else if (ch == 'a' || ch == 'A')
      axes = 1-axes;
   //  Switch display mode
   else if (ch == 'm' || ch == 'M')
      mode = 1-mode;
   //  Change field of view angle
   else if (ch == '-' && ch>1)
      fov--;
   else if (ch == '+' && ch<179)
      fov++;
   //  Reproject
   Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
  srand(time(NULL));
  for (int i = 0; i <= 10; i++)
    leaves_colors[i] = (rand()%(9 - 3) + 3)*0.1;

   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(600,600);
   glutCreateWindow("Sanghee Kim");
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   return 0;
}
