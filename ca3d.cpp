// ----------------------------------------
// OpenGL Cellular Automaton Engine 3D
// Krzysztof Jankowski <kj@p1x.in>
//
// (c)2015 P1X
// http://p1x.in
//
// gcc -Os ca3d.cpp -o ca3d.app -lglut -lGL -lGLU -lm
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
// ----------------------------------------------------------------------------


bool fullscreen_mode  = false;
char win_title[]      = "OpenGL Cellular Automaton Engine 3D";
static float VERSION  = 1.0f;
int win_width         = 512;
int win_height        = 384;
int win_x             = 256;
int win_y             = 100;

int FPS               = 60;
int refresh_ms        = 1000/FPS;


// AUTOMATON VARS
// ----------------------------------------------------------------------------
static int CELLS_ARRAY_SIZE[]    = {16, 16, 16};
static int MAX_CELLS             = CELLS_ARRAY_SIZE[0]*CELLS_ARRAY_SIZE[1]*CELLS_ARRAY_SIZE[2];
int half[]                        = {CELLS_ARRAY_SIZE[0] * 0.5,  CELLS_ARRAY_SIZE[1] * 0.5, CELLS_ARRAY_SIZE[2] * 0.5};
float cells_main_array[16][16][16];
float cells_buffer_array[16][16][16];

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


float cam_speed   = 0.5f;
float cam_x       = 10.0f;
float cam_y       = 10.0f;
float cam_z       = 20.0f;
float cam_look_x  = 0.0f;
float cam_look_y  = 0.0f;
float cam_look_z  = 0.0f;
float mouse_x     = 0.0f;
float mouse_y     = 0.0f;
float mouse_z     = 0.0f;






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
    glColor4f(treshold_c, treshold_c, 1.0f, 0.5f);
    glutSolidCube(s);
  glPopMatrix();
}

void simulation_setup(){
  for (int z = 0; z < CELLS_ARRAY_SIZE[2]; z++){
  for (int y = 0; y < CELLS_ARRAY_SIZE[1]; y++){
  for (int x = 0; x < CELLS_ARRAY_SIZE[0]; x++){
    cells_main_array[x][y][z] = random_f() > 0.90f ? random_fcolor() : 0.0f;
  }}}
}

void simulation_draw(){
  float c;
  float scale = 2.0f;
  float size = 1.8f;
  float new_x, new_y, new_z, new_c;
  
  for (int z = 0; z < CELLS_ARRAY_SIZE[2]; z++){
  for (int y = 0; y < CELLS_ARRAY_SIZE[1]; y++){
  for (int x = 0; x < CELLS_ARRAY_SIZE[0]; x++){
    c = cells_main_array[x][y][z];
    if ( c > 0.0f){
      new_x = (x - half[0]) * scale;
      new_y = (y - half[1]) * scale;
      new_z = (z - half[2]) * scale;
      new_c = (float)c;
      simulation_draw_cell(size * c, new_x, new_y, new_z, new_c);
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
      if(!(z==cz-1 and y==cy+1 and x==cx+1)){ // am I crazy already?
      if(!(z==cz+1 and y==cy-1 and x==cx-1)){
      if(!(z==cz+1 and y==cy-1 and x==cx+1)){
      if(!(z==cz+1 and y==cy+1 and x==cx-1)){
      if(!(z==cz+1 and y==cy+1 and x==cx+1)){
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
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    mouse_x = x;
    mouse_y = y;
  }
}

void special_keys(int key, int x, int y) {
   switch (key) {
      case GLUT_KEY_F1:
         fullscreen_mode = !fullscreen_mode;
         fullscreen_toggle();
         break;
      case GLUT_KEY_RIGHT:
        cam_x += cam_speed;
         break;
      case GLUT_KEY_LEFT:
        cam_x -= cam_speed;
         break;
      case GLUT_KEY_UP:
         cam_y += cam_speed;
         break;
      case GLUT_KEY_DOWN:
         cam_y -= cam_speed;
         break;
      case GLUT_KEY_F2:
        simulation_setup();
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
  gluPerspective (90.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
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
  glutMouseFunc(mouse);
}

void setup_menu(){

}

void camera_move(){
  glLoadIdentity();
  gluLookAt(cam_x, cam_y, cam_z, cam_look_x, cam_look_y, cam_look_z, 0.0, 1.0, 0.0);
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