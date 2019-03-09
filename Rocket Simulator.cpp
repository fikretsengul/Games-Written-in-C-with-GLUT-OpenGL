/*********
SpaceX Rocket Simulator
*********/

/*********
THE FEATURES OF THE GAME
----------
- 1: Screen design contains two areas and its requirements.
- 2: Name and surname are written.
- 3: Designed a complex racing object. (a rocket)
- 4: Objects are animated and they move with a random speed. They are making 2 rounds. User can start and stop the game.
- 5: Elapsed time and current/final winner are displayed.
- 6: At the end of the game, <F1> will can be used to start a new game.
- 7: The game and its objects are fully dynamic. Their location changes automaticlly when the winHeight or winWidth changes.
- 8: Credit system has been added. User can choose a rocket and can play a bet using their 100 credit.
----------
*********/

/*PART 1 -LOAD THE MATERIALS SOURCE*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <glut.h>
#include <math.h>
#include <time.h>

/*PART 2 - DEFINE VARIABLES OF MATERIALS*/
#define WINDOW_WIDTH  900
#define WINDOW_HEIGHT 700
#define TIMER_PERIOD  10 // Period for the timer.
#define TIMER_ON      1 // 0:disable timer, 1:enable timer


//2.1 defination of necessary variable
const float DEG2RAD = 3.14159 / 180;
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight;

//2.2 defination of stractures
typedef struct {
	int sec = 0, mil = 0;
}chronometer_s;

typedef struct {
	int credit;
	int choice;
	int bet;
}game_s;

typedef struct {
	int winner;
	bool isGameRunning;
	bool activeTimer;
	bool isGameFinished;
}tools_s;

typedef struct {
	char name[30];
	double number;
	double location;
	int headLocation;
	int speed;
	double ratio;
	int direction;
}rockets_s;

//2.3 initialization of stractures
chronometer_s chronometer;

game_s game = { 100, 0, 0 };

tools_s tools = {0, false, true, false};

rockets_s rockets[] = { { "Falcon 1 v0.0", 0, 0, 0, 0, 0, 1 }, { "Falcon 9 v1.0", 1.0, 0, 0, 0, 0, 1 }, { "Falcon 9 v1.1", 1.1, 0, 0, 0, 0, 1 }, { "Falcon 9 v1.2", 1.2, 0, 0, 0, 0, 1 } };


/*PART 3 - DO THE ENGINEERING*/
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

// I created an extra func. to create a circle
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
	glColor3ub(15, 15, 15);
	glRecti(-winWidth / 2, -(winHeight / 2), winWidth / 2, winHeight / 2);
}

//3.2 draw the lines
void drawLines()
{

	glColor3ub(45, 45, 45);
	glBegin(GL_LINES);
	//top horizontal line
	glVertex2f(-winWidth / 2, winHeight * 3 / 8 - 1);
	glVertex2f(winWidth / 2, winHeight * 3 / 8 - 1);
	//left middle vertical line
	glVertex2f(-winWidth / 4, -winHeight / 2);
	glVertex2f(-winWidth / 4, winHeight * 3 / 8);
	//middle vertical line
	glVertex2f(0, -winHeight / 2);
	glVertex2f(0, winHeight * 3 / 8);
	//right middle vertical line
	glVertex2f(winWidth / 4, -winHeight / 2);
	glVertex2f(winWidth / 4, winHeight * 3 / 8);
	glEnd();
}

