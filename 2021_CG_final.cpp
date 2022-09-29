#include <stdio.h>
#include <float.h>
#include <math.h>
#include <iostream>
#include <GL/glut.h>
/* Particles in a box */
#define MAX_NUM_PARTICLES 1000
#define INITIAL_NUM_PARTICLES 30
#define INITIAL_POINT_SIZE 5.0
#define INITIAL_SPEED 1.0

typedef int BOOL;
#define TRUE 1
#define FALSE 0
GLfloat WHITE[] = {1, 1, 1};
using namespace std;
int angleX, angleY, angleZ = 0;
unsigned int cubeMode = 0, spinMode = 0;
float spin = 0.0, delay = 50;
int scale = 4, light = 0, move = 1;
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;

typedef struct particle
{
	int color;		   /*²É¤lªºÃC¦â*/
	float position[3]; /*²É¤lªº¤Tºû®y¼Ð(X, Y, Z)*/
	float velocity[3]; /*²É¤lªº³t«×(dX, dY, dZ)*/
	float mass;		   /*²É¤lªº½è¶q*/
} particle;

particle particles[MAX_NUM_PARTICLES]; /* «Å§i²É¤l¨t²Î */

/* ªì©l¤Æ²É¤l¨t²Î */
int present_time;
int last_time;
int num_particles = INITIAL_NUM_PARTICLES;		// ªì©l¤Æ²É¤l¼Æ¶q¬°30
float point_size = INITIAL_POINT_SIZE;			// ªì©l¤Æ²É¤l¤j¤p¬° 5
float speed = INITIAL_SPEED;					// ªì©l¤Æ²É¤l³t«×±`¼Æ¬°1.0
bool gravity = FALSE;							// ªì©l¤Æ­«¤O(Ãö³¬)
bool elastic = FALSE;							// ªì©l¤Æ«D§¹¥þ¼u©Ê¸I¼²(Ãö³¬) */
bool repulsion = FALSE;							/* ªì©l¤Æ±Æ¥¸(Ãö³¬) */
float platform = 0.5;							/* ªì©l¤Æ¶ZÂ÷ */
float coef = 1.0;								/* ªì©l¤Æ²É¤l¼u©Ê«Y¼Æ(1.0 = §¹¥þ¼u©Ê¸I¼²) */
float d2[MAX_NUM_PARTICLES][MAX_NUM_PARTICLES]; /* ²É¤l¶¡¸I¼²³t«×¦V¶q */
void init(void);

/* «Ø¥ß¤@²ÕÃC¦â¯Á¤Þ(¶Â, ¬õ, ºñ, ÂÅ, «C, µµ, ¶À, ¥Õ) */
GLfloat colors[8][3] = {{0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 1.0, 1.0}, {1.0, 0.0, 1.0}, {1.0, 1.0, 0.0}, {1.0, 1.0, 1.0}};

void myIdle();

// void main_menu(int);
void collision(int);
float forces(int, int);

int myrandom(int m)
{
	return rand() % m;
} //random background color 0>=,<=225
#define NFACE 6
#define NVERT 8
void drawColorfulCube(GLdouble x0, GLdouble x1, GLdouble y0, GLdouble y1,
					  GLdouble z0, GLdouble z1)
{
	static GLfloat v[8][3];
	static GLfloat c[8][4] = {
		{0.0, 0.0, 0.0, 1.0}, {1.0, 0.0, 0.0, 1.0}, {0.0, 1.0, 0.0, 1.0}, {1.0, 1.0, 0.0, 1.0}, {0.0, 0.0, 1.0, 1.0}, {1.0, 0.0, 1.0, 1.0}, {0.0, 1.0, 1.0, 1.0}, {1.0, 1.0, 1.0, 1.0}};

	/*  indices of front, top, left, bottom, right, back faces  */
	static GLubyte indices[NFACE][4] = {
		{4, 5, 6, 7}, {2, 3, 7, 6}, {0, 4, 7, 3}, {0, 1, 5, 4}, {1, 5, 6, 2}, {0, 3, 2, 1}};

	v[0][0] = v[3][0] = v[4][0] = v[7][0] = x0;
	v[1][0] = v[2][0] = v[5][0] = v[6][0] = x1;
	v[0][1] = v[1][1] = v[4][1] = v[5][1] = y0;
	v[2][1] = v[3][1] = v[6][1] = v[7][1] = y1;
	v[0][2] = v[1][2] = v[2][2] = v[3][2] = z0;
	v[4][2] = v[5][2] = v[6][2] = v[7][2] = z1;
	//©w¸q°}¦C¼Æ­È«á
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	//«ü©w¨C­Ó³»ÂIªº®y¼Ð
	glVertexPointer(3, GL_FLOAT, 0, v);
	//«ü©w¦â±m®y¼Ð
	glColorPointer(4, GL_FLOAT, 0, c);
	//´£¨údata´è¬Vª«¥ó¦â±m
	glDrawElements(GL_QUADS, NFACE * 4, GL_UNSIGNED_BYTE, indices);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}
