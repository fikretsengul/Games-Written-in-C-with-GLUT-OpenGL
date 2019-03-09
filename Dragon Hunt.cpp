/*********
Dragon Hunt
*********/

/*********
THE FEATURES OF THE GAME
----------
- 1: Name and surname are written. Moreover, screen design contains three areas in two areas to save some space with its requirements.
- 2: Designed a crossbow weapon and an arrow can be moved only y axis.
- 3: Designed a complex target object which is a dragon and there is maximum 5 target objects at the same time.
- 4: The weapon can be fired by pressing space bar and target object will disappear when hit.
- 5: When target hit the point will be calculated.
- 6: Remaining time, total number of targets created, number of targets hit, points gained with the
last hit and total points (score) will be displayed.
- 7: User should be able to pause and play the game using F1 key.
- 8: Game should continue for 20 seconds. After pressing F1 key, a new game should start.
- 9: The game and its objects are fully dynamic. Their location changes automaticlly when the winHeight or winWidth changes.
----------
*********/

/*PART 1 -LOAD LIBRARIES*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <glut.h>
#include <math.h>
#include <time.h>

/*PART 2 - DEFINE VARIABLES*/
#define WINDOW_WIDTH  900
#define WINDOW_HEIGHT 700
#define TIMER_PERIOD 10 // Period for the timer.
#define TIMER_ON      1 // 0:disable timer, 1:enable timer

//2.1 defination of necessary variable
const float DEG2RAD = 3.14159 / 180;
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
	int passedDragons;
	int	killedDragons;
	int lastPoints;
	int totalPoints;
}game_s;

typedef struct {
	int x;
	int y;
	int realx;
	int realy;
	bool status;
}dragons_s;

typedef struct {
	int x;
	int realx;
	int y;
	bool isFired;
}bullet_s;

//2.3 initialization of stractures
chronometer_s chronometer = { 19, 99 };
tools_s tools = { 0, false, false };
game_s game = { 0, 0, 0, 0 };
dragons_s dragons[50];
bullet_s bullet = { 0, 0, 0, false };

