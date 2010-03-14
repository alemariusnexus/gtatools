/*
	Copyright 2010 David Lerch

	This file is part of gltest.

	gltest is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gltest is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gltest.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <gtaformats/gtadff.h>
#include <gtaformats/gtatxd.h>
#include <fstream>
#include <gtaformats/gtaimg.h>
#include <map>
#include <utility>

#define MODEL_NAME "infernus"

using namespace std;

#define PI 3.141593f



class StrComparator {
public:
	bool operator()(const char* s1, const char* s2) const {
		return strcmp(s1, s2) < 0;
	}
};




static map<const char*, GLuint, StrComparator> texes;

static GLuint texName = 0;

static int lastX = -1;
static int lastY = -1;
static GLfloat rotX = 0.0f;
static GLfloat rotY = 0.0f;
static GLenum polyMode = GL_FILL;
static int32_t geomIdx = 0;
static GLfloat scale = 10.0f;
DFFMesh* mesh;




void init(void) {
	//glDisable(GL_BLEND);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	//glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
	//glEnable(GL_TEXTURE_2D);


	//ifstream txdstream("/home/alemariusnexus/infernus.txd", ifstream::in | ifstream::binary);

	char txdTable[][256] = {
			"&" MODEL_NAME ".txd",
			//"&jizzy.txd"
			"/home/alemariusnexus/GTA/GTA/Grand Theft Auto San Andreas/models/generic/vehicle.txd"
	};

	IMGArchive img("/home/alemariusnexus/GTA/GTA/Grand Theft Auto San Andreas/models/gta3.img");


	DFFLoader dff;
	dff.setVerbose(true);

	mesh = dff.loadMesh(img.gotoEntry(MODEL_NAME ".dff"));
	mesh->mirrorYZ();

	/*DFFFrame** frames = mesh->getFrames();
	DFFGeometry** geoms = mesh->getGeometries();

	for (int32_t i = 0 ; i < mesh->getGeometryCount() ; i++) {
		DFFGeometry* geom = geoms[i];

		printf("Geometry %d\n", i);

		DFFGeometryPart** parts = geom->getParts();

		for (int32_t j = 0 ; j < geom->getPartCount() ; j++) {

		}
	}*/

	/*for (int32_t i = 0 ; i < mesh->getFrameCount() ; i++) {
		DFFFrame* frame = frames[i];
		DFFFrame* parent = frame->getParent();
		printf("%s (%s)", frame->getName(), parent == NULL ? "none" : parent->getName());

		for (int32_t j = 0 ; j < mesh->getGeometryCount() ; j++) {
			DFFGeometry* geom = geoms[j];

			if (geom->getAssociatedFrame() == frame) {
				printf(" => Geometry %d", j);
			}
		}

		printf("\n");
	}*/

	//exit(0);



	for (unsigned int i = 0 ; i < sizeof(txdTable)/sizeof(txdTable[0]) ; i++) {
		char* txdn = txdTable[i];

		TXDArchive* txd;
		ifstream* stream = NULL;

		if (txdn[0] == '&') {
			istream* stream = img.gotoEntry(txdn+1);
			txd = new TXDArchive(stream);
		} else {
			stream = new ifstream(txdn, ifstream::in | ifstream::binary);
			txd = new TXDArchive(stream);
		}

		for (int32_t j = 0 ; j < txd->getTextureCount() ; j++) {
			TXDTexture* tex = txd->nextTexture();
			uint8_t* rawData = txd->readTextureData(tex);
			uint8_t* data = new uint8_t[tex->getWidth() * tex->getHeight() * 3];
			//tex->convert(data, rawData, MIRROR_NONE);
			tex->convert(data, rawData, MIRROR_NONE, 3, 0, 1, 2, -1);
			//tex->convert(data, rawData);
			delete[] rawData;

			GLuint texName = 0;

			glGenTextures(1, &texName);
			glBindTexture(GL_TEXTURE_2D, texName);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex->getWidth(), tex->getHeight(),
					0, GL_RGB, GL_UNSIGNED_BYTE, data);

			printf("Loaded %s (%d)\n", tex->getDiffuseName(), texName);
			texes.insert(pair<const char*, GLuint>(tex->getDiffuseName(), texName));

			//delete tex;
		}

		if (stream != NULL) {
			delete stream;
		}
	}

	DFFXmlConverter conv;
	conv.save(mesh, "test.xml");
}

