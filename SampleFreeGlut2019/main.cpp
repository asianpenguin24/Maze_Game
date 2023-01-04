//	CS 450 Project 2: Animate a Helicopter
//	Jeremy Udarbe : udarbej@oregonstate.edu
//	Due 10/12

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <iostream>
#include <time.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif

#include "glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"
#include "loadobjfile.h"
#include "sphere.h"
#include "lighting.h"

using namespace std;

// title of these windows:

const char *WINDOWTITLE = { "OpenGL / GLUT Render -- Jeremy Udarbe" };
const char *GLUITITLE   = { "User Interface Window" };

// what the glui package defines as true and false:

const int GLUITRUE  = { true  };
const int GLUIFALSE = { false };

// the escape key:

const int ESCAPE = { 0x1b };

// initial window size:

const int INIT_WINDOW_WIDTH = { 720 };
const int INIT_WINDOW_HEIGHT = { 720 };
// other constants:

const float PI = { 3.14f };
#define MS_IN_THE_ANIMATION_CYCLE 7200
#define BLADE_RADIUS		1.0
#define BLADE_WIDTH			0.4

//	multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = { 1. };
const float SCLFACT = { 0.005f };

// minimum allowable scale factor:

const float MINSCALE = { 0.05f };

// scroll wheel button values:

const int SCROLL_WHEEL_UP   = { 3 };
const int SCROLL_WHEEL_DOWN = { 4 };

// equivalent mouse movement when we click a the scroll wheel:

const float SCROLL_WHEEL_CLICK_FACTOR = { 5. };

// active mouse buttons (or them together):

const int LEFT   = { 4 };
const int MIDDLE = { 2 };
const int RIGHT  = { 1 };

// which projection:

enum Projections
{
	ORTHO,
	PERSP
};

// which button:

enum ButtonVals
{
	RESET,
	QUIT
};

// window background color (rgba):

const GLfloat BACKCOLOR[ ] = { 0., 0., 0., 1. };

// line width for the axes:

const GLfloat AXES_WIDTH   = { 3. };

// the color numbers:
// this order must match the radio button order

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA,
	WHITE,
	BLACK
};

char * ColorNames[ ] =
{
	(char *)"Red",
	(char*)"Yellow",
	(char*)"Green",
	(char*)"Cyan",
	(char*)"Blue",
	(char*)"Magenta",
	(char*)"White",
	(char*)"Black"
};

// the color definitions:
// this order must match the menu order

const GLfloat Colors[ ][3] = 
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
	{ 1., 1., 1. },		// white
	{ 0., 0., 0. },		// black
};

// fog parameters:

const GLfloat FOGCOLOR[4] = { .0, .0, .0, 1. };
const GLenum  FOGMODE     = { GL_LINEAR };
const GLfloat FOGDENSITY  = { 0.30f };
const GLfloat FOGSTART    = { 1.5 };
const GLfloat FOGEND      = { 4. };


// what options should we compile-in?
// in general, you don't need to worry about these
// i compile these in to show class examples of things going wrong
//#define DEMO_Z_FIGHTING
//#define DEMO_DEPTH_BUFFER

// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
bool	Light0On;		// declare these as globals
GLuint	Light0;
int		lighting;
int		TextureOn;				// != 0 means to enable texture
int		DistortOn;				// != 0 means to distort the image
int		AxesOn;					// != 0 means to draw the axes
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn;			// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to force the creation of z-fighting
int		MainWindow;				// window id for main graphics window
float	Scale;					// scaling factor
float	Time;					// timer in the range [0.,1.)
int		WhichColor;				// index into Colors[ ]
int		WhichProjection;		// ORTHO or PERSP
int		Xmouse, Ymouse;			// mouse values
int	Xrot, Yrot;				// rotation angles in degrees
bool	ViewMode;				// inside = true, outside = false
bool	Frozen;
GLuint	Map;
GLuint	OBJfile;
GLuint	Texture;
int width = 1024, height = 512;
unsigned char* TextureArray;
bool player_mov;
int player_Xpos, player_Zpos;
int fish1, fish2, fish3, fish4, fish5;

// function prototypes:

void	Animate( );
void	Display( );
void	DoViewMenu(int);
void	DoAxesMenu( int );
void	DoColorMenu( int );
void	DoDepthBufferMenu( int );
void	DoDepthFightingMenu( int );
void	DoDepthMenu( int );
void	DoDebugMenu( int );
void	DoMainMenu( int );
void	DoProjectMenu( int );
void	DoShadowMenu();
void	DoRasterString( float, float, float, char * );
void	DoStrokeString( float, float, float, float, char * );
float	ElapsedSeconds( );
void	InitGraphics( );
float	RandomNum( );
void	InitLists( );
void	InitMenus( );
void	Keyboard( unsigned char, int, int );
void	MouseButton( int, int, int, int );
void	MouseMotion( int, int );
void	Reset( );
void	Resize( int, int );
void	Visibility( int );

void			Axes( float );
unsigned char *	BmpToTexture( char *, int *, int * );
void			HsvRgb( float[3], float [3] );
int				ReadInt( FILE * );
short			ReadShort( FILE * );

void			Cross(float[3], float[3], float[3]);
float			Dot(float [3], float [3]);
float			Unit(float [3], float [3]);


// main program:

