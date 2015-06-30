/*
 * Homework 3
 */
#include <time.h>
#include <stdio.h>
#include <math.h>
#include "CSCIx229.h"
int axes=0;       //  Display axes
int th=-20;         //  Azimuth of view angle
int ph=40;         //  Elevation of view angle
int zh=0;         //  Azimuth of light
int spin=0;
int fov=55;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=13.3;   //  Size of world
int    light=1;    //  Lighting
int move=1;        // Move light
float ylight=5;    // Elevation of light

// Light values
int emission  =   0;  // Emission intensity (%)
int ambient   =  30;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shinyvec[1];    // Shininess (value)

// Texture
unsigned int texture[6]; // dice
unsigned int board;
unsigned int marble;

int ntex=0;
int mode=0;

const int num_vertices = 13;
const int num_vertex_properties = 4;
int vertices[num_vertices][num_vertex_properties];

// TODO: workaround for Bus error: 10
const int num_edges = 21;
const int num_edge_properties = 2;
int edges[num_edges][num_vertex_properties];
int edge_counter = 0;

int obj;
double leaves_colors[10];

// Header on mac has M_PI otherwise don't
#ifndef M_PI
  #define M_PI 3.14159265358979323846
#endif

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
 *  Draw a ball
 *     at (x,y,z)
 *     radius r
 */
static void ball(double x,double y,double z,double r)
{
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //  White ball
   glColor3f(1,1,1);
   glutSolidSphere(1.0,16,16);
   //  Undo transofrmations
   glPopMatrix();
}


static void edge(double bx, double by, double bz,
                 double ex, double ey, double ez,
                 double r)
{
   double d = (1.0/32) * (2*M_PI);
   by += 0.1;
   ey += 0.1;

   glBegin(GL_QUAD_STRIP);
   glColor3f(0.8f,0.4f,0.2f);
   for (int i = 0; i <= 32; i++) {
       // Generate a pair of points, on top and bottom of the strip.
       glNormal3d(cos(d*i), sin(d*i), 0);  // Normal for BOTH points.
       glVertex3d(bx+r*cos(d*i), by+r*sin(d*i), bz); // Begin
       glVertex3d(ex+r*cos(d*i), ey+r*sin(d*i), ez); // End
   }
   glEnd();
}

