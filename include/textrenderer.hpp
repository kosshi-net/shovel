
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>

namespace krdr {



	GLuint createShader(const char vertSrc[], const char fragSrc[]);

	void writePPM(const int width, const int height, unsigned char*buffer){
	
		FILE *fp = fopen("file.ppm", "wb"); /* b - binary mode */

		(void) fprintf(fp, "P6\n%d %d\n255\n", width, height);

		for (int i = 0; i < width*height; ++i){
      		static unsigned char color[3];
			color[0] = buffer[i];
			color[1] = buffer[i];
			color[2] = buffer[i];
			(void) fwrite(color, 1, 3, fp);
		}
		
		(void) fclose(fp);
	};

	FT_Library ft;
	FT_Face face;
	FT_GlyphSlot g;

	GLuint textShader;
	GLuint textShader_aCoord;

	GLuint textShader_uTex;

	GLuint textShader_uColor;

	GLuint text_vbo;
	GLuint text_tex;


	struct {

		int width;
		int height;

		int x;
		int y;

		int left;
		int top;

		int advance;

	} AChar[128];

	int atlasHeight = 0;
	int atlasWidth = 0;

	unsigned char*atlas;

	float* geometryBuffer;

	int initTextRenderer(){

		if(FT_Init_FreeType(&ft)) {
			fprintf(stderr, "Could not init freetype library\n");
			MessageBox(0, "FreeType failed to initialize.", "Error!", 0);
			return -1;
		}

		if(FT_New_Face(ft, "Consolas.ttf", 0, &face)) {
			fprintf(stderr, "Could not open font\n");
			MessageBox(0, "Could not open font!", "Error!", 0);
			return -1;
		}

		FT_Set_Pixel_Sizes(face, 0, 20 );
		
		if(FT_Load_Char(face, 'X', FT_LOAD_RENDER)) {
			fprintf(stderr, "Could not load character 'X'\n");
			MessageBox(0, "FreeType is not working!", "Error!", 0);
			return -1;
		}

		
		g = face->glyph;

        // (face->glyph->bitmap.width, face->glyph->bitmap.rows)
        // (face->glyph->bitmap_left, face->glyph->bitmap_top)
        // face->glyph->advance.x


		int total_x = 0;
		int max_y = 0;
		for (int i = 0; i < 128; ++i)
		{
			if(FT_Load_Char(face, i, FT_LOAD_RENDER))
				continue;

			AChar[i].width  = g->bitmap.width;
			AChar[i].height = g->bitmap.rows;

			AChar[i].left = g->bitmap_left;
			AChar[i].top = g->bitmap_top;

			AChar[i].advance = g->advance.x/64;

			total_x += AChar[i].advance;
			max_y = std::max( AChar[i].height, max_y );

			AChar[i].x = total_x;
			AChar[i].y = 0;
		}



		atlasHeight = max_y;
		atlasWidth  = total_x;

		atlas = (unsigned char*) calloc(atlasHeight*atlasWidth, 1);

		for (int i = 0; i < 128; ++i)
		{
			if(FT_Load_Char(face, i, FT_LOAD_RENDER))
				continue;

			for (int y = 0; y < AChar[i].height; ++y)
			for (int x = 0; x < AChar[i].width;  ++x)
			{
				atlas[ atlasWidth * y + AChar[i].x + x ] = face->glyph->bitmap.buffer[ AChar[i].width * y + x];	
			}
			

		}

		writePPM(atlasWidth, atlasHeight, atlas);
		// for (int i = 0; i < atlasWidth*atlasHeight; ++i)
		// {
		// 	printf( "%i", atlas[i]);
		// }

		printf("CharAtlas: %i %i\n", atlasWidth, atlasHeight);

		geometryBuffer = (float*)malloc( 256 * sizeof(float)*4*4 );


		textShader = createShader( textVertSrc, textFragSrc );
		glUseProgram(textShader);
		textShader_aCoord =  glGetAttribLocation (textShader, "aCoord");
		textShader_uTex = glGetUniformLocation(textShader, "uTex");
		textShader_uColor =  glGetUniformLocation(textShader, "uColor");

		GLuint text_tex;
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &text_tex);
		glBindTexture(GL_TEXTURE_2D, text_tex);
		glUniform1i(textShader_uTex, 0);
		
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RED, atlasWidth, atlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, atlas
		);

		glGenBuffers(1, &text_vbo);
		glEnableVertexAttribArray(textShader_aCoord);
		glBindBuffer(GL_ARRAY_BUFFER, text_vbo);
		glVertexAttribPointer(textShader_aCoord, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glDisableVertexAttribArray(textShader_aCoord);

		return 1;
	}


	void drawText(const char *text, float x, float y, float sx, float sy, int fontSize) {
		glUseProgram(textShader);
		glBindBuffer(GL_ARRAY_BUFFER, text_vbo);
		glEnableVertexAttribArray(textShader_aCoord);

		// FT_Set_Pixel_Sizes(face, 0, fontSize);
		GLfloat black[4] = {0, 0, 0, 1};
		glUniform4fv(textShader_uColor, 1, black);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// glActiveTexture(GL_TEXTURE0);
		// glBindTexture(GL_TEXTURE_2D, text_tex);
		// glUniform1i(textShader_uTex, 0);

		// glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		const char *p;

		int bufIndex = 0;
		int count = 0;

		for(p = text; *p; p++) {


			int id = *p;
	 	
			// glTexImage2D(
			// 	GL_TEXTURE_2D, 0, GL_RED, g->bitmap.width, g->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer
			// );
	 

			float x2 = x + AChar[id].left * sx;
			float y2 = -y - AChar[id].top * sy;
			float w = AChar[id].width * sx;
			float h = AChar[id].height * sy;

			float uv[4] = {
	 			AChar[id].x / (float)atlasWidth,
	 			AChar[id].y / (float)atlasHeight,
	 			(AChar[id].x + AChar[id].width)	 / (float)atlasWidth,
	 			(AChar[id].y + AChar[id].height) / (float)atlasHeight,

			};


			GLfloat box[4*6] = {
					x2,     -y2    , uv[0], uv[1],
					x2 + w, -y2    , uv[2], uv[1],
					x2,     -y2 - h, uv[0], uv[3],

					x2 + w, -y2    , uv[2], uv[1],
					x2,     -y2 - h, uv[0], uv[3],
					x2 + w, -y2 - h, uv[2], uv[3],
			};

			for (int i = 0; i < 4*6; ++i)
			{
				geometryBuffer[bufIndex++] = box[i];
			}
			count++;
	 		

			x += (AChar[id].advance) * sx;
			// y += (g->advance.y/64) * sy;
		}

		glVertexAttribPointer(textShader_aCoord, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glBufferData(GL_ARRAY_BUFFER, bufIndex*sizeof(float), geometryBuffer, GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 6*count);

		glDisableVertexAttribArray(textShader_aCoord);

		GLenum err;
		while((err = glGetError()) != GL_NO_ERROR)
		{
			printf("OPENGL ERROR %i\n", err);
		}
	}



}