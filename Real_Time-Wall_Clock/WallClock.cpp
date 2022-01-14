// header files
#include <Windows.h>
#include <iostream>
#include <gl/freeglut.h>
#include <time.h>
#include <math.h>

#define DEGREE_TO_RADIAN 0.0174533
#define ROOT_OF_TWO 1.414213f

// global variables
bool hva_bFullScreen = false;
float f_sec_angle;
float f_min_angle;
float f_hr_angle;
time_t curr_time = NULL;
struct tm* time_ptr = NULL;
int i_sec_count = 0;
int i_min_count = 0;
int i_hr_count = 0;

// entry-point function
int main(int argc, char* argv[], char* envp[]) {

	// local function declarations
	void initialize(void);
	void resize(int, int);
	void display(void);
	void _display(int);
	void keyboard(unsigned char, int, int);
	void mouse(int, int, int, int);
	void uninitialize(void);

	// code
	PlaySound(TEXT("ClockTick.wav"), NULL, SND_LOOP | SND_ASYNC);
	
	// get current time
	time(&curr_time);
	time_ptr = localtime(&curr_time);
	f_hr_angle = 90.0f - (float)((time_ptr->tm_hour) * 30);
	f_min_angle = 90.0f - (float)((time_ptr->tm_min)) * 6;
	f_sec_angle = 90.0f - (float)((time_ptr->tm_sec) * 6);

	f_min_angle = f_min_angle - 0.1f * (float)(time_ptr->tm_sec);
	f_hr_angle = f_hr_angle - 0.008333f * (float)((time_ptr->tm_min * 60) + time_ptr->tm_sec);

	glutInit(&argc, argv);

	// std::cout << time_ptr->tm_hour << std::endl;
	// std::cout << time_ptr->tm_min << std::endl;
	// std::cout << time_ptr->tm_sec << std::endl;
	// std::cout << time_ptr->tm_yday << std::endl;

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowSize(650, 600);
	glutInitWindowPosition(420, 150);
	glutCreateWindow("Real-Time Wall Clock : Hrushikesh Ahire");

	initialize();

	glutReshapeFunc(resize);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutCloseFunc(uninitialize);

	glutIdleFunc(display);

	glutTimerFunc(125, _display, NULL);

	glutMainLoop();

	return 0;
}