void drawNormalCube(float x0, float y0, float x1, float y1, float z0)
{

	//float x0 = -1.0, y0 = -1, x1 = 1, y1 = 1, z0 = 1;
	float face[6][4][3] = {
		{{x0, y0, z0}, {x1, y0, z0}, {x1, y1, z0}, {x0, y1, z0}},	  //front
		{{x0, y1, -z0}, {x1, y1, -z0}, {x1, y0, -z0}, {x0, y0, -z0}}, //back
		{{x1, y0, z0}, {x1, y0, -z0}, {x1, y1, -z0}, {x1, y1, z0}},	  //right
		{{x0, y0, z0}, {x0, y1, z0}, {x0, y1, -z0}, {x0, y0, -z0}},	  //left
		{{x0, y1, z0}, {x1, y1, z0}, {x1, y1, -z0}, {x0, y1, -z0}},	  //top
		{{x0, y0, z0}, {x0, y0, -z0}, {x1, y0, -z0}, {x1, y0, z0}}	  //bottom
	};

	//rotate along z-axis
	glScalef(0.5, 0.5, 0.5);
	glBegin(GL_QUADS);
	glEnable(GL_LIGHTING);
	//front
	glColor3f(1.0, 0.0, 0.0);
	glVertex3fv(face[0][0]);
	glVertex3fv(face[0][1]);
	glVertex3fv(face[0][2]);
	glVertex3fv(face[0][3]);

	//back
	glColor3f(0.0, 1.0, 0.0);
	glVertex3fv(face[1][0]);
	glVertex3fv(face[1][1]);
	glVertex3fv(face[1][2]);
	glVertex3fv(face[1][3]);
	//right
	glColor3f(0.0, 0.0, 1.0);
	glVertex3fv(face[2][0]);
	glVertex3fv(face[2][1]);
	glVertex3fv(face[2][2]);
	glVertex3fv(face[2][3]);
	//left
	glColor3f(1.0, 1.0, 0.0);
	glVertex3fv(face[3][0]);
	glVertex3fv(face[3][1]);
	glVertex3fv(face[3][2]);
	glVertex3fv(face[3][3]);
	//top
	glColor3f(0.0, 1.0, 1.0);
	glVertex3fv(face[4][0]);
	glVertex3fv(face[4][1]);
	glVertex3fv(face[4][2]);
	glVertex3fv(face[4][3]);
	//bottom
	glColor3f(0.5, 0.0, 1.0);
	glVertex3fv(face[5][0]);
	glVertex3fv(face[5][1]);
	glVertex3fv(face[5][2]);
	glVertex3fv(face[5][3]);
	glShadeModel(GL_SMOOTH);
	glEnd();

	glFlush();
}
//x0 = -1, y0 = -1, x1 = 0, y1 = 0, z0 = 1;
//x0 = 0, y0 =0, x1 = 1, y1 = 1, z0 = 1;
//x0 = 1, y0 =-1, x1 = 2, y1 = 0, z0 = 1;
//x0 = 0, y0 = -2, x1 = 1, y1 = -1, z0 = 1;

