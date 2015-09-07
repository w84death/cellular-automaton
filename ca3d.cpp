// ----------------------------------------
// OpenGL Cellular Automaton Engine 3D
// Krzysztof Jankowski <kj@p1x.in>
//
// (c)2015 P1X
// http://p1x.in
//
// Repo:
// https://github.com/w84death/cellular-automaton
//
// Linux:
// gcc -Os ca3d.cpp -o ca3d.app -lglut -lGL -lGLU -lm
//
// OSX:
// gcc -o ca3d ca3d.cpp -framework GLUT -framework OpenGL
//
// ----------------------------------------

// LIBS
// ----------------------------------------

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <stdlib.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif
#include <stdio.h>
#include <time.h>
#include <math.h>

// SYSTEM VARS
// ----------------------------------------------------------------------------


bool fullscreen_mode  = true;
char win_title[]      = "OpenGL Cellular Automaton Engine 3D";
static float VERSION  = 1.6f;
int win_width         = 512;
int win_height        = 384;
int win_x             = 256;
int win_y             = 100;

static int FPS        = 60;
int refresh_ms        = 1000/FPS;

static float FOV      = 100.0f;
float cam_speed       = 0.8f;
float cam_move_speed  = 0.2f;
float cam_pos[]       = {20.0f, 14.0f, 20.0f, 5.0f, 0.0f, 5.0f};
float cam_look_pos[]  = {0.0f, 0.0f, 0.0f, 0.0f, 7.0f, 0.0f};
float cam_aim[]       = {0.0f, 0.0f};
float cam_fog_size    = 32.0f;
float cam_clear_color[4] = {0.3f, 0.05f, 0.6f, 1.0f};


// AUTOMATON VARS
// ----------------------------------------------------------------------------
static int CELLS_ARRAY_SIZE[]    = {36, 36, 36};
static int MAX_CELLS             = CELLS_ARRAY_SIZE[0]*CELLS_ARRAY_SIZE[1]*CELLS_ARRAY_SIZE[2];
int half[]                        = {CELLS_ARRAY_SIZE[0] * 0.5,  CELLS_ARRAY_SIZE[1] * 0.5, CELLS_ARRAY_SIZE[2] * 0.5};
float cells_main_array[36][36][36];
float cells_buffer_array[36][36][36];

static float CELL_ALIVE = 0.2f;
static float CELL_NEW   = 0.4f;
static float CELL_DEAD  = 0.0f;
static float CELL_MIN_COLOUR  = 0.2f;
static float CELL_MAX_COLOUR  = 0.8f;
static float CELL_STEP_COLOUR = 0.05f;

int STATE               = 0;
static int S_INT        = 0;
static int S_MENU       = 2;
static int S_SIMULATION = 4;
static int S_CREDITS    = 8;






// FUNCTIONS LIST
// ----------------------------------------------------------------------------

void display();
void render_loop();
void reshape();
void setup_lighting();
void setup_gl();
void setup_scene();
void camera_move();
void draw_floor();
void fullscreen_toggle();
void mouse();
void special_keys();
void simulation_loop();
void simulation_setup();
void simulation_draw();
void simulation_draw_cell();
void simulation_count_neigbours();
void simulationcell_gain_colour();
void simulationcell_lose_colour();
void simulation_do_work();













// MATH HELPERS
// ----------------------------------------------------------------------------



float random_f(){
   float r = (float)rand() / (float)RAND_MAX;
   return r;
}

float random_fcolor(){
   float r = random_f();
   if (r < CELL_MIN_COLOUR){
      r = 0.0f;
   }
   if  (r > CELL_MAX_COLOUR){
      r = CELL_MAX_COLOUR;
   }
   return r;
}





















// SIMULATION
// ----------------------------------------------------------------------------

void simulation_draw_cell(float s, float x, float y, float z, float c){
  float treshold_c;
  glPushMatrix();
    glTranslatef (x, y, z);
    glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
    glEnable ( GL_COLOR_MATERIAL ) ;
    treshold_c = c > 0.7 ? 0.7 : c;
    float color[] = {s > 0.45 ? 0.45 : s, s > 0.45 ? 0.45 : s, s > 0.45 ? 0.45 : s};
    color[0] += z*0.04f;
    color[1] += x*0.04f;
    color[2] += y*0.04f;
    glColor4f(color[0], color[1], color[2], 1.0f);
    glutSolidCube(s);
  glPopMatrix();
}

