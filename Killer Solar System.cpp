/*********
Killer Solar System
*********/

/*********
THE FEATURES OF THE GAME
----------
- 1: DONE! Display a label and a proper title for window.
- 2: DONE! Draw x and y axis and at least three concentric orbitS.
- 3: DONE! Design a gun object at the origin. It should contain at least one element that is not a circle or
line segment. The user should rotate it by moving the mouse pointer. It should also show the gun’s
current angle.
- 4: DONE! After clicking mouse left button, game starts and it creates (at least) three circular objects
with random color, position, radius and angular speed (clockwise or anticlockwise).
- 5: DONE! After game starts, the mouse left button click fires a circular bullet.
- 6: DONE! If the bullet hits any of the object, it disappears.
- 7: DONE! The game restarts after all objects are destroyed by the player.
- 8: The weapon is a 3D object which looks awesome. Also, dots style is integrated to make looking better.
- 9: Weapon fires 3 circular bullet instead of one.
- 10: Circular objects respawn right after it disappears in the correct axis with changed random values.
- 11: Time system is integrated to the game to make highest score.
- 12: It calculates the earned point and if it is higher than highest score game saves instead of it.
- 13. There is a bonus point opportunity if player hits multiple object at the same time with 'combos'.
- 14: Combos are calculated and multiplied. Moreover, it shows a proper message to the player about the combo.
----------
*********/

/*PART 1 -LOAD LIBRARIES*/
#include <glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

/*PART 2 - DEFINE VARIABLES*/
#define WINDOW_WIDTH  650
#define WINDOW_HEIGHT 650
#define TIMER_PERIOD   16 // Period the timer.
#define TIMER_ON        1 // 0:disable timer, 1:enable timer
#define D2R     0.0174532
#define PI         3.1415
#define drawOneLine(x1,y1,x2,y2)  glBegin(GL_LINES);

//2.1 defination of necessary variable
int axis[4] = { 25, 80, 135, 190 };
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight;

//2.2 defination of stractures
typedef struct {
	int sec, mil;
}chronometer_s;

typedef struct {
	int isActivated;
	bool isGameRunning;
	bool isGameFinished;
}tools_s;

typedef struct {
	float x, y;
} point_t;

typedef struct {
	int r, g, b;
}color_t;

typedef struct {
	point_t pos;
	int axis;
	int speed;
	int size;
	color_t color;
	int start;
	bool isActive;
}planet_t;

typedef struct {
	float x, y, angle;
}weapon_t;

typedef struct {
	bool state;
	float x;
	float y;
	float angle;
}particle_t;

typedef struct {
	float x, y;
}collision_t;

typedef struct {
	float angle;
	float x;
	float y;
} compass_t;

typedef struct {
	float A;
	float t[4];
}angle_t;

typedef struct {
	int highscore;
	int score;
	int combo;
}game_t;

//2.3 initialization of stractures
chronometer_s chronometer = { 14, 99 };
tools_s tools = { 0, false, false };
color_t colors[4];
planet_t planet[4];
weapon_t weapon;
particle_t particle;
collision_t collision[3];
compass_t compass;
angle_t angle;
game_t game = { 0, 0, 0 };


/*PART 3 - SOURCE CODE*/


void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;

	glLineWidth(225123123120);
	glPointSize(50);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
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
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void print(int x, int y, const char *string, void *font)
{
	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(string);

	for (i = 0; i < len; i++)
		glutBitmapCharacter(font, string[i]);
}

void vprint(int x, int y, void *font, const char *string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);

	for (i = 0; i < len; i++)
		glutBitmapCharacter(font, str[i]);
}

void vprint2(int x, int y, float size, char *string, ...)
{
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

	for (i = 0; i < len; i++)
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);

	glPopMatrix();
}

//3.1 a function to rotate vertexes in a proper way
void vertex(weapon_t P, weapon_t Tr, double angle) {
	double xp = (P.x * cos(angle) - P.y * sin(angle)) + Tr.x;
	double yp = (P.x * sin(angle) - P.y * cos(angle)) + Tr.y;
	glVertex2d(xp, yp);
}

