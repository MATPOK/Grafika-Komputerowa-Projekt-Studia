// Gl_template.c
//Wy³šczanie b³êdów przed "fopen"
#define  _CRT_SECURE_NO_WARNINGS

// £adowanie bibliotek:
#ifdef _MSC_VER                         // Check if MS Visual C compiler
#  pragma comment(lib, "opengl32.lib")  // Compiler-specific directive to avoid manually configuration
#  pragma comment(lib, "glu32.lib")     // Link libraries
#endif

// Ustalanie trybu tekstowego:
#ifdef _MSC_VER        // Check if MS Visual C compiler
#   ifndef _MBCS
#      define _MBCS    // Uses Multi-byte character set
#   endif
#   ifdef _UNICODE     // Not using Unicode character set
#      undef _UNICODE 
#   endif
#   ifdef UNICODE
#      undef UNICODE 
#   endif
#endif


#include <windows.h>            // Window defines
#include <gl\gl.h>              // OpenGL
#include <gl\glu.h>             // GLU library
#include <math.h>				// Define for sqrt
#include <stdio.h>

#define glRGB(x, y, z)	glColor3ub((GLubyte)x, (GLubyte)y, (GLubyte)z)
#define BITMAP_ID 0x4D42		// identyfikator formatu BMP
#define GL_PI 3.14

// Color Palette handle
HPALETTE hPalette = NULL;

// Application name and instance storeage
static LPCTSTR lpszAppName = "GL Template";
static HINSTANCE hInstance;

// Rotation amounts
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;
static GLfloat xObs = 0.0f;
static GLfloat yObs = 0.0f;

static GLsizei lastHeight;
static GLsizei lastWidth;
static GLsizei lastX;
static GLsizei lastY;

// Opis tekstury
BITMAPINFOHEADER	bitmapInfoHeader;	// nag³ówek obrazu
unsigned char*		bitmapData;			// dane tekstury
unsigned int		texture[5];			// obiekt tekstury

static int licznik;

double rot1, rot2, rot3, rot4, rot5, rot6, rot7, rot8, move1, movekulaX=0, movekulaY=0, movekulaZ=0, anglekula = 0.0f, rot11, rot12, rot13, rot14, rot15, rot16;
int stop1=0, stop2=0, stop3=0, stop4=0, stop5 = 0, licznikpom;



										// Declaration for Window procedure
LRESULT CALLBACK WndProc(HWND    hWnd,
	UINT    message,
	WPARAM  wParam,
	LPARAM  lParam);

// Set Pixel Format function - forward declaration
void SetDCPixelFormat(HDC hDC);


// Reduces a normal vector specified as a set of three coordinates,
// to a unit normal vector of length one.
void ReduceToUnit(float vector[3])
{
	float length;

	// Calculate the length of the vector		
	length = (float)sqrt((vector[0] * vector[0]) +
		(vector[1] * vector[1]) +
		(vector[2] * vector[2]));

	// Keep the program from blowing up by providing an exceptable
	// value for vectors that may calculated too close to zero.
	if (length == 0.0f)
		length = 1.0f;

	// Dividing each element by the length will result in a
	// unit normal vector.
	vector[0] /= length;
	vector[1] /= length;
	vector[2] /= length;
}


// Points p1, p2, & p3 specified in counter clock-wise order
void calcNormal(float v[3][3], float out[3])
{
	float v1[3], v2[3];
	static const int x = 0;
	static const int y = 1;
	static const int z = 2;

	// Calculate two vectors from the three points
	v1[x] = v[0][x] - v[1][x];
	v1[y] = v[0][y] - v[1][y];
	v1[z] = v[0][z] - v[1][z];

	v2[x] = v[1][x] - v[2][x];
	v2[y] = v[1][y] - v[2][y];
	v2[z] = v[1][z] - v[2][z];

	// Take the cross product of the two vectors to get
	// the normal vector which will be stored in out
	out[x] = v1[y] * v2[z] - v1[z] * v2[y];
	out[y] = v1[z] * v2[x] - v1[x] * v2[z];
	out[z] = v1[x] * v2[y] - v1[y] * v2[x];

	// Normalize the vector (shorten length to one)
	ReduceToUnit(out);
}



// Change viewing volume and viewport.  Called when window is resized
void ChangeSize(GLsizei w, GLsizei h, GLsizei x, GLsizei y)
{
	GLfloat nRange = 1000;
	GLfloat fAspect;
	// Prevent a divide by zero
	if (h == 0)
		h = 1;


	lastWidth = w;
	lastHeight = h;
	lastX = x;
	lastY = y;

	fAspect = (GLfloat)w / (GLfloat)h;
	// Set Viewport to window dimensions
	glViewport(x, y, w+1200, h+500);

	// Reset coordinate system
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Establish clipping volume (left, right, bottom, top, near, far)
	if (w <= h)
		glOrtho(-nRange, nRange, -nRange*h / w, nRange*h / w, -nRange, nRange);
	else
		glOrtho(-nRange*w / h, nRange*w / h, -nRange, nRange, -nRange, nRange);

	// Establish perspective: 
	/*
	gluPerspective(60.0f,fAspect,1.0,400);
	*/

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}



// This function does any needed initialization on the rendering
// context.  Here it sets up and initializes the lighting for
// the scene.
void SetupRC()
{
	// Light values and coordinates
	GLfloat  ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat  diffuseLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat  specular[] = { 1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat	 lightPos[] = { 0.0f, 150.0f, 150.0f, 1.0f };
	GLfloat  specref[] =  { 1.0f, 1.0f, 1.0f, 1.0f };


	glEnable(GL_DEPTH_TEST);	// Hidden surface removal
	glFrontFace(GL_CCW);		// Counter clock-wise polygons face out
	glEnable(GL_CULL_FACE);		// Do not calculate inside of jet

								// Enable lighting
	glEnable(GL_LIGHTING);

								// Setup and enable light 0
	glLightfv(GL_LIGHT0,GL_AMBIENT,ambientLight);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseLight);
	glLightfv(GL_LIGHT0,GL_SPECULAR,specular);
	glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
	glEnable(GL_LIGHT0);

								// Enable color tracking
	glEnable(GL_COLOR_MATERIAL);

								// Set Material properties to follow glColor values
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

								// All materials hereafter have full specular reflectivity
								// with a high shine
	glMaterialfv(GL_FRONT, GL_SPECULAR,specref);
	glMateriali(GL_FRONT,GL_SHININESS,128);


								// White background
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	// Black brush
	glColor3f(0.0, 0.0, 0.0);
}

