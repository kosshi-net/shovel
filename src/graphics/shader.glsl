<shader chunkVertSrc/>

	#version 100

	precision mediump float;

	attribute vec3 aVertex;
	attribute vec3 aColor;

	varying vec3 vColor;

	uniform mat4 uMP;
	uniform mat4 uMV;

	uniform vec3 uFogColor;

	void main() {


		float dist = length( uMV * vec4( aVertex, 1.0) );
		float vFogFactor = 1.0 - pow( clamp( (dist) / 350.0, 0.0, 1.0), 1.1);

		vColor = mix(uFogColor, aColor, vFogFactor);
		
	 	gl_Position = uMP*uMV*vec4(aVertex, 1.0);
	};

<shader chunkFragSrc/>

	#version 100

	precision mediump float;

	varying vec3 vColor;



	void main() {

		gl_FragColor = vec4(vColor, 1.0);
	};

<shader textVertSrc/>

	#version 120

	attribute vec4 aCoord;
	varying vec2 texcoord;

	void main(void) {
	  gl_Position = vec4(aCoord.xy, 0, 1);
	  texcoord = aCoord.zw;
	}

<shader textFragSrc/>

	#version 120

	varying vec2 texcoord;
	uniform sampler2D uTex;
	uniform vec4 uColor;

	void main(void) {
		gl_FragColor = vec4(1, 1, 1, texture2D(uTex, texcoord).r)*uColor;
		// gl_FragColor = vec4(1,1,1,1);
	}


<shader boxVertSrc/>

	#version 100
	precision mediump float;

	attribute vec3 aVertex;

	uniform vec3 uColor;
	varying vec3 vColor;

	uniform mat4 uMP;
	uniform mat4 uMV;

	void main() {
		vColor = uColor;
	 	gl_Position = uMP*uMV*vec4(aVertex, 1.0);
	};

<shader boxFragSrc/>

	#version 100
	precision mediump float;

	varying vec3 vColor;

	void main() {
		gl_FragColor = vec4(vColor, 1.0);
	};