//3.2 a function to draw background and frames
void drawBackground()
{
	glColor3f(0, 1, 1);
	glBegin(GL_LINES);
	glVertex2f(-350, 0);
	glVertex2f(350, 0);

	if (tools.isGameRunning == false)
	{
		glVertex2f(0, 350);
		glVertex2f(0, -240);
		glVertex2f(-105, -240);
		glVertex2f(105, -240);
		glVertex2f(-105, -275);
		glVertex2f(105, -275);
		glVertex2f(-105, -240);
		glVertex2f(-105, -275);
		glVertex2f(105, -240);
		glVertex2f(105, -275);
		glVertex2f(0, -275);
		glVertex2f(0, -350);
	}
	else
	{
		glVertex2f(0, 350);
		glVertex2f(0, -350);
	}

	glColor3ub(204, 204, 204);
	glVertex2f(-320, 320);
	glVertex2f(-197, 320);
	glVertex2f(-197, 320);
	glVertex2f(-197, 270);
	glVertex2f(-197, 270);
	glVertex2f(-320, 270);
	glVertex2f(-320, 270);
	glVertex2f(-320, 320);
	glVertex2f(-320, -320);
	glVertex2f(-197, -320);
	glVertex2f(-197, -320);
	glVertex2f(-197, -270);
	glVertex2f(-197, -270);
	glVertex2f(-320, -270);
	glVertex2f(-320, -270);
	glVertex2f(-320, -320);
	glVertex2f(320, 320);
	glVertex2f(197, 320);
	glVertex2f(197, 320);
	glVertex2f(197, 270);
	glVertex2f(197, 270);
	glVertex2f(320, 270);
	glVertex2f(320, 270);
	glVertex2f(320, 320);
	glVertex2f(320, -320);
	glVertex2f(197, -320);
	glVertex2f(197, -320);
	glVertex2f(197, -270);
	glVertex2f(197, -270);
	glVertex2f(320, -270);
	glVertex2f(320, -270);
	glVertex2f(320, -320);
	glEnd();
}

//3.3 a function to draw game informations
void drawInformations()
{
	glColor3ub(204, 204, 204);
	if (!tools.isGameRunning)
		vprint(-90, -260, GLUT_BITMAP_9_BY_15, "Right Click to Start");
	vprint(-310, 300, GLUT_BITMAP_8_BY_13, "FIKRET SENGUL");
	vprint(-305, 280, GLUT_BITMAP_8_BY_13, "Homework #3");

	vprint(-264, -288, GLUT_BITMAP_8_BY_13, "%d", game.highscore);
	vprint(-312, -308, GLUT_BITMAP_8_BY_13, "Highest Score");

	vprint(202, 300, GLUT_BITMAP_8_BY_13, "Remaining Time");
	vprint(238, 280, GLUT_BITMAP_8_BY_13, "%2d:%2d", chronometer.sec, chronometer.mil);

	vprint(253, -288, GLUT_BITMAP_8_BY_13, "%d", game.score);
	vprint(238, -308, GLUT_BITMAP_8_BY_13, "Score");
}

//3.4 a function to draw circulat objects axises
void drawCirle()
{
	float r = fabs(angle.A);

	glColor3ub(51, 153, 255);
	circle_wire(0, 0, r + 25);
	circle_wire(0, 0, r + 80);
	circle_wire(0, 0, r + 135);
	circle_wire(0, 0, r + 190);

	glColor3f(1.0, 1.0, 1.0);
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(1, 0x0101);
	drawOneLine(150.0, 100.0, 250.0, 100.0);
	glEnd();
}

