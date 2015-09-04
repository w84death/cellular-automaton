// ----------------------------------------
// OpenGL Cellular Automaton Engine
// Krzysztof Jankowski <kj@p1x.in>
//
// (c)2015 P1X
// http://p1x.in
//
// gcc -Os ogl.cpp -o ogl.app -lglut -lGL -lGLU -lm
//
// ----------------------------------------

// LIBS
// ----------------------------------------

#include <GL/freeglut.h>
#include <GL/gl.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

// SYSTEM VARS
// ----------------------------------------

int FPS       = 60;
bool fullScreenMode  = true;
char title[]         = "OpenGL Cellular Automaton Engine";
static float VERSION = 1.4f;
int windowWidth      = 320;
int windowHeight     = 240;
int windowPosX       = 50;
int windowPosY       = 50;
int refreshMills     = 1000/FPS;
float camera_scale   = 20.0f;

// AUTOMATION VARS
// ----------------------------------------
static int CELLS_ARRAY_SIZE   = 48;
static int MAX_CELLS = CELLS_ARRAY_SIZE*CELLS_ARRAY_SIZE;
float cells_main_array[48][48];
float cells_buffer_array[48][48];

static float CELL_START_COLOR = 0.5f;
static float CELL_STEP_COLOUR = 0.005f;
static float CELL_MIN_COLOUR  = 0.05f;
static float CELL_MAX_COLOUR  = 1.0f;
bool automation_mode          = false;
int stat_iteration            = 0;
int stat_alive                = 0;
int stat_change               = 0;

// INIT
// ----------------------------------------

void initGL() {

   glClearColor(0.1f, 0.1f, 1.0f, 1.0f);
}

// HELPERS
// ----------------------------------------

float random_f(){
   float r = (float)rand() / (float)RAND_MAX;
   return r;
}

float random_fcolor(){
   float r = random_f();
   if (r < 0.2f){
      r = 0.0f;
   }
   if  (r > 0.8f){
      r = 0.8f;
   }
   return r;
}

void reshape(GLsizei width, GLsizei height) {
   if (height == 0) height = 1;
   GLfloat aspect = (GLfloat)width / (GLfloat)height;

   glViewport(0, 0, width, height);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   if (width >= height) {
      gluOrtho2D(-camera_scale * aspect, camera_scale * aspect, -camera_scale, camera_scale);
   } else {
      gluOrtho2D(-camera_scale, camera_scale, -camera_scale / aspect, camera_scale / aspect);
   }
}

void change_fps(int change_fps){
   int new_fps = FPS + change_fps;
   if (new_fps < 1){
      new_fps = 1;
   }
   if (new_fps > 60){
      new_fps = 60;
   }
   FPS = new_fps;
   refreshMills = 1000/FPS;
}

// CELLULAR AUTOMATION
// ----------------------------------------

void init_arrays(){

   for (int y = 0; y < CELLS_ARRAY_SIZE; y++){
      for (int x = 0; x < CELLS_ARRAY_SIZE; x++){
         cells_main_array[x][y] = 0.0f;
         cells_buffer_array[x][y] = 0.0f;
      }
   }
}

void fill_array(){
   float new_cell;

   for (int y = 0; y < CELLS_ARRAY_SIZE; y++){
      for (int x = 0; x < CELLS_ARRAY_SIZE; x++ ){
         if (random_f() >= 0.8){
            cells_main_array[x][y] = CELL_START_COLOR;
         }else{
            cells_main_array[x][y] = 0.0f;
         }
      }
   }

   stat_iteration = 0;
}

int count_cells(int cx, int cy, float treshold){
   int count = 0;

   for (int y = cy-1; y <= cy+1; y++){
      for (int x = cx-1; x <= cx+1; x++){
         if (x >= 0 and y >= 0 and x < CELLS_ARRAY_SIZE and y < CELLS_ARRAY_SIZE and !( x == cx and y == cy)){
            if (cells_main_array[x][y] > treshold){
               count++;
            }
         }
      }
   }

   return count;
}

float cell_gain_colour(float colour){
   float new_colour = colour + CELL_STEP_COLOUR;
   if (new_colour > CELL_MAX_COLOUR){
      new_colour = CELL_MAX_COLOUR;
   }
   return new_colour;
}

float cell_lose_colour(float colour){
   float new_colour = colour - CELL_STEP_COLOUR;
   if (new_colour < CELL_MIN_COLOUR){
      new_colour = CELL_MIN_COLOUR;
   }
   return new_colour;
}

