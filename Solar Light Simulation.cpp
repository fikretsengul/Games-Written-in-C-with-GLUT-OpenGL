/*********
Solar Light Simulation with Objects
*********/

/*********
THE FEATURES OF THE GAME
----------
- 1: Display a label (your name, surname, “HOMEWORK #4” text) and a proper title for window.
- 2: Display a bottom bar that shows the status of light sources (ON or OFF). F1, F2, F3 and F4 keys are used to switch their states.
- 3: F5 key should pause/start the animation.
- 4: F6 key should restart the animation by assigning new random values to the global variables.
- 5: Draw a white sun (mine is yellow) and at least two (I've four) planets rotating in different orbits around the sun. Use random values for the radius, rotation speed and rotation direction of the planets.
- 6: Display three lights: red, green and blue. They move (with different velocities) in the scene, and they reflect from borders.
- 7: Calculate lighting effect of all active light sources (three lights and the sun) on the planets.
- 8: There are stars, their coords are calculated at the beginning and being displayed on display func, also they are glowing.
- 9: The stars color is glowing means changing and also its reflections color is changing.
----------
*********/


/*PART 1 -LOAD LIBRARIES*/
#include <glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include "vec.h"


/*PART 2 - DEFINE VARIABLES*/
#define WINDOW_WIDTH  600
#define WINDOW_HEIGHT 650
#define TIMER_PERIOD  16 // Period for the timer.
#define TIMER_ON       1 // 0:disable timer, 1:enable timer
#define D2R 0.0174532
#define NUM 4
#define drawOneLine(x1,y1,x2,y2)  glBegin(GL_LINES);

//2.1 defination of necessary variable
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight;

//2.2 defination of stractures
typedef struct {
	float r, g, b;
} color_t;

typedef struct {
	vec_t   pos;
	color_t color;
	vec_t   vel;
	bool activation;
} light_t;

typedef struct {
	vec_t pos;
	vec_t N;
} vertex_t;

typedef struct {
	float x, y;
	float gravity, inclination, radius;
	bool aspect;
	vec_t v;
	vertex_t p;
	color_t tint;
}objects_t;

typedef struct {
	int x, y;
}stars_t;

typedef struct {
	bool gamestatus;
}tools_t;

//2.3 initialization of stractures
light_t light[NUM];
objects_t objects[NUM]; // objects array
stars_t stars[500]; // for stars coords
float radius; // range between objects
tools_t tools = { true }; // tool kit


						  /*PART 3 - SOURCE CODE*/

color_t mulColor(float k, color_t c) {
	color_t tmp = { k * c.r, k * c.g, k * c.b };
	return tmp;
}

color_t addColor(color_t c1, color_t c2) {
	color_t tmp = { c1.r + c2.r, c1.g + c2.g, c1.b + c2.b };
	return tmp;
}

// WARNING! EDITED TO LIMIT THE DISTANCE EFFECT FOR REALISTIC LIGHTING EFFECT
double distanceImpact(double d) {
	if (d>250) // edited part.
		return 0;
	return (-1.0 / 250.0) * d + 1.0;
}

color_t calculateColor(light_t source, vertex_t v) {
	vec_t L = subV(source.pos, v.pos);
	vec_t uL = unitV(L);
	float factor = dotP(uL, v.N) * distanceImpact(magV(L));
	return mulColor(factor, source.color);
}

void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r*cos(angle), y + r*sin(angle));
	}
	glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
	float angle;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r*cos(angle), y + r*sin(angle));
	}
	glEnd();
}

void print(int x, int y, char *string, void *font)
{
	int len, i;

	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i<len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}

void vprint(int x, int y, void *font, char *string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i<len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}

void vprint2(int x, int y, float size, char *string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i<len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}

