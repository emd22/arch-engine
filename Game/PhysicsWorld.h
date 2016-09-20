#pragma once
#include "GameObject.h"

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

#include <vector>
#include <utility>

typedef std::pair<btRigidBody*, btCollisionShape*> PhysicsBodyInfo;
typedef std::pair<PhysicsBodyInfo, std::shared_ptr<GameObject>> PhysicsObjectPair;

class PhysicsWorld
{
public:
	PhysicsWorld();
	~PhysicsWorld();

	void RegisterObject(std::shared_ptr<GameObject> object, double mass=1.0);

	void Update(double dt);

private:
	btBroadphaseInterface *broadphase; 
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;

	std::vector<PhysicsObjectPair> objects;
};