//3.3 draw the information card
void drawInformation()
{

	// the upside color
	glColor3ub(0, 0, 0);
	glRecti(winWidth / 2, winHeight / 2, -(winWidth / 2), winHeight * 3 / 8);

	glColor3f(1, 1, 1);

	//top left two information
	vprint(-(winWidth / 2 - 55), (winHeight * 7 / 16 + 5), GLUT_BITMAP_9_BY_15, "time");
	vprint(-(winWidth / 2 - 50), (winHeight * 7 / 16 - 15), GLUT_BITMAP_8_BY_13, "%2d:%2d", chronometer.sec, chronometer.mil);
	vprint(-(winWidth / 2 - 170), (winHeight * 7 / 16 + 5), GLUT_BITMAP_9_BY_15, "mil/h");
	vprint(-(winWidth / 2 - 172), (winHeight * 7 / 16 - 15), GLUT_BITMAP_8_BY_13, "%d", (rockets)->speed);

	// top two information
	vprint(-110, (winHeight * 7 / 16 + 17), GLUT_BITMAP_9_BY_15, "FIKRET SENGUL / 21503111");

	vprint(-118, (winHeight * 7 / 16 - 5), GLUT_BITMAP_9_BY_15, " - WELCOME TO SIMULATOR -");

	if (tools.isGameRunning == 1)
		vprint(-130, (winHeight * 7 / 16 - 25), GLUT_BITMAP_9_BY_15, "press f or s to speed up/down");
	else
		vprint(-130, (winHeight * 7 / 16 - 25), GLUT_BITMAP_9_BY_15, "press space bar to start/stop");

	//top right two information
	vprint((winWidth / 2 - 100), (winHeight * 7 / 16 + 5), GLUT_BITMAP_9_BY_15, "credit");
	vprint((winWidth / 2 - 85), (winHeight * 7 / 16 - 15), GLUT_BITMAP_8_BY_13, "%d", game.credit);
	vprint((winWidth / 2 - 205), (winHeight * 7 / 16 + 5), GLUT_BITMAP_9_BY_15, "winner");
	vprint((winWidth / 2 - 232), (winHeight * 7 / 16 - 15), GLUT_BITMAP_8_BY_13, "%s", (rockets+tools.winner-1)->name);
}

//3.4 draw the rocket shapes and return its head coord
int drawRocket(int x, int z, int t, double n)
{

	// a variable for the location of the rocket head
	int headCord;

	// the head of the rocket
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_TRIANGLES);
	glVertex2f(-(winWidth * 3 / 8 / z - 12.5), -(winHeight / 2 - 100 - x) * t);
	glVertex2f(-(winWidth * 3 / 8 / z + 12.5), -(winHeight / 2 - 100 - x) * t);
	glVertex2f(-(winWidth * 3 / 8 / z), -(winHeight / 2 - 125 - x) * t);
	glEnd();

	// the body of the rocket
	glColor3f(0.8, 0.498039, 0.196078);
	glRectf(-(winWidth * 3 / 8 / z + 12.5), -(winHeight / 2 - 40 - x) * t, -(winWidth * 3 / 8 / z - 12.5), -(winHeight / 2 - 100 - x) * t);

	// the window of the rocket
	glColor3ub(32, 32, 32);
	circle(-(winWidth * 3 / 8 / z), -(winHeight / 2 - 90 - x) * t, 5);

	// the left wing of the rocket
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_TRIANGLES);
	glVertex2f(-(winWidth * 3 / 8 / z + 12.5), -(winHeight / 2 - 40 - x) * t);
	glVertex2f(-(winWidth * 3 / 8 / z + 25), -(winHeight / 2 - 40 - x) * t);
	glVertex2f(-(winWidth * 3 / 8 / z + 12.5), -(winHeight / 2 - 70 - x) * t);
	glEnd();

	// the right wing of the rocket
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_TRIANGLES);
	glVertex2f(-(winWidth * 3 / 8 / z - 12.5), -(winHeight / 2 - 40 - x) * t);
	glVertex2f(-(winWidth * 3 / 8 / z - 25), -(winHeight / 2 - 40 - x) * t);
	glVertex2f(-(winWidth * 3 / 8 / z - 12.5), -(winHeight / 2 - 70 - x) * t);
	glEnd();

	// the color changes of the outer fume
	if ((x % 2) == 0)
		glColor3f(1.0, 0.25, 0.0);
	else
		glColor3f(1.0, 0.816, 0.0);

	// the shape of the outer fume
	glBegin(GL_POLYGON);
	glVertex2f(-(winWidth * 3 / 8 / z), -(winHeight / 2 - x) * t);
	glVertex2f(-(winWidth * 3 / 8 / z + 20), -(winHeight / 2 - 20 - x) * t);
	glVertex2f(-(winWidth * 3 / 8 / z + 12.5), -(winHeight / 2 - 40 - x) * t);
	glVertex2f(-(winWidth * 3 / 8 / z - 12.5), -(winHeight / 2 - 40 - x) * t);
	glVertex2f(-(winWidth * 3 / 8 / z - 20), -(winHeight / 2 - 20 - x) * t);
	glEnd();

	// the color changes of the inner fume
	if ((x % 2) == 0)
		glColor3f(1.0, 0.816, 0.0);
	else
		glColor3f(1.0, 0.25, 0.0);

	// the shape of the inner fume
	glBegin(GL_POLYGON);
	glVertex2f(-(winWidth * 3 / 8 / z), -(winHeight / 2 - 15 - x) * t);
	glVertex2f(-(winWidth * 3 / 8 / z + 13), -(winHeight / 2 - 25 - x) * t);
	glVertex2f(-(winWidth * 3 / 8 / z + 12.5), -(winHeight / 2 - 40 - x) * t);
	glVertex2f(-(winWidth * 3 / 8 / z - 12.5), -(winHeight / 2 - 40 - x) * t);
	glVertex2f(-(winWidth * 3 / 8 / z - 13), -(winHeight / 2 - 25 - x) * t);
	glEnd();

	// the number of the rocket
	glColor3f(0, 0, 0);
	vprint(-(winWidth * 3 / 8 / z + 9), -(winHeight / 2 - 55 - x - 5) * t, GLUT_BITMAP_HELVETICA_12, "%.1f", n);

	// the variable for the location of the rocket head
	headCord = -(winHeight / 2 - 125 - x) * t;

	// return the location of the rocket head
	return headCord;
}