void display(void) {
	printf("=== Display ===\n");

	//glDisable(GL_BLEND);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	//glColor3f(1.0f, 0.0f, 0.0f);

	//glPolygonMode(GL_FRONT_AND_BACK, polyMode);

	//glEnableClientState(GL_VERTEX_ARRAY);

	//glLoadIdentity();

	glBindTexture(GL_TEXTURE_2D, texName);

	//glScalef(5.0f, 5.0f, 5.0f);

	/*glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-2.0, -1.0, 0.0);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-2.0, 1.0, 0.0);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.0, 1.0, 0.0);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.0, -1.0, 0.0);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(1.0, -1.0, 0.0);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(1.0, 1.0, 0.0);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(2.41421, 1.0, -1.41421);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(2.41421, -1.0, -1.41421);
	glEnd();*/

	//glBindTexture(GL_TEXTURE_2D, texName);
	//glutSolidSphere(5.0, 30, 20);

	glPushMatrix();

	glScalef(scale, scale, scale);
	glRotatef(rotX, 0.0f, 1.0f, 0.0f);
	glRotatef(rotX, 0.0f, 0.0f, 1.0f);
	glRotatef(rotY, 1.0f, 0.0f, 0.0f);


	DFFGeometry** geoms = mesh->getGeometries();

	for (int32_t i = 0 ; i < mesh->getGeometryCount() ; i++) {
		if (i != geomIdx) {
			//continue;
		}

		DFFGeometry* geom = geoms[i];

		/*printf("################### %s\n", geom->getAssociatedFrame()->getName());

		if (strstr(geom->getAssociatedFrame()->getName(), "dam") != NULL) {
			printf("***** %s contains LOD\n", geom->getAssociatedFrame()->getName());
			continue;
		}*/

		if (	strcmp(geom->getAssociatedFrame()->getName(), "chassis") != 0
		) {
			continue;
		}

		if (geom->getVertices() == NULL) {
			continue;
		}

		if (geom->getUVCoordSets() == NULL) {
			printf("Has no UV coords!\n");
		} else {
			printf("Has UV coords!\n");
		}

		//glBindTexture(GL_TEXTURE_2D, texNames[i]);

		float* verts = geom->getVertices();
		float* uv = NULL;

		if (geom->getUVCoordSets() != NULL) {
			uv = geom->getUVCoordSet(0);
		}

		//printf("Rendering texture %d\n", i);

		//glVertexPointer(3, GL_FLOAT, 0, geom->getVertices());
		//glTexCoordPointer(2, GL_FLOAT, 0, geom->getUVCoordSet(0));

		glPushMatrix();

		const float* trans = geom->getAssociatedFrame()->getTranslation();
		printf("Translation: %f %f %f\n", trans[0], trans[1], trans[2]);
		glTranslatef(trans[0], trans[1], trans[2]);



		DFFGeometryPart** parts = geom->getParts();
		for (int32_t j = 0 ; j < geom->getPartCount() ; j++) {
			DFFGeometryPart* part = parts[j];
			int32_t* indices = part->getIndices();

			DFFMaterial* mat = part->getMaterial();

			if (mat->getTextureCount() > 0) {
				DFFTexture* tex = mat->getTextures()[0];
				//printf("Binding %s\n", tex->getDiffuseName());
				GLuint texName = texes.find(tex->getDiffuseName())->second;
				glBindTexture(GL_TEXTURE_2D, texName);
				//printf("Apply %s (%d) to part %d\n", tex->getDiffuseName(), texName, j);
			} else {
				//GLuint texName = texes.find("vehiclelights128")->second;
				//glBindTexture(GL_TEXTURE_2D, texName);
				//printf("Part %d has no texture\n", j);
			}

			if (geom->isTriangleStripFormat()) {
				glBegin(GL_TRIANGLE_STRIP);
			} else {
				glBegin(GL_TRIANGLES);
			}

			for (int32_t k = 0 ; k < part->getIndexCount() ; k++) {
				int idx = indices[k];

				if (geom->getUVCoordSets() != NULL) {
					float cx = uv[idx*2];
					float cy = uv[idx*2 + 1];
					glTexCoord2f(cx, cy);
				}

				glVertex3f(verts[idx*3], verts[idx*3 + 1], verts[idx*3 + 2]);
			}

			glEnd();
		}

		glPopMatrix();
	}

	glPopMatrix();

	glFlush();
	glDisable(GL_TEXTURE_2D);
	glutSwapBuffers();

	printf("\n");
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
	//glOrtho(-50.0, 50.0, -50.0, 50.0, -1.0, 1.0);
	glOrtho(-50.0, 50.0, -50.0, 50.0, -500.0, 500.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 50.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}


void kbd(unsigned char c, int x, int y) {
	switch (c) {
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

void motion(int x, int y) {
	float motX = lastX-x;
	float motY = lastY-y;

	rotX += 0.1f * motX;
	if (rotX > 360.0f) rotX -= 360.0f;

	rotY += 0.1f * motY;
	if (rotY > 360.0f) rotY -= 360.0f;

	lastX = x;
	lastY = y;

	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {
		lastX = x;
		lastY = y;
		glutMotionFunc(motion);
	} else {
		glutMotionFunc(NULL);
	}
}

int main(int argc, char** argv) {
	//img = new IMGArchive("/home/alemariusnexus/GTA/GTA/Grand Theft Auto San Andreas/models/gta3.img");

	/*DFFLoader dff;
	dff.setVerbose(true);

	//ifstream stream("/home/alemariusnexus/infernus.dff", ifstream::in | ifstream::binary);

	char dffName[32];
	sprintf(dffName, "%s.dff", modelName);

	//istream* stream = img->gotoEntry(dffName);
	mesh = dff.loadMesh(stream);
	mesh->mirrorYZ();*/

	/*texWidth = 256;
	texHeight = 256;

	texData = new GLubyte[texWidth * texHeight * 4];

	for (int16_t x = 0 ; x < texWidth ; x++) {
		for (int16_t y = 0 ; y < texHeight ; y++) {
			texData[y*texWidth + x] = 255;
			texData[y*texWidth + x + 1] = 255;
			texData[y*texWidth + x + 2] = 255;
			texData[y*texWidth + x + 3] = 255;
		}
	}*/




	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(kbd);
	glutKeyboardUpFunc(kbdup);
	glutMouseFunc(mouse);
	glutMainLoop();
	return 0;
}

