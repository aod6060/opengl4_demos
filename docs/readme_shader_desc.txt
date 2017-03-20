This file contains information about how a shader description file is setup.

Here is an example

{
	"shaders": [
		{"type": "vertex", "path": "data/shaders/shader.vert"}
		{"type": "fragment", "path": "data/shaders/shader.frag"}
		{"type": "geometry", "path": "data/shaders/shader.geom"}
		{"type": "tessellation_contr", "path": "path/shaders/shader.tesc"}
		{"type": "tessellation_eval", "path": "path/shaders/shader.tese"}
	],
	"program_create_uniforms": [
		{"create_uniform": "int_test"},
		{"create_uniform": "ivec2_test"},
		{"create_uniform": "ivec3_test"},
		{"create_uniform": "ivec4_test"},
		{"create_uniform": "float_test"},
		{"create_uniform": "vec2_test"},
		{"create_uniform": "vec3_test"},
		{"create_uniform": "vec4_test"},
		{"create_uniform": "mat2_test"},
		{"create_uniform": "mat3_test"},
		{"create_uniform": "mat4_test"}
	],
	"program_set_default_uniform_values": [
		{"name": "int_test", "int": 1},
		{"name": "ivec2_test", "ivec2": [1, 1]},
		{"name": "ivec3_test", "ivec3": [1, 1, 1]},
		{"name": "ivec4_test", "ivec4": [1, 1, 1, 1]},
		{"name": "float_test", "float": 1.0},
		{"name": "vec2_test", "vec2": [1.0, 1.0]},
		{"name": "vec3_test", "vec3": [1.0, 1.0, 1.0]},
		{"name": "vec4_test", "vec4": [1.0, 1.0, 1.0, 1.0},
		{"name": "mat2_test", "mat2": [[1.0, 0.0], [0.0, 1.0]]},
		{"name": "mat3_test", "mat3": [[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]]},
		{"name": "mat4_test", "mat4": [[1.0, 0.0, 0.0, 0.0], [0.0, 1.0, 0.0, 0.0], [0.0, 0.0, 1.0, 0.0], [0.0, 0.0, 0.0, 1.0]]}
	]
}

This is pretty much how a file should be setup with default values for this file format
that is needed to simpify how shader work...