void skrzynka(void)
{
	glColor3d(0.8, 0.7, 0.3);


	glEnable(GL_TEXTURE_2D); // W³¹cz teksturowanie

	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_QUADS);
	glNormal3d(0, 0, 1);
	glTexCoord2d(1.0, 1.0); glVertex3d(25, 25, 25);
	glTexCoord2d(0.0, 1.0); glVertex3d(-25, 25, 25);
	glTexCoord2d(0.0, 0.0); glVertex3d(-25, -25, 25);
	glTexCoord2d(1.0, 0.0); glVertex3d(25, -25, 25);
	glEnd();
	
	
	glBegin(GL_QUADS);
	glNormal3d(1, 0, 0);
	glTexCoord2d(1.0, 1.0); glVertex3d(25, 25, 25);
	glTexCoord2d(0.0, 1.0); glVertex3d(25, -25, 25);
	glTexCoord2d(0.0, 0.0); glVertex3d(25, -25, -25);
	glTexCoord2d(1.0, 0.0); glVertex3d(25, 25, -25);
	glEnd();

	

	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_QUADS);
	glNormal3d(0, 0, -1);
	glTexCoord2d(1.0, 1.0); glVertex3d(25, 25, -25);
	glTexCoord2d(0.0, 1.0); glVertex3d(25, -25, -25);
	glTexCoord2d(0.0, 0.0); glVertex3d(-25, -25, -25);
	glTexCoord2d(1.0, 0.0); glVertex3d(-25, 25, -25);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3d(0, 0, 1);
	glTexCoord2d(1.0, 1.0); glVertex3d(25, -25, -25);
	glTexCoord2d(0.0, 1.0); glVertex3d(25, 25, -25);
	glTexCoord2d(0.0, 0.0); glVertex3d(-25, 25, -25);
	glTexCoord2d(1.0, 0.0); glVertex3d(-25, -25, -25);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3d(-1, 0, 0);
	glTexCoord2d(1.0, 1.0); glVertex3d(-25, 25, -25);
	glTexCoord2d(0.0, 1.0); glVertex3d(-25, -25, -25);
	glTexCoord2d(0.0, 0.0); glVertex3d(-25, -25, 25);
	glTexCoord2d(1.0, 0.0); glVertex3d(-25, 25, 25);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3d(1, 0, 0);
	glTexCoord2d(1.0, 1.0); glVertex3d(-25, -25, -25);
	glTexCoord2d(0.0, 1.0); glVertex3d(-25, 25, -25);
	glTexCoord2d(0.0, 0.0); glVertex3d(-25, 25, 25);
	glTexCoord2d(1.0, 0.0); glVertex3d(-25, -25, 25);
	glEnd();

	/*glNormal3d(0, 1, 0);
	glVertex3d(25, 25, 25);
	glVertex3d(25, 25, -25);
	glVertex3d(-25, 25, -25);
	glVertex3d(-25, 25, 25);*/

	glBegin(GL_QUADS);
	glNormal3d(0, -1, 0);
	glTexCoord2d(1.0, 1.0); glVertex3d(25, -25, 25);
	glTexCoord2d(0.0, 1.0); glVertex3d(-25, -25, 25);
	glTexCoord2d(0.0, 0.0); glVertex3d(-25, -25, -25);
	glTexCoord2d(1.0, 0.0); glVertex3d(25, -25, -25);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3d(0, 1, 0);
	glTexCoord2d(1.0, 1.0); glVertex3d(-25, -25, 25);
	glTexCoord2d(0.0, 1.0); glVertex3d(25, -25, 25);
	glTexCoord2d(0.0, 0.0); glVertex3d(25, -25, -25);
	glTexCoord2d(1.0, 0.0); glVertex3d(-25, -25, -25);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3d(-1, 0, 0);
	glTexCoord2d(1.0, 1.0); glVertex3d(25, -25, 25);
	glTexCoord2d(0.0, 1.0); glVertex3d(25, 25, 25);
	glTexCoord2d(0.0, 0.0); glVertex3d(25, 25, -25);
	glTexCoord2d(1.0, 0.0); glVertex3d(25, -25, -25);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3d(0, 0, -1);
	glTexCoord2d(1.0, 1.0); glVertex3d(-25, 25, 25);
	glTexCoord2d(0.0, 1.0); glVertex3d(25, 25, 25);
	glTexCoord2d(0.0, 0.0); glVertex3d(25, -25, 25);
	glTexCoord2d(1.0, 0.0); glVertex3d(-25, -25, 25);
	glEnd();

	glDisable(GL_TEXTURE_2D); // Wy³¹cz teksturowanie
	
	
}

void walec01(void)
{
	GLUquadricObj*obj;
	obj = gluNewQuadric();
	gluQuadricNormals(obj, GLU_SMOOTH);
	glColor3d(1, 0, 0);
	glPushMatrix();
	gluCylinder(obj, 20, 20, 30, 15, 7);
	gluCylinder(obj, 0, 20, 0, 15, 7);
	glTranslated(0, 0, 60);
	glRotated(180.0, 0, 1, 0);
	gluCylinder(obj, 0, 20, 30, 15, 7);
	glPopMatrix();
}

void kula(void)
{
	GLUquadricObj*obj;
	obj = gluNewQuadric();
	gluQuadricTexture(obj, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glColor3d(1.0, 0.8, 0.8);
	glEnable(GL_TEXTURE_2D);
	gluSphere(obj, 7, 15, 7);
	glDisable(GL_TEXTURE_2D);
}




// LoadBitmapFile
// opis: ³aduje mapê bitow¹ z pliku i zwraca jej adres.
//       Wype³nia strukturê nag³ówka.
//	 Nie obs³uguje map 8-bitowych.
unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
	FILE *filePtr;							// wskaŸnik pozycji pliku
	BITMAPFILEHEADER	bitmapFileHeader;		// nag³ówek pliku
	unsigned char		*bitmapImage;			// dane obrazu
	int					imageIdx = 0;		// licznik pikseli
	unsigned char		tempRGB;				// zmienna zamiany sk³adowych

												// otwiera plik w trybie "read binary"
	filePtr = fopen(filename, "rb");
	if (filePtr == NULL)
		return NULL;

	// wczytuje nag³ówek pliku
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);

	// sprawdza, czy jest to plik formatu BMP
	if (bitmapFileHeader.bfType != BITMAP_ID)
	{
		fclose(filePtr);
		return NULL;
	}

	// wczytuje nag³ówek obrazu
	fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

	// ustawia wskaŸnik pozycji pliku na pocz¹tku danych obrazu
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// przydziela pamiêæ buforowi obrazu
	bitmapImage = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage);

	// sprawdza, czy uda³o siê przydzieliæ pamiêæ
	if (!bitmapImage)
	{
		free(bitmapImage);
		fclose(filePtr);
		return NULL;
	}

	// wczytuje dane obrazu
	fread(bitmapImage, 1, bitmapInfoHeader->biSizeImage, filePtr);

	// sprawdza, czy dane zosta³y wczytane
	if (bitmapImage == NULL)
	{
		fclose(filePtr);
		return NULL;
	}

	// zamienia miejscami sk³adowe R i B 
	for (imageIdx = 0; imageIdx < bitmapInfoHeader->biSizeImage; imageIdx += 3)
	{
		tempRGB = bitmapImage[imageIdx];
		bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
		bitmapImage[imageIdx + 2] = tempRGB;
	}

	// zamyka plik i zwraca wskaŸnik bufora zawieraj¹cego wczytany obraz
	fclose(filePtr);
	return bitmapImage;
}

