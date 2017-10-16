#ifndef SHOVEL_UTIL_INCLUDED 
#define SHOVEL_UTIL_INCLUDED 1

#define PI 3.14159265
float degToRad (float degrees) {
	return fmod(degrees, 360.0f) * PI / 180.0f;
};

#endif