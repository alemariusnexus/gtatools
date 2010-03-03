#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <gtadff.h>
#include <fstream>

using std::ifstream;

#define PI 3.141593f

static GLfloat rotX = 0.0f;
static GLfloat rotY = 0.0f;
static GLenum polyMode = GL_FILL;
static int32_t geomIdx = 0;
static GLfloat scale = 10.0f;
DFFMesh* mesh;

void init(void) {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0f, 0.0f, 0.0f);

	glPolygonMode(GL_FRONT_AND_BACK, polyMode);

	glEnableClientState(GL_VERTEX_ARRAY);

	glLoadIdentity();

	glScalef(scale, scale, scale);
	glRotatef(rotX, 1.0f, 0.0f, 0.0f);
	glRotatef(rotY, 0.0f, 1.0f, 0.0f);



	DFFGeometry** geoms = mesh->getGeometries();

	for (int32_t i = 0 ; i < mesh->getGeometryCount() ; i++) {
		if (i != geomIdx) {
			continue;
		}

		DFFGeometry* geom = geoms[i];
		glVertexPointer(3, GL_FLOAT, 0, geom->getVertices());

		if (geom->isTriangleStripFormat())
			glBegin(GL_TRIANGLE_STRIP);
		else
			glBegin(GL_TRIANGLES);

			DFFGeometryPart** parts = geom->getParts();
			for (int32_t j = 0 ; j < geom->getPartCount() ; j++) {
				DFFGeometryPart* part = parts[j];
				int32_t* indices = part->getIndices();

				for (int32_t k = 0 ; k < part->getIndexCount() ; k++) {
					glArrayElement(indices[k]);
				}
			}

		glEnd();
	}

	printf("displayed!\n");

	glFlush();
	glutSwapBuffers();
}

void rotx() {
	rotX += 0.5f;
	if (rotX > 360.0f) rotX -= 360.0f;
	glutPostRedisplay();
}

void roty() {
	rotY += 0.5f;
	if (rotY > 360.0f) rotY -= 360.0f;
	glutPostRedisplay();
}

void doscale() {
	scale += 0.1f;
	glutPostRedisplay();
}

void unscale() {
	scale -= 0.1f;
	glutPostRedisplay();
}

void reshape(int w, int h) {
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-50.0, 50.0, -50.0, 50.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void kbd(unsigned char c, int x, int y) {
	switch (c) {
	case 'q':
		glutIdleFunc(rotx);
		break;
	case 'e':
		glutIdleFunc(roty);
		break;
	case 'w':
		polyMode = GL_LINE;
		glutPostRedisplay();
		break;
	case 's':
		polyMode = GL_FILL;
		glutPostRedisplay();
		break;
	case 'd':
		polyMode = GL_POINT;
		glutPostRedisplay();
		break;
	case 'n':
		geomIdx++;
		if (geomIdx >= mesh->getGeometryCount()) {
			geomIdx = 0;
		}
		glutPostRedisplay();
		break;
	case 'p':
		geomIdx--;
		if (geomIdx < 0) {
			geomIdx = mesh->getGeometryCount()-1;
		}
		glutPostRedisplay();
		break;
	case 'i':
		glutIdleFunc(doscale);
		break;
	case 'o':
		glutIdleFunc(unscale);
		break;
	}
}

void kbdup(unsigned char c, int x, int y) {
	switch (c) {
	case 'q':
		glutIdleFunc(NULL);
		break;
	case 'e':
		glutIdleFunc(NULL);
		break;
	case 'i':
		glutIdleFunc(NULL);
		break;
	case 'o':
		glutIdleFunc(NULL);
		break;
	}
}

/*
 *  Request double buffer display mode.
 *  Register mouse input callback functions
 */
int main(int argc, char** argv) {
	DFFLoader dff;
	ifstream stream("/home/alemariusnexus/test/infernus.dff", ifstream::in | ifstream::binary);

	mesh = dff.loadMesh(&stream);
	mesh->mirrorYZ();



	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(kbd);
	glutKeyboardUpFunc(kbdup);
	glutMainLoop();
	return 0;
}

