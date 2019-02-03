/*********
CTIS164 - Template Source Program
----------
STUDENT : Mian Muhammad Umair Amer
SECTION : 03
HOMEWORK: 02
----------
PROBLEMS: Works perfectly fine, just put GL/ in front of glut.h in library include because i use without GL/
*********/

#include <glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include<time.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 680

#define TIMER_PERIOD     10 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

//Global Variables For This Program
int Cx = -300, Cy = -140; //Cannon's X and Y Cordinates
int Bx = Cx + 18, By = Cy + 30; // Bullets X and Y cordinates according to cannon
int min = 20, sec = 0; //for timmer


bool fire = false; //For Bullet

//states
#define START 0
#define RUN 1
#define END 2

int gamestate = START;
bool activetimmer = false;

int TotalJets = 0, firedJets = 0, TotalPoints = 0, LastPoint = 0;

//structure for jets
typedef struct {
	int x, y;
	bool collision = false;
}jet_t;

jet_t jet[5];

void intializeVariables()
{
	TotalJets = 0;
	firedJets = 0;
	TotalPoints = 0;
	LastPoint = 0;
	min = 20;
	sec = 0;
	fire = false;
	//Giving jets Cordinates
	for (int i = 0; i < 5; i++)
	{
		jet[i].y = rand() % 251 + 45;
		jet[i].x = (-400 - 110) - (i * 150);
	}
}

//
// to draw circle, center at (x,y)
// radius r
//
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

	glLineWidth(7);
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
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
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
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
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
	for (i = 0; i < len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}

//Draw Bullets
void bullets(int x, int y)
{
	//Bullet Head
	glColor3ub(255, 153, 51);
	glBegin(GL_TRIANGLES);
	glVertex2f(x, y);
	glVertex2f(x + 7, y + 30);
	glVertex2f(x + 14, y);
	glEnd();

	//Bullet Curve
	glBegin(GL_QUADS);
	glVertex2f(x, y - 15);
	glVertex2f(x - 3, y - 20);
	glVertex2f(x + 17, y - 20);
	glVertex2f(x + 14, y - 15);
	glEnd();

	//Bullet length/body
	glColor3ub(100, 100, 100);
	glRectf(x, y, x + 14, y - 15);
	glRectf(x - 3, y - 20, x + 17, y - 50);
}

//Draws Jets at given X,Y Cordinates
void drawJets(int x, int y)
{
	//window
	glColor3ub(0, 0, 200);
	circle(x + 55, y + 15, 15);

	//Body
	glColor3ub(51, 102, 0);
	glBegin(GL_POLYGON);
	glVertex2f(x, y);
	glVertex2f(x, y + 30);
	glVertex2f(x + 55, y + 30);
	glVertex2f(x + 55, y + 15);
	glVertex2f(x + 70, y + 15);
	glVertex2f(x + 70, y);
	glEnd();

	//window
	glColor3ub(0, 0, 200);
	glRectf(x + 45, y + 15, x + 55, y + 30);

	//pointy end of jet
	glColor3ub(0, 102, 0);
	glBegin(GL_TRIANGLES);
	glVertex2f(x + 70, y + 15);
	glVertex2f(x + 110, y);
	glVertex2f(x + 70, y);
	glEnd();

	//tail
	glColor3ub(150, 0, 0);
	glBegin(GL_POLYGON);
	glVertex2f(x, y + 30);
	glVertex2f(x, y + 45);
	glVertex2f(x + 15, y + 45);
	glVertex2f(x + 20, y + 37);
	glVertex2f(x + 30, y + 30);
	glEnd();

	//exhaust
	glColor3ub(0, 0, 0);
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x, y + 30);
	glVertex2f(x - 25, y + 22);
	glVertex2f(x - 25, y + 10);
	glEnd();

	//wings
	glColor3ub(150, 0, 0);
	glBegin(GL_QUADS);
	glVertex2f(x + 20, y + 15);
	glVertex2f(x + 12, y - 18);
	glVertex2f(x + 22, y - 10);
	glVertex2f(x + 35, y + 15);
	glEnd();


}