//3.1 a function to init the global values also to restart them
void initObjectsProps() {
	for (int i = 0; i < NUM; i++) {
		objects[i].gravity = rand() % 3 + 1;
		objects[i].inclination = rand() % 361;
		objects[i].aspect = rand() % 2;
		objects[i].radius = rand() % 6 + 20;
	}

	light[0] = { { (double)(rand() % (250 + 1 - (-250)) + (-250)), (double)(rand() % (250 + 1 - (-250)) + (-250)) },{ 1, 0, 0 },{ 3,  2 }, true };
	light[1] = { { (double)(rand() % (250 + 1 - (-250)) + (-250)), (double)(rand() % (250 + 1 - (-250)) + (-250)) },{ 0, 1, 0 },{ -2, -1 }, true };
	light[2] = { { (double)(rand() % (250 + 1 - (-250)) + (-250)), (double)(rand() % (250 + 1 - (-250)) + (-250)) },{ 0, 0, 1 },{ 3, -2 }, true };
	light[3] = { { 0, 0 },{ 1, 1, 0 },{ 0 - 0 }, true };

	for (int i = 0; i <= 500; i++) {
		stars[i].x = (rand() % 600 - 300);
		stars[i].y = (rand() % 600 - 300);
	}
}

//3.2 a function to display necessary informations
void displayInfos() {
	glColor3f(.1, .1, .1);
	glRectf(-300, -300, 300, -325);
	glRectf(-300, 300, 300, 325);

	glColor3f(1, 1, 1);
	vprint(-290, 308, GLUT_BITMAP_9_BY_15, "HOMEWORK #4");
	vprint(76, 308, GLUT_BITMAP_9_BY_15, "21503111 / Fikret Sengul");

	vprint(-270, -315, GLUT_BITMAP_HELVETICA_10, "F1(Red):%s", (light[0].activation) ? "ON" : "OFF");
	vprint(-170, -315, GLUT_BITMAP_HELVETICA_10, "F2(Green):%s", (light[1].activation) ? "ON" : "OFF");
	vprint(-70, -315, GLUT_BITMAP_HELVETICA_10, "F3(Blue):%s", (light[2].activation) ? "ON" : "OFF");
	vprint(30, -315, GLUT_BITMAP_HELVETICA_10, "F4(Sun):%s", (light[3].activation) ? "ON" : "OFF");
	vprint(120, -315, GLUT_BITMAP_HELVETICA_10, "F5(Anim):%s", (tools.gamestatus) ? "ON" : "OFF");
	vprint(220, -315, GLUT_BITMAP_HELVETICA_10, "F6:Restart");
}

//3.3 a function to create and spawn the objects
void spawnObjects(objects_t objects, float radius) {

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.3, 0.3, 0.3);

	// do the necessary calculation
	objects.x = radius * cos(objects.inclination * D2R), objects.y = radius * sin(objects.inclination * D2R);

	// check its inclinations to move it that direction
	if (objects.aspect == 1)
		objects.x *= -1;

	// spawn the first white tint color of object
	glVertex2f(objects.x, objects.y);

	// for loop to update objects reflection 100 time(s) in every milisecond
	for (int j = 0; j <= 100; j++) {

		// same for every pieces of single object
		objects.inclination = 2 * PI * j / 100;

		// do the necessary calculations
		objects.v = { objects.x + objects.radius*cos(objects.inclination), objects.y + objects.radius*sin(objects.inclination) };
		objects.p = { { objects.v.x, objects.v.y } };
		objects.p.N = unitV(subV({ objects.v.x, objects.v.y }, { objects.x, objects.y }));

		// clear the tints for everytime to reflect
		objects.tint = { 0, 0, 0 };

		//reflect the color tints of objects
		for (int k = 0; k < NUM; k++)
			if (light[k].activation == true)
				objects.tint = addColor(objects.tint, calculateColor(light[k], objects.p));

		//give a tint color to objects
		glColor3f(objects.tint.r, objects.tint.g, objects.tint.b);

		//spawn the objects with their properties
		glVertex2f(objects.v.x, objects.v.y);
	}
	glEnd();
}