//3.5 initiate rockets
void callRockets()
{
	(rockets + 0)->headLocation = drawRocket((rockets + 0)->location, 1, (rockets + 0)->direction, (rockets+0)->number);
	(rockets + 1)->headLocation = drawRocket((rockets + 1)->location, 3, (rockets + 1)->direction, (rockets + 1)->number);
	(rockets + 2)->headLocation = drawRocket((rockets + 2)->location, -3, (rockets + 2)->direction, (rockets + 2)->number);
	(rockets + 3)->headLocation = drawRocket((rockets + 3)->location, -1, (rockets + 3)->direction, (rockets + 3)->number);
}

//3.6 race rockets with random speeds
void racing()
{
	for (int i = 0; i < 4; i++)
	{
		(rockets + i)->location += (rand() % 3) + (rockets + i)->ratio;
		(rockets + i)->speed += (rand() % 3) + (rockets + i)->ratio - (rand() % 1);
		if ((rockets + i)->ratio < 0 && (rockets + i)->speed > 18150)
			(rockets + i)->speed -= (rand() % 2);
	}
}

//3.7 move the stars when rockets in space
void moveStars()
{

	// create random points for stars and move them
	glColor3f(1.0, 1.0, 1.0);
	glPointSize(0.3);
	glBegin(GL_POINTS);
	for (int y = -(winWidth / 2); y <= winWidth / 2; y += rand() % 150)
		for (int x = -(winHeight / 2); x <= winHeight * 3 / 8; x += rand() % 150)
			glVertex2i(y, x);
	glEnd();
}

//3.8 check the winner
void whoIsWinner(int x1, int x2, int x3, int x4)
{

	// check rockets positions and find the winner
	if ((rockets + 0)->direction == 1 && (rockets + 1)->direction == 1 && (rockets + 2)->direction == 1 && (rockets + 3)->direction == 1)
	{
		if (x1 >= x2)
		{
			if (x3 >= x4)
			{
				if (x3 >= x1)
					tools.winner = 3;
				else
					tools.winner = 1;
			}
			else
			{
				if (x4 >= x1)
					tools.winner = 4;
				else
					tools.winner = 1;
			}
		}
		else
		{
			if (x3 >= x4)
			{
				if (x3 >= x2)
					tools.winner = 3;
				else
					tools.winner = 2;
			}
			else
			{
				if (x4 >= x2)
					tools.winner = 4;
				else
					tools.winner = 2;
			}
		}
	}
	else
	{
		if (x1 <= x2)
		{
			if (x3 <= x4)
			{
				if (x3 <= x1)
					tools.winner = 3;
				else
					tools.winner = 1;
			}
			else
			{
				if (x4 <= x1)
					tools.winner = 4;
				else
					tools.winner = 1;
			}
		}
		else
		{
			if (x3 <= x4)
			{
				if (x3 <= x2)
					tools.winner = 3;
				else
					tools.winner = 2;
			}
			else
			{
				if (x4 <= x2)
					tools.winner = 4;
				else
					tools.winner = 2;
			}
		}
	}
}

