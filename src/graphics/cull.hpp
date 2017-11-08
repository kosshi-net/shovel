#include <cmath>

namespace Culler {

	float*createPlaneBuffer();
	
	void extractPlanes(float* M, float*planes);

	bool frustumcull(float*planes, float*loc, float radius);
}