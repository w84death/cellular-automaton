// ----------------------------------------
// OpenGL Cellular Automaton Engine
// Krzysztof Jankowski <kj@p1x.in>
//
// (c)2015 P1X
// http://p1x.in
//
// gcc ogl.cpp -o ogl.app -lglut -lGL -lGLU
// g++ -Os ogl.cpp -o ogl.app -lglut -lGL -lGLU
//
// ----------------------------------------

// LIBS
// ----------------------------------------

// #include <GL/glut.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <stdio.h>
#include <time.h>

// SYSTEM VARS
// ----------------------------------------

int FPS       = 60;
bool fullScreenMode  = true;
char title[]         = "OpenGL Cellular Automaton Engine";
int windowWidth      = 320;
int windowHeight     = 240;
int windowPosX       = 50;
int windowPosY       = 50;
int refreshMills     = 1000/FPS;
float camera_scale   = 0.6f;

// AUTOMATION VARS
// ----------------------------------------
static int CELLS_ARRAY_SIZE   = 96;
float cells_main_array[96][96];
float cells_buffer_array[96][96];

static float CELL_START_COLOR = 0.4f;
static float CELL_STEP_COLOUR = 0.01f;
static float CELL_MIN_COLOUR  = 0.01f;
static float CELL_MAX_COLOUR  = 0.9f;
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
         if (random_f() >= 0.9){
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
   stat_iteration++;
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

void draw_cells(){
   float cell;
   float cell_size = 1.0f / CELLS_ARRAY_SIZE;
   float half_size = cell_size * 0.5f;
   float new_x;
   float new_y;
   char buf[256];

   for (int y = 0; y < CELLS_ARRAY_SIZE; y++){
      for (int x = 0; x < CELLS_ARRAY_SIZE; x++){
         cell = cells_main_array[x][y];
         if (cell > 0.0f){
            new_x = (cell_size*x) - 0.5f;
            new_y = (cell_size*y) - 0.5f;
            glBegin(GL_QUADS);
               glColor3f(cell, cell, 1.0f);
               glVertex2f(new_x - half_size, new_y - half_size);
               glVertex2f(new_x - half_size, new_y + half_size);
               glVertex2f(new_x + half_size, new_y + half_size);
               glVertex2f(new_x + half_size, new_y - half_size);
            glEnd();
         }
      }
   }

   // TITLE
   glColor3f(1.0f, 1.0f, 1.0f);
   glRasterPos3f(-0.5f, 0.5f, 0.0f);
   glutBitmapString( GLUT_BITMAP_9_BY_15, (unsigned char*) title);

   // STATS
   glColor3f(1.0f, 1.0f, 1.0f);
   glRasterPos3f(0.0f, 0.5f, 0.0f);
   snprintf(buf, sizeof(buf) - 1, "ITERATION: [%i] ALIVE: [%i] CHANGE: [%i]", stat_iteration, stat_alive, stat_change);
   glutBitmapString( GLUT_BITMAP_9_BY_15, (unsigned char*) buf);
}

void display() {
   glClear(GL_COLOR_BUFFER_BIT);
   draw_cells();
   glutSwapBuffers();
}



// MAIN
// ----------------------------------------

int main(int argc, char** argv) {
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE);

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
   init_automation();
   glutMainLoop();
   return 0;
}