void initialize(void) {

	// code
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void resize(int width, int height) {

	// code
	glViewport(0, 0, width, height);
}

void _display(int x) {

	// code
	f_sec_angle = f_sec_angle - 0.75f;
	f_min_angle = f_min_angle - 0.0125f;
	f_hr_angle = f_hr_angle - 0.00104166f;

	i_sec_count++;

	// std::cout << "***sec_angle : " << f_sec_angle << std::endl;
	if (i_sec_count == 480) {

		i_min_count++;
		f_sec_angle = 90.0f - (float)((time_ptr->tm_sec) * 6);
		// std::cout << "***sec_angle : " << f_sec_angle << std::endl;
		i_sec_count = 0;
		// std::cout << "count, " << i_min_count << " min_angle : " << f_min_angle << std::endl;
		if (i_min_count == 60) {	
			i_hr_count++;
			f_min_angle = 90.0f - (float)((time_ptr->tm_min) * 6) - 0.1f * (float)time_ptr->tm_sec;
			// std::cout << "count, "<< i_min_count <<" ***min_angle : " << f_min_angle << std::endl;
			i_min_count = 0;
			if (i_hr_count % 12 == 0) {
				f_hr_angle = 90.0f - (float)((time_ptr->tm_hour) * 30) - 0.008333f * (float)((time_ptr->tm_min * 60) + time_ptr->tm_sec);
				// std::cout << "*count : " << i_hr_count << ", hr_angle : " << f_hr_angle << std::endl;
			}
			// std::cout <<"count : " << i_hr_count << ", hr_angle : " << f_hr_angle << std::endl;
		}
	}

	glutTimerFunc(125, _display, NULL);
}

void display(void) {

	// variable declaration
	static double dAngle;
	static float f_x = 0.60f;
	static float f_y = 0.60f;
	static float f_sec_r = 0.38f;
	static float f_min_r = 0.53f;
	static float f_hr_r = 0.40f;
	

	// static float f_r = (float)sqrt((double)(f_x * f_x + f_y * f_y));

	// code
	glClear(GL_COLOR_BUFFER_BIT);
	
	// Border
	glPointSize(14.0f);
	glEnable(GL_POINT_SMOOTH);
	glBegin(GL_POINTS);
	glColor3f(0.4f, 0.4f, 0.4f);
	for (dAngle = 0.0; dAngle < 360.0; dAngle = dAngle + 1.0) {
		glVertex3f((f_x + 0.03f) * cos(dAngle * DEGREE_TO_RADIAN), (f_y + 0.03) * sin(dAngle * DEGREE_TO_RADIAN), 0.0f);
	}

	glEnd();

	// Clock
	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.8f, 0.8f);
	
	// degree * 0.0174533 = radian
	for (dAngle = 0.0; dAngle <= 360.00; dAngle = dAngle + 0.50) {
		glVertex3f(f_x * cos(dAngle * DEGREE_TO_RADIAN), f_y * sin(dAngle * DEGREE_TO_RADIAN), 0.0f);
	}
	glEnd();

	// Points
	// thick points
	glPointSize(12.0f);
	glEnable(GL_POINT_SMOOTH);
	glBegin(GL_POINTS);
	glColor3f(0.3f, 0.3f, 0.3f);
	
	for (dAngle = 0.0; dAngle < 360.00; dAngle = dAngle + 90.00) {
		
		glVertex3f((f_x - 0.03f) * cos(dAngle * DEGREE_TO_RADIAN), (f_y - 0.03f) * sin(dAngle * DEGREE_TO_RADIAN), 0.0f);
	}
	glEnd();

	// normal points
	glPointSize(6.0f);
	glBegin(GL_POINTS);
	glColor3f(0.45f, 0.45f, 0.45f);

	for (dAngle = 0.0; dAngle < 360.00; dAngle = dAngle + 30.00) {

		if ((((int)dAngle) % 90) == 0)
			continue;
		glVertex3f((f_x - 0.03f) * cos(dAngle * DEGREE_TO_RADIAN), (f_y - 0.03f) * sin(dAngle * DEGREE_TO_RADIAN), 0.0f);
	}

	glEnd();

	// Lines

	glEnable(GL_LINE_SMOOTH);
	glLineWidth(1.0f);
	glBegin(GL_LINES);

	// second's line
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.0f, 0.0f);
	glVertex2f((f_sec_r * ROOT_OF_TWO) * cos(f_sec_angle * DEGREE_TO_RADIAN), (f_sec_r * ROOT_OF_TWO) * sin(f_sec_angle * DEGREE_TO_RADIAN));

	glEnd();

	glLineWidth(2.0f);
	glBegin(GL_LINES);
	// minute's line
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(f_min_r * cos(f_min_angle * DEGREE_TO_RADIAN), f_min_r * sin(f_min_angle * DEGREE_TO_RADIAN));

	glLineWidth(2.0f);
	// hour's line
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(f_hr_r * cos(f_hr_angle * DEGREE_TO_RADIAN), f_hr_r * sin(f_hr_angle * DEGREE_TO_RADIAN));

	glEnd();

	// arrow heads for lines
	glBegin(GL_TRIANGLES);

	// second's line
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f((f_sec_r * ROOT_OF_TWO) * cos(f_sec_angle * DEGREE_TO_RADIAN), (f_sec_r * ROOT_OF_TWO) * sin(f_sec_angle * DEGREE_TO_RADIAN));
	glVertex2f(((f_sec_r - 0.01f) * ROOT_OF_TWO) * cos((f_sec_angle + 2.0f) * DEGREE_TO_RADIAN), ((f_sec_r - 0.01) * ROOT_OF_TWO) * sin((f_sec_angle + 2.0f) * DEGREE_TO_RADIAN));
	glVertex2f(((f_sec_r - 0.01f) * ROOT_OF_TWO) * cos((f_sec_angle - 2.0f) * DEGREE_TO_RADIAN), ((f_sec_r - 0.01) * ROOT_OF_TWO) * sin((f_sec_angle - 2.0f) * DEGREE_TO_RADIAN));

	// minute's line
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(f_min_r * cos(f_min_angle * DEGREE_TO_RADIAN), f_min_r * sin(f_min_angle * DEGREE_TO_RADIAN));
	glVertex2f((f_min_r - 0.01f) * cos((f_min_angle + 2.0f) * DEGREE_TO_RADIAN), (f_min_r - 0.01f) * sin((f_min_angle + 2.0f) * DEGREE_TO_RADIAN));
	glVertex2f((f_min_r - 0.01f) * cos((f_min_angle - 2.0f) * DEGREE_TO_RADIAN), (f_min_r - 0.01f) * sin((f_min_angle - 2.0f) * DEGREE_TO_RADIAN));

	// hour's line
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex2f(f_hr_r * cos(f_hr_angle * DEGREE_TO_RADIAN), f_hr_r * sin(f_hr_angle * DEGREE_TO_RADIAN));
	glVertex2f((f_hr_r - 0.015f) * cos((f_hr_angle + 2.5f) * DEGREE_TO_RADIAN), (f_hr_r - 0.015f) * sin((f_hr_angle + 2.5f) * DEGREE_TO_RADIAN));
	glVertex2f((f_hr_r - 0.015f) * cos((f_hr_angle - 2.5f) * DEGREE_TO_RADIAN), (f_hr_r - 0.015f) * sin((f_hr_angle - 2.5f) * DEGREE_TO_RADIAN));

	glEnd();

	glPointSize(12.0f);
	glEnable(GL_POINT_SMOOTH);
	glBegin(GL_POINTS);
	glColor3f(0.3f, 0.3f, 0.3f);
	glVertex3f(0.0f, 0.0f, 0.0f);

	glEnd();

	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {

	// code
	switch (key) {
	case 27:
		glutLeaveMainLoop();
		break;
	case 'F':
	case 'f':
		if (hva_bFullScreen == false) {
			glutFullScreen();
			hva_bFullScreen = true;
		}
		else {
			glutLeaveFullScreen();
			hva_bFullScreen = false;
		}
		break;
	default:
		break;
	}
}

void mouse(int button, int state, int x, int y) {

	// code
	switch (button) {
	case GLUT_LEFT_BUTTON:
		PlaySound(NULL, 0, 0);
		glutLeaveMainLoop();
		break;
	default:
		break;
	}
}

void uninitialize(void) {

	// code
	PlaySound(NULL, 0, 0);
}


