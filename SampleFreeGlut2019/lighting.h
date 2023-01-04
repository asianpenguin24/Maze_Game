#include <iostream>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"
#pragma once

using namespace std;

float White[];
float* Array3(float, float, float);
float* MulArray3(float, float [3]);
void SetMaterial(float, float, float, float);
void SetMaterial(float, float, float, float);
void SetPointLight(int, float, float, float, float, float, float);
void SetSpotLight(int, float, float, float, float, float, float, float, float, float);