void drawMultCube(int move)
{

	float x0, y0, x1, y1, z0, z1;
	//rotate along z-axis
	glScalef(0.2, 0.2, 0.2);
	glBegin(GL_QUADS);
	glEnable(GL_LIGHTING);
	int count = 69;
	while (count > -1)
	{
		while ((count > 32 && count < 39) || (count > 45 && count < 52) || (count > 56 && count < 65))
		{
			count--;
		}
		int totalface = count / 13;
		if (totalface == 4 || totalface == 5)
		{
			if (totalface == 4)
			{
				x0 = 1.5;
			}
			else if (totalface == 5)
			{
				x0 = -2.5;
			}
			int counts = count % 13; //52-56 right 65-69 left
			if (counts == 0)
				y0 = 0.5, z0 = 1;
			else if (counts == 4)
				y0 = -1.5, z0 = -1;
			else if (counts == 3)
				y0 = -1.5, z0 = 1;
			else if (counts == 2)
				y0 = -0.5, z0 = 0;
			else if (counts == 1)
				y0 = 0.5, z0 = -1;
		}
		else if (totalface == 2 || totalface == 3)
		{
			if (totalface == 2)
			{
				y0 = 1.5;
			}
			else if (totalface == 3)
			{
				y0 = -2.5;
			}
			int counts = count % 13; //26-32 top 39-45 bottom
			if (counts == 0)
				x0 = -1.5, z0 = 1;
			else if (counts == 6)
				x0 = 0.5, z0 = -1;
			else if (counts == 5)
				x0 = -1.5, z0 = -1;
			else if (counts == 4)
				x0 = 1.5, z0 = 0;
			else if (counts == 3)
				x0 = -0.5, z0 = 0;
			else if (counts == 2)
				x0 = -2.5, z0 = 0.0;
			else if (counts == 1)
				x0 = 0.5, z0 = 1.0;
		}
		else if (totalface <= 1)
		{
			if (totalface == 0)
			{
				z0 = 2.0;
			}
			else if (totalface == 1)
			{
				z0 = -2.0;
			}
			int counts = count % 13; //0-12 front 13-25 back
			if (counts == 0)
				x0 = -2.5, y0 = -2.5;
			else if (counts == 12)
				x0 = -1.5, y0 = -1.5;
			else if (counts == 11)
				x0 = -2.5, y0 = -0.5;
			else if (counts == 10)
				x0 = -1.5, y0 = 0.5;
			else if (counts == 9)
				x0 = -2.5, y0 = 1.5;
			else if (counts == 8)
				x0 = -0.5, y0 = 1.5;
			else if (counts == 7)
				x0 = -0.5, y0 = -2.5;
			else if (counts == 6)
				x0 = 1.5, y0 = -2.5;
			else if (counts == 5)
				x0 = 1.5, y0 = 1.5;
			else if (counts == 4)
				x0 = -0.5, y0 = -0.5;
			else if (counts == 3)
				x0 = 0.5, y0 = 0.5;
			else if (counts == 2)
				x0 = 1.5, y0 = -0.5;
			else if (counts == 1)
				x0 = 0.5, y0 = -1.5;
		}
		x0 = x0 * move;
		y0 = y0 * move;
		z0 = z0 * move;
		x1 = x0 + 1;
		y1 = y0 + 1;
		z1 = z0 - 1;
		float face[6][4][3] = {
			{{x0, y0, z0}, {x1, y0, z0}, {x1, y1, z0}, {x0, y1, z0}}, //front
			{{x0, y1, z1}, {x1, y1, z1}, {x1, y0, z1}, {x0, y0, z1}}, //back
			{{x1, y0, z0}, {x1, y0, z1}, {x1, y1, z1}, {x1, y1, z0}}, //right
			{{x0, y0, z0}, {x0, y1, z0}, {x0, y1, z1}, {x0, y0, z1}}, //left
			{{x0, y1, z0}, {x1, y1, z0}, {x1, y1, z1}, {x0, y1, z1}}, //top
			{{x0, y0, z0}, {x0, y0, z1}, {x1, y0, z1}, {x1, y0, z0}}  //bottom
		};

		//front
		glColor3f(1.0, 0.0, 0.0);
		glVertex3fv(face[0][0]);
		glVertex3fv(face[0][1]);
		glVertex3fv(face[0][2]);
		glVertex3fv(face[0][3]);

		//back
		glColor3f(0.0, 1.0, 0.0);
		glVertex3fv(face[1][0]);
		glVertex3fv(face[1][1]);
		glVertex3fv(face[1][2]);
		glVertex3fv(face[1][3]);
		//right
		glColor3f(0.0, 0.0, 1.0);
		glVertex3fv(face[2][0]);
		glVertex3fv(face[2][1]);
		glVertex3fv(face[2][2]);
		glVertex3fv(face[2][3]);
		//left
		glColor3f(1.0, 1.0, 0.0);
		glVertex3fv(face[3][0]);
		glVertex3fv(face[3][1]);
		glVertex3fv(face[3][2]);
		glVertex3fv(face[3][3]);
		//top
		glColor3f(0.0, 1.0, 1.0);
		glVertex3fv(face[4][0]);
		glVertex3fv(face[4][1]);
		glVertex3fv(face[4][2]);
		glVertex3fv(face[4][3]);
		//bottom
		glColor3f(0.5, 0.0, 1.0);
		glVertex3fv(face[5][0]);
		glVertex3fv(face[5][1]);
		glVertex3fv(face[5][2]);
		glVertex3fv(face[5][3]);
		count--;
	}
	glShadeModel(GL_SMOOTH);
	glEnd();

	glFlush();
}
GLfloat vertices[][3] = {{-1.0, -1.0, -1.0}, {1.0, -1.0, -1.0}, {1.0, 1.0, -1.0}, {-1.0, 1.0, -1.0}, {-1.0, -1.0, 1.0}, {1.0, -1.0, 1.0}, {1.0, 1.0, 1.0}, {-1.0, 1.0, 1.0}};
GLfloat color[][4] = {{0.0, 0.0, 0.0, 0.5}, {1.0, 0.0, 0.0, 0.5}, {1.0, 1.0, 0.0, 0.5}, {0.0, 1.0, 0.0, 0.5}, {0.0, 0.0, 1.0, 0.5}, {1.0, 0.0, 1.0, 0.5}, {1.0, 1.0, 1.0, 0.5}, {0.0, 1.0, 1.0, 0.5}};

