'use strict';


let cube_vertices = [ // AS VEC3
	0,0,0,
	1,0,0,
	1,1,0,
	0,1,0,

	0,0,1,
	1,0,1,
	1,1,1,
	0,1,1,
];

let face_quad_index = [ 
	1, 5, 6, 2,
	3, 2, 6, 7,
	5, 4, 7, 6,
	4, 0, 3, 7,
	4, 5, 1, 0,
	0, 1, 2, 3,
];
let quad_to_triangle_index = [
	0, 3, 1,
	2, 1, 3
]

function get_face(n, vertex_array, index_array, callcount){
	// n => 0 && n < 6
	let fqi = Array(4);

	for (let i = 0; i < 4; i++) {
		fqi[i] = face_quad_index[n * 4 + i];
	}

	for (let i = 0; i < 4; i++) {
		for (let j = 0; j < 3; j++) 
			vertex_array[ (callcount * 4 * 3) + (i * 3 + j)] = cube_vertices[ fqi[i] * 3 + j ];
	}
	
	for (let i = 0; i < 6; i++) { 
		// index_array[callcount*6 + i] = fqi[quad_to_triangle_index[i]] + callcount * 4;
		index_array[callcount*6 + i] = quad_to_triangle_index[i] + callcount * 4;
	}

	return [vertex_array, index_array];
}

function genArrays() {

	let vertex = [];
	let index = [];

	for (var i = 0; i < 6; i++) {
		get_face(i, vertex, index, i);
	}

	return {vertex: vertex, index: index};

}

function genCLUT(){
	let g = genArrays();

	let txt = "";

	txt += "{";
	for (let i = 0; i < g.vertex.length/3; i++) {

		txt += `\n ${g.vertex[i*3 + 0]}.0f, ${g.vertex[i*3 + 1]}.0f, ${g.vertex[i*3 + 2]}.0f,`;
	}
	txt += "}\n{";

	for (let i = 0; i < g.index.length/3; i++) {
		txt += `\n ${g.index[i*3 + 0]}, ${g.index[i*3 + 1]}, ${g.index[i*3 + 2]},`;
	}
	txt += "}";
	return txt;
}