//Draws cannon
void drawCannon()
{
	//BODY
	glColor3ub(0, 0, 0);
	glBegin(GL_QUADS);
	// midlle trapezium // main refrence point
	glVertex2f(Cx, Cy);
	glVertex2f(Cx + 5, Cy + 35);
	glVertex2f(Cx + 45, Cy + 35);
	glVertex2f(Cx + 50, Cy);
	// top most part of cannon
	glVertex2f(Cx + 10, Cy + 35);
	glVertex2f(Cx + 15, Cy + 60);
	glVertex2f(Cx + 35, Cy + 60);
	glVertex2f(Cx + 40, Cy + 35);
	//bottom part of trapizium
	glVertex2f(Cx - 5, Cy);
	glVertex2f(Cx - 10, Cy - 25);
	glVertex2f(Cx + 60, Cy - 25);
	glVertex2f(Cx + 55, Cy);

	//pattern
	glColor3ub(255, 153, 51);
	glVertex2f(Cx + 5, Cy + 35);
	glVertex2f(Cx + 7, Cy + 38);
	glVertex2f(Cx + 43, Cy + 38);
	glVertex2f(Cx + 45, Cy + 35);
	//pattern
	glVertex2f(Cx - 5, Cy);
	glVertex2f(Cx - 3, Cy + 5);
	glVertex2f(Cx + 53, Cy + 5);
	glVertex2f(Cx + 55, Cy);
	glEnd();

	// bottom curve
	glColor3ub(0, 0, 0);
	circle(Cx + 25, Cy - 40, 40);

	//TYRES
	glColor3ub(64, 64, 64);
	// changed linewidth in circlewire function
	circle_wire(Cx - 10, Cy - 70, 20);
	circle_wire(Cx + 60, Cy - 70, 20);

	glLineWidth(3);
	glBegin(GL_LINES);
	//Inner Lines of left tyre
	glVertex2f(Cx - 10, Cy - 70);
	glVertex2f(Cx - 30, Cy - 70);
	glVertex2f(Cx - 10, Cy - 70);
	glVertex2f(Cx - 10, Cy - 90);
	glVertex2f(Cx - 10, Cy - 70);
	glVertex2f(Cx + 10, Cy - 70);
	glVertex2f(Cx - 10, Cy - 70);
	glVertex2f(Cx - 10, Cy - 50);
	//inner lines of right tyre
	glVertex2f(Cx + 60, Cy - 70);
	glVertex2f(Cx + 40, Cy - 70);
	glVertex2f(Cx + 60, Cy - 70);
	glVertex2f(Cx + 60, Cy - 90);
	glVertex2f(Cx + 60, Cy - 70);
	glVertex2f(Cx + 80, Cy - 70);
	glVertex2f(Cx + 60, Cy - 70);
	glVertex2f(Cx + 60, Cy - 50);
	glEnd();
	//Name
	glColor3ub(255, 153, 51);
	vprint(Cx - 2, Cy - 40, GLUT_BITMAP_9_BY_15, "DESTRO");
}

//Displays Static Background
void displayBackground()
{
	//Land
	glBegin(GL_QUADS);
	glColor3ub(153, 76, 0);
	glVertex2f(-400, -250);
	glVertex2f(400, -250);
	glColor3ub(0, 0, 0);
	glVertex2f(400, -315);
	glVertex2f(-400, -315);
	glEnd();

	//Grass
	glColor3ub(0, 153, 0);
	glRectf(-400, -250, 400, -230);

	//Sky
	glBegin(GL_QUADS);
	glColor3ub(51, 153, 255);
	glVertex2f(-400, 340);
	glVertex2f(400, 340);
	glColor3ub(255, 255, 255);
	glVertex2f(400, -230);
	glVertex2f(-400, -230);
	glEnd();

	//instructions bar
	glColor3ub(0, 0, 0);
	glRectf(-400, -340, 400, -315);

	//Information
	vprint(-360, -270, GLUT_BITMAP_HELVETICA_18, "TIMER");
	vprint(-220, -270, GLUT_BITMAP_HELVETICA_18, "Total Jets");
	vprint(-60, -270, GLUT_BITMAP_HELVETICA_18, "Fired Jets");
	vprint(120, -270, GLUT_BITMAP_HELVETICA_18, "Last Point");
	vprint(280, -270, GLUT_BITMAP_HELVETICA_18, "Total Points");
}

