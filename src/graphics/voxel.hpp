#ifndef SHOVEL_GRAPHICS_VOXEL_INCLUDED
#define SHOVEL_GRAPHICS_VOXEL_INCLUDED 1

#include <glm/mat4x4.hpp>

namespace graphics {
	namespace voxel {
		int init();

		void setFogColor(float r, float g, float b, float a);

		void meshTick();
		void draw(glm::mat4 modelview, glm::mat4 projection);

	}
}

#endif