void simulation_setup(){
  for (int z = 0; z < CELLS_ARRAY_SIZE[2]; z++){
  for (int y = 0; y < CELLS_ARRAY_SIZE[1]; y++){
  for (int x = 0; x < CELLS_ARRAY_SIZE[0]; x++){
    cells_main_array[x][y][z] = CELL_DEAD;
    if (z>CELLS_ARRAY_SIZE[2]*0.20 and z < CELLS_ARRAY_SIZE[2]*0.80){
    if (y>CELLS_ARRAY_SIZE[1]*0.20 and y < CELLS_ARRAY_SIZE[1]*0.80){
    if (x>CELLS_ARRAY_SIZE[0]*0.20 and x < CELLS_ARRAY_SIZE[0]*0.80){
      cells_main_array[x][y][z] = (random_f() > 0.85) ? random_fcolor() : CELL_DEAD;  
    }}}
  }}}
}

void simulation_draw(){
  float c;
  float scale = 1.2f;
  float size = 0.1f;
  float new_x, new_y, new_z, new_c, new_s;

  for (int z = 0; z < CELLS_ARRAY_SIZE[2]; z++){
  for (int y = 0; y < CELLS_ARRAY_SIZE[1]; y++){
  for (int x = 0; x < CELLS_ARRAY_SIZE[0]; x++){
    c = cells_main_array[x][y][z];
    if ( c > CELL_ALIVE){
      new_x = (x - half[0]) * scale;
      new_y = (y - half[1]) * scale;
      new_z = (z - half[2]) * scale;
      new_c = (float)c;
      new_s = (size + (c*1.5)); // + (sin(x*y) * 0.1);
      simulation_draw_cell(new_s, new_x, new_y, new_z, new_c);
    }
  }}}
}


float simulation_cell_gain_colour(float colour){
   float new_colour = colour + CELL_STEP_COLOUR;
   if (new_colour > CELL_MAX_COLOUR){
      new_colour = CELL_MAX_COLOUR;
   }
   return new_colour;
}

float simulation_cell_lose_colour(float colour){
   float new_colour = colour - CELL_STEP_COLOUR;
   if (new_colour < CELL_MIN_COLOUR){
      new_colour = CELL_MIN_COLOUR;
   }
   return new_colour;
}

int simulation_count_neigbours(int cx, int cy, int cz, float treshold){
  int neigbours = 0;

  for (int z = cz-1; z <= cz+1; z++){
  for (int y = cy-1; y <= cy+1; y++){
  for (int x = cx-1; x <= cx+1; x++){
    if (x >= 0 and y >= 0 and z >= 0 and x < CELLS_ARRAY_SIZE[0] and y < CELLS_ARRAY_SIZE[1] and z < CELLS_ARRAY_SIZE[2]){
      if(!( x == cx and y == cy and z == cz)){
      if(!(z==cz-1 and y==cy-1 and x==cx-1)){
      if(!(z==cz-1 and y==cy-1 and x==cx+1)){
      if(!(z==cz-1 and y==cy+1 and x==cx-1)){
      if(!(z==cz-1 and y==cy+1 and x==cx+1)){
      if(!(z==cz+1 and y==cy-1 and x==cx-1)){
      if(!(z==cz+1 and y==cy-1 and x==cx+1)){
      if(!(z==cz+1 and y==cy+1 and x==cx-1)){
      if(!(z==cz+1 and y==cy+1 and x==cx+1)){ // am I crazy already?
      if (cells_main_array[x][y][z] >= treshold){
        neigbours++;
      }}}}}}}}}}
    }
  }}}

  return neigbours;
}