//3.5 a function to draw particles when weapon is fired
void drawParticles()
{
	glColor3f(1, 1, 1);
	if (particle.state)
	{
		circle((particle.x + 60) * cos(particle.angle - 50) - (particle.y + 0) * sin(particle.angle - 50), (particle.x + 60) * sin(particle.angle - 50) + (particle.y + 0) * cos(particle.angle - 50), 4);

		circle((particle.x + 60) * cos(particle.angle) - (particle.y + 0) * sin(particle.angle), (particle.x + 60) * sin(particle.angle) + (particle.y + 0) * cos(particle.angle), 4);

		circle((particle.x + 60) * cos(particle.angle + 50) - (particle.y + 0) * sin(particle.angle + 50), (particle.x + 60) * sin(particle.angle + 50) + (particle.y + 0) * cos(particle.angle + 50), 4);
	}


	collision[0].x = (particle.x + 60) * cos(particle.angle - 50) - (particle.y + 0) * sin(particle.angle - 50);
	collision[0].y = (particle.x + 60) * sin(particle.angle - 50) + (particle.y + 0) * cos(particle.angle - 50);
	collision[1].x = (particle.x + 60) * cos(particle.angle) - (particle.y + 0) * sin(particle.angle);
	collision[1].y = (particle.x + 60) * sin(particle.angle) + (particle.y + 0) * cos(particle.angle);
	collision[2].x = (particle.x + 60) * cos(particle.angle + 50) - (particle.y + 0) * sin(particle.angle + 50);
	collision[2].y = (particle.x + 60) * sin(particle.angle + 50) + (particle.y + 0) * cos(particle.angle + 50);
}

//3.6 a function to draw 3d weapon
void drawWeapon()
{
	glLineWidth(1);
	glColor3f(1, 1, 1);
	glBegin(GL_LINES);
	vertex({ -15, 10 }, { 0, 0 }, (weapon.angle + 180)*D2R);
	vertex({ -50, 30 }, { 0, 0 }, (weapon.angle + 180)*D2R);
	vertex({ -15, -10 }, { 0, 0 }, (weapon.angle + 180)*D2R);
	vertex({ -50, -30 }, { 0, 0 }, (weapon.angle + 180)*D2R);
	vertex({ -50, 30 }, { 0, 0 }, (weapon.angle + 180)*D2R);
	vertex({ -50, 20 }, { 0, 0 }, (weapon.angle + 180)*D2R);
	vertex({ -50, -30 }, { 0, 0 }, (weapon.angle + 180)*D2R);
	vertex({ -50, -20 }, { 0, 0 }, (weapon.angle + 180)*D2R);
	vertex({ -50, 20 }, { 0, 0 }, (weapon.angle + 180)*D2R);
	vertex({ -50, 8 }, { 0, 0 }, (weapon.angle + 180)*D2R);
	vertex({ -50, -20 }, { 0, 0 }, (weapon.angle + 180)*D2R);
	vertex({ -50, -8 }, { 0, 0 }, (weapon.angle + 180)*D2R);
	vertex({ -10, 0 }, { 0, 0 }, (weapon.angle + 180)*D2R);
	vertex({ -50, 0 }, { 0, 0 }, (weapon.angle + 180)*D2R);
	glEnd();
}

//3.7 a function to draw circular objects
void drawPlanets()
{
	float r = fabs(angle.A);

	if (tools.isGameRunning)
	{
		for (int i = 0; i < 4; i++)
		{
			if (planet[i].isActive)
			{
				planet[i].pos.x = (r + planet[i].axis)*cos(angle.t[i] * D2R);
				planet[i].pos.y = (r + planet[i].axis)*sin(angle.t[i] * D2R);
				glColor3ub(planet[i].color.r, planet[i].color.g, planet[i].color.b);
				circle(planet[i].pos.x, planet[i].pos.y, planet[i].size);
				glColor3f(1, 1, 1);
				vprint((r + planet[i].axis)*cos(angle.t[i] * D2R) - 12, (r + planet[i].axis)*sin(angle.t[i] * D2R) - 5, GLUT_BITMAP_8_BY_13, "%0.f", angle.t[i]);
			}
		}
	}
}

//3.8 a function to display the mouses angle
void displayAngles()
{
	float r = fabs(angle.A);
	float ang = compass.angle * D2R;

	compass.angle = atan2(compass.y, compass.x) * 180 / PI;

	if (compass.angle < 0)
		vprint((r + 210)*cos(ang) - 12, (r + 210)*sin(ang) - 5, GLUT_BITMAP_8_BY_13, "%0.f", compass.angle + 360);
	else
		vprint((r + 210)*cos(ang) - 12, (r + 210)*sin(ang) - 5, GLUT_BITMAP_8_BY_13, "%0.f", compass.angle);
}

