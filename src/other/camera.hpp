// Header only file

#include <glm/vec3.hpp>

class Camera {
	public:
		glm::vec3 loc;
		double yaw;
		double pitch;

		Camera(glm::vec3 vec, double y, double x) : loc(vec), yaw(y), pitch(x) {}
};