void simulation_do_work(){
   int neigbours;
   float cell;
   float new_cell;

  for (int z = 0; z < CELLS_ARRAY_SIZE[2]; z++){
  for (int y = 0; y < CELLS_ARRAY_SIZE[1]; y++){
  for (int x = 0; x < CELLS_ARRAY_SIZE[0]; x++){
    neigbours = simulation_count_neigbours(x, y, z, CELL_ALIVE);
    cell = cells_main_array[x][y][z];
    if (cell > CELL_ALIVE){
      if (neigbours < 2 or neigbours > 6){
         new_cell = simulation_cell_lose_colour(cell);
      }else{
         new_cell = simulation_cell_gain_colour(cell);
      }
    }else{
        if (neigbours == 5 ){
           new_cell = simulation_cell_gain_colour(cell);
        }else{
           new_cell = CELL_DEAD;
        }
     }
    cells_buffer_array[x][y][z] = new_cell;
  }}}
}


void simulation_swap_arrays(){
  float old_cell;
  float new_cell;

  for (int z = 0; z < CELLS_ARRAY_SIZE[2]; z++){
  for (int y = 0; y < CELLS_ARRAY_SIZE[1]; y++){
  for (int x = 0; x < CELLS_ARRAY_SIZE[0]; x++){
    old_cell = cells_main_array[x][y][z];
    new_cell = cells_buffer_array[x][y][z];
    cells_main_array[x][y][z] = new_cell;
  }}}
}

void simulation_loop(){
  simulation_do_work();
  simulation_swap_arrays();
}















// INPUTS
// ----------------------------------------------------------------------------

void mouse(int button, int state, int x, int y) {
  // void
}

void special_keys(int key, int x, int y) {
   switch (key) {
      case GLUT_KEY_F1:
         fullscreen_mode = !fullscreen_mode;
         fullscreen_toggle();
         break;
      case GLUT_KEY_RIGHT:
        if(fabs(cam_look_pos[0]-cam_look_pos[3]) < cam_speed ){
          cam_look_pos[0] += cam_speed;
        }
        break;
      case GLUT_KEY_LEFT:
        if(fabs(cam_look_pos[0]-cam_look_pos[3]) < cam_speed ){
          cam_look_pos[0] -= cam_speed;
        }
        break;
      case GLUT_KEY_UP:
        if(fabs(cam_look_pos[1]-cam_look_pos[4]) < cam_speed ){
          cam_look_pos[1] += cam_speed;
        }
        break;
      case GLUT_KEY_DOWN:
        if(fabs(cam_look_pos[1]-cam_look_pos[4]) < cam_speed ){
          cam_look_pos[1] -= cam_speed;
        }
        break;
      case GLUT_KEY_F2:
        simulation_setup();
        break;
      case GLUT_KEY_F3:
        cam_pos[0] = 0.0f;
        cam_pos[1] = 0.0f;
        cam_pos[2] = 28.0f;
        cam_look_pos[0] = 0.0f;
        cam_look_pos[1] = 0.0f;
        cam_look_pos[2] = 0.0f;
        break;
      case GLUT_KEY_F4:
        cam_pos[0] = 0.0f;
        cam_pos[1] = 0.0f;
        cam_pos[2] = 4.0f;
        cam_look_pos[0] = 0.0f;
        cam_look_pos[1] = 0.0f;
        cam_look_pos[2] = 0.0f;
        break;
      case GLUT_KEY_F5:
        cam_pos[0] = 20.0f;
        cam_pos[1] = 14.0f;
        cam_pos[2] = 20.0f;
        cam_look_pos[0] = 0.0f;
        cam_look_pos[1] = 0.0f;
        cam_look_pos[2] = 0.0f;
        break;

   }
}