//3.9 a function to initiate necessary variables
void initiation()
{
	// generate rand number
	srand(time(NULL));

	// init colors
	colors[0] = { 255,0,255 }; // purple
	colors[1] = { 255, 102, 0 }; // red
	colors[2] = { 0, 255, 0 }; // green
	colors[3] = { 255,255,0 }; // yellow

							   // init necessary variables
	angle.A = 100;
	compass.x = compass.y = 0;
	particle.x = particle.y = 0;

	// set necessary variables' status
	particle.state = false;
	tools.isGameRunning = false;


	// init the circular objects
	int colorx = rand() % 4;
	int axisx = rand() % 4;

	for (int i = 0; i < 4; i++)
	{
		colorx = (colorx + 1) % 4;
		axisx = (axisx + 1) % 4;

		planet[i].start = rand() % 360 + 1;
		angle.t[i] = planet[i].start;
		planet[i].size = rand() % 17 + 17;
		planet[i].axis = axis[axisx];
		planet[i].color = colors[colorx];
		planet[i].speed = (rand() % (2 + 1 - -1)) + -1;
		planet[i].isActive = true;
		while (planet[i].speed == 0)
			planet[i].speed = (rand() % (2 + 1 - -1)) + -1;
	}
}

//3.10 a function to move the particles
void destroy()
{
	particle.x += 8 * cos(particle.angle * D2R);
	particle.y += 8 * sin(particle.angle * D2R);
}

//3.11 a function to recall the circular object when it disappear
void recall(int i)
{
	planet[i].start = rand() % 360 + 1;
	angle.t[i] = planet[i].start;
	planet[i].size = rand() % 17 + 17;
	planet[i].speed = (rand() % (2 + 1 - -1)) + -1;
	planet[i].isActive = true;
	while (planet[i].speed == 0)
		planet[i].speed = (rand() % (2 + 1 - -1)) + -1;
}

//3.12 a function to reset the particles status
void reset()
{
	particle.x = particle.y = 0;
	particle.state = false;
}

//3.13 a function to check whether any of particle hits any of circular object or not
void collisionCheck() {
	float distance[3];

	for (int i = 0; i < 4; i++) {
		distance[0] = sqrt((collision[0].x - planet[i].pos.x) * (collision[0].x - planet[i].pos.x) + (collision[0].y - planet[i].pos.y) * (collision[0].y - planet[i].pos.y));
		distance[1] = sqrt((collision[1].x - planet[i].pos.x) * (collision[1].x - planet[i].pos.x) + (collision[1].y - planet[i].pos.y) * (collision[1].y - planet[i].pos.y));
		distance[2] = sqrt((collision[2].x - planet[i].pos.x) * (collision[2].x - planet[i].pos.x) + (collision[2].y - planet[i].pos.y) * (collision[2].y - planet[i].pos.y));

		if (distance[0] <= planet[i].size + 7 || distance[1] <= planet[i].size + 7 || distance[2] <= planet[i].size + 7 && planet[i].isActive && particle.state)
		{
			game.combo++;
		}

		if (distance[0] <= planet[i].size + 7 || distance[1] <= planet[i].size + 7 || distance[2] <= planet[i].size + 7 && planet[i].isActive)
		{
			planet[i].isActive = false;
			recall(i);
		}
	}
}

//3.14 a function to calculate point with combos
void earnPoint()
{
	switch (game.combo)
	{
	case 1: game.score += 1; break;
	case 2: game.score += 4; break;
	case 3: game.score += 16; break;
	case 4: game.score += 64; break;
	case 5: game.score += 256; break;
	case 6: game.score += 1024; break;
	}
}

