#include "world_buffer.h"

#include <iostream>
#include <string>
#include <GL/glew.h>
#include <btBulletDynamicsCommon.h>
#include "primitive.h"


btBroadphaseInterface *bbroadphase;
btDefaultCollisionConfiguration *bcollisionConfiguration;
btCollisionDispatcher *bcollisionDispatcher;
btSequentialImpulseConstraintSolver *bsolver;
btDiscreteDynamicsWorld *bworld;


// This function generates new unique binding indices for UBOs starting from 1.
int nextBindingIndex() {
	static int lastUsedBindingIndex = 0;
	return ++lastUsedBindingIndex;
}

template<class T>
inline WorldBuffer<T>::WorldBuffer(GLuint shader, const char *bufName, bool trackSize) :
	bufName(bufName), uboBindingIndex(nextBindingIndex()), shader(shader), trackSize(trackSize) {
	// Bind the UBO to the shader at a set binding point.
	std::string uboName = std::string("StaticWorldBuffer") + bufName;
	glUniformBlockBinding(shader, glGetUniformBlockIndex(shader, uboName.c_str()), uboBindingIndex);

	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);

	// Generate the buffers with a large size.
	glBufferData(GL_UNIFORM_BUFFER, sizeof(T) * UBO_SIZE, NULL, GL_STATIC_DRAW);

	glBindBufferBase(GL_UNIFORM_BUFFER, uboBindingIndex, ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

template<class T>
const void WorldBuffer<T>::flushEntireContents() {
	if (contents.size() > UBO_SIZE) {
		std::string msg = std::string("Size too large: ") + std::to_string(contents.size());
		throw std::runtime_error(msg);
	}

	glBindBuffer(GL_UNIFORM_BUFFER, ubo);

	// Fill the start of the buffer with our data.
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(T) * contents.size(), contents.data());

	if (trackSize) {
		// Tell the shader how many objects we've loaded into its buffer.
		std::string swbName = std::string("currentSizeSwb") + bufName;
		glUniform1i(glGetUniformLocation(shader, swbName.c_str()), (GLint)contents.size());
	}
}

btRigidBody *body, *floors;
WorldBuffer<SpherePrimitive> *swbSpheres;