/*PART 3 - SOURCE CODE*/
void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void semicircle(float radius, float u, float v)
{
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_POLYGON);

	for (int i = 135; i <= 315; i++)
	{
		float degInRad = i * DEG2RAD;
		glVertex2f(u + cos(degInRad)*radius, v + (sin(degInRad))*radius);//100,100 specifies centre of the circle
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
	for (i = 0; i<len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}

void vprint2(int x, int y, float size, const char *string, ...) {
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

//3.1 set the initial background color
void setBackgroundColor()
{
	glBegin(GL_POLYGON);
	glColor3ub(0, 255, 255);
	glVertex2f(-(winWidth / 2), winHeight * 3 / 8);
	glVertex2f(winWidth / 2, winHeight * 3 / 8);
	glColor3ub(255, 255, 255);
	glVertex2f(winWidth / 2, -(winHeight / 2));
	glVertex2f(-(winWidth / 2), -(winHeight / 2));
	glEnd();
}

//3.2 draw the lines
void drawLines()
{
	glColor3ub(45, 45, 45);
	glBegin(GL_LINES);
	//top horizontal line
	glVertex2f(-winWidth / 2, winHeight * 3 / 8 - 1);
	glVertex2f(winWidth / 2, winHeight * 3 / 8 - 1);
	glEnd();
}

//3.3 draw the information card
void drawInformation()
{
	// the upside color
	glColor3ub(0, 0, 0);
	glRecti(winWidth / 2, winHeight / 2, -(winWidth / 2), winHeight * 3 / 8);

	glColor3f(1, 1, 1);

	// top left two information
	vprint(-(winWidth / 2 - 55), (winHeight * 7 / 16 + 5), GLUT_BITMAP_9_BY_15, "time");

	if (tools.isGameRunning == 1)
		vprint(-(winWidth / 2 - 55), (winHeight * 7 / 16 - 15), GLUT_BITMAP_8_BY_13, "%2d:%2d", chronometer.sec, chronometer.mil);
	else if (tools.isGameFinished == 1)
		vprint(-(winWidth / 2 - 65), (winHeight * 7 / 16 - 15), GLUT_BITMAP_8_BY_13, "%2d", chronometer.sec);
	else
		vprint(-(winWidth / 2 - 65), (winHeight * 7 / 16 - 15), GLUT_BITMAP_8_BY_13, "%2d", chronometer.sec + 1);

	vprint(-(winWidth / 2 - 150), (winHeight * 7 / 16 + 5), GLUT_BITMAP_9_BY_15, "last");
	vprint(-(winWidth / 2 - 155), (winHeight * 7 / 16 - 15), GLUT_BITMAP_8_BY_13, "%2d", game.lastPoints);

	//top right two information
	vprint((winWidth / 2 - 120), (winHeight * 7 / 16 + 5), GLUT_BITMAP_9_BY_15, "passed");
	vprint((winWidth / 2 - 107), (winHeight * 7 / 16 - 15), GLUT_BITMAP_8_BY_13, "%2d", game.passedDragons);
	vprint((winWidth / 2 - 220), (winHeight * 7 / 16 + 5), GLUT_BITMAP_9_BY_15, "killed");
	vprint((winWidth / 2 - 205), (winHeight * 7 / 16 - 15), GLUT_BITMAP_8_BY_13, "%2d", game.killedDragons);


	// top center two information
	if (chronometer.sec == 19 && chronometer.mil > 98)
	{
		vprint(-110, (winHeight * 7 / 16 + 17), GLUT_BITMAP_9_BY_15, "FIKRET SENGUL / 21503111");

		vprint(-75, (winHeight * 7 / 16 - 5), GLUT_BITMAP_9_BY_15, " - DRAGON HUNT -");
		vprint(-135, (winHeight * 7 / 16 - 25), GLUT_BITMAP_9_BY_15, "press <f1> to start a new game");
	}
	else
	{
		glColor3f(0, 0, 0);
		glBegin(GL_POLYGON);
		glVertex2f(-150, winHeight * 7 / 16 + 35);
		glVertex2f(+150, winHeight * 7 / 16 - 35);
		glVertex2f(+150, winHeight * 7 / 16 + 35);
		glVertex2f(-150, winHeight * 7 / 16 - 35);
		glVertex2f(+150, winHeight * 7 / 16 - 35);
		glEnd();


		glColor3ub(255, 255, 255);
		vprint(-1, (winHeight * 7 / 16 + 17), GLUT_BITMAP_9_BY_15, "%2d", game.totalPoints);
		glColor3ub(255, 255, 255);
		vprint(-40, (winHeight * 7 / 16 - 5), GLUT_BITMAP_9_BY_15, "TOTAL POINTS");
		vprint(-175, (winHeight * 7 / 16 - 25), GLUT_BITMAP_9_BY_15, "<SpaceBar> to fire, <f1> to pause/restart.");
	}
}

//3.4 draw the dragon shapes and return the real coord to check collision
void drawDragon(int x, int y, int i)
{
	// head of dragon
	glBegin(GL_TRIANGLES);
	glColor3ub(249, 175, 4);
	glVertex2f(-(winWidth * 3 / 8 / x + 4), -(winHeight / 2 - 100 - y));
	glVertex2f(-(winWidth * 3 / 8 / x + 12), -(winHeight / 2 - 90 - y));
	glVertex2f(-(winWidth * 3 / 8 / x + 4), -(winHeight / 2 - 90 - y));
	glEnd();

	// neck of dragon
	glColor3ub(51, 102, 0);
	glRectf(-(winWidth * 3 / 8 / x - 4), -(winHeight / 2 - 100 - y), -(winWidth * 3 / 8 / x + 4), -(winHeight / 2 - 60 - y));

	// body of dragon
	glColor3ub(25, 51, 0);
	glRectf(-(winWidth * 3 / 8 / x - 12), -(winHeight / 2 - 60 - y), -(winWidth * 3 / 8 / x + 12), -(winHeight / 2 - 15 - y));

	// leg of dragon
	glRectf(-(winWidth * 3 / 8 / x + 8), -(winHeight / 2 - 15 - y), -(winWidth * 3 / 8 / x + 4), -(winHeight / 2 - y));
	glRectf(-(winWidth * 3 / 8 / x - 8), -(winHeight / 2 - 15 - y), -(winWidth * 3 / 8 / x - 4), -(winHeight / 2 - y));

	// wing of dragon
	if (tools.isActivated % 2 == 1)
	{
		// left wing
		glBegin(GL_POLYGON);
		glColor3ub(25, 51, 0);
		glVertex2f(-(winWidth * 3 / 8 / x + 12), -(winHeight / 2 - 35 - y));
		glVertex2f(-(winWidth * 3 / 8 / x + 36), -(winHeight / 2 - 35 - y));
		glVertex2f(-(winWidth * 3 / 8 / x + 52), -(winHeight / 2 - 10 - y));
		glVertex2f(-(winWidth * 3 / 8 / x + 36), -(winHeight / 2 - 10 - y));
		glVertex2f(-(winWidth * 3 / 8 / x + 36), -(winHeight / 2 - y));
		glVertex2f(-(winWidth * 3 / 8 / x + 28), -(winHeight / 2 - 10 - y));
		glVertex2f(-(winWidth * 3 / 8 / x + 28), -(winHeight / 2 + 10 - y));
		glColor3ub(255, 255, 255);
		glVertex2f(-(winWidth * 3 / 8 / x + 12), -(winHeight / 2 - 15 - y));
		glEnd();

		// right wing
		glBegin(GL_POLYGON);
		glColor3ub(25, 51, 0);
		glVertex2f(-(winWidth * 3 / 8 / x - 12), -(winHeight / 2 - 35 - y));
		glVertex2f(-(winWidth * 3 / 8 / x - 36), -(winHeight / 2 - 35 - y));
		glVertex2f(-(winWidth * 3 / 8 / x - 52), -(winHeight / 2 - 10 - y));
		glVertex2f(-(winWidth * 3 / 8 / x - 36), -(winHeight / 2 - 10 - y));
		glVertex2f(-(winWidth * 3 / 8 / x - 36), -(winHeight / 2 - y));
		glVertex2f(-(winWidth * 3 / 8 / x - 28), -(winHeight / 2 - 10 - y));
		glVertex2f(-(winWidth * 3 / 8 / x - 28), -(winHeight / 2 + 10 - y));
		glColor3ub(255, 255, 255);
		glVertex2f(-(winWidth * 3 / 8 / x - 12), -(winHeight / 2 - 15 - y));
		glEnd();
	}
	else
	{
		// left wing
		glBegin(GL_POLYGON);
		glVertex2f(-(winWidth * 3 / 8 / x + 12), -(winHeight / 2 - 35 - y));
		glVertex2f(-(winWidth * 3 / 8 / x + 36), -(winHeight / 2 - 35 - y));
		glVertex2f(-(winWidth * 3 / 8 / x + 52), -(winHeight / 2 - 60 - y));
		glVertex2f(-(winWidth * 3 / 8 / x + 36), -(winHeight / 2 - 60 - y));
		glVertex2f(-(winWidth * 3 / 8 / x + 36), -(winHeight / 2 - 70 - y));
		glVertex2f(-(winWidth * 3 / 8 / x + 28), -(winHeight / 2 - 60 - y));
		glVertex2f(-(winWidth * 3 / 8 / x + 28), -(winHeight / 2 - 80 - y));
		glVertex2f(-(winWidth * 3 / 8 / x + 12), -(winHeight / 2 - 60 - y));
		glEnd();

		// right wing
		glBegin(GL_POLYGON);
		glVertex2f(-(winWidth * 3 / 8 / x - 12), -(winHeight / 2 - 35 - y));
		glVertex2f(-(winWidth * 3 / 8 / x - 36), -(winHeight / 2 - 35 - y));
		glVertex2f(-(winWidth * 3 / 8 / x - 52), -(winHeight / 2 - 60 - y));
		glVertex2f(-(winWidth * 3 / 8 / x - 36), -(winHeight / 2 - 60 - y));
		glVertex2f(-(winWidth * 3 / 8 / x - 36), -(winHeight / 2 - 70 - y));
		glVertex2f(-(winWidth * 3 / 8 / x - 28), -(winHeight / 2 - 60 - y));
		glVertex2f(-(winWidth * 3 / 8 / x - 28), -(winHeight / 2 - 80 - y));
		glVertex2f(-(winWidth * 3 / 8 / x - 12), -(winHeight / 2 - 60 - y));
		glEnd();
	}

	//returns the real cord to check collision
	(dragons + i)->realx = -(winWidth * 3 / 8 / x + 52);
	(dragons + i)->realy = -(winHeight / 2 - 100 - y);
}

//3.5 draw the crossbow and arrow and return the real cord
void drawBullet(int y, int x)
{
	// the crossbow
	glLineWidth(2);
	glColor3ub(0, 0, 0);
	glBegin(GL_LINES);
	glVertex2f(-(winWidth / 2), 10 + y);
	glVertex2f(-(winWidth / 2 - 5), 30 + y);
	glVertex2f(-(winWidth / 2), -10 + y);
	glVertex2f(-(winWidth / 2 - 5), -30 + y);
	glVertex2f(-(winWidth / 2 - 5), 30 + y);
	glVertex2f(-(winWidth / 2 - 15), 20 + y);
	glVertex2f(-(winWidth / 2 - 5), -30 + y);
	glVertex2f(-(winWidth / 2 - 15), -20 + y);
	glVertex2f(-(winWidth / 2 - 15), 20 + y);
	glVertex2f(-(winWidth / 2 - 15), 8 + y);
	glVertex2f(-(winWidth / 2 - 15), -20 + y);
	glVertex2f(-(winWidth / 2 - 15), -8 + y);
	glVertex2f(-(winWidth / 2) + x, 0 + y);
	glVertex2f(-(winWidth / 2 - 15) + x, 0 + y);
	glEnd();

	// the arrow
	glColor3ub(0, 0, 0);
	glBegin(GL_TRIANGLES);
	glVertex2f(-(winWidth / 2 - 14) + x, 10 + y);
	glVertex2f(-(winWidth / 2 - 14) + x, -10 + y);
	glVertex2f(-(winWidth / 2 - 34) + x, 0 + y);
	glEnd();

	// return the real coord
	bullet.realx = -(winWidth / 2 - 20) + x;
}

//3.6 initiate dragons
void callDragons()
{
	for (int i = 0; i < 50; i++)
	{
		// just to be sure the random didn't generate 0 for dragons x coord
		if ((dragons + tools.isActivated)->x == 0)
			(dragons + tools.isActivated)->x = 1;

		// if the next dragon is ready spawn it
		if ((dragons + i)->status == true)
			drawDragon((dragons + i)->x, (dragons + i)->y, i);

		// if the first dragon appears make ready to next dragon to be spawned
		if ((dragons + tools.isActivated)->y >= 0)
		{
			tools.isActivated++;
			game.passedDragons++;
			(dragons + tools.isActivated)->status = true;
		}
	}
}

//3.6 move dragons
void moveDragons()
{
	for (int i = 0; i < 50; i++)
		if ((dragons + i)->status == true)
			(dragons + i)->y += 2;
}

//3.7 resets the game informations when f1 is pressed
void reset()
{
	// reset initialization of dragons
	dragons[0] = { (rand() % (10 + 1 - -10)) + -10, -120, 0, 0, true };
	for (int i = 1; i < 50; i++)
		dragons[i] = { (rand() % (10 + 1 - -10)) + -10, -120, 0, 0, false };

	// reset the chronometer
	chronometer.mil = 99;
	chronometer.sec = 19;

	// reset the game statistics
	game.lastPoints = 0;
	game.totalPoints = 0;
	game.passedDragons = 0;
	game.killedDragons = 0;

	// reset the game tools
	tools.isActivated = 0;
	tools.isGameFinished = false;
	tools.isGameRunning = true;

	// reset the arrow
	bullet.x = 0;
	bullet.y = 0;
	bullet.isFired = false;
}

//3.8 check the collision
void checkCollision()
{
	for (int i = 0; i < 50; i++)
		if ((dragons + i)->realy >= bullet.y && (dragons + i)->realy <= bullet.y + 104 && bullet.realx >= (dragons + i)->realx && bullet.realx <= (dragons + i)->realx + 104 && (dragons + i)->status == true)
		{
			if ((dragons + i)->realy >= 90 && (dragons + i)->realy <= 100)
				game.lastPoints = 1;
			else if ((dragons + i)->realy >= 80 && (dragons + i)->realy <= 89)
				game.lastPoints = 2;
			else if ((dragons + i)->realy >= 70 && (dragons + i)->realy <= 79)
				game.lastPoints = 3;
			else if ((dragons + i)->realy >= 60 && (dragons + i)->realy <= 69)
				game.lastPoints = 4;
			else if ((dragons + i)->realy >= 40 && (dragons + i)->realy <= 59)
				game.lastPoints = 5;
			else if ((dragons + i)->realy >= 30 && (dragons + i)->realy <= 39)
				game.lastPoints = 4;
			else if ((dragons + i)->realy >= 20 && (dragons + i)->realy <= 29)
				game.lastPoints = 3;
			else if ((dragons + i)->realy >= 10 && (dragons + i)->realy <= 19)
				game.lastPoints = 2;
			else if ((dragons + i)->realy >= 0 && (dragons + i)->realy <= 9)
				game.lastPoints = 1;

			game.killedDragons += 1;
			game.totalPoints += game.lastPoints;

			(dragons + i)->status = false;
		}
}

//3.9 displays the game contents
void display() {
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	// change the background color
	setBackgroundColor();

	// initiate the rockets
	callDragons();

	//// draw the lines and information card
	drawLines();
	drawInformation();

	// draw the crossbow and arrow
	drawBullet(bullet.y, bullet.x);

	// check whether the game is running or not
	if (tools.isGameRunning == true)
	{
		// move dragons function
		moveDragons();

		// check whether bullet is fired or not
		if (bullet.isFired == true)
		{
			// move the arrow
			bullet.x += 5;

			// call check collusion function
			checkCollision();

			// if the arrow reaches to end of screen reload the crossbow
			if (bullet.x >= winWidth)
			{
				bullet.x = 0;
				bullet.isFired = false;
			}
		}
	}

	glutSwapBuffers();
}

// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// fire the arrow when SPACE is pressed and while game is running.
	if (key == 32 && tools.isGameRunning == true)
		bullet.isFired = true;

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

// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
void onSpecialKeyDown(int key, int x, int y)
{
	// if game is finished and f1 is pressed, reset the game
	if (key == GLUT_KEY_F1)
	{
		if (tools.isGameRunning == false && tools.isGameFinished == false)
			tools.isGameRunning = true;
		else if (tools.isGameRunning == false && tools.isGameFinished == true)
			reset();
		else
			tools.isGameRunning = false;
	}


	// if up or down key pressed and bullet isn't fired move the crossbow
	if (key == GLUT_KEY_UP && bullet.isFired == false)
		bullet.y+=5;

	if (key == GLUT_KEY_DOWN && bullet.isFired == false)
		bullet.y-=5;

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
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

// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
void onClick(int button, int stat, int x, int y)
{
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

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
	// Write your codes here.

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.

	// if game is running calculate the time
	if (tools.isGameRunning == true)
	{
		chronometer.mil--;

		if (chronometer.mil == 0)
		{
			chronometer.sec--;
			chronometer.mil = 100;
		}

		if (chronometer.sec == 0)
		{
			chronometer.mil = 0;
			tools.isGameRunning = false;
			tools.isGameFinished = true;
		}
	}

	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()
}
#endif

void Init() {
	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Dragon Hunt");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	//// display the game informations on the console
	//displayGameInformation();

	// keyboard registration
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	Init();

	// generates a random number
	srand(time(0));


	// initialization the dragons
	dragons[0] = { (rand() % (10 + 1 - -10)) + -10, -120, 0, 0, true };
	for (int i = 1; i < 50; i++)
		dragons[i] = { (rand() % (10 + 1 - -10)) + -10, -120, 0, 0, false };

	glutMainLoop();
}