//3.15 a function to call another functions to be displayed
void display()
{
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	// call necessary functions
	drawBackground();
	drawInformations();
	drawCirle();
	drawPlanets();
	drawWeapon();
	displayAngles();

	if (tools.isGameRunning)
	{

		// show the combos informations
		switch (game.combo)
		{
		case 0: vprint(195, -255, GLUT_BITMAP_9_BY_15, "x0"); break;
		case 1: vprint(195, -255, GLUT_BITMAP_9_BY_15, "Not bad! x1"); break;
		case 2: vprint(195, -255, GLUT_BITMAP_9_BY_15, "Nice shot! x2"); break;
		case 3: vprint(195, -255, GLUT_BITMAP_9_BY_15, "Great shot! x3"); break;
		case 4: vprint(195, -255, GLUT_BITMAP_9_BY_15, "Perfect! x4"); break;
		case 5: vprint(195, -255, GLUT_BITMAP_9_BY_15, "Lucky! x5"); break;
		case 6: vprint(195, -255, GLUT_BITMAP_9_BY_15, "Very lucky! x6"); break;
		}

		// to activate particles and check the collision
		if (particle.state)
		{
			drawParticles();
			destroy();
			collisionCheck();

			if (particle.x >= 350 || particle.x <= -350 || particle.y >= 350 || particle.y <= -350)
				reset();
		}
		else
		{
			// calculate the point
			if (game.combo > 0)
			{
				earnPoint();
				game.combo = 0;
			}
		}
	}

	//


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

void onSpecialKeyDown(int key, int x, int y)
{
	// Write your codes here.
	switch (key)
	{
	case GLUT_KEY_UP: up = true; break;
	case GLUT_KEY_DOWN: down = true; break;
	case GLUT_KEY_LEFT: left = true; break;
	case GLUT_KEY_RIGHT: right = true; break;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onSpecialKeyUp(int key, int x, int y)
{
	// Write your codes here.
	switch (key)
	{
	case GLUT_KEY_UP: up = false; break;
	case GLUT_KEY_DOWN: down = false; break;
	case GLUT_KEY_LEFT: left = false; break;
	case GLUT_KEY_RIGHT: right = false; break;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//3.16 a function to do the actions related to mouse click
void onClick(int button, int stat, int x, int y)
{
	// start the game if it is not running
	if (button == GLUT_RIGHT_BUTTON)
	{
		if (tools.isGameRunning == false && tools.isGameFinished == false)
			tools.isGameRunning = true;
		else if (tools.isGameRunning == false && tools.isGameFinished == true)
		{
			chronometer.mil = 99;
			chronometer.sec = 14;

			// reset the game
			game.score = 0;
			reset();

			// set game status as true
			tools.isGameRunning = true;
		}
	}

	// fire the weapon
	if (tools.isGameRunning == true && button == GLUT_LEFT_BUTTON && !particle.state)
	{
		particle.angle = compass.angle * D2R;
		particle.state = true;
	}

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

void onMoveDown(int x, int y)
{
	// Write your codes here.

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//3.17 a function to calculate angles
void onMove(int x, int y)
{
	// Write your codes here.

	compass.x = x - winWidth / 2;
	compass.y = winHeight / 2 - y;
	weapon.angle = atan2(winHeight / 2 - y, x - winWidth / 2) * 180 / PI;

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//3.18 a function to calculate time and do the actions related to time
#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.

	if (tools.isGameRunning == true)
	{

		// calculate the mouse angle
		for (int i = 0; i < 4; i++)
		{
			if (planet[i].isActive)
			{
				angle.t[i] += planet[i].speed;

				if (angle.t[i] > 360)
					angle.t[i] -= 360;

				if (angle.t[i] < 0)
					angle.t[i] += 360;
			}
		}

		chronometer.mil--;

		if (chronometer.mil == 0)
		{
			chronometer.sec--;
			chronometer.mil = 100;
		}

		if (chronometer.sec == 0)
		{

			if (game.score > game.highscore)
				game.highscore = game.score;

			chronometer.mil = 0;
			tools.isGameRunning = false;
			tools.isGameFinished = true;
		}
	}

	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()
}
#endif

void Init()
{
	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	//glutInitWindowPosition(100, 100);
	glutCreateWindow("Killer Solar System");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	// keyboard registration
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	// mouse registration
	glutMouseFunc(onClick);
	glutMotionFunc(onMoveDown);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	Init();

	// init the necessary variables
	initiation();

	glutMainLoop();
}