//
// To display onto window using OpenGL commands
//
void display()
{

	//
	// clear window to black
	//
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	displayBackground();

	// For Returning bullet to its start point otherwise keep it moving
	if (!fire)
		bullets(Cx + 18, Cy + 30);
	else
		bullets(Bx, By);

	drawCannon();

	if (gamestate == RUN)
	{//Collision 
		for (int i = 0; i < 5; i++)
		{
			if (jet[i].collision == false && (By + 30 >= jet[i].y && By + 30 <= jet[i].y + 30) && (Bx + 7 >= jet[i].x - 25 && Bx + 7 <= jet[i].x + 110))
			{//Scoring
				if ((Bx + 7 >= jet[i].x - 25 && Bx + 7 <= jet[i].x - 10) || (Bx + 7 >= jet[i].x + 95 && Bx + 7 <= jet[i].x + 110))
					LastPoint = 1;
				else if ((Bx + 7 > jet[i].x - 10 && Bx + 7 < jet[i].x + 5) || (Bx + 7 > jet[i].x + 80 && Bx + 7 < jet[i].x + 95))
					LastPoint = 2;
				else if ((Bx + 7 >= jet[i].x + 5 && Bx + 7 <= jet[i].x + 20) || (Bx + 7 >= jet[i].x + 65 && Bx + 7 <= jet[i].x + 80))
					LastPoint = 3;
				else if ((Bx + 7 > jet[i].x + 20 && Bx + 7 < jet[i].x + 35) || (Bx + 7 > jet[i].x + 50 && Bx + 7 < jet[i].x + 65))
					LastPoint = 4;
				else if ((Bx + 7 >= jet[i].x + 35 && Bx + 7 <= jet[i].x + 50))
					LastPoint = 5;

				TotalPoints += LastPoint;
				jet[i].collision = true;
				firedJets++;
			}
	
			if (jet[i].collision != true)
				drawJets(jet[i].x, jet[i].y);
		}
		glColor3ub(200, 200, 0);
		vprint(-180, -330, GLUT_BITMAP_9_BY_15, "<SpaceBar> to Fire, <F1> to Play/Pause");
	}
	//Instructions
	if (gamestate == START)
	{
		glColor3ub(200, 200, 0);
		vprint(-80, -330, GLUT_BITMAP_9_BY_15, "<F1> to START GAME");
	}
	else if (gamestate == END)
	{
		glColor3ub(200, 200, 0);
		vprint(-80, -330, GLUT_BITMAP_9_BY_15, "<F1> to START NEW GAME");
		glColor3ub(255, 0, 0);
		vprint(-80, 150, GLUT_BITMAP_TIMES_ROMAN_24, "GAME OVER");
	}

	glColor3ub(0, 255, 0);
	vprint2(-360, -300, 0.18, "%02d:%02d", min, sec);
	vprint2(-190, -300, 0.18, "%02d", TotalJets);
	vprint2(-30, -300, 0.18, "%02d", firedJets);
	vprint2(145, -300, 0.18, "%02d", LastPoint);
	vprint2(315, -300, 0.18, "%02d", TotalPoints);

	glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);
	//To Fire From SpaceBar
	if (key == 32 && fire == false && gamestate == RUN && min > 0)
		fire = true;



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

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
	// Write your codes here.
	switch (key)
	{
	case GLUT_KEY_F1:
		if (gamestate == START)
		{
			activetimmer = true;
			intializeVariables();
			gamestate = RUN;
		}
		else if (gamestate == RUN)
			activetimmer = !activetimmer;
		else if (gamestate == END)
		{
			gamestate = START;
			intializeVariables();
		}
		break;
	}

	//To Move Cannon
	if (key == GLUT_KEY_RIGHT && Cx + 70 <= 400)
		Cx += 7;

	if (key == GLUT_KEY_LEFT && Cx - 20 >= -400)
		Cx -= 7;

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
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

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
	// Write your codes here.

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
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
	if (activetimmer && gamestate == RUN)
	{
		//Counter
		if (sec != 0)
			sec--;
		else
		{
			min -= 1;
			sec = 99;
		}
		//To Stop The Game
		if (min == 0 && sec == 0)
			gamestate = END;

		//For Moving the Bullet till top of the screen
		if (By - 45 <= 340)
			By += 6;
		else
			fire = false;

		//Resetting Bullet Position in cannon
		if (!fire || gamestate == END)
		{
			Bx = Cx + 18;
			By = Cy + 30;
		}

		//Moving Jets
		for (int i = 0; i < 5; i++)
		{
			if (jet[i].x >= 400)
			{
				jet[i].y = rand() % 251 + 45;
				jet[i].x = (-winWidth / 2 - 110);
				jet[i].collision = false;
			}
			else
				jet[i].x += 5;

			//Counting total number of jets appearing from leftside of the window
			if (jet[i].x == -400)
				TotalJets++;
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
	glutCreateWindow("WarZone : The Might Of Destro - Mian Muhammad Umair Amer");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	//
	// keyboard registration
	//
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	//
	// mouse registration
	//
	glutMouseFunc(onClick);
	glutMotionFunc(onMoveDown);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	Init();
	srand(time(0));
	glutMainLoop();
}