void automation(){
   int count;
   float cell;
   float new_cell;

   for (int y = 0; y < CELLS_ARRAY_SIZE; y++){
      for (int x = 0; x < CELLS_ARRAY_SIZE; x++){
         count = count_cells(x, y, 0.0f);
         cell = cells_main_array[x][y];
         if (cell > 0.0f){
            if (count < 2 or count > 3){
               new_cell = 0.0f;
            }else{
               new_cell = cell_gain_colour(cell);
            }
         }else{
            if (count == 3){
               new_cell = CELL_START_COLOR;
            }else{
               new_cell = 0.0f;
            }
         }
         cells_buffer_array[x][y] = new_cell;
      }
   }
}

void automation2(){
   int count;
   float cell;
   float new_cell;

   for (int y = 0; y < CELLS_ARRAY_SIZE; y++){
      for (int x = 0; x < CELLS_ARRAY_SIZE; x++){
         count = count_cells(x, y, 0.3f);
         cell = cells_main_array[x][y];
         if (cell > 0.2f){
            if (count < 2 or count > 3){
               new_cell = cell_lose_colour(cell);
            }else{
               new_cell = cell_gain_colour(cell);
            }
         }else{
            if (count == 3){
               new_cell = cell_gain_colour(cell);
            }else{
               new_cell = 0.0f;
            }
         }
         cells_buffer_array[x][y] = new_cell;
      }
   }
}

void swap_arrays(){
   float old_cell;
   float new_cell;
   stat_alive = 0;
   stat_change = 0;

   for (int y = 0; y < CELLS_ARRAY_SIZE; y++){
      for (int x = 0; x < CELLS_ARRAY_SIZE; x++){
         old_cell = cells_main_array[x][y];
         new_cell = cells_buffer_array[x][y];
         cells_main_array[x][y] = new_cell;
         if (new_cell > 0.0f) stat_alive++;
         if (old_cell != new_cell) stat_change++;
      }
   }
}
void clear_buffer_array(){}

 void init_automation(){
   init_arrays();
   fill_array();
}

void run_automation(){
   if (automation_mode){
      automation();
   }else{
      automation2();
   }
   if (stat_alive > 0) {
      stat_iteration++;
   }
   swap_arrays();
}

// INPUT
// ----------------------------------------

void keyboard(unsigned char key, int x, int y) {
   switch (key) {
      case 27:     // ESC key
         exit(0);
         break;
      case 13: // enter
         fill_array();
         break;
      case 32: // space
         automation_mode = !automation_mode;
         break;
   }
}

void specialKeys(int key, int x, int y) {
   switch (key) {
      case GLUT_KEY_F1:
         fullScreenMode = !fullScreenMode;
         if (fullScreenMode) {
            glutFullScreen();
         } else {
            glutReshapeWindow(windowWidth, windowHeight);
            glutPositionWindow(windowPosX, windowPosX);
         }
         break;
      case GLUT_KEY_RIGHT:
         break;
      case GLUT_KEY_LEFT:
         break;
      case GLUT_KEY_UP:
         change_fps(1);
         break;
      case GLUT_KEY_DOWN:
         change_fps(-1);
         break;
   }
}

void mouse(int button, int state, int x, int y) {
   if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
      // mouse_x = x;
      // mouse_y = y;
   }
}


// RENDERING
// ----------------------------------------

void Timer(int value) {
   run_automation();
   glutPostRedisplay();
   glutTimerFunc(refreshMills, Timer, 0);
}

void draw_floor(){

   glPushMatrix();
   glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
   glEnable ( GL_COLOR_MATERIAL ) ;
   glColor4f(0.5f, 0.5f, 0.8f, 1.0f);
   float size= CELLS_ARRAY_SIZE*0.8f;
   glBegin(GL_POLYGON);
       glVertex2d(-size, size);
       glVertex2d(-size, -size);
       glVertex2d(size, -size);
       glVertex2d(size, size);
   glEnd(); 
   glPopMatrix();
}

void draw_stats(){
   char buf[256];

   glPushMatrix();
   glTranslatef (-camera_scale, camera_scale-2, 0);
      // TITLE
   glColor3f(1.0f, 1.0f, 1.0f);
   glRasterPos3f(0.0f, 0.0f, 0.0f);
   snprintf(buf, sizeof(buf) - 1, "%s - version %f", title, VERSION);
   glutBitmapString( GLUT_BITMAP_9_BY_15, (unsigned char*) buf);
   glPopMatrix();

   glPushMatrix();
   glTranslatef (-camera_scale, -camera_scale+2, 0);  
   // STATS
   glColor3f(1.0f, 1.0f, 1.0f);
   glRasterPos3f(0.0f, 0.0f, 0.0f);
   snprintf(buf, sizeof(buf) - 1, "ITERATION: [%i] ALIVE: [%i/%i] CHANGE: [%i]", stat_iteration, stat_alive, MAX_CELLS, stat_change);
   glutBitmapString( GLUT_BITMAP_9_BY_15, (unsigned char*) buf);
   glPopMatrix();
}

