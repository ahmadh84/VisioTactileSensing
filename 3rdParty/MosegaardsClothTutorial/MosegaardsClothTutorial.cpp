#include <GL/gl.h>
#include <GL/glut.h> 
#include <math.h>
#include <vector>
#include <iostream>
#include <stdio.h>

#include "Cloth.h"

int mMouseX = 640;
int mMouseY = 360;
double camXPos = -2.5, camYPos = -9.0, camZPos = 5.0;
double camXRot = -25.0, camYRot = 0.8;
bool mRotate = false;

// Just below are three global variables holding the actual animated stuff; Cloth and Ball
Cloth cloth1(14, 10, 55, 45);  // one Cloth object of the Cloth class
Vec3 ball_pos(7.0, 1.0, -5.0);  // the center of our one ball
float ball_radius = 0.5;  // the radius of our one ball

/* This is where all the standard Glut/OpenGL stuff is, and where the methods of Cloth are called; 
 addForce(), windForce(), timeStep(), ballCollision(), and drawShaded()*/

void init() {
	glShadeModel(GL_SMOOTH);
	glClearColor(0.2f, 0.2f, 0.4f, 0.5f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_COLOR_MATERIAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat lightPos[4] = {-1.0, 1.0, 0.5, 0.0};
	glLightfv(GL_LIGHT0, GL_POSITION, (GLfloat *) &lightPos);

	glEnable(GL_LIGHT1);

	GLfloat lightAmbient1[4] = {0.0, 0.0, 0.0, 0.0};
	GLfloat lightPos1[4] = {1.0, 0.0, -0.2, 0.0};
	GLfloat lightDiffuse1[4] = {0.5, 0.5, 0.3, 0.0};

	glLightfv(GL_LIGHT1, GL_POSITION, (GLfloat *) &lightPos1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, (GLfloat *) &lightAmbient1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, (GLfloat *) &lightDiffuse1);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
}

float ball_time = 0;  // counter for used to calculate the z position of the ball below

/* display method called each frame*/
void display(void) {
	// calculating positions

	ball_time++;
	// ball_pos.f[2] = -7.0; // cos(ball_time / 50.0) * 7;

	// cloth1.addForce(Vec3(0.0, -0.2, 0.0) * TIME_STEPSIZE2);  // add gravity each frame, pointing down
	// cloth1.windForce(Vec3(0.5, 0, 0.2) * TIME_STEPSIZE2);  // generate some wind each frame
	cloth1.timeStep();  // calculate the particle positions of the next frame
	cloth1.ballCollision(ball_pos, ball_radius);  // resolve collision with the ball

	// drawing

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// printf("rots: {%.3lf, %.3lf}, pos: {%.3lf, %.3lf, %.3lf}\n", camXRot, camYRot, camXPos, camYPos, camZPos);
	glRotatef(camXRot, 0.0f, 1.0f, 0.0f);        // Rotate our camera on the x-axis (looking up and down)
	glRotatef(camYRot, 1.0f, 0.0f, 0.0f);        // Rotate our camera on the  y-axis (looking left and right)
	glTranslatef(camXPos, camYPos, camZPos);

	glTranslatef(-6.5, 6, -9.0f);  // move camera out and center on the cloth
	glRotatef(25, 0, 1, 0);  // rotate a bit to see the cloth from the side
	cloth1.drawShaded();  // finally draw the cloth with smooth shading

	// Drawing the ball
	/*
	glPushMatrix();  // to draw the ball we use glutSolidSphere, and need to draw the sphere at the position of the ball
	glTranslatef(ball_pos.f[0], ball_pos.f[1], ball_pos.f[2]);  // hence the translation of the sphere onto the ball position
	glColor3f(0.4f, 0.8f, 0.5f);
	glutSolidSphere(ball_radius - 0.1, 50, 50);  // draw the ball, but with a slightly lower radius, otherwise we could get ugly visual artifacts of cloth penetrating the ball slightly
	glPopMatrix();
*/

	glutSwapBuffers();
	glutPostRedisplay();
}

void reshape(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(h == 0) gluPerspective(80, (float) w, 1.0, 5000.0);
	else gluPerspective(80, (float) w / (float) h, 1.0, 5000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y) {

	double tmp = 0.5;
	double dBall = 0.1;
	switch(key) {
		case 'W': camZPos += tmp; break;
		case 's': camZPos -= tmp; break;
		case 'a': camXPos += tmp; break;
		case 'd': camXPos -= tmp; break;
		case 'q': camYPos -= tmp; break;
		case 'e': camYPos += tmp; break;
		case ' ': ball_pos.f[1] -= dBall; break;
		case '4': ball_pos.f[0] -= dBall; break;
		case '6': ball_pos.f[0] += dBall; break;
		case '8': ball_pos.f[2] -= dBall; break;
		case '2': ball_pos.f[2] += dBall; break;
	}
	glutPostRedisplay();
}

void mouseClick(int button, int state, int x, int y) {

	mMouseX = x;
	mMouseY = y;

	static bool mMouseDown = false;
	mMouseDown = !mMouseDown;
	if(mMouseDown) {
		mRotate = true;
	}
	glutPostRedisplay();
}

void mouseDrag(int x, int y) {

	double deltaX = x - mMouseX;
	double deltaY = y - mMouseY;

	mMouseX = x;
	mMouseY = y;

	if(mRotate) {
		GLfloat vertMouseSensitivity = 10.0f;
		GLfloat horizMouseSensitivity = 10.0f;
		camXRot += deltaX / vertMouseSensitivity;
		camYRot += deltaY / horizMouseSensitivity;
	}

	glutPostRedisplay();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1280, 720);

	glutCreateWindow("Cloth Tutorial from Jesper Mosegaard");
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseDrag);
	glutKeyboardFunc(keyboard);
//  glutPassiveMotionFunc( mouseMove );

	glutMainLoop();
}