void keyboard(unsigned char key, int x, int y) {
   switch (key) {
      case 27:     // ESC key
         //exit(0);
         break;
      case 13: // enter

         break;
      case 113: // q
        if(fabs(cam_pos[1]-cam_pos[4]) < cam_speed ){
          cam_pos[1] += cam_speed;
          cam_look_pos[1] += cam_speed;
        }
        break;
      case 101: // e
        if(fabs(cam_pos[1]-cam_pos[4]) < cam_speed ){
          cam_pos[1] -= cam_speed;
          cam_look_pos[1] -= cam_speed;
        }
        break;
      case 97: // a
        if(fabs(cam_pos[0]-cam_pos[3]) < cam_speed ){
          cam_pos[0] -= cam_speed;
          cam_look_pos[0] -= cam_speed;
        }
        break;
      case 100: // d
        if(fabs(cam_pos[0]-cam_pos[3]) < cam_speed ){
          cam_pos[0] += cam_speed;
          cam_look_pos[0] += cam_speed;
        }
        break;

      case 119: // w
        if(fabs(cam_pos[2]-cam_pos[5]) < cam_speed ){
          cam_pos[2] -= cam_speed;
          cam_look_pos[2] -= cam_speed;
        }
        break;
      case 115: // s
        if(fabs(cam_pos[2]-cam_pos[5]) < cam_speed ){
          cam_pos[2] += cam_speed;
          cam_look_pos[2] += cam_speed;
        }
        break;
   }
}









// OPEN GL
// ----------------------------------------------------------------------------

void reshape(GLsizei width, GLsizei height) {
  if (width == 0) width = 12;
  if (height == 0) height = 12;

  glViewport(0, 0, width, height);
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluPerspective (FOV, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void fullscreen_toggle(){
  if (fullscreen_mode) {
    glutFullScreen();
  } else {
    glutReshapeWindow(win_width, win_height);
    glutPositionWindow(win_x, win_y);
  }
}

void setup_lighting(){
  float amb = 0.6f;
  float diff = 0.3f;
  float spec = 0.2f;

  GLfloat global_ambient[]  = {amb, amb, amb, 0.2f};
  GLfloat light_ambient[]   = {amb, amb, amb, 1.0f };
  GLfloat light_diffuse[]   = {diff, diff, diff, 1.0f };
  GLfloat light_specular[]  = {spec, spec, spec, 1.0f };
  GLfloat light_position[]  = {0.0f, 0.0f, 0.0f, 0.0f };

  glEnable(GL_LIGHTING);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glEnable(GL_LIGHT0);

  glFogi(GL_FOG_MODE, GL_LINEAR);
  glFogfv(GL_FOG_COLOR, cam_clear_color);
  glFogf(GL_FOG_START, 0.0f);
  glFogf(GL_FOG_END, cam_fog_size);
  glEnable(GL_FOG);
}

void setup_gl(){
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
  glClear(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
  glEnable (GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);
  glClearDepth(1.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void setup_app() {
  glutCreateWindow(win_title);
  fullscreen_toggle();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutSpecialFunc(special_keys);
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);
}

void setup_menu(){

}

void camera_move(){
  glLoadIdentity();
  float treshold = 0.05f;
  for (int i = 0; i < 3; i++){
    if(cam_pos[i+3] > cam_pos[i]){
      cam_pos[i+3] -= cam_move_speed;
    }
    if(cam_pos[i+3] < cam_pos[i]){
      cam_pos[i+3] += cam_move_speed;
    }
  }

  for (int i = 0; i < 3; i++){
    if(cam_look_pos[i+3] > cam_look_pos[i]){
      cam_look_pos[i+3] -= cam_move_speed;
    }
    if(cam_look_pos[i+3] < cam_look_pos[i]){
      cam_look_pos[i+3] += cam_move_speed;
    }
  }


  gluLookAt(cam_pos[3], cam_pos[4], cam_pos[5],
    cam_look_pos[3], cam_look_pos[4], cam_look_pos[5],
    0.0, 1.0, 0.0);
}

void setup_scene(){
  setup_lighting();
  STATE = S_SIMULATION;
}














// MAIN LOOPS
// ----------------------------------------------------------------------------

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0.3f, 0.05f, 0.6f, 1.0f);

  glLoadIdentity();
  camera_move();

  if (STATE == S_SIMULATION){
    simulation_draw();
  }

  glutSwapBuffers();
}

void render_loop(int value) {
  simulation_loop();
  glutPostRedisplay();
  glutTimerFunc(refresh_ms, render_loop, 0);
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  setup_app();
  setup_menu();
  setup_scene();
  simulation_setup();
  glutTimerFunc(0, render_loop, 0);
  glutMainLoop();
   return 0;
}



// The MIT License (MIT)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.