void polygon(int a, int b, int c, int d)
{
	glBegin(GL_POLYGON);
	glColor4fv(color[a]);
	glTexCoord2f(0.0, 0.0);
	glVertex3fv(vertices[a]);
	glColor4fv(color[b]);
	glTexCoord2f(0.0, 1.0);
	glVertex3fv(vertices[b]);
	glColor4fv(color[c]);
	glTexCoord2f(1.0, 1.0);
	glVertex3fv(vertices[c]);
	glColor4fv(color[d]);
	glTexCoord2f(1.0, 0.0);
	glVertex3fv(vertices[d]);
	glEnd();
}
void colorcube()
{
	/* map vertices to faces */

	polygon(0, 3, 2, 1);
	polygon(2, 3, 7, 6);
	polygon(0, 4, 7, 3);
	polygon(1, 2, 6, 5);
	polygon(4, 5, 6, 7);
	polygon(0, 1, 5, 4);
} //?½è‰²
GLfloat RED[] = {1, 0, 0};

class Camera
{
public:
	Camera() : theta(0), y(3), dTheta(0.04), dy(0.2), lenth(0.5) {}
	double getX() { return lenth * 10 * cos(theta); }
	double getY() { return lenth * y; }
	double getZ() { return lenth * 10 * sin(theta); }
	void init()
	{
		theta = 0;
		y = 3;
		dTheta = 0.04;
		dy = 0.2;
		lenth = 0.5;
	}
	void moveNear() { lenth = lenth * 1.05; }
	void moveFar() { lenth = lenth * 0.95; }
	void moveRight() { theta += dTheta; }
	void moveLeft() { theta -= dTheta; }
	void moveUp() { y += dy; }
	void moveDown()
	{
		if (y > dy)
			y -= dy;
	}

private:
	double theta;
	double y;
	double dTheta;
	double dy;
	double lenth;
};
Camera camera;
void thiswillDrawCube(GLint mode)
{

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//glPushMatrix();
	glRotatef(35, 0.0, 0.0, 0.0);
	glRotatef(spin, 0.0, 0.0, 0.0);
	glRotatef(angleX, 1.0, 0.0, 0.0); //rotate the cube along x-axis
	glRotatef(angleY, 0.0, 1.0, 0.0); //rotate along y-axis
	glRotatef(angleZ, 0.0, 0.0, 1.0);
	if (mode != 1)
	{
		scale = 4;
		GLubyte image[0][0][3];
		int i, j, c;
		for (i = 0; i < 0; i++) //texture
		{
			for (j = 0; j < 0; j++)
			{
				c = ((((i & 0x4) == 0) ^ ((j & 0x4)) == 0)) * 120;
				image[i][j][0] = (GLubyte)c;
				image[i][j][1] = (GLubyte)c - 30;
				image[i][j][2] = (GLubyte)c - 30;
			}
		}
		if (mode == 0)
			drawNormalCube(-1, -1, 1, 1, 1);
		else if (mode == 2)
			drawMultCube(move);
		else if (mode == 3)
		{
			drawColorfulCube(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5);
		}
		else if (mode == 4)
		{
			glScalef(0.5, 0.5, 0.5);
			colorcube();
		}
		glEnable(GL_TEXTURE_2D);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, 0, 0, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	}
	else if (mode == 1)
	{
		glScalef(0.5, 0.5, 0.5);
		GLubyte image[64][64][3];
		int i, j, c;
		for (i = 0; i < 64; i++) //texture
		{
			for (j = 0; j < 64; j++)
			{
				c = ((((i & scale) == 0) ^ ((j & scale)) == 0)) * 120;
				image[i][j][0] = (GLubyte)c;
				image[i][j][1] = (GLubyte)c - 30;
				image[i][j][2] = (GLubyte)c - 30;
			}
		}
		colorcube();
		glEnable(GL_TEXTURE_2D);
		//»P°}¦C¤j¤p¬Û¦Pªº°Ñ¼Æ
		glTexImage2D(GL_TEXTURE_2D, 0, 3, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	}
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}
void lightControl()
{
	GLfloat light_position[] = {2.0f, 1.0f, 1.0f, 1.0f}; //¥ú·½ªº¦ì¸m¦b¥@¬É®y¼Ð¨t¶ê¤ß¡A»ô¦¸®y¼Ð§Î¦¡
	GLfloat light_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};	 //RGBA¼Ò¦¡ªºÀô¹Ò¥ú¡A
	GLfloat light_diffuse[] = {0.9f, 1.0f, 1.0f, 1.0f};	 //RGBA¼Ò¦¡ªºº©¤Ï®g¥ú
	GLfloat light_specular[] = {0.9f, 1.0f, 1.0f, 1.0f}; //RGBA¼Ò¦¡¤UªºÃè­±¥ú
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	//¶}±Ò¿O¥ú
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	GLfloat mat_ambient[] = {0.0f, 0.0f, 1.0f, 1.0f};  //©w¸qª«ÅéªºÀô¹Ò¥úÃC¦â
	GLfloat mat_diffuse[] = {0.0f, 0.0f, 0.5f, 1.0f};  //©w¸qª«Åéªºº©¤Ï®g¥úÃC¦â
	GLfloat mat_specular[] = {1.0f, 1.0f, 1.0f, 1.0f}; //©w¸qª«ÅéªºÃè­±¤Ï®g¥úÃC¦â
	GLfloat mat_emission[] = {0.0f, 0.0f, 0.0f, 1.0f}; //©w¸qª«Åéªº¿ç®g¥úÃC¦â
	GLfloat mat_shininess = 4.0f;
	glBegin(GL_QUADS);

	glNormal3d(0, 1, 0);
	for (int x = 0; x < 8 - 1; x++)
	{
		for (int z = 0; z < 8 - 1; z++)
		{

			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE,
						 (x + z) % 2 == 0 ? RED : WHITE);
			glVertex3d(x, -1, z);
			glVertex3d(x + 1, -1, z);
			glVertex3d(x + 1, -1, z + 1);
			glVertex3d(x, -1, z + 1);
		}
	}
	glEnd();
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
	glTranslatef(7.0f, 0.0f, 0.0f);
}
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glLoadIdentity();
	gluLookAt(camera.getX(), camera.getY(), camera.getZ(),
			  0.0, 0.0, 0.0,
			  0.0, 1.0, 0.0);
	if (light == 1)
	{
		lightControl();
	}
	glBegin(GL_POINTS); /* render all particles */
	for (int i = 0; i < num_particles; i++)
	{
		glColor3fv(colors[particles[i].color]);
		glVertex3fv(particles[i].position);
	}
	glEnd();
	glColor3f(0.0, 0.0, 0.0);
	thiswillDrawCube(cubeMode);

	glPopMatrix();
	glutSwapBuffers();
}
void autospinDisplay(int n)
{
	//·íspinMode¬°1®É¦Û°Ê±ÛÂà
	if (spinMode == 1)
	{
		if (angleY > 360)
			angleY = 0;
		else
			angleY += 1;
		glutPostRedisplay();
		if (n > 100)
			n = 1;
		//¥Î©ó³]©w©w®É¾¹ªº¦^¼·¨ç¦¡,delay³]©w¬°100
		else
			glutTimerFunc(delay, autospinDisplay, n);
		n++;
	}
}
void spindDisplay()
{
	spin = spin + 5;
	if (spin > 360) //when overflow
		spin = spin - 360;
	glutPostRedisplay();
} //spin display

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	//[a,d,w,s],[<,>,^,v]->±±¨îµø¨¤//[x]¥Hx¶b±ÛÂà,[y]¥Hy¶b±ÛÂà,[z]¥Hz¶b±ÛÂà
	//¤j¤p¼g¬Ò¥i
	case 'a':
		camera.moveLeft();
		break;
	case 'd':
		camera.moveRight();
		break;
	case 'w':
		camera.moveUp();
		break;
	case 's':
		camera.moveDown();
		break;
	case 'x':
		angleX = (angleX + 10) % 360;
		break;
	case 'y':
		angleY = (angleY + 10) % 360;
		break;
	case 'z':
		angleZ = (angleZ + 10) % 360;
		break;
	case 'A':
		angleX -= 5;
		break;
	case 'D':
		angleX += 5;
		break;
	case 'W':
		angleY -= 5;
		break;
	case 'S':
		angleY += 5;
		break;
	case 'X':
		angleX = (angleX - 10) % 360;
		break;
	case 'Y':
		angleY = (angleY - 10) % 360;
		break;
	case 'Z':
		angleZ = (angleZ - 10) % 360;
		break;
	//auto spinning
	//[r]±±¨î¦Û°Ê±ÛÂà ,[r]reset, [c]§ó§ïmode
	case 'r':
		spinMode = spinMode ^ (1 << 0); // §ó§ïª¬ºA
		if (spinMode == 1)
		{
			//·í±ÛÂà®É­I´ºÀH¾÷ÅÜ´«
			glClearColor(0, .0, .0, .0);
			cout << "Start Spinning ";
		}
		else if (spinMode == 0)
		{
			glClearColor(0.0, 0.0, 0.0, 0.0);
			cout << "Stop Spinning";
		}
		cout << endl;
		autospinDisplay(spinMode);

		break;
	//reset
	case 'q':
		cout << "Reset" << endl;
		light = 0;
		camera.init();
		glClearColor(0, .0, 0, .0);
		cubeMode = 0;
		spinMode = 0;
		spin = 0;
		angleX = 0;
		angleY = 0;
		angleZ = 0;
		platform = 0.5;
		display();
		autospinDisplay(spinMode);
		break;
	//change mode  0= Normal Cube , 1 = Colorful Cube
	case 'o':
		cubeMode = 0;
		platform = 0.5;
		cout << "Cube Mode : Normal Cube" << endl;
		display();
		break;
	case 'c':
		cubeMode = 1;
		platform = 0.5;
		scale = 4;
		cout << "Cube Mode : Color Cube" << endl;
		display();
		break;
	case 'm':
		cubeMode = 2;
		move = 1;
		cout << cubeMode << endl;
		cout << "Cube Mode : Mult shape" << endl;
		display();
		break;
	case 'l': //Spacebar
		if (light == 0)
			cout << "Light OFF";
		else if (light == 1)
			cout << "Light ON";
		light = light ^ (1 << 0);

		cout << endl;
		display();
	}
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) //mouse function...
{
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			if (cubeMode == 2)
			{
				move++;

				platform = platform * 1.5;
			}
		}
		//       glClearColor((double)myrandom(255)/255, (double)myrandom(255)/255, (double)myrandom(255)/255, (double)myrandom(255)/255);
		//       glutIdleFunc(spindDisplay);
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{
			if (cubeMode == 2)
			{
				if (move > 1)
					move--;
				if (platform >= 0.5)
					platform = platform * 0.75;
			}
			//    	glClearColor(0, .0, 0, .0);
			//		glutIdleFunc(NULL);
		}
		break;
	case 3: //wheel up
		if (state == GLUT_UP)
			camera.moveNear();
		break;
	case 4: //wheel down
		if (state == GLUT_UP)
			camera.moveFar();
		break;
	default:
		break;
	}
}
//·í¦³¤W¤U¥ª¥kÁä¿é¤J®É¡A½Õ¾ã¨¤«×¡A
// void SpecialKeys(int key, int x, int y)
// {
// 	switch (key)
// 	{
// 	case GLUT_KEY_LEFT:
// 		if (cubeMode == 1 || cubeMode == 4)
// 		{
// 			cubeMode = 3;
// 			display();
// 		}
// 		break;
// 	case GLUT_KEY_RIGHT:
// 		if (cubeMode == 1 || cubeMode == 3)
// 		{
// 			cubeMode = 4;
// 			display();
// 		}
// 		break;
// 	case GLUT_KEY_UP:
// 		scale = scale * 2;
// 		break;
// 	case GLUT_KEY_DOWN:
// 		if (scale == 0)
// 			scale = 2;
// 		scale = scale / 2;
// 		break;