/*
 *  Draw a vertex
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void vertex(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y+dy,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Enable textures
   //glEnable(GL_TEXTURE_2D);
   glColor3f(1,1,1);
   //  Front
   //glBindTexture(GL_TEXTURE_2D,texture[0]);
   glBegin(GL_QUADS);
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
   glTexCoord2f(1,0); glVertex3f(+1,-1, 1);
   glTexCoord2f(1,1); glVertex3f(+1,+1, 1);
   glTexCoord2f(0,1); glVertex3f(-1,+1, 1);
   glEnd();
   //  Back
   //glBindTexture(GL_TEXTURE_2D,texture[1]);
   glBegin(GL_QUADS);
   glNormal3f( 0, 0,-1);
   glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,-1);
   glEnd();
   //  Right
   //glBindTexture(GL_TEXTURE_2D,texture[2]);
   glBegin(GL_QUADS);
   glNormal3f(+1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,+1);
   glEnd();
   //  Left
   //glBindTexture(GL_TEXTURE_2D,texture[3]);
   glBegin(GL_QUADS);
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,+1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   glEnd();
   //  Top
   //glBindTexture(GL_TEXTURE_2D,texture[4]);
   glBegin(GL_QUADS);
   glNormal3f( 0,+1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,+1,+1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   glEnd();
   //  Bottom
   //glBindTexture(GL_TEXTURE_2D,texture[5]);
   glBegin(GL_QUADS);
   glNormal3f( 0,-1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,-1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,-1,+1);
   glEnd();
   //  Undo transformations and textures
   glPopMatrix();
}

static void cowboy(double x,double y,double z,
                   double dx,double dy,double dz,
                   double th)
{
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y+dy,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   glCallList(obj);
   glPopMatrix();
}


/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   const double len=2.5;  //  Length of axes
   double Ex = -2*dim*Sin(th)*Cos(ph);
   double Ey = +2*dim        *Sin(ph);
   double Ez = +2*dim*Cos(th)*Cos(ph);
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   glDepthMask(GL_TRUE);
   // glEnable(GL_CULL_FACE);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective - set eye position
   gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
   //  Light switch
   if (light)
   {
      //  Translate intensity to color vectors
      float Ambient[]   = {0.3,0.3,0.3,1.0};
      float Diffuse[]   = {1,1,1,1};
      float Specular[]  = {1,1,0,1};
      float white[]     = {1,1,1,1};
      //  Light direction
      float Position[]  = {10*Cos(zh),ylight,10*Sin(zh),1.0};
      //  Draw light position as ball (still no lighting here)
      ball(Position[0],Position[1],Position[2] , 0.1);
      //  Enable lighting with normalization
      glEnable(GL_LIGHTING);
      glEnable(GL_NORMALIZE);
      //  glColor sets ambient and diffuse color materials
      glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
      glEnable(GL_COLOR_MATERIAL);
      //  Enable light 0
      glEnable(GL_LIGHT0);
      glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
      glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
      glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
      glLightfv(GL_LIGHT0,GL_POSITION,Position);
      glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,32.0f);
      glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   }
   else
      glDisable(GL_LIGHTING);

   // draw vertices
   int i;
   for (i = 0; i < num_vertices; i++){
     int point = vertices[i][3];
     if (point == 0) {
       continue;
     } else if (point > 3) {
       double size = 0.15 + (point*0.01);
       cowboy(vertices[i][0],size*2+1,vertices[i][2],
              size, size, size,
              0);
     } else {
       vertex(vertices[i][0],vertices[i][1],vertices[i][2],
              0.2,point*0.2,0.2,
              0);
     }
   }

   // draw edges
   for (i = 0; i < edge_counter; i++) {
     int begin = edges[i][0];
     int end = edges[i][1];
     edge(vertices[begin][0], vertices[begin][1], vertices[begin][2],
          vertices[end][0], vertices[end][1], vertices[end][2],
          0.05);
   }

   land(0,-0.2f,0, 12,0.2,12, 0);
   float e = edge_counter*0.02 + 1;
   // x,y,z, dx,dy,dz,th, th,tr,lh,lr, green, type
   tree(8,0,11.5, e*1,e*1,e*1, 0, 1,0.2,2.0,1.0, leaves_colors[0], 1);
   tree(9,0,9, 1,1,1, 0, 1,0.2,1.7,1.0, leaves_colors[1], 2);
   tree(11,0,11, 1,e*1,1, 0, 1.3,0.2,1.5,1.0, leaves_colors[2], 3);

   tree(-9,0,-10,     e*1,e*1,e*1,0, 1.0,0.2,2.1,1.1, leaves_colors[0], 0);
   tree(-10,0,-8,     1,1,1,0, 1.4,0.1,1.8,0.8, leaves_colors[1], 1);
   tree(-11.5,0,-6.0, e*1,e*1,e*1,0, 1.1,0.3,1.7,0.5, leaves_colors[2], 2);
   tree(-10,0,-3,     1,1,1,0, 1.0,0.2,1.4,1.0, leaves_colors[3], 3);
   tree(-9,0,-5,     1,1,1,0, 1.0,0.2,1.4,1.0, leaves_colors[3], 4);

   //  Draw axes
   glDisable(GL_LIGHTING);
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
   //Print("Angle=%d,%d  Dim=%.1f Light=%s",th,ph,dim,light?"On":"Off");
   Print("Developer %d and %d modified same files.",
         edges[edge_counter][0], edges[edge_counter][1]);
   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void idle()
{
   //  Elapsed time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*t,360.0);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
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
   else if (key == GLUT_KEY_PAGE_DOWN)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key == GLUT_KEY_PAGE_UP && dim>1)
      dim -= 0.1;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Project(fov,asp,dim);
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
   //  Toggle light
   else if (ch == 'l' || ch == 'L')
      light = 1-light;
   //  Toggle light movement
   else if (ch == 's' || ch == 'S')
      move = 1-move;
   //  Light elevation
   else if (ch=='[')
      ylight -= 0.1;
   else if (ch==']')
      ylight += 0.1;

   spin %= 360;
   glutIdleFunc(move?idle:NULL);
   //  Reproject
   Project(fov,asp,dim);
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
   Project(fov,asp,dim);
}

void animate_graph()
{
  vertices[edges[edge_counter][0]][3]++;
  vertices[edges[edge_counter][1]][3]++;
  edge_counter += 1;

  if (edge_counter >= num_edges) {
    int i;
    for (i = 0; i < num_vertices; i++) {
      vertices[i][3] = 0;
    }
    edge_counter = 0;
  }
}

void rotate_scene()
{
  th += 1;
  th %= 360;
  glutTimerFunc(1,rotate_scene, 0);
}

void timerFunc()
{
  animate_graph();
  glutTimerFunc(1000, timerFunc, 0);
}

const char* getfield(char* line, int num)
{
    const char* tok;
    for (tok = strtok(line, ","); tok && *tok; tok = strtok(NULL, ",\n"))
    {
      if (!--num)
        return tok;
    }
    return NULL;
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
    FILE* e_stream = fopen("edges.csv", "r");
    char line[1024];
    int counter = 0;
    while (fgets(line, 1024, e_stream))
    {
      int i;
      for (i = 0; i < num_edge_properties; i++) {
        char* tmp = strdup(line);
        edges[counter][i] = atoi(getfield(tmp, i+1));
        free(tmp);
      }
      counter++;
    }

    FILE* v_stream = fopen("vertices.csv", "r");
    counter = 0;
    while (fgets(line, 1024, v_stream))
    {
      int i;
      for (i = 0; i < num_vertex_properties; i++) {
        char* tmp = strdup(line);
        vertices[counter][i] = atoi(getfield(tmp, i+1));
        free(tmp);
      }
      counter++;
    }

    srand(time(NULL));
    for (int i =0; i <= 10; i++)
      leaves_colors[i] = (rand()%(9-3) + 3)*0.1;

   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(900,600);
   glutCreateWindow("Sanghee Kim");
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   glutIdleFunc(idle);

   glutTimerFunc(1000, timerFunc, 0);
   glutTimerFunc(1, rotate_scene, 0);
   obj = LoadOBJ(argv[1]);
   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   glutMainLoop();
   return 0;
}