//RAMIE ROBOTA
void ramie(double r1, double r2, double h, double d) {
	double angle, x, y;
	glColor3d(0.5, 1, 1);
	
	
	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0.0, 0.0, -1.0);
	glVertex3d(0.0f, 0.0f, 0.0f);
	for (angle = GL_PI; angle <= (2.0f * GL_PI); angle += (GL_PI / 8.0f))
	{
		x = r1 * sin(angle);
		y = r1 * cos(angle);
		glVertex3d(x, y, 0.0);
	}
	glEnd();
	
	glBegin(GL_QUAD_STRIP);
	for (angle = GL_PI; angle <= (2.0f * GL_PI); angle += (GL_PI / 8.0f))
	{
		x = r1 * sin(angle);
		y = r1 * cos(angle);
		glNormal3d(sin(angle), cos(angle), 0.0);
		glVertex3d(x, y, 0);
		glVertex3d(x, y, h);
	}
	glEnd();
	
	glBegin(GL_TRIANGLE_FAN);
	//glVertex3d(0.0f, 0.0f, 0.0f);
	glNormal3d(0.0, 0.0, 1.0);
	glVertex3d(0.0f, 0.0f, h);
	for (angle = 0.0f; angle >= -(GL_PI); angle -= (GL_PI / 8.0f))
	{
		x = r1 * sin(angle);
		y = r1 * cos(angle);
		//glVertex3d(x, y, 0.0);
		glVertex3d(x, y, h);
	}
	glEnd();
	
	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0.0, 0.0, -1.0);
	glVertex3d(d, 0.0f, 0.0f);
	for (angle = 0.0f; angle <= GL_PI; angle += (GL_PI / 8.0f))
	{
		x = r2 * sin(angle);
		y = r2 * cos(angle);
		glVertex3d(x+d, y, 0.0);
	}
	glEnd();
	
	glBegin(GL_QUAD_STRIP);
	for (angle = 0.0f; angle <= GL_PI; angle += (GL_PI / 8.0f))
	{
		x = r2 * sin(angle);
		y = r2 * cos(angle);
		glNormal3d(sin(angle), cos(angle), 0.0);
		glVertex3d(x+d, y, 0);
		glVertex3d(x+d, y, h);
	}
	glEnd();
	
	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0.0, 0.0, 1.0);
	//glVertex3d(0.0f, 0.0f, 0.0f);
	glVertex3d(d, 0.0f, h);
	for (angle = -(GL_PI); angle >= -(2.0f * GL_PI); angle -= (GL_PI / 8.0f))
	{
		x = r2 * sin(angle);
		y = r2 * cos(angle);
		//glVertex3d(x, y, 0.0);
		glVertex3d(x+d, y, h);
	}
	glEnd();
	/*
	glEnable(GL_TEXTURE_2D); // W³¹cz teksturowanie

	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glBegin(GL_QUADS);
	glNormal3d(0, 0, 1);
	glTexCoord2d(1.0, 1.0); glVertex3d(25, 25, 25);
	glTexCoord2d(0.0, 1.0); glVertex3d(-25, 25, 25);
	glTexCoord2d(0.0, 0.0); glVertex3d(-25, -25, 25);
	glTexCoord2d(1.0, 0.0); glVertex3d(25, -25, 25);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_QUADS);
	glNormal3d(1, 0, 0);
	glTexCoord2d(1.0, 1.0); glVertex3d(25, 25, 25);
	glTexCoord2d(0.0, 1.0); glVertex3d(25, -25, 25);
	glTexCoord2d(0.0, 0.0); glVertex3d(25, -25, -25);
	glTexCoord2d(1.0, 0.0); glVertex3d(25, 25, -25);
	glEnd();

	glDisable(GL_TEXTURE_2D); // Wy³¹cz teksturowanie
	*/
	glEnable(GL_TEXTURE_2D); // W³¹cz teksturowanie
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glBegin(GL_QUADS);
	glNormal3d(0.0, 0.0, 1.0);
	glTexCoord2d(0.0, 0.0); glVertex3d(0, -r1, h);
	glTexCoord2d(1.0, 0.0); glVertex3d(d, -r2, h);
	glTexCoord2d(1.0, 1.0); glVertex3d(d, r2, h);
	glTexCoord2d(0.0, 1.0); glVertex3d(0, r1, h);
	glEnd();
	glDisable(GL_TEXTURE_2D); // Wy³¹cz teksturowanie

	glBegin(GL_QUADS); {
		float v[3][3] = { {d, r2, h},
							{d, r2, 0},
							{0, r1, 0}
		};
		float norm[3];
		calcNormal(v, norm);
		glNormal3d(norm[0], norm[1], norm[2]);
		glVertex3d(d, r2, h);
		glVertex3d(d, r2, 0);
		glVertex3d(0, r1, 0);
		glVertex3d(0, r1, h);
	}
	glEnd();

	glBegin(GL_QUADS);
	glNormal3d(0.0, 0.0, -1.0);
	glVertex3d(0, r1, 0);
	glVertex3d(d, r2, 0);
	glVertex3d(d, -r2, 0);
	glVertex3d(0, -r1, 0);
	
	glEnd();

	glBegin(GL_QUADS); {
		float v[3][3] = { {d, -r2, h},
							{0, -r1, h},
							{0, -r1, 0}
		};
		float norm[3];
		calcNormal(v, norm);
		glNormal3d(norm[0], norm[1], norm[2]);
		glVertex3d(d, -r2, h);
		glVertex3d(0, -r1, h);
		glVertex3d(0, -r1, 0);
		glVertex3d(d, -r2, 0);
	}
	glEnd();

	
}


