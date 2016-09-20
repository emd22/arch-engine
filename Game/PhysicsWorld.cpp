#include "PhysicsWorld.h"

PhysicsWorld::PhysicsWorld()
{// Build the broadphase
	broadphase = new btDbvtBroadphase();

	// Set up the collision configuration and dispatcher
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);

	// The actual physics solver
	solver = new btSequentialImpulseConstraintSolver;

	// The world.
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -1, 0));
}

PhysicsWorld::~PhysicsWorld()
{
	delete dynamicsWorld;
	delete solver;
	delete dispatcher;
	delete collisionConfiguration;
	delete broadphase;

	// delete all objects
	for (auto &&it : objects) {
		btRigidBody *body = it.first.first;
		btCollisionShape *shape = it.first.second;
		if (body != nullptr) {
			delete body->getMotionState();
			delete body;
		}
		if (shape != nullptr) {
			delete shape;
		}
	}
}

void PhysicsWorld::RegisterObject(std::shared_ptr<GameObject> object, double mass)
{
	if (object->GetMesh() == nullptr) {
		throw "no mesh attached to object";
	}

	btVector3 localInertia(0, 0, 0);
	btCollisionShape *shape;

	if (mass == 0.0) {
		// static objects use triangle mesh shape
		btTriangleMesh *trimesh = new btTriangleMesh();

		auto &mesh = object->GetMesh();
		auto &vertices = mesh->GetVertices();
		auto &indices = mesh->GetIndices();

		for (size_t i = 0; i < indices.size(); i += 3) {
			Vertex v0 = vertices[indices[i]];
			Vertex v1 = vertices[indices[i + 1]];
			Vertex v2 = vertices[indices[i + 2]];

			v0.x *= object->scale.x;
			v0.y *= object->scale.y;
			v0.z *= object->scale.z;

			v1.x *= object->scale.x;
			v1.y *= object->scale.y;
			v1.z *= object->scale.z;

			v2.x *= object->scale.x;
			v2.y *= object->scale.y;
			v2.z *= object->scale.z;

			btVector3 bv0(v0.x, v0.y, v0.z);
			btVector3 bv1(v1.x, v1.y, v1.z);
			btVector3 bv2(v2.x, v2.y, v2.z);

			trimesh->addTriangle(bv2, bv1, bv0);
		}

		shape = new btBvhTriangleMeshShape(trimesh, true);//new btBoxShape(btVector3(1, 1, 1)); // TODO: calculate bounding box of object
		// TODO: delete trimesh
	} else {
		// use convex hull shape
		btConvexHullShape *hull = new btConvexHullShape();

		auto &mesh = object->GetMesh();
		auto &vertices = mesh->GetVertices();
		auto &indices = mesh->GetIndices();

		for (size_t i = 0; i < indices.size(); i++) {
			Vertex v0 = vertices[indices[i]];

			v0.x *= object->scale.x;
			v0.y *= object->scale.y;
			v0.z *= object->scale.z;

			btVector3 bv0(v0.x, v0.y, v0.z);

			hull->addPoint(bv0);
		}

		shape = hull;

		shape->calculateLocalInertia(mass, localInertia);
	}

	btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(
		btQuaternion(0, 0, 0, 1), btVector3(object->translation.x, object->translation.y, object->translation.z)));

	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(
		mass,                  // mass
		motionState,        // initial position
		shape,              // collision shape of body
		localInertia    // local inertia
	);

	btRigidBody *rigidBody = new btRigidBody(rigidBodyCI);
	dynamicsWorld->addRigidBody(rigidBody);

	PhysicsObjectPair pair { {rigidBody, shape}, object  };
	objects.push_back(pair);
}

void PhysicsWorld::Update(double dt)
{
	dynamicsWorld->stepSimulation(dt);

	// loop through all objects connected to the physics world
	for (auto &&it : objects) {
		btRigidBody *body = it.first.first;
		std::shared_ptr<GameObject> &object = it.second;

		// update the object's position based on it's physics position.
		btTransform trans;
		body->getMotionState()->getWorldTransform(trans);
		object->translation = Vector3(trans.getOrigin().x(), trans.getOrigin().y(), trans.getOrigin().z());
		object->rotation = Quaternion(trans.getRotation().x(), trans.getRotation().y(), 
			trans.getRotation().z(), trans.getRotation().w());
		object->rotation.Invert();
		object->UpdateMatrix();
	}
}