// 	default:
// 		cout << "Special key " << key << " == " << key << endl;
// 	}
// 	glutPostRedisplay();
// }
void main_menu(int index)
{
	switch (index)
	{
	case (1): // 2­¿²É¤l¼Æ¶q 
		num_particles = 2 * num_particles;
		init();
		break;
	case (2): // 1/2­¿²É¤l¼Æ¶q 
		num_particles = num_particles / 2;
		init();
		break;
	case (3): // 2­¿²É¤l³t«×
		speed = 2.0 * speed;
		init();
		break;
	case (4): // 1/2­¿²É¤l³t«× 
		speed = speed / 2.0;
		init();
		break;
	case (5): // 2­¿²É¤l¤j¤p
		point_size = 2.0 * point_size;
		init();
		break;
	case (6): // 1/2­¿²É¤l¤j¤p 
		point_size = point_size / 2.0;
		if (point_size < 1.0)
			point_size = 1.0;
		init();
		break;
	case (7): //­«¤O¤Á´« 
		gravity = !gravity;
		init();
		break;
	case (8): // ²É¤l«D¼u©Ê¸I¼²¤Á´« 
		elastic = !elastic;
		if (elastic)
			coef = 0.9;
		else
			coef = 1.0;
		init();
		break;
	case (9): // ²É¤l¶¡¸I¼²¤Á´« 
		repulsion = !repulsion;
		init();
		break;
	case (10): // 2­¿¾B¸n¤j¤p 
		scale = scale * 2;
		break;
	case (11):	//1/2­¿¾B¸n¤j¤p 
		if (scale == 0)
			scale = 2;
		scale = scale / 2;
		break;
	case (12):	// 2­¿ª«Åé¶¡¶Z 
		if (cubeMode == 2)
		{
			move++;

			platform = platform * 1.5;
		}
		break;
	case (13):	//1/2­¿ª«Åé¶¡¶Z 
		if (cubeMode == 2)
		{
			if (move > 1)
				move--;
			if (platform >= 0.5)
				platform = platform * 0.75;
		}
		break;
	case (14):
		exit(0);
		break;
	}
}

