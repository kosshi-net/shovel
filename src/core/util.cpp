#include <cmath>
#define PI 3.14159265
float degToRad (float degrees) {
	return fmod(degrees, 360.0f) * PI / 180.0f;
};