void draw_one_cell(float x, float y, float size){
   glPushMatrix();
   glTranslatef (x, y, 0.0f);
   glColorMaterial ( GL_FRONT, GL_AMBIENT_AND_DIFFUSE ) ;
   glEnable ( GL_COLOR_MATERIAL ) ;
   float color = size > 0.45 ? 0.45 : size;
   glColor4f(color, color, 1.0, 1.0f);
   //glScalef(size,size,0.0f);
   float rf = size*45.0f;
   glRotatef(rf, 1.0f, 1.0f, 1.0f);
   // GLUquadricObj *quadratic;
   // quadratic = gluNewQuadric();
   // gluQuadricNormals(quadratic, GLU_SMOOTH);
   // float quad_size = stat_alive < (MAX_CELLS)*0.2 ? 16 : 12;
   // gluSphere(quadratic, size > 0.6 ? 0.6 : size, quad_size, quad_size);

   glutSolidCube(size);
   glPopMatrix();
}

void draw_cells(){
   float cell;
   float half_size = CELLS_ARRAY_SIZE * 0.5f;

   for (int y = 0; y < CELLS_ARRAY_SIZE; y++){
      for (int x = 0; x < CELLS_ARRAY_SIZE; x++){
         cell = cells_main_array[x][y];
         if (cell > 0.0f){
            //glColor3f(cell, cell, 1.0f);
            draw_one_cell(x-half_size,y-half_size,cell);
         }
      }
   }

   draw_stats();
}

 void camera_setup(){
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(120.0, 1.0f, 500.0f, -500.0f);
   glMatrixMode(GL_MODELVIEW);
 }

void camera_movement(){
   //float z = cos(stat_iteration*0.01f)*0.4f;
   //glTranslatef (0.0f, 0.0f, z);
  // glRotatef(z, 1.0f, 1.0f, 0.0f);
}

void display() {
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   //glEnable(GL_DEPTH_TEST);
   
   //glMatrixMode(GL_MODELVIEW);
   //glLoadIdentity();
   //gluLookAt (1.0f, 1.0f, 1.0f, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

   //draw_floor();
   draw_cells();
   //camera_movement();
   glutSwapBuffers();
}

void ambient_lighting(){
  glEnable(GL_LIGHTING);
 
  double amb = .2;
  GLfloat global_ambient[] = {amb,amb,amb, 0.1};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
}

void add_point_light(const float x, const float y, const float z, const float amb, const float diff, const float spec)
{
  glEnable(GL_LIGHTING);
 
  
  // GLfloat light_ambient[] = { 0.3, 0.0, 0.0, 1.0 };
  // GLfloat light_diffuse[] = { 0.0, 1.0, 0.0, 1.0 };
  // GLfloat light_specular[] = { 0.9, 0.9, 1.0, 1.0 };
  
  GLfloat light_ambient[] = { amb,amb,amb, 1.0 };
  GLfloat light_diffuse[] = {diff, diff, diff, 1.0 };
  GLfloat light_specular[] = {spec, spec, spec, 1.0 };
 
  GLfloat light_position[] = {x,y,z, 0.0 };
 
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
 
 light_position[0] = -x;
 light_position[1] = y;
 light_position[2] = -z;

   glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT1, GL_POSITION, light_position);
 
  glEnable(GL_LIGHT0); //enable the light after setting the properties
  glEnable(GL_LIGHT1); //enable the light after setting the properties
 
}

// MAIN
// ----------------------------------------

int main(int argc, char** argv) {
   glutInit(&argc, argv);

   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
   glEnable(GL_MULTISAMPLE);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   glEnable(GL_LINE_SMOOTH);
   glHint(GL_LINE_SMOOTH, GL_NICEST);

   glEnable(GL_POINT_SMOOTH);
   glHint(GL_POINT_SMOOTH, GL_NICEST);

   glutCreateWindow(title);
   if(fullScreenMode){
      glutFullScreen();
   }else{
      glutInitWindowSize(windowWidth, windowHeight);
      glutInitWindowPosition(windowPosX, windowPosY);
   }

   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutTimerFunc(0, Timer, 0);

   glutKeyboardFunc(keyboard);
   glutSpecialFunc(specialKeys);
   //glutMouseFunc(mouse);

   initGL();
   //ambient_lighting();
   add_point_light(0.5f, 0.6f, 3.0f, 0.6, 0.3, 1);
   camera_setup();
   init_automation();
   glutMainLoop();
   return 0;
}