//DOPISANE NA ZAJ TWORZENIE WALCA
void walec(double r, double h)
{
	double angle, x, y;
	glColor3d(0.5, 1, 0);
	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0.0, 0.0, -1.0);
	glVertex3d(0.0f, 0.0f, 0.0f);
	for (angle = 0.0f; angle <= (2.0f * GL_PI); angle += (GL_PI / 8.0f))
	{
		x = r * sin(angle);
		y = r * cos(angle);
		glVertex3d(x, y, 0.0);
	}
	glEnd();
	glBegin(GL_QUAD_STRIP);
	for (angle = 0.0f; angle <= (2.0f * GL_PI); angle += (GL_PI / 8.0f))
	{
		x = r * sin(angle);
		y = r * cos(angle);
		glNormal3d(sin(angle), cos(angle), 0.0);
		glVertex3d(x, y, 0);
		glVertex3d(x, y, h);
	}
	glEnd();
	glBegin(GL_TRIANGLE_FAN);
	//glVertex3d(0.0f, 0.0f, 0.0f);
	glNormal3d(0.0, 0.0, 1.0);
	glVertex3d(0.0f, 0.0f, h);
	//for (angle = 0.0f; angle <= (2.0f * GL_PI); angle += (GL_PI / 8.0f))
	for (angle = 0.0f; angle >= -(2.0f * GL_PI); angle -= (GL_PI / 8.0f))
	{
		x = r * sin(angle);
		y = r * cos(angle);
		//glVertex3d(x, y, 0.0);
		glVertex3d(x, y, h);
	}
	glEnd();
}

//DOPISANE NA ZAJ TWORZENIE 2 ŒCIAN SZESCIAKATA
void szescian(void)
{
	glBegin(GL_QUADS);
	glColor3d(1, 0.5, 0);
		glVertex3d(20, 20, 20);
		glVertex3d(-20, 20, 20);
		glVertex3d(-20, -20, 20);
		glVertex3d(20, -20, 20);
	glColor3d(0, 0.5, 1);
		glVertex3d(20, 20, 20);
		glVertex3d(20, -20, 20);
		glVertex3d(20, -20, -20);
		glVertex3d(20, 20, -20);
	glColor3d(0, 1, 0);
		glVertex3d(20, 20, 20);
		glVertex3d(20, 20, -20);
		glVertex3d(-20, 20, -20);
		glVertex3d(-20, 20, 20);
	glColor3d(0, 0.5, 0);
		glVertex3d(20, 20, -20);
		glVertex3d(20, -20, -20);
		glVertex3d(-20, -20, -20);
		glVertex3d(-20, 20, -20);
	glColor3d(1, 0.5, 1);
		glVertex3d(-20, 20, 20);
		glVertex3d(-20, 20, -20);
		glVertex3d(-20, -20, -20);
		glVertex3d(-20, -20, 20);
	glColor3d(0, 0, 1);
		glVertex3d(-20, -20, 20);
		glVertex3d(-20, -20, -20);
		glVertex3d(20, -20, -20);
		glVertex3d(20, -20, 20);
	glEnd();
}

void ostroslup(double a, double b, double h) {

	glBegin(GL_QUADS);
	glColor3d(0, 0.5, 1);
	glVertex3d(a, 0, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, b, 0);
	glVertex3d(a, b, 0);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3d(0, 0.5, 1);
	glVertex3d(0, 0, 0);
	glVertex3d(a, 0, 0);
	glVertex3d(0.5 * a, 0.5 * b, h);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3d(0, 0, 1);
	glVertex3d(0, b, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0.5 * a, 0.5 * b, h);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3d(1, 0.5, 1);
	glVertex3d(a, b, 0);
	glVertex3d(0, b, 0);
	glVertex3d(0.5 * a, 0.5 * b, h);
	glEnd();
	
	glBegin(GL_TRIANGLES);
	glColor3d(0, 1, 0);
	glVertex3d(a, 0, 0);
	glVertex3d(a, b, 0);
	glVertex3d(0.5 * a, 0.5 * b, h);
	glEnd();

}

