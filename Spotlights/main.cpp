#include <iostream>
#include <memory>
#include <gl/glew.h>
#include <gl/freeglut.h>

#include <MouseCamera.h>
#include <Mesh.h>
#include <Shader.h>
#include <Texture.h>

const char* WINDOW_TITLE = "[CSCI-4800/5800] Shader and GPU Programming";
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 800;
GLint g_glutWindowIdentifier;

std::shared_ptr<MouseCameraf> camera = nullptr;
std::shared_ptr<Mesh> plane = nullptr;
std::shared_ptr<Mesh> sphere = nullptr;
std::shared_ptr<Texture> plane_texture = nullptr;

/* Spotlight Structure */
struct Light {
    Vector3f position;
    Vector3f target;
    Vector3f ambient;
    Vector3f diffuse;
    Vector3f specular;
    float exponent;
    float cutoff;
};

/* Three Spotlights */
Light light0, light1, light2;

void g_init() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glewInit();

    camera = std::make_shared<MouseCameraf>(3.0f);
	camera->setPosition(10.0f, 1.57f, 1.57f * 0.7f);

    plane = std::make_shared<Mesh>();
    plane->load("models/plane.obj");
    plane->loadShader("shaders/SpotLights.vert", "shaders/SpotLights.frag");
    plane->setDiffuseTexture("textures/grid_diffuse.png");
    plane->setNormalTexture("textures/grid_normal.png");
    plane->setSpecularTexture("textures/grid_specular.png");
    // TODO: Load shader, textures

    sphere = std::make_shared<Mesh>();
    sphere->load("models/sphere.obj");
    sphere->loadShader("shaders/SpotLights.vert", "shaders/SpotLights.frag");
	sphere->setDiffuseTexture("textures/metal_diffuse.png");
	sphere->setNormalTexture("textures/metal_normal.png");
	sphere->setSpecularTexture("textures/metal_specular.png");
    sphere->setPosition(0.0f, 1.0f, 0.0f);
    // TODO: Load shader, textures

    // TODO: Init Spotlights
	light0.position = Vector3f(20.0f, 4.0f, 0.0f);
    light0.target = Vector3f(0.0f, 0.5f, 0.0f);
	light0.ambient = Vector3f(0.5f, 0.0f, 0.0f);
	light0.diffuse = Vector3f(0.5f, 0.0f, 0.0f);
    light0.specular = Vector3f(1.0f, 1.0f, 1.0f);
    light0.exponent = 128.0f;
    light0.cutoff = 5.0f;

	light1.position = Vector3f(-20.0f, 4.0f, 0.0f);
	light1.target = Vector3f(0.0f, 0.5f, 0.0f);
	light1.ambient = Vector3f(0.0f, 0.5f, 0.0f);
	light1.diffuse = Vector3f(0.0f, 0.5f, 0.0f);
	light1.specular = Vector3f(1.0f, 1.0f, 1.0f);
	light1.exponent = 128.0f;
	light1.cutoff = 5.0f;

	light2.position = Vector3f(8.0f, 4.0f, 20.0f);
	light2.target = Vector3f(0.0f, 0.5f, 0.0f);
	light2.ambient = Vector3f(0.0f, 0.0f, 0.5f);
	light2.diffuse = Vector3f(0.0f, 0.0f, 0.5f);
	light2.specular = Vector3f(1.0f, 1.0f, 1.0f);
	light2.exponent = 128.0f;
	light2.cutoff = 5.0f;
}

void g_glutReshapeFunc(int width, int height) {
	glViewport(0, 0, width, height);
    camera->setPerspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
	glutPostRedisplay();
}

void RenderMesh(const std::shared_ptr<Mesh>& mesh) {
    Matrix4f transform = mesh->getTransform().toMatrix();
    Matrix4f view = camera->getViewMatrix();
    Matrix4f modelViewMatrix = transform * camera->getViewMatrix();
    Matrix3f normalMatrix = Matrix4f::NormalMatrix(modelViewMatrix);
    Matrix4f projectionMatrix = camera->getProjectionMatrix();

    mesh->beginRender();
    mesh->getShader()->uniformMatrix("projectionMatrix", projectionMatrix);
    mesh->getShader()->uniformMatrix("modelViewMatrix", modelViewMatrix);
    mesh->getShader()->uniformMatrix("normalMatrix", normalMatrix);

    // TODO: Uniform Light Parameters

    // Note: To pass uniform variables that belong to a structure within the shader use
    // the following example: 
    //
    //      mesh->getShader()->uniformVector("Light.position", light0.position);
    mesh->getShader()->uniformVector("light0.position", light0.position);
    mesh->getShader()->uniformVector("light0.target", light0.target);
    mesh->getShader()->uniformVector("light0.ambient", light0.ambient);
    mesh->getShader()->uniformVector("light0.diffuse", light0.diffuse);
    mesh->getShader()->uniformVector("light0.specular", light0.specular);
    mesh->getShader()->uniform1f("light0.exponent", light0.exponent);
    mesh->getShader()->uniform1f("light0.cutoff", light0.cutoff);

	mesh->getShader()->uniformVector("light1.position", light1.position);
	mesh->getShader()->uniformVector("light1.target", light1.target);
	mesh->getShader()->uniformVector("light1.ambient", light1.ambient);
	mesh->getShader()->uniformVector("light1.diffuse", light1.diffuse);
	mesh->getShader()->uniformVector("light1.specular", light1.specular);
	mesh->getShader()->uniform1f("light1.exponent", light1.exponent);
	mesh->getShader()->uniform1f("light1.cutoff", light1.cutoff);

	mesh->getShader()->uniformVector("light2.position", light2.position);
	mesh->getShader()->uniformVector("light2.target", light2.target);
	mesh->getShader()->uniformVector("light2.ambient", light2.ambient);
	mesh->getShader()->uniformVector("light2.diffuse", light2.diffuse);
	mesh->getShader()->uniformVector("light2.specular", light2.specular);
	mesh->getShader()->uniform1f("light2.exponent", light2.exponent);
	mesh->getShader()->uniform1f("light2.cutoff", light2.cutoff);

    mesh->endRender();
}

void g_glutDisplayFunc() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    RenderMesh(plane);
    RenderMesh(sphere);

    glFlush();
}

void g_glutMotionFunc(int x, int y) {
	camera->onMouseMove(x, y);
	glutPostRedisplay();
}

void g_glutMouseFunc(int button, int state, int x, int y) {
	if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN ) camera->onMouseButton(LB_DOWN, x, y);
    if ( button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN ) camera->onMouseButton(RB_DOWN, x, y);
    if ( button == GLUT_LEFT_BUTTON && state == GLUT_UP ) camera->onMouseButton(LB_UP, x, y);
    if ( button == GLUT_RIGHT_BUTTON && state == GLUT_UP ) camera->onMouseButton(RB_UP, x, y);
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	g_glutWindowIdentifier = glutCreateWindow(WINDOW_TITLE);

	glutDisplayFunc(g_glutDisplayFunc);
	glutReshapeFunc(g_glutReshapeFunc);
    glutMotionFunc(g_glutMotionFunc);
    glutMouseFunc(g_glutMouseFunc);

	g_init();

	glutMainLoop();
	return 0;
}