void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	int i, j;

	for (i = 0; i < num_particles; i++)
	{ // ªì©l¤Æ²É¤l½è¶q, ÃC¦â, ®y¼Ð, ³t«× 
		particles[i].mass = 2.0;
		particles[i].color = i % 8;
		for (j = 0; j < 3; j++)
		{
			//¦ì¸m¥H³t«×§ó·s
			//³t«×³q¹L­pºâ¸Ó²É¤l¤Wªº¤O§ó·s 
			particles[i].position[j] = 0.5 * platform * ((float)rand() / RAND_MAX) - 0.5 * platform;
			particles[i].velocity[j] = speed * 1.0 * ((float)rand() / RAND_MAX) - 1.0;
		}
	}
	glPointSize(point_size); // ³]©w²É¤l¤j¤p 
	glEnable(GL_DEPTH_TEST);
}

void myIdle()
{
	int i, j, k;
	float dt;
	present_time = glutGet(GLUT_ELAPSED_TIME);
	dt = 0.001 * (present_time - last_time); //¨C³æ¦ì§ó·s®É¶¡ 
	for (i = 0; i < num_particles; i++)
	{
		for (j = 0; j < 3; j++)
		{
			//®Ú¾Ú§@¥Î¦b²É¤l¤Wªº¤O§ó·sª¬ºA 
			particles[i].position[j] += dt * particles[i].velocity[j];
			particles[i].velocity[j] += dt * forces(i, j) / particles[i].mass;
		}
		collision(i);
	}
	if (repulsion)	//²É¤l¤§¶¡¸I¼² 
		for (i = 0; i < num_particles; i++)
			for (k = 0; k < i; k++)
			{
				d2[i][k] = 0.0;
				//¸I¼²«áª¬ºA§ó·s 
				for (j = 0; j < 3; j++)
					d2[i][k] += (particles[i].position[j] - particles[k].position[j]) *
								(particles[i].position[j] - particles[k].position[j]);
				d2[k][i] = d2[i][k];
			}
	last_time = present_time;
	glutPostRedisplay();
}
float forces(int i, int j)
{
	int k;
	float force = 0.0;
	if (gravity && j == 1)
		force = -1.0; // ¤è¦V¦V¤U1³æ¦ìªº­«¤O 
	if (repulsion)
		for (k = 0; k < num_particles; k++)
		{ // ­pºâ²É¤l¤§¶¡ªº¸I¼²
			if (k != i)
				force += 0.001 * (particles[i].position[j] - particles[k].position[j]) / (0.001 + d2[i][k]);
		}
	return (force);
}