void graniastoslup(double a, double b, double h) {

	// Dolna podstawa
	glBegin(GL_QUADS);
	glNormal3d(0, 0, -1);
	glVertex3d(a, 0, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, b, 0);
	glVertex3d(a, b, 0);
	glEnd();

	// Przednia œciana
	glBegin(GL_QUADS);
	glNormal3d(0, -1, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(a, 0, 0);
	glVertex3d(a, 0, h);
	glVertex3d(0, 0, h);
	glEnd();

	// Lewa œciana
	glBegin(GL_QUADS);
	glNormal3d(-1, 0, 0);
	glVertex3d(0, b, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, h);
	glVertex3d(0, b, h);
	glEnd();

	// Tylna œciana
	glBegin(GL_QUADS);
	glNormal3d(0, 1, 0);
	glVertex3d(a, b, 0);
	glVertex3d(0, b, 0);
	glVertex3d(0, b, h);
	glVertex3d(a, b, h);
	glEnd();

	// Prawa œciana
	glBegin(GL_QUADS);
	glNormal3d(1, 0, 0);
	glVertex3d(a, 0, 0);
	glVertex3d(a, b, 0);
	glVertex3d(a, b, h);
	glVertex3d(a, 0, h);
	glEnd();

	// Górna podstawa
	glBegin(GL_QUADS);
	glNormal3d(0, 0, 1);
	glVertex3d(0, 0, h);
	glVertex3d(a, 0, h);
	glVertex3d(a, b, h);
	glVertex3d(0, b, h);
	glEnd();

}

void podloga(double a, double b, double h) {
	glPushMatrix();
	glTranslated(200, 1200, 0);
	// Dolna podstawa
	glEnable(GL_TEXTURE_2D); // W³¹cz teksturowanie
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glBegin(GL_QUADS);
	glNormal3d(0, 0, -1);
	glTexCoord2d(0.0, 1.0); glVertex3d(a, 0, 0);
	glTexCoord2d(0.0, 0.0); glVertex3d(0, 0, 0);
	glTexCoord2d(1.0, 0.0); glVertex3d(0, b, 0);
	glTexCoord2d(1.0, 1.0); glVertex3d(a, b, 0);
	glEnd();
	glEnd();
	glDisable(GL_TEXTURE_2D); // Wy³¹cz teksturowanie

	// Przednia œciana
	glBegin(GL_QUADS);
	glNormal3d(0, -1, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(a, 0, 0);
	glVertex3d(a, 0, h);
	glVertex3d(0, 0, h);
	glEnd();

	// Lewa œciana
	glBegin(GL_QUADS);
	glNormal3d(-1, 0, 0);
	glVertex3d(0, b, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, h);
	glVertex3d(0, b, h);
	glEnd();

	// Tylna œciana
	glBegin(GL_QUADS);
	glNormal3d(0, 1, 0);
	glVertex3d(a, b, 0);
	glVertex3d(0, b, 0);
	glVertex3d(0, b, h);
	glVertex3d(a, b, h);
	glEnd();

	// Prawa œciana
	glBegin(GL_QUADS);
	glNormal3d(1, 0, 0);
	glVertex3d(a, 0, 0);
	glVertex3d(a, b, 0);
	glVertex3d(a, b, h);
	glVertex3d(a, 0, h);
	glEnd();

	// Górna podstawa
	glBegin(GL_QUADS);
	glNormal3d(0, 0, 1);
	 glVertex3d(0, 0, h);
	 glVertex3d(a, 0, h);
	 glVertex3d(a, b, h);
	 glVertex3d(0, b, h);
	 glEnd();
	 glPopMatrix();
	
}
void graniastoslup_napis(double a, double b, double h) {

	// Dolna podstawa
	glBegin(GL_QUADS);
	glNormal3d(0, 0, -1);
	glVertex3d(a, 0, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, b, 0);
	glVertex3d(a, b, 0);
	glEnd();

	// Przednia œciana
	glBegin(GL_QUADS);
	glNormal3d(0, -1, 0);
	 glVertex3d(0, 0, 0);
	 glVertex3d(a, 0, 0);
	 glVertex3d(a, 0, h);
	 glVertex3d(0, 0, h);
	glEnd();
	
	
	
	// Lewa œciana
	glBegin(GL_QUADS);
	glNormal3d(-1, 0, 0);
	glVertex3d(0, b, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, h);
	glVertex3d(0, b, h);
	glEnd();

	// Tylna œciana
	glBegin(GL_QUADS);
	glNormal3d(0, 1, 0);
	glVertex3d(a, b, 0);
	glVertex3d(0, b, 0);
	glVertex3d(0, b, h);
	glVertex3d(a, b, h);
	glEnd();

	// Prawa œciana
	glBegin(GL_QUADS);
	glNormal3d(1, 0, 0);
	 glVertex3d(a, 0, 0);
	 glVertex3d(a, b, 0);
	 glVertex3d(a, b, h);
	 glVertex3d(a, 0, h);
	glEnd();
	

	// Górna podstawa
	glEnable(GL_TEXTURE_2D); // W³¹cz teksturowanie
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glBegin(GL_QUADS);
	glNormal3d(0, 0, 1);
	glTexCoord2d(0.0, 1.0); glVertex3d(0, 0, h);
	glTexCoord2d(0.0, 0.0); glVertex3d(a, 0, h);
	glTexCoord2d(1.0, 0.0); glVertex3d(a, b, h);
	glTexCoord2d(1.0, 1.0); glVertex3d(0, b, h);
	glEnd();
	glDisable(GL_TEXTURE_2D); // Wy³¹cz teksturowanie
}

void stozek(double r, double h) {
	
	double x, y, angle;
	glColor3d(0, 0.5, 0.5);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(0.0f, 0.0f, 0.0f);
	for (angle = 0.0f; angle <= (2.0f * GL_PI); angle += (GL_PI / 8.0f))
	{
		x = r * sin(angle);
		y = r * cos(angle);
		glVertex3d(x, y, 0.0);
	}
	glEnd();
	glColor3d(1, 0.5, 0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(0.0f, 0.0f, h);
	for (angle = 0.0f; angle <= (2.0f * GL_PI); angle += (GL_PI / 8.0f)) {
		y = r * sin(angle);
		x = r * cos(angle);
		glVertex3d(x, y, 0);
	}
	glEnd();
}

void robot(double d1, double d2, double d3, double d4)
{
	glPushMatrix();
		glRotated(-90, 1, 0, 0);
		glRotated(d4, 0, 0, 1);
		glTranslated(0, 0, -50);
		walec(30, 5);
		glTranslated(0, 0, 5);
		walec(10, 40);
		glTranslated(0, 0, 40);
		glRotated(d1, 0, 0, 1);
		walec(10, 40);
		glTranslated(0, 0, 40);
		glRotated(90, 0, 1, 0);
		glTranslated(0, 0, -20);
		walec(10, 40);
		glTranslated(-10, 10, 40);
		glRotated(-90 , 1, 0, 0);
		graniastoslup_napis(20, 40, 2);
	glPopMatrix();
}

void nowy_robot(double d1, double d2, double d3, double d4, double d5, double obrot, double flaga)
{
	
		glTranslated(d5, 0, -d4);
		glPushMatrix();
		glRotated(-90, 1, 0, 0);
		glRotated(obrot, 0, 0, 1);
		glTranslated(0, 0, -50);
		walec(30, 5);
		glRotated(90, 0, 1, 0);
		glTranslated(-15, -15, -15);
		walec(10, 30);
		glTranslated(0, 0, 8);
		glRotated(90, 1, 0, 0);
		glRotated(-45, 0, 1, 0);
		glRotated(-80 + d1, 0, 1, 0);
		glTranslated(-5, 2.5, 0);
		graniastoslup(10, 10, 40);
		glTranslated(2.5, 20, 40);
		glRotated(90, 1, 0, 0);
		walec(10, 30);
		glRotated(-90, 0, 1, 0);
		glRotated(45, 1, 0, 0);
		glTranslated(10, -2.5, 2.5);
		glRotated(-90 + d2 * (-1), 1, 0, 0);
		graniastoslup(10, 10, 60);
		glTranslated(-10, 5, 60);
		glRotated(90, 0, 1, 0);
		walec(10, 30);
		glRotated(90, 1, 0, 0);
		glRotated(-45, 0, 1, 0);
		glTranslated(-5, 10, 5);
		glRotated(30+d3 * (-1), 0, 1, 0);
		graniastoslup(10, 10, 25);
		glPopMatrix();
	
		
	
	
	
}

void nowy_drugi_robot(double d1, double d2, double d3, double d4, double d5, double obrot)
{

	glTranslated(d5, 0, -d4);
	glPushMatrix();
	glRotated(-90, 1, 0, 0);
	glRotated(obrot, 0, 0, 1);
	glTranslated(0, 0, -50);
	walec(30, 5);
	glRotated(90, 0, 1, 0);
	glTranslated(-15, -15, -15);
	walec(10, 30);
	glTranslated(0, 0, 8);
	glRotated(90, 1, 0, 0);
	glRotated(-45, 0, 1, 0);
	glRotated(-80 + d1, 0, 1, 0);
	glTranslated(-5, 2.5, 0);
	graniastoslup(10, 10, 40);
	glTranslated(2.5, 20, 40);
	glRotated(90, 1, 0, 0);
	walec(10, 30);
	glRotated(-90, 0, 1, 0);
	glRotated(45, 1, 0, 0);
	glTranslated(10, -2.5, 2.5);
	glRotated(-90 + d2 * (-1), 1, 0, 0);
	graniastoslup(10, 10, 60);
	glTranslated(-10, 5, 60);
	glRotated(90, 0, 1, 0);
	walec(10, 30);
	glRotated(90, 1, 0, 0);
	glRotated(-45, 0, 1, 0);
	glTranslated(-5, 10, 5);
	glRotated(30 + d3 * (-1), 0, 1, 0);
	graniastoslup(10, 10, 25);
	glPopMatrix();





}

void tasma(double d1) {
	glColor3d(0, 0, 1);
	glPushMatrix();
	glTranslated(0, -40, 40);
	glRotated(-90, 0, 1, 0);
	graniastoslup(60, 40, 300);
	glTranslated(10, 40, 0);
	glColor3d(0, 1, 0);
	graniastoslup(40, 2, 300);
	glTranslated(20+movekulaY, 7+movekulaZ, 280-d1+movekulaX);
	kula();
	glPopMatrix();
}

void dwa_roboty()
{
	robot(rot1, rot2, rot3, rot4);
	glTranslated(80, 0, 0);
	robot(rot1, rot2, rot3, rot4);

}
void dwa_nowe_roboty(double flaga)
{
	glTranslated(-200, 0, 0);
	robot(rot1, rot2, rot3, rot5);
	glTranslated(200, 0, 0);
	glTranslated(120, 0, 0);
	nowy_robot(rot1, rot2, rot3, rot6, rot7, rot8, flaga);

}

void scena(double d1) {
	tasma(d1);
	glTranslated(-150, 40, 150);
	dwa_nowe_roboty(d1);
	glTranslated(-600, -48, -1400);
	glRotated(90, 1, 0, 0);
	glColor3d(1, 0, 0);
	podloga(500, 500, 2);
	glRotated(-90, 1, 0, 0);
	glTranslated(600, 26, 1500);
	skrzynka();
	glTranslated(-7, 25, 80);
	nowy_drugi_robot(rot11=20, rot12, rot13, rot14, rot15, rot16);
	
}


// Called to draw scene
void RenderScene(void)
{
	//float normal[3];	// Storeage for calculated surface normal

	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Save the matrix state and do the rotations
	glPushMatrix();
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);
	glTranslatef(xObs, 0, 0);
	glTranslatef(0, 0, yObs);
	/////////////////////////////////////////////////////////////////
	// MIEJSCE NA KOD OPENGL DO TWORZENIA WLASNYCH SCEN:		   //
	/////////////////////////////////////////////////////////////////

	//Sposób na odróŸnienie "przedniej" i "tylniej" œciany wielok¹ta:
	glPolygonMode(GL_BACK, GL_LINE);

	//Uzyskanie siatki:
	//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	//Wyrysowanie prostokata:
	//glRectd(-10.0, -10.0, 20.0, 20.0);
	//NA ZAJECIACH WYWOLANIE RYSOWANIA SZESCIOKATA (2 SCIANY)
	//szescian();
	//NA ZAJECIACH WYWOLANIE RYSOWANIA WALCA
	//walec(30, 20);
	//ramie(10.1, 40.3, 20.2, 60.6);
	//ostroslup(24, 10, 15);
	//stozek(20, 30);
	//robot(rot1, rot2, rot3, rot4);
	//nowy_robot(rot1, rot5, rot3, rot4);
	//dwa_nowe_roboty();
	//dwa_roboty();
	//skrzynka();
	//walec01();
	//kula();
	scena(move1);
	//tasma(rot1);
	//graniastoslup(10, 10, 10);

	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	// Flush drawing commands
	glFlush();
}


// Select the pixel format for a given device context
void SetDCPixelFormat(HDC hDC)
{
	int nPixelFormat;

	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),  // Size of this structure
		1,                                                              // Version of this structure    
		PFD_DRAW_TO_WINDOW |                    // Draw to Window (not to bitmap)
		PFD_SUPPORT_OPENGL |					// Support OpenGL calls in window
		PFD_DOUBLEBUFFER,                       // Double buffered
		PFD_TYPE_RGBA,                          // RGBA Color mode
		24,                                     // Want 24bit color 
		0,0,0,0,0,0,                            // Not used to select mode
		0,0,                                    // Not used to select mode
		0,0,0,0,0,                              // Not used to select mode
		32,                                     // Size of depth buffer
		0,                                      // Not used to select mode
		0,                                      // Not used to select mode
		PFD_MAIN_PLANE,                         // Draw in main plane
		0,                                      // Not used to select mode
		0,0,0 };                                // Not used to select mode

												// Choose a pixel format that best matches that described in pfd
	nPixelFormat = ChoosePixelFormat(hDC, &pfd);

	// Set the pixel format for the device context
	SetPixelFormat(hDC, nPixelFormat, &pfd);
}