//3.9 display the game informations on the console
void displayGameInformation()
{
	printf("\t\t\t\t\tWELCOME TO SPACEX ROCKET SIMULATOR GAME\n\t\t\t\t\t---------------------------------------\n\n* In the simulator game, there are four rocket which have their own random speeds that change every second.\nThe names of the rockets are:\n\n1. 'Falcon 1 v0.0' -> The first privately funded liquid-fueled rocket that succesfully reached the orbit.\n2. 'Falcon 9 v1.0' -> The first private company to send a spacecraft to the International Space Station.\n3. 'Falcon 9 v1.1' -> The first landing of an orbital rocket's first stage on an ocean platform.\n4. 'Falcon 9 v1.2' -> The first reflight of a commercial cargo spacecraft.\n\n- You will choose a rocket that you think will win and play a bet with 100 credits given to you, you will earn\nas credit as much as you bet.\n- The game will automatically shut down if your credit is not enough to continue the game or if you reach 200 credit.\n\n\t\t\t\t\t\t      ----------\n\t\t\t\t\t\t      GOOD LUCK!\n\n");

	// the rocket choice input from the user
	printf("Pick up a rocket from the top and enter its number (1-4): ");
	scanf("%d", &game.choice);

	// the data validation
	while (game.choice < 1 || game.choice > 4)
	{
		printf("You've entered invalid number! Please enter a number between 1 and 4: ");
		scanf("%d", &game.choice);
	}

	// the bet input from the user
	printf("\nEnter the amount of credit you want to bet (1-100): ");
	scanf("%d", &game.bet);

	// the data validation
	while (game.bet < 1 || game.bet > 100)
	{
		printf("You've entered invalid number! Please enter a number between 1 and 100: ");
		scanf("%d", &game.bet);
	}
}

//3.10 check the if you win or not and show the status table
void checkBet()
{

	// draw the box for final status information
	glColor3f(0, 0, 0);
	glBegin(GL_POLYGON);
	glVertex2f(-150, winHeight * 7 / 16 + 35);
	glVertex2f(+150, winHeight * 7 / 16 - 35);
	glVertex2f(+150, winHeight * 7 / 16 + 35);
	glVertex2f(-150, winHeight * 7 / 16 - 35);
	glVertex2f(+150, winHeight * 7 / 16 - 35);
	glEnd();

	// check whether user win or not and display it
	if (tools.winner == game.choice)
	{
		glColor3ub(102, 204, 0);
		vprint(-35, (winHeight * 7 / 16 + 17), GLUT_BITMAP_9_BY_15, "YOU WON!");
		glColor3ub(255, 255, 255);
		vprint(-130, (winHeight * 7 / 16 - 5), GLUT_BITMAP_9_BY_15, "Your choice was %s", (rockets + game.choice-1)->name);
		vprint(-88, (winHeight * 7 / 16 - 25), GLUT_BITMAP_9_BY_15, "press f1 to restart", (rockets + game.choice - 1)->name);
	}
	else
	{
		glColor3ub(204, 0, 0);
		vprint(-40, (winHeight * 7 / 16 + 17), GLUT_BITMAP_9_BY_15, "YOU LOST!");
		glColor3ub(255, 255, 255);
		vprint(-130, (winHeight * 7 / 16 - 5), GLUT_BITMAP_9_BY_15, "Your choice was %s", (rockets + game.choice - 1)->name);
		vprint(-88, (winHeight * 7 / 16 - 25), GLUT_BITMAP_9_BY_15, "press f1 to restart", (rockets + game.choice - 1)->name);
	}
}

