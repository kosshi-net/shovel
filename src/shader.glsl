<vertshader>
#version 100

precision mediump float;

attribute vec3 aVertex;
attribute vec3 aColor;

varying vec3 vColor;

uniform mat4 uMP;
uniform mat4 uMV;

void main() {


	float dist = length(uMV*vec4(aVertex,1.0));
	float vFogFactor = 1.0 -  pow( clamp( (dist)/(512.0), 0.0, 1.0), 1.5);

	vColor = mix(vec3(0.6, 0.6, 0.8), aColor, vFogFactor);
	
 	gl_Position = uMP*uMV*vec4(aVertex, 1.0);
};

<vertshader>
<fragshader>
#version 100

precision mediump float;

varying vec3 vColor;


void main() {

	gl_FragColor = vec4(vColor, 0.0);
};

<fragshader>