int
main( int argc, char *argv[ ] )
{
	// turn on the glut package:
	// (do this before checking argc and argv since it might
	// pull some command line arguments out)
	srand((unsigned int)time(NULL));

	glutInit( &argc, argv );

	// setup all the graphics stuff:

	InitGraphics( );

	// init all the global variables used by Display( ):

	Reset( );

	// create the display structures that will not change:

	InitLists( );

	// setup all the user interface stuff:

	InitMenus( );

	// draw the scene once and wait for some interaction:
	// (this will never return)

	glutSetWindow( MainWindow );
	glutMainLoop( );

	// glutMainLoop( ) never returns
	// this line is here to make the compiler happy:

	return 0;
}


// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutMainLoop( ) do it

void
Animate( )
{
	// put animation stuff in here -- change some global variables
	// for Display( ) to find:

	int ms = glutGet(GLUT_ELAPSED_TIME);	// milliseconds
	ms %= MS_IN_THE_ANIMATION_CYCLE;
	Time = (float)ms / (float)MS_IN_THE_ANIMATION_CYCLE;        // [ 0., 1. )

	// force a call to Display( ) next time it is convenient:

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// draw the complete scene:

void
Display( )
{
	if( DebugOn != 0 )
	{
		fprintf( stderr, "Display\n" );
	}

	// set which window we want to do the graphics into:

	glutSetWindow( MainWindow );

	// erase the background:

	glDrawBuffer( GL_BACK );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable( GL_DEPTH_TEST );
#ifdef DEMO_DEPTH_BUFFER
	if( DepthBufferOn == 0 )
		glDisable( GL_DEPTH_TEST );
#endif

	// specify shading to be flat:

	glShadeModel( GL_FLAT );

	// set the viewport to a square centered in the window:

	GLsizei vx = glutGet( GLUT_WINDOW_WIDTH );
	GLsizei vy = glutGet( GLUT_WINDOW_HEIGHT );
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = ( vx - v ) / 2;
	GLint yb = ( vy - v ) / 2;
	glViewport( xl, yb,  v, v );

	// set the viewing volume:
	// remember that the Z clipping  values are actually
	// given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	if( WhichProjection == ORTHO )
		glOrtho( -3., 3.,     -3., 3.,     0.1, 1000. );
	else
		gluPerspective( 90., 1.,	0.1, 1000. );

	// place the objects into the scene:

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );

	// set the eye position, look-at position, and up-vector:

	if (ViewMode == 0) {
		//first person view
		gluLookAt(0, 0, 1, 0, 0, 0, 0, 1, 0);
	}
	else {
		gluLookAt(0, 32, 4, 0, 0, 0, 0, 1, 0);
		// virds eye view
		if (Scale < MINSCALE)
			Scale = MINSCALE;
		glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);
	}
	if (player_mov == true) {
		//movement
		switch (Yrot) {
		//North
		case 0:
			player_Zpos += 4;
			break;
		//East
		case 90:
			player_Xpos -= 4;
			break;
		//South
		case 180:
			player_Zpos -= 4;
			break;
		//West
		case 270:
			player_Xpos += 4;
			break;
		}
		printf("position( %d, %d) angle( %d) \n", player_Xpos, player_Zpos, Yrot);
		player_mov = false;
	}
	// rotate the scene:
	if (Yrot == 360) Yrot = 0;
	if (Yrot == -45) Yrot = 315;
	glRotatef((GLfloat)Yrot, 0., 1., 0.);
	glRotatef((GLfloat)Xrot, 1., 0., 0.);
	glTranslatef(player_Xpos, 0, player_Zpos);
	
	// set the fog parameters:
	// (this is really here to do intensity depth cueing)

	if( DepthCueOn != 0 )
	{
		glFogi( GL_FOG_MODE, FOGMODE );
		glFogfv( GL_FOG_COLOR, FOGCOLOR );
		glFogf( GL_FOG_DENSITY, FOGDENSITY );
		glFogf( GL_FOG_START, FOGSTART );
		glFogf( GL_FOG_END, FOGEND );
		glEnable( GL_FOG );
	}
	else
	{
		glDisable( GL_FOG );
	}

	// possibly draw the axes:
	if( AxesOn != 0 )
	{
		glColor3fv( &Colors[WhichColor][0] );
		glCallList( AxesList );
	}

	// since we are using glScalef( ), be sure normals get unitized:
	glEnable(GL_NORMALIZE);

	if (lighting) {
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		SetPointLight(GL_LIGHT0, -player_Xpos, 0., -player_Zpos, 1., 1., 1.);
	}
	else {
		
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
	}
	if (TextureOn) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}
	else {
		glDisable(GL_TEXTURE_2D);
	}

	
	//outside viewing environment
	glPushMatrix();
	glShadeModel(GL_FLAT);
	SetMaterial(0.25, 1, 0.4, 256);
	glRotatef(180, 0., 1., 0.);
	glScalef(-1, 1, 1);
	glColor3f(0.25, 1, 0.4);
	glCallList(Map);
	glPopMatrix();

	//player sphere
	glPushMatrix();
	glShadeModel(GL_SMOOTH);
	SetMaterial(1, 0, 0, 256);
	glTranslatef(-player_Xpos, -2, -player_Zpos);
	glRotatef(360. * Time, 0., 1., 0.);
	glColor3f(1, 0, 0);
	OsuSphere(1, 16, 16, DistortOn, Time);
	glPopMatrix();
	
	//fish 1
	if (player_Xpos == -4 && player_Zpos == 8) fish1 = 1;
	if (fish1 == 0) {
		glPushMatrix();
		glShadeModel(GL_SMOOTH);
		SetMaterial(0.25, 0.75, 1, 256);
		glTranslatef(4, -1.5, -8);
		glRotatef(360. * Time, 0., 1., 0.);
		glScalef(0.6, 0.6, 0.6);
		glColor3f(0.25, 0.75, 1);
		glCallList(OBJfile);
		glPopMatrix();
	}

	//fish 2
	if (player_Xpos == 12 && player_Zpos == 4) fish2 = 1;
	if (fish2 == 0) {
		glPushMatrix();
		glShadeModel(GL_SMOOTH);
		SetMaterial(0.25, 0.75, 1, 256);
		glTranslatef(-12, -1.5, -4);
		glRotatef(360. * Time, 0., 1., 0.);
		glScalef(0.6, 0.6, 0.6);
		glColor3f(0.25, 0.75, 1);
		glCallList(OBJfile);
		glPopMatrix();
	}

	//fish 3
	if (player_Xpos == 12 && player_Zpos == -16) fish3 = 1;
	if (fish3 == 0) {
		glPushMatrix();
		glShadeModel(GL_SMOOTH);
		SetMaterial(0.25, 0.75, 1, 256);
		glTranslatef(-12, -1.5, 16);
		glRotatef(360. * Time, 0., 1., 0.);
		glScalef(0.6, 0.6, 0.6);
		glColor3f(0.25, 0.75, 1);
		glCallList(OBJfile);
		glPopMatrix();
	}

	//fish 4
	if (player_Xpos == -20 && player_Zpos == -8) fish4 = 1;
	if (fish4 == 0) {
		glPushMatrix();
		glShadeModel(GL_SMOOTH);
		SetMaterial(0.25, 0.75, 1, 256);
		glTranslatef(20, -1.5, 8);
		glRotatef(360. * Time, 0., 1., 0.);
		glScalef(0.6, 0.6, 0.6);
		glColor3f(0.25, 0.75, 1);
		glCallList(OBJfile);
		glPopMatrix();
	}

	//EXIT
	if (player_Xpos == -24 && player_Zpos == -20 && fish1 == 1 && fish2 == 1 && fish3 == 1 && fish4 == 1) fish5 = 1;
	if (fish5 == 0) {
		glPushMatrix();
		glShadeModel(GL_SMOOTH);
		SetMaterial(1, 1, 0.25, 256);
		glTranslatef(24, -1.5, 20);
		glRotatef(360. * Time, 0., 1., 0.);
		glScalef(0.6, 0.6, 0.6);
		glColor3f(1, 1, 0.25);
		glCallList(OBJfile);
		glPopMatrix();
	}
