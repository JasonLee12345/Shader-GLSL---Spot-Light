#version 330 
#extension GL_ARB_explicit_attrib_location : require 
#extension GL_ARB_explicit_uniform_location : require 

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D specularTexture;

/* Uniform variables for Camera and Light Direction */ 
uniform mat4 modelViewMatrix;

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

in vec3 interpSurfaceNormal;
in vec3 interpVertexPosition;
in vec2 interpTextureCoord;

/* Light direction in tangent space */
in vec3 light0_position_eye;
in vec3 light0_target_eye;
in vec3 lightVector0;

in vec3 light1_position_eye;
in vec3 light1_target_eye;
in vec3 lightVector1;

in vec3 light2_position_eye;
in vec3 light2_target_eye;
in vec3 lightVector2;

layout( location = 0 ) out vec4 fragColor;

vec4 calculation_values() {

	vec3 fragmentNormal = normalize(texture2D(normalTexture, interpTextureCoord).xyz * 2.0 - 1.0);

	vec3 spotLight0_dir_eye =  normalize(light0_target_eye - light0_position_eye);
	vec3 spotLight0_pos_eye = light0_position_eye;

	vec3 spotLight1_dir_eye =  normalize(light1_target_eye - light1_position_eye);
	vec3 spotLight1_pos_eye = light1_position_eye;

	vec3 spotLight2_dir_eye =  normalize(light2_target_eye - light2_position_eye);
	vec3 spotLight2_pos_eye = light2_position_eye;

	vec3 l = normalize(spotLight0_pos_eye - interpVertexPosition);
	vec3 c = normalize(-interpVertexPosition);
	vec3 r = normalize(-reflect(l, interpSurfaceNormal));

	vec3 ll = normalize(spotLight1_pos_eye - interpVertexPosition);
	vec3 rr = normalize(-reflect(ll, interpSurfaceNormal));

	vec3 lll = normalize(spotLight2_pos_eye - interpVertexPosition);
	vec3 rrr = normalize(-reflect(lll, interpSurfaceNormal));

	float angle0 = acos( dot(spotLight0_dir_eye, -l) );
	float cutoff0 = radians( clamp(light0.cutoff, 0.0, 90.0) );

	float angle1 = acos( dot(spotLight1_dir_eye, -ll) );
	float cutoff1 = radians( clamp(light1.cutoff, 0.0, 90.0) );

	float angle2 = acos( dot(spotLight2_dir_eye, -lll) );
	float cutoff2 = radians( clamp(light2.cutoff, 0.0, 90.0) );

	//-------------------------------------------------------------------------- 
    // Light and material properties.
    //-------------------------------------------------------------------------- 
    vec4 Ia0 = vec4(light0.ambient, 1.0f);
    vec4 Id0 = vec4(light0.diffuse, 1.0f);
    vec4 Is0 = vec4(light0.specular, 1.0f);

	vec4 Ia1 = vec4(light1.ambient, 1.0f);
    vec4 Id1 = vec4(light1.diffuse, 1.0f);
    vec4 Is1 = vec4(light1.specular, 1.0f);

	vec4 Ia2 = vec4(light2.ambient, 1.0f);
    vec4 Id2 = vec4(light2.diffuse, 1.0f);
    vec4 Is2 = vec4(light2.specular, 1.0f);
 
    vec4 Ka = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    vec4 Kd = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    vec4 Ks = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	float shininess = 16.0f;
 
    vec4 Iambient = vec4(0.0f);
    vec4 Idiffuse = vec4(0.0f);
    vec4 Ispecular = vec4(0.0f);

	vec4 Iambient1 = vec4(0.0f);
    vec4 Idiffuse1 = vec4(0.0f);
    vec4 Ispecular1 = vec4(0.0f);

	vec4 Iambient2 = vec4(0.0f);
    vec4 Idiffuse2 = vec4(0.0f);
    vec4 Ispecular2 = vec4(0.0f);

	//-------------------------------------------------------------------------- 
	// Assigning actual color values.
	//-------------------------------------------------------------------------- 
	Iambient = 0.2 * (Ia0 * Ka);
	Iambient1 = 0.2 * (Ia1 * Ka);
	Iambient2 = 0.2 * (Ia2 * Ka);
	
	if (angle0 < cutoff0) {
		float lambertComponent = max(0.0f, dot(fragmentNormal, lightVector0));
		Idiffuse = texture2D(diffuseTexture, interpTextureCoord);
		Idiffuse *= (Id0 * Kd) * lambertComponent;

		Ispecular = (Is0 * Ks) * pow(max(dot(r, c), 0.0f), shininess);
		Ispecular *= texture2D(specularTexture, interpTextureCoord);

		float spotFactor0 = pow( dot(-l, spotLight0_dir_eye), light0.exponent );
		Iambient += spotFactor0 * (Idiffuse + Ispecular);
	}
	
	if (angle1 < cutoff1) {
		float lambertComponent1 = max(0.0f, dot(fragmentNormal, lightVector1));
		Idiffuse1 = texture2D(diffuseTexture, interpTextureCoord);
		Idiffuse1 *= (Id1 * Kd) * lambertComponent1;

		Ispecular1 = (Is1 * Ks) * pow(max(dot(rr, c), 0.0f), shininess);
		Ispecular1 *= texture2D(specularTexture, interpTextureCoord);

		float spotFactor1 = pow( dot(-ll, spotLight1_dir_eye), light1.exponent );
		Iambient1 += spotFactor1 * (Idiffuse1 + Ispecular1);
	}

	if (angle2 < cutoff2) {
		float lambertComponent2 = max(0.0f, dot(fragmentNormal, lightVector2));
		Idiffuse2 = texture2D(diffuseTexture, interpTextureCoord);
		Idiffuse2 *= (Id2 * Kd) * lambertComponent2;

		Ispecular2 = (Is2 * Ks) * pow(max(dot(rrr, c), 0.0f), shininess);
		Ispecular2 *= texture2D(specularTexture, interpTextureCoord);

		float spotFactor2 = pow( dot(-lll, spotLight2_dir_eye), light2.exponent );
		Iambient2 += spotFactor2 * (Idiffuse2 + Ispecular2);
	}

	vec4 ColorCombined = Iambient + Iambient1 + Iambient2;
	ColorCombined += 0.5 * texture2D(diffuseTexture, interpTextureCoord);
	return ColorCombined;
}

void main(void) {

	// TODO: Phong-based spotlights for material surface modeling using diffuse, normal, and specular textures
	vec4 color = calculation_values();
	
	fragColor = vec4(color);
}