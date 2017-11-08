#ifndef SHOVEL_RENDERER_BOX_INCLUDED
#define SHOVEL_RENDERER_BOX_INCLUDED 1

#include <glm/mat4x4.hpp>

namespace graphics {
	namespace box {

		int init();


		void draw_wireframe_cube(glm::mat4,glm::mat4);

	}
}

#endif