void collision(int n)
{
	// ¸I¼²´ú¸Õ»P¸I¼²¤Ï®g 
	int i;
	for (i = 0; i < 3; i++)
	{
		if (particles[n].position[i] >= 0.5 * platform)
		{
			particles[n].velocity[i] = -coef * particles[n].velocity[i];
			particles[n].position[i] = 0.5 * platform - coef * (particles[n].position[i] - 0.5 * platform);
		}
		if (particles[n].position[i] <= -0.5 * platform)
		{
			particles[n].velocity[i] = -coef * particles[n].velocity[i];
			particles[n].position[i] = -0.5 * platform - coef * (particles[n].position[i] + 0.5 * platform);
		}
	}
}

void reshape(int w, int h)
{

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 30.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camera.getX(), camera.getY(), camera.getZ(),
			  0.0, 0.0, 0.0,
			  0.0, 1.0, 0.0);
}

int main(int argc, char **argv)
{
	cout << "¾Þ§@»¡©ú:" << endl
		 << "[a, d, w, s]  -> ±±¨îµø¨¤" << endl
		 << "[x]- > ¥Hx¶b±ÛÂà, [y] -> ¥Hy¶b±ÛÂà, [z] -> ¥Hz¶b±ÛÂà" << endl
		 << "[r] -> ±±¨î¦Û°Ê±ÛÂà , [q] -> reset, [c] -> colorful, [m] -> Mult, [o] -> Normal" << endl
		 << "[l] -> ¶}±Ò¿O¥ú" << endl
		 << "(»Ý¦bcªº¼Ò¦¡¤U¾Þ±±)" << endl
		 << "\t[<, >] -> ±±¨î¦â±m´è¬V¤è¦V" << endl
		 << "[·Æ¹«¥kÁä] -> ¶}°_¾Þ§@¿ï³æ " << endl
		 << "[·Æ¹«ºu½ü] -> ±±¨î»·ªñµø¨¤" << endl;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(80, 80);
	glutInitWindowSize(800, 600); //µøµ¡¤j¤p
								  //  	glutInitWindowSize(600, 600)
								  //  	glutInitWindowPosition(75, 50);
	glutCreateWindow("3D CUBE");
	init();

	glutCreateMenu(main_menu); // «Ø¥ß¾Þ§@¿ï³æ
	glutAddMenuEntry("more particles", 1);
	glutAddMenuEntry("fewer particles", 2);
	glutAddMenuEntry("faster", 3);
	glutAddMenuEntry("slower", 4);
	glutAddMenuEntry("larger particles", 5);
	glutAddMenuEntry("smaller particles", 6);
	glutAddMenuEntry("toggle gravity", 7);
	glutAddMenuEntry("toggle restitution", 8);
	glutAddMenuEntry("toggle repulsion", 9);
	glutAddMenuEntry("[C mode] mask texture bigger", 10);
	glutAddMenuEntry("[C mode] mask texture smaller", 11);
	glutAddMenuEntry("[M mode] object space bigger", 12);
	glutAddMenuEntry("[M mode] object space smaller", 13);
	glutAddMenuEntry("quit", 14);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	//	glutSpecialFunc(SpecialKeys);	   //³B²z¤W¤U¥ª¥kÁä¿é¤J(¬°special key)
	glutDisplayFunc(display); //¥Î©óÃ¸¹Ï
	glutIdleFunc(myIdle);
	glutReshapeFunc(reshape);	//§ïÅÜµøµ¡¤j¤p®É¡Aºû«ù¹Ï§Î¤ñ¨Ò
	glutKeyboardFunc(keyboard); // ³B²z´¶³q«öÁä¿é¤J
	glutMouseFunc(mouse);		//³B²z·Æ¹«¿é¤J
	glutMainLoop();
	return 0;
}