#ifdef DEMO_Z_FIGHTING
	if( DepthFightingOn != 0 )
	{
		glPushMatrix( );
			glRotatef( 90.,   0., 1., 0. );
			glCallList( BoxList );
		glPopMatrix( );
	}
#endif

	// draw some gratuitous text that just rotates on top of the scene:

	
	// swap the double-buffered framebuffers:

	glutSwapBuffers( );

	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !

	glFlush( );
}

void
DoDistort(int id)
{
	DistortOn = id;
	glutSetWindow(MainWindow);
	glutPostRedisplay();
}

void
DoViewMenu(int id)
{
	ViewMode = id;
	glutSetWindow( MainWindow );
	glutPostRedisplay();
}


void
DoAxesMenu( int id )
{
	AxesOn = id;
	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoColorMenu( int id )
{
	WhichColor = id - RED;
	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDebugMenu( int id )
{
	DebugOn = id;
	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthBufferMenu( int id )
{
	DepthBufferOn = id;
	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthFightingMenu( int id )
{
	DepthFightingOn = id;
	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthMenu( int id )
{
	DepthCueOn = id;
	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// main menu callback:

void
DoMainMenu( int id )
{
	switch( id )
	{
		case RESET:
			Reset( );
			break;

		case QUIT:
			// gracefully close out the graphics:
			// gracefully close the graphics window:
			// gracefully exit the program:
			glutSetWindow( MainWindow );
			glFinish( );
			glutDestroyWindow( MainWindow );
			exit( 0 );
			break;

		default:
			fprintf( stderr, "Don't know what to do with Main Menu ID %d\n", id );
	}

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoProjectMenu( int id )
{
	WhichProjection = id;
	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// use glut to display a string of characters using a raster font:

void
DoRasterString( float x, float y, float z, char *s )
{
	glRasterPos3f( (GLfloat)x, (GLfloat)y, (GLfloat)z );
	char c;			// one character to print
	for( ; ( c = *s ) != '\0'; s++ )
	{
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, c );
	}
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString( float x, float y, float z, float ht, char *s )
{
	glPushMatrix( );
		glTranslatef( (GLfloat)x, (GLfloat)y, (GLfloat)z );
		float sf = ht / ( 119.05f + 33.33f );
		glScalef( (GLfloat)sf, (GLfloat)sf, (GLfloat)sf );
		char c;			// one character to print
		for( ; ( c = *s ) != '\0'; s++ )
		{
			glutStrokeCharacter( GLUT_STROKE_ROMAN, c );
		}
	glPopMatrix( );
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds( )
{
	// get # of milliseconds since the start of the program:
	int ms = glutGet( GLUT_ELAPSED_TIME );

	// convert it to seconds:
	return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus( )
{
	glutSetWindow( MainWindow );

	int numColors = sizeof( Colors ) / ( 3*sizeof(int) );
	int colormenu = glutCreateMenu( DoColorMenu );
	for( int i = 0; i < numColors; i++ )
	{
		glutAddMenuEntry( ColorNames[i], i );
	}
	
	int distortmenu = glutCreateMenu(DoDistort);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int viewmodemenu = glutCreateMenu(DoViewMenu);
	glutAddMenuEntry("INSIDE", 1 );
	glutAddMenuEntry("OUTSIDE", 0);

	int axesmenu = glutCreateMenu( DoAxesMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthcuemenu = glutCreateMenu( DoDepthMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthbuffermenu = glutCreateMenu( DoDepthBufferMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthfightingmenu = glutCreateMenu( DoDepthFightingMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int debugmenu = glutCreateMenu( DoDebugMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int projmenu = glutCreateMenu( DoProjectMenu );
	glutAddMenuEntry( "Orthographic",  ORTHO );
	glutAddMenuEntry( "Perspective",   PERSP );

	int mainmenu = glutCreateMenu( DoMainMenu );
	glutAddSubMenu(   "Distort",       distortmenu);
	glutAddSubMenu(   "View Mode",     viewmodemenu);
	glutAddSubMenu(   "Axes",          axesmenu);
	glutAddSubMenu(   "Colors",        colormenu);

#ifdef DEMO_DEPTH_BUFFER
	glutAddSubMenu(   "Depth Buffer",  depthbuffermenu);
#endif

#ifdef DEMO_Z_FIGHTING
	glutAddSubMenu(   "Depth Fighting",depthfightingmenu);
#endif

	glutAddSubMenu(   "Depth Cue",     depthcuemenu);
	glutAddSubMenu(   "Projection",    projmenu );
	glutAddMenuEntry( "Reset",         RESET );
	glutAddSubMenu(   "Debug",         debugmenu);
	glutAddMenuEntry( "Quit",          QUIT );

// attach the pop-up menu to the right mouse button:

	glutAttachMenu( GLUT_RIGHT_BUTTON );
}



// initialize the glut and OpenGL libraries:
//	also setup display lists and callback functions

void
InitGraphics( )
{
	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	// set the initial window configuration:

	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT );

	// open the window and set its title:

	MainWindow = glutCreateWindow( WINDOWTITLE );
	glutSetWindowTitle( WINDOWTITLE );

	// set the framebuffer clear values:

	glClearColor( BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3] );

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow( MainWindow );
	glutDisplayFunc( Display );
	glutReshapeFunc( Resize );
	glutKeyboardFunc( Keyboard );
	glutMouseFunc( MouseButton );
	glutMotionFunc( NULL );
	glutPassiveMotionFunc(NULL);
	//glutPassiveMotionFunc( NULL );
	glutVisibilityFunc( Visibility );
	glutEntryFunc( NULL );
	glutSpecialFunc( NULL );
	glutSpaceballMotionFunc( NULL );
	glutSpaceballRotateFunc( NULL );
	glutSpaceballButtonFunc( NULL );
	glutButtonBoxFunc( NULL );
	glutDialsFunc( NULL );
	glutTabletMotionFunc( NULL );
	glutTabletButtonFunc( NULL );
	glutMenuStateFunc( NULL );
	glutTimerFunc( -1, NULL, 0 );
	glutIdleFunc( Animate );

	// init glew (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit( );
	if( err != GLEW_OK )
	{
		fprintf( stderr, "glewInit Error\n" );
	}
	else
		fprintf( stderr, "GLEW initialized OK\n" );
	fprintf( stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

	//grab texture from directory
	TextureArray = BmpToTexture("squid_smile.bmp", &width, &height);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &Texture);

	glBindTexture(GL_TEXTURE_2D, Texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureArray);

	glMatrixMode(GL_TEXTURE);
}

float
RandomNum() {
	//generates a number between 0.00 to 1.00
	float start = 0.00f;
	float end = 1.00f;
	float num = start + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (end - start)));
	//round the decimal to 2 places
	num = (float)((int)(num * 100 + 0.5)) / 100;
	return num;
}

// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists( )
{
	glutSetWindow( MainWindow );

	//map
	Map = glGenLists(1);
	glNewList(Map, GL_COMPILE);
	glBegin(GL_QUADS);
	//level 1
	glVertex3f(-2, 2, 6); glVertex3f(-2, -2, 6); glVertex3f(-6, -2, 6); glVertex3f(-6, 2, 6);
	glVertex3f(-6, 2, 6); glVertex3f(-6, -2, 6); glVertex3f(-6, -2, -6); glVertex3f(-6, 2, -6);
	glVertex3f(-6, 2, -6); glVertex3f(-6, -2, -6); glVertex3f(6, -2, -6); glVertex3f(6, 2, -6);
	glVertex3f(6, 2, -6); glVertex3f(6, -2, -6); glVertex3f(6, -2, 6); glVertex3f(6, 2, 6);
	glVertex3f(6, 2, 6); glVertex3f(6, -2, 6); glVertex3f(2, -2, 6); glVertex3f(2, 2, 6);
	//level 2
	glVertex3f(2, 2, 6); glVertex3f(2, -2, 6); glVertex3f(2, -2, 10); glVertex3f(2, 2, 10);
	glVertex3f(-6, 2, -2); glVertex3f(-6, -2, -2); glVertex3f(-10, -2, -2); glVertex3f(-10, 2, -2);
	glVertex3f(-10, 2, 10); glVertex3f(-10, -2, 10); glVertex3f(10, -2, 10); glVertex3f(10, 2, 10);
	glVertex3f(10, 2, 10); glVertex3f(10, -2, 10); glVertex3f(10, -2, -10); glVertex3f(10, 2, -10);
	glVertex3f(10, 2, -10); glVertex3f(10, -2, -10); glVertex3f(6, -2, -10); glVertex3f(6, 2, -10);
	glVertex3f(2, 2, -10); glVertex3f(2, -2, -10); glVertex3f(-10, -2, -10); glVertex3f(-10, 2, -10);
	glVertex3f(-10, 2, -10); glVertex3f(-10, -2, -10); glVertex3f(-10, -2, -2); glVertex3f(-10, 2, -2);
	glVertex3f(-10, 2, 2); glVertex3f(-10, -2, 2); glVertex3f(-10, -2, 10); glVertex3f(-10, 2, 10);
	//level 3
	glVertex3f(2, 2, 10); glVertex3f(2, -2, 10); glVertex3f(2, -2, 14); glVertex3f(2, 2, 14);
	glVertex3f(-10, 2, 2); glVertex3f(-10, -2, 2); glVertex3f(-14, -2, 2); glVertex3f(-14, 2, 2);
	glVertex3f(-14, 2, 14); glVertex3f(-14, -2, 14); glVertex3f(-10, -2, 14); glVertex3f(-10, 2, 14);
	glVertex3f(-6, 2, 14); glVertex3f(-6, -2, 14); glVertex3f(2, -2, 14); glVertex3f(2, 2, 14);
	glVertex3f(6, 2, 14); glVertex3f(6, -2, 14); glVertex3f(14, -2, 14); glVertex3f(14, 2, 14);
	glVertex3f(14, 2, 14); glVertex3f(14, -2, 14); glVertex3f(14, -2, -14); glVertex3f(14, 2, -14);
	glVertex3f(14, 2, -14); glVertex3f(14, -2, -14); glVertex3f(-14, -2, -14); glVertex3f(-14, 2, -14);
	glVertex3f(-14, 2, -14); glVertex3f(-14, -2, -14); glVertex3f(-14, -2, 14); glVertex3f(-14, 2, 14);
	//level 4
	glVertex3f(14, 2, 10); glVertex3f(14, -2, 10); glVertex3f(18, -2, 10); glVertex3f(18, 2, 10);
	glVertex3f(-10, 2, -14); glVertex3f(-10, -2, -14); glVertex3f(-10, -2, -18); glVertex3f(-10, 2, -18);
	glVertex3f(-18, 2, 18); glVertex3f(-18, -2, 18); glVertex3f(18, -2, 18); glVertex3f(18, 2, 18);
	glVertex3f(18, 2, 18); glVertex3f(18, -2, 18); glVertex3f(18, -2, -2); glVertex3f(18, 2, -2);
	glVertex3f(18, 2, -6); glVertex3f(18, -2, -6); glVertex3f(18, -2, -18); glVertex3f(18, 2, -18);
	glVertex3f(18, 2, -18); glVertex3f(18, -2, -18); glVertex3f(2, -2, -18); glVertex3f(2, 2, -18);
	glVertex3f(-2, 2, -18); glVertex3f(-2, -2, -18); glVertex3f(-18, -2, -18); glVertex3f(-18, 2, -18);
	glVertex3f(-18, 2, -18); glVertex3f(-18, -2, -18); glVertex3f(-18, -2, -6); glVertex3f(-18, 2, -6);
	glVertex3f(-18, 2, -2); glVertex3f(-18, -2, -2); glVertex3f(-18, -2, 6); glVertex3f(-18, 2, 6);
	glVertex3f(-18, 2, 10); glVertex3f(-18, -2, 10); glVertex3f(-18, -2, 18); glVertex3f(-18, 2, 18);
	//level 5
	glVertex3f(18, 2, -6); glVertex3f(18, -2, -6); glVertex3f(22, -2, -6); glVertex3f(22, 2, -6);
	glVertex3f(-2, 2, -18); glVertex3f(-2, -2, -18); glVertex3f(-2, -2, -22); glVertex3f(-2, 2, -22);
	glVertex3f(-18, 2, 6); glVertex3f(-18, -2, 6); glVertex3f(-22, -2, 6); glVertex3f(-22, 2, 6);
	
	glVertex3f(-22, 2, 22); glVertex3f(-22, -2, 22); glVertex3f(22, -2, 22); glVertex3f(22, 2, 22);
	glVertex3f(22, 2, 22); glVertex3f(22, -2, 22); glVertex3f(22, -2, -18); glVertex3f(22, 2, -18);
	glVertex3f(22, 2, -22); glVertex3f(22, -2, -22); glVertex3f(-22, -2, -22); glVertex3f(-22, 2, -22);
	glVertex3f(-22, 2, -22); glVertex3f(-22, -2, -22); glVertex3f(-22, -2, 22); glVertex3f(-22, 2, 22);
	glEnd();
	glEndList();

	OBJfile = glGenLists(1);
	glNewList(OBJfile, GL_COMPILE);
	LoadObjFile("bunny.obj");
	glEndList();

	// create the axes:
	AxesList = glGenLists( 1 );
	glNewList( AxesList, GL_COMPILE );
		glLineWidth( AXES_WIDTH );
			Axes( 1.5 );
		glLineWidth( 1. );
	glEndList( );
}


// the keyboard callback:

void
Keyboard( unsigned char c, int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Keyboard: '%c' (0x%0x)\n", c, c );

	switch( c )
	{
		case 'v':
		case 'V':
			ViewMode = !ViewMode;
			break;
		case '0':
			Light0On = !Light0On;
			break;
		case 'u':
		case 'U':
			TextureOn = !TextureOn;
			break;
		case 'i':
		case 'I':
			lighting = !lighting;
			break;
		case 'o':
		case 'O':
			WhichProjection = ORTHO;
			break;
		case 'p':
		case 'P':
			WhichProjection = PERSP;
			break;
		//turn left
		case 'a':
		case 'A':
			Yrot -= 45;
			break;
		//turn right
		case 'd':
		case 'D':
			Yrot += 45;
			break;
		//movement
		case 'w':
		case 'W':
			player_mov = true;
			break;
		case 'f':
		case 'F':
			Frozen = !Frozen;
			if (Frozen)
				glutIdleFunc(NULL);
			else
				glutIdleFunc(Animate);
			break;

		case 'q':
		case 'Q':
		case ESCAPE:
			DoMainMenu( QUIT );	// will not return here
			break;				// happy compiler

		default:
			fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );
	}

	// force a call to Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// called when the mouse button transitions down or up:

void
MouseButton( int button, int state, int x, int y )
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if( DebugOn != 0 )
		fprintf( stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y );

	// get the proper button bit mask:

	switch( button )
	{
		case GLUT_LEFT_BUTTON:
			b = LEFT;		break;

		case GLUT_MIDDLE_BUTTON:
			b = MIDDLE;		break;

		case GLUT_RIGHT_BUTTON:
			b = RIGHT;		break;

		case SCROLL_WHEEL_UP:
			Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
			// keep object from turning inside-out or disappearing:
			if (Scale < MINSCALE)
				Scale = MINSCALE;
			break;

		case SCROLL_WHEEL_DOWN:
			Scale -= SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
			// keep object from turning inside-out or disappearing:
			if (Scale < MINSCALE)
				Scale = MINSCALE;
			break;

		default:
			b = 0;
			fprintf( stderr, "Unknown mouse button: %d\n", button );
	}

	// button down sets the bit, up clears the bit:

	if( state == GLUT_DOWN )
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();

}


// called when the mouse moves while a button is down:

void
MouseMotion( int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "MouseMotion: %d, %d\n", x, y );

	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if( ( ActiveButton & LEFT ) != 0 )
	{
		Xrot += ( ANGFACT*dy );
		Yrot += ( ANGFACT*dx );
	}

	if( ( ActiveButton & MIDDLE ) != 0 )
	{
		Scale += SCLFACT * (float) ( dx - dy );

		// keep object from turning inside-out or disappearing:

		if( Scale < MINSCALE )
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset( )
{
	Light0On = true;
	TextureOn = 1;
	DistortOn = 0;
	lighting = 1;
	ActiveButton = 0;
	AxesOn = 1;
	DebugOn = 0;
	DepthBufferOn = 1;
	DepthFightingOn = 0;
	DepthCueOn = 0;
	Scale  = 1.0;
	WhichColor = WHITE;
	WhichProjection = PERSP;
	Xrot = Yrot = 0;
	Frozen = 0;
	player_mov = false;
	player_Xpos = 0, player_Zpos = 0;
	fish1 = 0, fish2 = 0, fish3 = 0, fish4 = 0, fish5 = 0;
}


// called when user resizes the window:

void
Resize( int width, int height )
{
	if( DebugOn != 0 )
		fprintf( stderr, "ReSize: %d, %d\n", width, height );

	// don't really need to do anything since the window size is
	// checked each time in Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// handle a change to the window's visibility:

void
Visibility ( int state )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Visibility: %d\n", state );

	if( state == GLUT_VISIBLE )
	{
		glutSetWindow( MainWindow );
		glutPostRedisplay( );
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[ ] = {
		0.f, 1.f, 0.f, 1.f
	      };

static float xy[ ] = {
		-.5f, .5f, .5f, -.5f
	      };

static int xorder[ ] = {
		1, 2, -3, 4
		};

static float yx[ ] = {
		0.f, 0.f, -.5f, .5f
	      };

static float yy[ ] = {
		0.f, .6f, 1.f, 1.f
	      };

static int yorder[ ] = {
		1, 2, 3, -2, 4
		};

static float zx[ ] = {
		1.f, 0.f, 1.f, 0.f, .25f, .75f
	      };

static float zy[ ] = {
		.5f, .5f, -.5f, -.5f, 0.f, 0.f
	      };

static int zorder[ ] = {
		1, 2, 3, 4, -5, 6
		};

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes( float length )
{
	glBegin( GL_LINE_STRIP );
		glVertex3f( length, 0., 0. );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., length, 0. );
	glEnd( );
	glBegin( GL_LINE_STRIP );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., 0., length );
	glEnd( );

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 4; i++ )
		{
			int j = xorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( base + fact*xx[j], fact*xy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 5; i++ )
		{
			int j = yorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( fact*yx[j], base + fact*yy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 6; i++ )
		{
			int j = zorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( 0.0, fact*zy[j], base + fact*zx[j] );
		}
	glEnd( );

}

// read a BMP file into a Texture:

#define VERBOSE		false
#define BMP_MAGIC_NUMBER	0x4d42
#ifndef BI_RGB
#define BI_RGB			0
#define BI_RLE8			1
#define BI_RLE4			2
#endif


// bmp file header:
struct bmfh
{
	short bfType;		// BMP_MAGIC_NUMBER = "BM"
	int bfSize;		// size of this file in bytes
	short bfReserved1;
	short bfReserved2;
	int bfOffBytes;		// # bytes to get to the start of the per-pixel data
} FileHeader;

// bmp info header:
struct bmih
{
	int biSize;		// info header size, should be 40
	int biWidth;		// image width
	int biHeight;		// image height
	short biPlanes;		// #color planes, should be 1
	short biBitCount;	// #bits/pixel, should be 1, 4, 8, 16, 24, 32
	int biCompression;	// BI_RGB, BI_RLE4, BI_RLE8
	int biSizeImage;
	int biXPixelsPerMeter;
	int biYPixelsPerMeter;
	int biClrUsed;		// # colors in the palette
	int biClrImportant;
} InfoHeader;

// read a BMP file into a Texture:

unsigned char *
BmpToTexture( char *filename, int *width, int *height )
{
	FILE *fp;
#ifdef _WIN32
        errno_t err = fopen_s( &fp, filename, "rb" );
        if( err != 0 )
        {
		fprintf( stderr, "Cannot open Bmp file '%s'\n", filename );
		return NULL;
        }
#else
	FILE *fp = fopen( filename, "rb" );
	if( fp == NULL )
	{
		fprintf( stderr, "Cannot open Bmp file '%s'\n", filename );
		return NULL;
	}
#endif

	FileHeader.bfType = ReadShort( fp );

	// if bfType is not BMP_MAGIC_NUMBER, the file is not a bmp:

	if( VERBOSE ) fprintf( stderr, "FileHeader.bfType = 0x%0x = \"%c%c\"\n",
			FileHeader.bfType, FileHeader.bfType&0xff, (FileHeader.bfType>>8)&0xff );
	if( FileHeader.bfType != BMP_MAGIC_NUMBER )
	{
		fprintf( stderr, "Wrong type of file: 0x%0x\n", FileHeader.bfType );
		fclose( fp );
		return NULL;
	}

	FileHeader.bfSize = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "FileHeader.bfSize = %d\n", FileHeader.bfSize );

	FileHeader.bfReserved1 = ReadShort( fp );
	FileHeader.bfReserved2 = ReadShort( fp );

	FileHeader.bfOffBytes = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "FileHeader.bfOffBytes = %d\n", FileHeader.bfOffBytes );

	InfoHeader.biSize = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biSize = %d\n", InfoHeader.biSize );
	InfoHeader.biWidth = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biWidth = %d\n", InfoHeader.biWidth );
	InfoHeader.biHeight = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biHeight = %d\n", InfoHeader.biHeight );

	const int nums = InfoHeader.biWidth;
	const int numt = InfoHeader.biHeight;

	InfoHeader.biPlanes = ReadShort( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biPlanes = %d\n", InfoHeader.biPlanes );

	InfoHeader.biBitCount = ReadShort( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biBitCount = %d\n", InfoHeader.biBitCount );

	InfoHeader.biCompression = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biCompression = %d\n", InfoHeader.biCompression );

	InfoHeader.biSizeImage = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biSizeImage = %d\n", InfoHeader.biSizeImage );

	InfoHeader.biXPixelsPerMeter = ReadInt( fp );
	InfoHeader.biYPixelsPerMeter = ReadInt( fp );

	InfoHeader.biClrUsed = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biClrUsed = %d\n", InfoHeader.biClrUsed );

	InfoHeader.biClrImportant = ReadInt( fp );

	// fprintf( stderr, "Image size found: %d x %d\n", ImageWidth, ImageHeight );

	// pixels will be stored bottom-to-top, left-to-right:
	unsigned char *texture = new unsigned char[ 3 * nums * numt ];
	if( texture == NULL )
	{
		fprintf( stderr, "Cannot allocate the texture array!\n" );
		return NULL;
	}

	// extra padding bytes:

	int requiredRowSizeInBytes = 4 * ( ( InfoHeader.biBitCount*InfoHeader.biWidth + 31 ) / 32 );
	if( VERBOSE )	fprintf( stderr, "requiredRowSizeInBytes = %d\n", requiredRowSizeInBytes );

	int myRowSizeInBytes = ( InfoHeader.biBitCount*InfoHeader.biWidth + 7 ) / 8;
	if( VERBOSE )	fprintf( stderr, "myRowSizeInBytes = %d\n", myRowSizeInBytes );

	int oldNumExtra =  4*(( (3*InfoHeader.biWidth)+3)/4) - 3*InfoHeader.biWidth;
	if( VERBOSE )	fprintf( stderr, "Old NumExtra padding = %d\n", oldNumExtra );

	int numExtra = requiredRowSizeInBytes - myRowSizeInBytes;
	if( VERBOSE )	fprintf( stderr, "New NumExtra padding = %d\n", numExtra );

	// this function does not support compression:

	if( InfoHeader.biCompression != 0 )
	{
		fprintf( stderr, "Wrong type of image compression: %d\n", InfoHeader.biCompression );
		fclose( fp );
		return NULL;
	}
	
	// we can handle 24 bits of direct color:
	if( InfoHeader.biBitCount == 24 )
	{
		rewind( fp );
		fseek( fp, FileHeader.bfOffBytes, SEEK_SET );
		int t;
		unsigned char *tp;
		for( t = 0, tp = texture; t < numt; t++ )
		{
			for( int s = 0; s < nums; s++, tp += 3 )
			{
				*(tp+2) = fgetc( fp );		// b
				*(tp+1) = fgetc( fp );		// g
				*(tp+0) = fgetc( fp );		// r
			}

			for( int e = 0; e < numExtra; e++ )
			{
				fgetc( fp );
			}
		}
	}

	// we can also handle 8 bits of indirect color:
	if( InfoHeader.biBitCount == 8 && InfoHeader.biClrUsed == 256 )
	{
		struct rgba32
		{
			unsigned char r, g, b, a;
		};
		struct rgba32 *colorTable = new struct rgba32[ InfoHeader.biClrUsed ];

		rewind( fp );
		fseek( fp, sizeof(struct bmfh) + InfoHeader.biSize - 2, SEEK_SET );
		for( int c = 0; c < InfoHeader.biClrUsed; c++ )
		{
			colorTable[c].r = fgetc( fp );
			colorTable[c].g = fgetc( fp );
			colorTable[c].b = fgetc( fp );
			colorTable[c].a = fgetc( fp );
			if( VERBOSE )	fprintf( stderr, "%4d:\t0x%02x\t0x%02x\t0x%02x\t0x%02x\n",
				c, colorTable[c].r, colorTable[c].g, colorTable[c].b, colorTable[c].a );
		}

		rewind( fp );
		fseek( fp, FileHeader.bfOffBytes, SEEK_SET );
		int t;
		unsigned char *tp;
		for( t = 0, tp = texture; t < numt; t++ )
		{
			for( int s = 0; s < nums; s++, tp += 3 )
			{
				int index = fgetc( fp );
				*(tp+0) = colorTable[index].r;	// r
				*(tp+1) = colorTable[index].g;	// g
				*(tp+2) = colorTable[index].b;	// b
			}

			for( int e = 0; e < numExtra; e++ )
			{
				fgetc( fp );
			}
		}

		delete[ ] colorTable;
	}

	fclose( fp );

	*width = nums;
	*height = numt;
	return texture;
}

int
ReadInt( FILE *fp )
{
	const unsigned char b0 = fgetc( fp );
	const unsigned char b1 = fgetc( fp );
	const unsigned char b2 = fgetc( fp );
	const unsigned char b3 = fgetc( fp );
	return ( b3 << 24 )  |  ( b2 << 16 )  |  ( b1 << 8 )  |  b0;
}

short
ReadShort( FILE *fp )
{
	const unsigned char b0 = fgetc( fp );
	const unsigned char b1 = fgetc( fp );
	return ( b1 << 8 )  |  b0;
}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb( float hsv[3], float rgb[3] )
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while( h >= 6. )	h -= 6.;
	while( h <  0. ) 	h += 6.;

	float s = hsv[1];
	if( s < 0. )
		s = 0.;
	if( s > 1. )
		s = 1.;

	float v = hsv[2];
	if( v < 0. )
		v = 0.;
	if( v > 1. )
		v = 1.;

	// if sat==0, then is a gray:

	if( s == 0.0 )
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:
	
	float i = (float)floor( h );
	float f = h - i;
	float p = v * ( 1.f - s );
	float q = v * ( 1.f - s*f );
	float t = v * ( 1.f - ( s * (1.f-f) ) );

	float r=0., g=0., b=0.;			// red, green, blue
	switch( (int) i )
	{
		case 0:
			r = v;	g = t;	b = p;
			break;
	
		case 1:
			r = q;	g = v;	b = p;
			break;
	
		case 2:
			r = p;	g = v;	b = t;
			break;
	
		case 3:
			r = p;	g = q;	b = v;
			break;
	
		case 4:
			r = t;	g = p;	b = v;
			break;
	
		case 5:
			r = v;	g = p;	b = q;
			break;
	}

	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}

void
Cross(float v1[3], float v2[3], float vout[3])
{
	float tmp[3];
	tmp[0] = v1[1] * v2[2] - v2[1] * v1[2];
	tmp[1] = v2[0] * v1[2] - v1[0] * v2[2];
	tmp[2] = v1[0] * v2[1] - v2[0] * v1[1];
	vout[0] = tmp[0];
	vout[1] = tmp[1];
	vout[2] = tmp[2];
}

float
Dot(float v1[3], float v2[3])
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

float
Unit(float vin[3], float vout[3])
{
	float dist = vin[0] * vin[0] + vin[1] * vin[1] + vin[2] * vin[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		vout[0] = vin[0] / dist;
		vout[1] = vin[1] / dist;
		vout[2] = vin[2] / dist;
	}
	else
	{
		vout[0] = vin[0];
		vout[1] = vin[1];
		vout[2] = vin[2];
	}
	return dist;
}