//3.4 a function to display them
void display() {

	// clear window to black
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	// to make the color of sun changing
	if (light[3].activation && tools.gamestatus) {
		if ((rand() % 2) == 0) {
			glColor3f(1, 1, 0);
			light[3].color = { 1.0f, 0.75f, 0.0f };
		}
		else {
			glColor3f(1.0f, 0.75f, 0.0f);
			light[3].color = { 1, 1, 0 };
		}
		circle(0, 0, 32);
	}

	// to show the stars
	for (int i = 0; i <= 500; i++) {
		if (tools.gamestatus) {
			if ((rand() % 2) == 0)
				glColor3f(1.0, 1.0, 1.0);
			else
				glColor3f(0, 0, 0);
		}
		else
			glColor3f(1, 1, 1);
		glPointSize(0.3);
		glBegin(GL_POINTS);
		glVertex2i(stars[i].x, stars[i].y);
		glEnd();
	}

	// display lights
	for (int i = 0; i < NUM; i++) {
		if (light[i].activation) {
			// display colors on the screen
			glColor3f(light[i].color.r, light[i].color.g, light[i].color.b);

			// make the size of the sun is bigger than other objects
			if (i == NUM - 1)
				circle(light[i].pos.x, light[i].pos.y, 30);
			else
				circle(light[i].pos.x, light[i].pos.y, 10);
		}
	}

	// update radius to 120 to give them a distance
	radius = 120;

	// spawn objects at different distance
	for (int i = 0; i < NUM; i++) {
		spawnObjects(objects[i], radius);
		radius += 50;
	}

	// display info messages
	displayInfos();

	glutSwapBuffers();

}

void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//3.5 a function to do special actions when clicked the keyboard
void onSpecialKeyDown(int key, int x, int y)
{
	// key settings for actions
	switch (key) {
	case GLUT_KEY_F1: light[0].activation = !light[0].activation; break;
	case GLUT_KEY_F2: light[1].activation = !light[1].activation; break;
	case GLUT_KEY_F3: light[2].activation = !light[2].activation; break;
	case GLUT_KEY_F4: light[3].activation = !light[3].activation; break;
	case GLUT_KEY_F5: tools.gamestatus = !tools.gamestatus; break;
	case GLUT_KEY_F6: initObjectsProps(); break;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onSpecialKeyUp(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = false; break;
	case GLUT_KEY_DOWN: down = false; break;
	case GLUT_KEY_LEFT: left = false; break;
	case GLUT_KEY_RIGHT: right = false; break;
	}
	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onClick(int button, int stat, int x, int y)
{
	// Write your codes here.

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onResize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(); // refresh window.
}

void onMoveDown(int x, int y) {
	// Write your codes here.

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onMove(int x, int y) {
	// Write your codes here.

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//3.6 a timer function to make the movements and check the game status
#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);

	// check game status to start or stop
	if (tools.gamestatus) {
		for (int i = 0; i < NUM; i++) {
			light[i].pos = addV(light[i].pos, light[i].vel);

			// tab back from the walls.
			if (light[i].pos.x > WINDOW_WIDTH / 2 - 10 || light[i].pos.x < -WINDOW_WIDTH / 2 + 10) {
				light[i].vel.x *= -1;
			}

			if (light[i].pos.y > WINDOW_HEIGHT / 2 - 35 || light[i].pos.y < -WINDOW_HEIGHT / 2 + 35) {
				light[i].vel.y *= -1;
			}

			// to move the objects
			objects[i].inclination += 0.5 * objects[i].gravity;
		}
	}

	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif

//3.7 a function to init some settings
void Init() {

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// generate rand number
	srand(time(NULL));

	initObjectsProps();
}

void main(int argc, char *argv[]) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Solar light simulation with objects");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	glutMouseFunc(onClick);
	glutMotionFunc(onMoveDown);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	Init();

	glutMainLoop();
}