//3.11 reverse rockets coord system for returning back and check whether game is finished or not
void reverseCordSysAndisFinished(int x1, int x2, int x3, int x4)
{

	// reverse cord system
	if (x1 >= winHeight * 3 / 8)
	{
		(rockets + 0)->location = winHeight * 1 / 8;
		(rockets + 0)->direction *= -1;
	}
	else if (x2 >= winHeight * 3 / 8)
	{
		(rockets + 1)->location = winHeight * 1 / 8;
		(rockets + 1)->direction *= -1;
	}
	else if (x3 >= winHeight * 3 / 8)
	{
		(rockets + 2)->location = winHeight * 1 / 8;
		(rockets + 2)->direction *= -1;
	}
	else if (x4 >= winHeight * 3 / 8)
	{
		(rockets + 3)->location = winHeight * 1 / 8;
		(rockets + 3)->direction *= -1;
	}

	// check whether game is finished or not
	if (x1 <= -(winHeight / 2) || x2 <= -(winHeight / 2) || x3 <= -(winHeight / 2) || x4 <= -(winHeight / 2))
	{
		// if user won or lost, inc. or dec. the credit
		if (tools.winner == game.choice)
			game.credit += game.bet;
		else
			game.credit -= game.bet;
		// if game is finished stop the game and make isGameFinished value true
		tools.isGameFinished = true;
		tools.isGameRunning = false;
	}

}

//3.12 resets the game informations when f1 is pressed
void reset()
{
	tools.winner = 0;
	tools.isGameRunning = false;
	tools.isGameFinished = false;

	// reset coordination of rockets
	for (int i = 0; i < 4; i++)
	{
		(rockets + i)->location = 0;
		(rockets + i)->direction = 1;
		(rockets + i)->speed = 0;
	}

	// reset the chronometer
	chronometer.mil = 0;
	chronometer.sec = 0;
}

//3.13 displays the game contents
void display() {
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	// change the background color
	setBackgroundColor();

	// draw the lines and information card
	drawLines();
	drawInformation();

	// initiate the rockets
	callRockets();

	// check whether the game is running or not
	if (tools.isGameRunning == true)
	{
		// race rockets with random speeds
		racing();

		// move the stars
		moveStars();

		// check the winner
		whoIsWinner((rockets + 0)->headLocation, (rockets + 1)->headLocation, (rockets + 2)->headLocation, (rockets + 3)->headLocation);

		// reverse rockets coord system for returning back and check whether game is finished or not
		reverseCordSysAndisFinished((rockets + 0)->headLocation, (rockets + 1)->headLocation, (rockets + 2)->headLocation, (rockets + 3)->headLocation);
	}

	// if game is finished check the if you win or not and show the status table
	if (tools.isGameFinished)
		checkBet();

	// when credit is not enough to continue exit from the game
	if (game.credit < game.bet)
		exit(0);

	glutSwapBuffers();
}

// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// run or stop the game when SPACE is pressed and game have not finished yet.
	if (key == 32 && tools.isGameFinished == 0) {
		if (tools.isGameRunning)
			tools.isGameRunning = false;
		else
		{
			// the loop for initiating rockets speed from 18.000 mil/h
			for (int i = 0; i < 4; i++)
				(rockets + i)->speed = 18000;
			tools.isGameRunning = true;
		}

	}

	// speed up rockets when S/s is pressed.
	if (key == 83 || key == 115)
		for (int i = 0; i < 4; i++)
			// if it is equal or bigger than zero user can decrement the speed
			if ((rockets + i)->ratio >= 0)
				(rockets + i)->ratio -= 0.5;


	// speed down rockets when F/f is pressed.
	if (key == 70 || key == 102)
		// if it is equal or smaller than zero user can increment the speed
		for (int i = 0; i < 4; i++)
			if ((rockets + i)->ratio <= 2)
				(rockets + i)->ratio += 0.5;

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
	if (key == GLUT_KEY_F1 && tools.isGameFinished == 1) reset();

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
	if (tools.isGameRunning == true && tools.activeTimer == true)
	{
		chronometer.mil++;

		if (chronometer.mil % 100 == 0)
		{
			chronometer.sec++;
			chronometer.mil = 0;
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
	glutCreateWindow("SpaceX - Rocket Simulator");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	// display the game informations on the console
	displayGameInformation();

	// keyboard registration
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	// mouse registration

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	Init();

	// generates random number
	srand(time(0));

	glutMainLoop();
}
