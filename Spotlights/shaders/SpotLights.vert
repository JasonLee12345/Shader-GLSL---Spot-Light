#version 330 
#extension GL_ARB_explicit_attrib_location : require 
#extension GL_ARB_explicit_uniform_location : require 

/* Strict Binding for Cross-hardware Compatability */
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 tangent;
layout(location = 3) in vec3 textureCoordinate;
layout(location = 4) in vec3 color;

/* Uniform variables for Camera and Light Direction */ 
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalMatrix;

struct Light {
	vec3 position;
	vec3 target;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float exponent;
	float cutoff;
};

uniform Light light0;
uniform Light light1;
uniform Light light2;

out vec3 interpSurfaceNormal;
out vec3 interpVertexPosition;
out vec2 interpTextureCoord;

/* Light direction in tangent space */
out vec3 light0_position_eye;
out vec3 light0_target_eye;
out vec3 lightVector0;

out vec3 light1_position_eye;
out vec3 light1_target_eye;
out vec3 lightVector1;

out vec3 light2_position_eye;
out vec3 light2_target_eye;
out vec3 lightVector2;

void main(void) {
	vec4 vPosition = vec4(position, 1.0f);
    vec3 vertexPositionEye = vec3(modelViewMatrix * vPosition);
    interpVertexPosition = vertexPositionEye;
    interpSurfaceNormal = normalize(normalMatrix * normal);
	interpTextureCoord = vec2(textureCoordinate);

	/* Translate spotlight position and target to eye coordinates */
	light0_position_eye =  vec3( modelViewMatrix * vec4(light0.position, 1.0f) );
	light0_target_eye =  vec3( modelViewMatrix * vec4(light0.target, 1.0f) );
	vec3 spotLight0_dir_eye = vec3(light0_position_eye - interpVertexPosition);

	light1_position_eye =  vec3( modelViewMatrix * vec4(light1.position, 1.0f) );
	light1_target_eye =  vec3( modelViewMatrix * vec4(light1.target, 1.0f) );
	vec3 spotLight1_dir_eye = vec3(light1_position_eye - interpVertexPosition);

	light2_position_eye =  vec3( modelViewMatrix * vec4(light2.position, 1.0f) );
	light2_target_eye =  vec3( modelViewMatrix * vec4(light2.target, 1.0f) );
	vec3 spotLight2_dir_eye = vec3(light2_position_eye - interpVertexPosition);

	//---------------------------------------------------------------------------- 
	// TBN Matrix for normal mapping.
	//---------------------------------------------------------------------------- 
	vec3 n = interpSurfaceNormal;
	vec3 t = normalize(normalMatrix * vec3(tangent.xyz));
	vec3 b = cross(n, t) * tangent.w;
	mat3 tbnMatrix = mat3(t, b, n);

	lightVector0 = normalize(spotLight0_dir_eye) * tbnMatrix;
	lightVector1 = normalize(spotLight1_dir_eye) * tbnMatrix;
	lightVector2 = normalize(spotLight2_dir_eye) * tbnMatrix;

	//-------------------------------------------------------------------------- 
	// Transform the vertex for the fragment shader. 
	//-------------------------------------------------------------------------- 
    gl_Position = projectionMatrix * modelViewMatrix * vPosition;
}