// If necessary, creates a 3-3-2 palette for the device context listed.
HPALETTE GetOpenGLPalette(HDC hDC)
{
	HPALETTE hRetPal = NULL;	// Handle to palette to be created
	PIXELFORMATDESCRIPTOR pfd;	// Pixel Format Descriptor
	LOGPALETTE *pPal;			// Pointer to memory for logical palette
	int nPixelFormat;			// Pixel format index
	int nColors;				// Number of entries in palette
	int i;						// Counting variable
	BYTE RedRange, GreenRange, BlueRange;
	// Range for each color entry (7,7,and 3)


	// Get the pixel format index and retrieve the pixel format description
	nPixelFormat = GetPixelFormat(hDC);
	DescribePixelFormat(hDC, nPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	// Does this pixel format require a palette?  If not, do not create a
	// palette and just return NULL
	if (!(pfd.dwFlags & PFD_NEED_PALETTE))
		return NULL;

	// Number of entries in palette.  8 bits yeilds 256 entries
	nColors = 1 << pfd.cColorBits;

	// Allocate space for a logical palette structure plus all the palette entries
	pPal = (LOGPALETTE*)malloc(sizeof(LOGPALETTE) + nColors * sizeof(PALETTEENTRY));

	// Fill in palette header 
	pPal->palVersion = 0x300;		// Windows 3.0
	pPal->palNumEntries = nColors; // table size

								   // Build mask of all 1's.  This creates a number represented by having
								   // the low order x bits set, where x = pfd.cRedBits, pfd.cGreenBits, and
								   // pfd.cBlueBits.  
	RedRange = (1 << pfd.cRedBits) - 1;
	GreenRange = (1 << pfd.cGreenBits) - 1;
	BlueRange = (1 << pfd.cBlueBits) - 1;

	// Loop through all the palette entries
	for (i = 0; i < nColors; i++)
	{
		// Fill in the 8-bit equivalents for each component
		pPal->palPalEntry[i].peRed = (i >> pfd.cRedShift) & RedRange;
		pPal->palPalEntry[i].peRed = (unsigned char)(
			(double)pPal->palPalEntry[i].peRed * 255.0 / RedRange);

		pPal->palPalEntry[i].peGreen = (i >> pfd.cGreenShift) & GreenRange;
		pPal->palPalEntry[i].peGreen = (unsigned char)(
			(double)pPal->palPalEntry[i].peGreen * 255.0 / GreenRange);

		pPal->palPalEntry[i].peBlue = (i >> pfd.cBlueShift) & BlueRange;
		pPal->palPalEntry[i].peBlue = (unsigned char)(
			(double)pPal->palPalEntry[i].peBlue * 255.0 / BlueRange);

		pPal->palPalEntry[i].peFlags = (unsigned char)NULL;
	}


	// Create the palette
	hRetPal = CreatePalette(pPal);

	// Go ahead and select and realize the palette for this device context
	SelectPalette(hDC, hRetPal, FALSE);
	RealizePalette(hDC);

	// Free the memory used for the logical palette structure
	free(pPal);

	// Return the handle to the new palette
	return hRetPal;
}


// Entry point of all Windows programs
int APIENTRY WinMain(HINSTANCE       hInst,
	HINSTANCE       hPrevInstance,
	LPSTR           lpCmdLine,
	int                     nCmdShow)
{
	MSG                     msg;            // Windows message structure
	WNDCLASS        wc;                     // Windows class structure
	HWND            hWnd;           // Storeage for window handle

	hInstance = hInst;

	// Register Window style
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	// No need for background brush for OpenGL window
	wc.hbrBackground = NULL;

	wc.lpszMenuName = NULL;
	wc.lpszClassName = lpszAppName;

	// Register the window class
	if (RegisterClass(&wc) == 0)
		return FALSE;


	// Create the main application window
	hWnd = CreateWindow(
		lpszAppName,
		lpszAppName,

		// OpenGL requires WS_CLIPCHILDREN and WS_CLIPSIBLINGS
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,

		// Window position and size
		50, 50,
		400, 400,
		NULL,
		NULL,
		hInstance,
		NULL);

	// If window was not created, quit
	if (hWnd == NULL)
		return FALSE;


	// Display the window
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	// Process application messages until the application closes
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}




// Window procedure, handles all messages for this program
LRESULT CALLBACK WndProc(HWND    hWnd,
	UINT    message,
	WPARAM  wParam,
	LPARAM  lParam)
{
	static HGLRC hRC;               // Permenant Rendering context
	static HDC hDC;                 // Private GDI Device context

	switch (message)
	{
		// Window creation, setup for OpenGL
	case WM_CREATE:

		//Ustawienie timera
		SetTimer(hWnd, 101, 200, NULL);
		// Store the device context
		hDC = GetDC(hWnd);

		// Select the pixel format
		SetDCPixelFormat(hDC);

		// Create palette if needed
		hPalette = GetOpenGLPalette(hDC);

		// Create the rendering context and make it current
		hRC = wglCreateContext(hDC);
		wglMakeCurrent(hDC, hRC);
		SetupRC();
		glGenTextures(3, &texture[0]);                  // tworzy obiekt tekstury			

														// ³aduje pierwszy obraz tekstury:
		bitmapData = LoadBitmapFile("Bitmapy\\checker.bmp", &bitmapInfoHeader);

		glBindTexture(GL_TEXTURE_2D, texture[0]);       // aktywuje obiekt tekstury

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		// tworzy obraz tekstury
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmapInfoHeader.biWidth,
			bitmapInfoHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmapData);

		if (bitmapData)
			free(bitmapData);

		// ³aduje drugi obraz tekstury:
		bitmapData = LoadBitmapFile("Bitmapy\\crate.bmp", &bitmapInfoHeader);
		glBindTexture(GL_TEXTURE_2D, texture[1]);       // aktywuje obiekt tekstury

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		// tworzy obraz tekstury
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmapInfoHeader.biWidth,
			bitmapInfoHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmapData);

		// ³aduje trzeci (puma) obraz tekstury:
		bitmapData = LoadBitmapFile("Bitmapy\\MATEUSZ.bmp", &bitmapInfoHeader);
		glBindTexture(GL_TEXTURE_2D, texture[2]);       // aktywuje obiekt tekstury

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		// tworzy obraz tekstury
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmapInfoHeader.biWidth,
			bitmapInfoHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmapData);

		// ³aduje czwarty (trawa) obraz tekstury:
		bitmapData = LoadBitmapFile("Bitmapy\\PODLOGA.bmp", &bitmapInfoHeader);
		glBindTexture(GL_TEXTURE_2D, texture[3]);       // aktywuje obiekt tekstury

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		// tworzy obraz tekstury
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmapInfoHeader.biWidth,
			bitmapInfoHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmapData);
		if (bitmapData)
			free(bitmapData);

		// ustalenie sposobu mieszania tekstury z t³em
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		break;

		// Window is being destroyed, cleanup
	case WM_DESTROY:

		//Kasowanie timera
		KillTimer(hWnd, 101);
		// Deselect the current rendering context and delete it
		wglMakeCurrent(hDC, NULL);
		wglDeleteContext(hRC);

		// Delete the palette if it was created
		if (hPalette != NULL)
			DeleteObject(hPalette);

		// Tell the application to terminate after the window
		// is gone.
		PostQuitMessage(0);
		break;

		// Window is resized.
	case WM_SIZE:
		// Call our function which modifies the clipping
		// volume and viewport
		ChangeSize(LOWORD(lParam), HIWORD(lParam), xObs, yObs);
		break;


		// The painting function.  This message sent by Windows 
		// whenever the screen needs updating.
	case WM_PAINT:
	{
		// Call OpenGL drawing code
		RenderScene();

		SwapBuffers(hDC);

		// Validate the newly painted client area
		ValidateRect(hWnd, NULL);
	}
	break;

	// Windows is telling the application that it may modify
	// the system palette.  This message in essance asks the 
	// application for a new palette.
	case WM_QUERYNEWPALETTE:
		// If the palette was created.
		if (hPalette)
		{
			int nRet;

			// Selects the palette into the current device context
			SelectPalette(hDC, hPalette, FALSE);

			// Map entries from the currently selected palette to
			// the system palette.  The return value is the number 
			// of palette entries modified.
			nRet = RealizePalette(hDC);

			// Repaint, forces remap of palette in current window
			InvalidateRect(hWnd, NULL, FALSE);

			return nRet;
		}
		break;


		// This window may set the palette, even though it is not the 
		// currently active window.
	case WM_PALETTECHANGED:
		// Don't do anything if the palette does not exist, or if
		// this is the window that changed the palette.
		if ((hPalette != NULL) && ((HWND)wParam != hWnd))
		{
			// Select the palette into the device context
			SelectPalette(hDC, hPalette, FALSE);

			// Map entries to system palette
			RealizePalette(hDC);

			// Remap the current colors to the newly realized palette
			UpdateColors(hDC);
			return 0;
		}
		break;

		// Key press, check for arrow keys to do cube rotation.
	case WM_KEYDOWN:
	{
		if (wParam == VK_UP)
			xRot -= 5.0f;

		if (wParam == VK_DOWN)
			xRot += 5.0f;

		if (wParam == VK_LEFT)
			yRot -= 5.0f;

		if (wParam == VK_RIGHT)
			yRot += 5.0f;
		if (wParam == 'A')
			xObs -= 5.0f;
		if (wParam == 'D')
			xObs += 5.0f;
		if (wParam == 'W')
			yObs -= 5.0f;
		if (wParam == 'S')
			yObs += 5.0f;

		xRot = (const int)xRot % 360;
		yRot = (const int)yRot % 360;

		if (wParam == '1' && rot1 >= 0)
			rot1 -= 5.0f;

		if (wParam == '2' && rot1 <= 45)
			rot1 += 5.0f;

		if (wParam == '3' && rot2 >= 0)
			rot2 -= 5.0f;

		if (wParam == '4' && rot2 <= 70)
			rot2 += 5.0f;

		if (wParam == '5' && rot3 >= 0)
			rot3 -= 5.0f;

		if (wParam == '6' && rot3 <= 45)
			rot3 += 5.0f;

		if (wParam == 'Q' )
			rot8 += 5.0f;

		InvalidateRect(hWnd, NULL, FALSE);
	}
	break;

	
	//Obs³uga timera
	case WM_TIMER:
		if (wParam == 101)
		{
			licznik++;
			if (move1 <= 245) {
				move1 += 50.0;
			}

			//if (licznik < 15)
			//	//rot5 += 15.0;
			//if (licznik > 15 && licznik < 30)
			//	//rot5 -= 15.0;
			//if (licznik > 30)
			//{
			//	licznik = 0;
			//}
			rot5 += 15.0;
			
			if (move1 >= 245) {
				if (rot1 >= -9.0 && stop1 == 0)
					rot1 -= 2.0;
				if (rot1 >= -9.0 && stop1 == 0)
				{
					rot3 -= 2.0;
				}
				if (rot1 <= -9.0 && stop2 == 0) {
					stop1 += 1;
					stop2 += 1;
				}
				if (stop1 == 1 && stop3 == 0) {
					if (movekulaZ <= 40)
					{
						movekulaZ += 6.0f;
					}
					if (rot1 <= 10.0) {
						rot1 += 3.0;
					}
					if (rot2 <= 3) {
						rot2 += 1.0;
					}
					if (rot3 <= 15.0) {
						rot3 += 4.0;
					}
					if (movekulaZ >= 10 && rot1 >= 10.0 && rot2 >= 3 && rot3 >= 15.0 && stop3 == 0) {
						stop1 += 1;
						stop3 += 1;
					}
				}
			}
				if (stop1 == 2) {
					if (rot8 <= 170.0) {
						rot8 += 5.3;
					}
					
					if (anglekula <= GL_PI-0.01)
					{
					
						
						movekulaX += 7.7 * cos(anglekula);
						movekulaY += 7.7 * sin(anglekula);
						anglekula += (GL_PI / 33.0f);

						
					}
					if (rot8 >= 170.0 && anglekula >= GL_PI - 0.01 && stop1 == 2) {
						stop1 += 1;
					}
				}
				if (stop1 == 3) {
					if (rot1 >= -4) {
						rot1 -= 3.0;
					}
					if (movekulaY <= 174.0) {
						movekulaY += 2.3;
					}
					if (rot2 <= 20.0) {
						rot2 += 3.0;
					}
					if (rot3 <= 30) {
						rot3 += 3.0;
					}
					if (rot1 <= -4 && movekulaY >= 174.0 && rot2 >= 20 && rot3 >= 30 && stop1 == 3) {
						stop1 += 1;
					}
				}
				if (stop1 == 4) {
					if (movekulaZ >= -12.0) {
						movekulaZ -= 3.0;
					}
					if (movekulaZ <= -12.0 && stop1 == 4) {
						stop1 += 1;
					}
				}
				if (stop1 == 5) {
					if (rot8 >= 0.0) {
						rot8 -= 5.0;
					}
					if (rot1 <= 5.0) {
						rot1 += 3.0;
					}
					if (rot2 >= 30.0) {
						rot2 -= 3.0;
					}
					if (rot3 >= 20) {
						rot3 -= 3.0;
					}
					if (rot11 >= -100) {
						rot11 -= 3.0;
					}
					if (rot8 <= 0.0 && stop1 == 5) {
						stop1 += 1;
					}
				}
				
				
				
					
				
			licznikpom++;
			
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;

	default:   // Passes it on if unproccessed
		return (DefWindowProc(hWnd, message, wParam, lParam));

	}

	return (0L);
}