void initWorldBuffer(GLuint shader) {
	// Initialise bullet
	bbroadphase = new btDbvtBroadphase();
	bcollisionConfiguration = new btDefaultCollisionConfiguration();
	bcollisionDispatcher = new btCollisionDispatcher(bcollisionConfiguration);
	bsolver = new btSequentialImpulseConstraintSolver();
	bworld = new btDiscreteDynamicsWorld(bcollisionDispatcher, bbroadphase, bsolver, bcollisionConfiguration);
	bworld->setGravity(btVector3(0, 0, -9.8f));

	{
		btDefaultMotionState *motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0), btVector3(0, 0, 0)));
		btCollisionShape *shape = new btSphereShape(1.f);
		btRigidBody::btRigidBodyConstructionInfo bodyinf(1.f, motionState, shape);
		body = new btRigidBody(bodyinf);
		body->setRestitution(0.9f);
		bworld->addRigidBody(body);
	}

	{
		btDefaultMotionState *motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0), btVector3(0, 0, -6.f)));
		btCollisionShape *shape = new btStaticPlaneShape(btVector3(0, 0, 1), 0.f);
		btRigidBody::btRigidBodyConstructionInfo bodyinf(0.f, motionState, shape);
		floors = new btRigidBody(bodyinf);
		floors->setRestitution(0.9f);
		bworld->addRigidBody(floors);
	}

	// swb = Static World Buffer
	// Contains all world data that will not change (so we can use GL_STATIC_DRAW).
	swbSpheres = new WorldBuffer<SpherePrimitive>(shader, "Spheres");
	swbSpheres->getContents().push_back(SpherePrimitive(0, vec3(), 1.f));
	swbSpheres->getContents().push_back(SpherePrimitive(1, vec3(-0.7f, 0.f, 0.7f), 0.5f));
	swbSpheres->getContents().push_back(SpherePrimitive(1, vec3(0.7f, 0.0f, 0.7f), 0.5f));
	swbSpheres->getContents().push_back(SpherePrimitive(0, vec3(1.7f, 2.0f, 0.7f), 0.5f));
	swbSpheres->getContents().push_back(SpherePrimitive(1, vec3(0.7f, 2.0f, 0.7f), 0.5f));
	swbSpheres->getContents().push_back(SpherePrimitive(0, vec3(0.7f, -2.0f, 1.7f), 0.5f));
	swbSpheres->getContents().push_back(SpherePrimitive(1, vec3(1.7f, -2.0f, 1.7f), 0.5f));
	swbSpheres->getContents().push_back(SpherePrimitive(0, vec3(1.7f, 2.0f, 0.7f), 0.5f));
	swbSpheres->getContents().push_back(SpherePrimitive(1, vec3(0.7f, 2.0f, 0.7f), 0.5f));
	swbSpheres->getContents().push_back(SpherePrimitive(0, vec3(0.7f, 3.0f, 1.7f), 0.5f));
	swbSpheres->flushEntireContents();

	WorldBuffer<TrianglePrimitive> swbTris(shader, "Tris");
	swbTris.getContents().push_back(TrianglePrimitive(2, vec3(-6, -6, -6), vec3(6, -6, -6), vec3(-6, 6, -6)));
	swbTris.getContents().push_back(TrianglePrimitive(2, vec3(-6, 6, -6), vec3(6, -6, -6), vec3(6, 6, -6)));
	swbTris.getContents().push_back(TrianglePrimitive(2, vec3(-6, -6, -6), vec3(-6, -6, 6), vec3(6, -6, -6)));
	swbTris.getContents().push_back(TrianglePrimitive(2, vec3(-6, -6, 6), vec3(6, -6, 6), vec3(6, -6, -6)));
	swbTris.getContents().push_back(TrianglePrimitive(2, vec3(-6, 6, -6), vec3(6, 6, -6), vec3(-6, 6, 6)));
	swbTris.getContents().push_back(TrianglePrimitive(2, vec3(-6, 6, 6), vec3(6, 6, -6), vec3(6, 6, 6)));
	swbTris.getContents().push_back(TrianglePrimitive(2, vec3(6, 6, -6), vec3(6, -6, -6), vec3(6, 6, 6)));
	swbTris.getContents().push_back(TrianglePrimitive(2, vec3(6, 6, 6), vec3(6, -6, -6), vec3(6, -6, 6)));
	swbTris.getContents().push_back(TrianglePrimitive(2, vec3(-6, 6, -6), vec3(-6, 6, 6), vec3(-6, -6, -6)));
	swbTris.getContents().push_back(TrianglePrimitive(2, vec3(-6, 6, 6), vec3(-6, -6, 6), vec3(-6, -6, -6)));
	swbTris.getContents().push_back(TrianglePrimitive(2, vec3(-6, -6, 6), vec3(-6, 6, 6), vec3(6, -6, 6)));
	swbTris.getContents().push_back(TrianglePrimitive(2, vec3(-6, 6, 6), vec3(6, 6, 6), vec3(6, -6, 6)));
	swbTris.flushEntireContents();

	// smb = Static Material Buffer
	WorldBuffer<Material> smb(shader, "Materials", false);
	smb.getContents().push_back(Material(vec3(1, 0, 0), vec3(.6f, .3f, .3f), 32.f));
	smb.getContents().push_back(Material(vec3(0, 0, 0), vec3(.4f, .4f, .4f), 24.f));
	smb.getContents().push_back(Material(vec3(1, 1, 1), vec3(.6f, .6f, .6f), 64.f));
	smb.flushEntireContents();
}

void tickWorldBuffer(float delta) {
	bworld->stepSimulation(delta);
	btVector3 v = body->getCenterOfMassPosition();
	swbSpheres->getContents()[0].o = glm::vec4(v.getX(), v.getY(), v.getZ(), 0);
	swbSpheres->flushEntireContents();
}
