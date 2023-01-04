#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <vector>
#include "glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"

#pragma once

// delimiters for parsing the obj file:

#define OBJDELIMS		" \t"

struct Vertex {float x, y, z;};
struct Normal {float nx, ny, nz;};
struct TextureCoord {float s, t, p;};
struct face {int v, n, t;};

int		LoadObjFile(char* name);
void	Cross(float[3], float[3], float[3]);
char*	ReadRestOfLine(FILE*);
void	ReadObjVTN(char*, int*, int*, int*);
float	Unit(float[3]);
float	Unit(float[3], float[3]);