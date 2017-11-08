#ifndef SHOVEL_GRAPHICS_TEXT_INCLUDED
#define SHOVEL_GRAPHICS_TEXT_INCLUDED 1

namespace graphics {
	namespace text {
		int init();

		void draw(
			const char *text, 
			float x,  float y, 
			float sx, float sy, 
			int fontSize
		);
	}
}

#endif