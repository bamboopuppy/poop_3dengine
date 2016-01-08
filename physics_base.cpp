#include "stdafx.h"
#include "tools.h"


btDiscreteDynamicsWorld* dynamicsWorld;
btDiscreteDynamicsWorld* get_physics_world()
{
	return dynamicsWorld;
}
//btRigidBody* fallRigidBody;

std::vector<t_collision_shape*> collision_shapes;


btBroadphaseInterface* broadphase;
btDefaultCollisionConfiguration* collisionConfiguration;
btCollisionDispatcher* dispatcher;
btSequentialImpulseConstraintSolver* solver;
/* Character */
btKinematicCharacterController* m_character;
btPairCachingGhostObject* m_ghostObject;
//btBroadphaseInterface* broadphase;
/* ///Character */
/*
struct character_kinematic {
	btKinematicCharacterController* m_character;
	btPairCachingGhostObject* m_ghostObject;
};*/
struct character_kinematic * create_character(float width, float height, glm::vec3 location)
{
	/* CHARACTER */
	struct character_kinematic * new_character = (struct character_kinematic *) malloc(sizeof(struct character_kinematic));
	btTransform startTransform;
	startTransform.setIdentity ();
	//startTransform.setOrigin (btVector3(0.0, 4.0, 0.0));
	startTransform.setOrigin (btVector3(location.x, location.y,location.z));//1.0f,1.0f,1.0f));
	new_character->m_ghostObject = new btPairCachingGhostObject();
	new_character->m_ghostObject->setWorldTransform(startTransform);
	broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
	//sweepBP->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
	btScalar characterHeight=height;//1.0;
	btScalar characterWidth =width;//0.5;
	btConvexShape* capsule = new btCapsuleShape(characterWidth,characterHeight);
	new_character->m_ghostObject->setCollisionShape (capsule);
	new_character->m_ghostObject->setCollisionFlags (btCollisionObject::CF_CHARACTER_OBJECT);
	btScalar stepHeight = btScalar(0.25);
	new_character->m_character = new btKinematicCharacterController (new_character->m_ghostObject,capsule,stepHeight);
	//m_character->setUseGhostSweepTest(false);
   //m_character->setUpInterpolate(true);
	dynamicsWorld->addCollisionObject(new_character->m_ghostObject,btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter|btBroadphaseProxy::DefaultFilter);
	dynamicsWorld->addAction(new_character->m_character);
	new_character->old_position = glm::vec3(0.0f, 0.0f, 0.0f);
	return new_character;
	/*///CHARACTER */
}
void add_fixed_constraint(btFixedConstraint * fixed, struct d3_object * object1, d3_object * object2)
{
	//struct physics_joint * new_joint = (struct physics_joint *)malloc(sizeof(struct physics_joint));
	//if (new_joint == NULL) { printf("failed to add new fixed joint\n");return; }
	//new_joint->joint = (void*) fixed;
	//new_joint->type = FIXEDCONSTRAINT;
	//object->joints->resize(1);
	if(object1->physics_obj->joints.size >= MAX_JOINTS) {printf("can't add joint, too many joints already on object\n"); return;}
	if(object2->physics_obj->joints.size >= MAX_JOINTS) {printf("can't add joint, too many joints already on object\n"); return;}
	object1->physics_obj->joints.joint[object1->physics_obj->joints.size] = (void *) fixed;
	object2->physics_obj->joints.joint[object2->physics_obj->joints.size] = (void *) fixed;
	object1->physics_obj->joints.type[object1->physics_obj->joints.size] =FIXEDCONSTRAINT;
	object2->physics_obj->joints.type[object2->physics_obj->joints.size] =FIXEDCONSTRAINT;
	object1->physics_obj->joints.connected[object1->physics_obj->joints.size] = object2;
	object2->physics_obj->joints.connected[object2->physics_obj->joints.size] = object1;
	object1->physics_obj->joints.size++;
	object2->physics_obj->joints.size++;


	dynamicsWorld->addConstraint(fixed, true);
}
void phx_init()
{
	//int i;
	//printf("phx_init start\n");
	///-----initialization_start-----
	broadphase = new btDbvtBroadphase();
	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	btCollisionDispatcher* dispatcher = new	btCollisionDispatcher(collisionConfiguration);
	solver = new btSequentialImpulseConstraintSolver;
	btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);
	btVector3 worldMin(-1000,-1000,-1000);
	btVector3 worldMax(1000,1000,1000);
	//btAxisSweep3* sweepBP = new btAxisSweep3(worldMin,worldMax);
	broadphase = new btDbvtBroadphase();
	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	

	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0,-9,0));
	collision_shapes.clear();
}

glm::vec3 get_box_dimensions(std::vector<glm::vec4> * mesh_points, bool center)
{
	glm::vec3 dimensions = glm::vec3(0.0,0.0,0.0); // go through array and find the maximum dimensions of mesh to make a physics box around it...
	float largestx = 0.0; float smallestx = 0.0;
	float largesty = 0.0; float smallesty = 0.0;
	float largestz = 0.0; float smallestz = 0.0;
	unsigned int i = 0;
	largestx = (*mesh_points)[i].x;
	smallestx = (*mesh_points)[i].x;
	largesty = (*mesh_points)[i].y;
	smallesty = (*mesh_points)[i].y;
	largestz = (*mesh_points)[i].z;
	smallestz = (*mesh_points)[i].z;
	for (i = 1; i < mesh_points->size(); i++)
	{
		largestx = std::max(largestx, (*mesh_points)[i].x);
		smallestx = std::min(smallestx, (*mesh_points)[i].x);
		largesty = std::max(largesty, (*mesh_points)[i].y);
		smallesty = std::min(smallesty, (*mesh_points)[i].y);
		largestz = std::max(largestz, (*mesh_points)[i].z);
		smallestz = std::min(smallestz, (*mesh_points)[i].z);
	}
	dimensions.x = largestx - smallestx; dimensions.y = largesty - smallesty; dimensions.z = largestz - smallestz;
	if (dimensions.x < 0.001) dimensions.x = 0.001f;
	if (dimensions.y < 0.001) dimensions.y = 0.001f;
	if (dimensions.z < 0.001) dimensions.z = 0.001f;
	if (center && (abs(largestx + smallestx) > 0.001 || abs(largesty + smallesty) > 0.001 || abs(largestz + smallestz) > 0.001)) // center the mesh if it isn't already
	{
		float x_offset = -(largestx + smallestx)/2;
		float y_offset = -(largesty + smallesty)/2;
		float z_offset = -(largestz + smallestz)/2;
		offset_mesh(mesh_points, glm::vec3(x_offset, y_offset, z_offset));
		printf("mesh offset by (%f, %f, %f)\n", x_offset, y_offset, z_offset);
	}
	return dimensions;
}

struct t_collision_shape * find_shape(glm::vec3 dimensions)
{
	struct t_collision_shape * new_shape;
	unsigned int i;
	for (i = 0; i < collision_shapes.size(); i++) {
		if (collision_shapes[i]->dimensions.x == dimensions.x && collision_shapes[i]->dimensions.y == dimensions.y && collision_shapes[i]->dimensions.z == dimensions.z){
			return collision_shapes[i];
		}
	}
	new_shape = (struct t_collision_shape *) malloc(sizeof(struct t_collision_shape));
	new_shape->dimensions = dimensions;
	btVector3 vector_convert;
	//vector_convert.x = dimensions.x; vector_convert.y = dimensions.y; vector_convert.z = dimensions.z;

	new_shape->shape = new btBoxShape(btVector3(dimensions.x/2, dimensions.y/2, dimensions.z/2));//btVector3(2,2,2));
	printf("dimensions (%f, %f, %f)\n", dimensions.x, dimensions.y, dimensions.z);
	collision_shapes.push_back(new_shape);
	return new_shape;
}

void init_physics_joints(struct physics_joints * joints)
{
	int i;
	joints->size = 0;
	for (i = 0; i < MAX_JOINTS; i++)
	{
		joints->joint[i] = NULL;
		joints->type[i] = 0;
		joints->connected[i] = NULL;
	}
}

struct physics_object * get_physics_object(struct d3_object * object)
{

	struct physics_object * new_physics_object = (struct physics_object *) malloc(sizeof(struct physics_object));
	new_physics_object->collision_shape = find_shape(object->points->dimensions);


	btQuaternion quat;
    quat.setEulerZYX(glm::radians(object->rotation.z),glm::radians(object->rotation.y),glm::radians(object->rotation.x));
	new_physics_object->fallMotionState =
                //new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(object->position.x, object->position.y, object->position.z)));//0, 50, 0)));
				//new btDefaultMotionState(btTransform(btQuaternion(glm::radians(object->rotation.z), glm::radians(object->rotation.y), glm::radians(object->rotation.x)), btVector3(object->position.x, object->position.y, object->position.z)));//0, 50, 0)));
				new btDefaultMotionState(btTransform(quat, btVector3(object->position.x, object->position.y, object->position.z)));//0, 50, 0)));
	btScalar mass = 0;
	if (object->flags & D3_OBJECT_STATIC)
		mass = 0;//1;
	else
		mass = object->points->dimensions.x * object->points->dimensions.y * object->points->dimensions.z;
    btVector3 fallInertia(0, 0, 0);
    new_physics_object->collision_shape->shape->calculateLocalInertia(mass, fallInertia);
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, new_physics_object->fallMotionState, new_physics_object->collision_shape->shape, fallInertia);

    new_physics_object->fallRigidBody = new btRigidBody(fallRigidBodyCI);
	new_physics_object->fallRigidBody->setUserIndex(object->id);
	//new_physics_object->joints = (std::vector<physics_joint*> *) malloc(sizeof(std::vector<physics_joint*>));
	

	init_physics_joints(&new_physics_object->joints);

    //dynamicsWorld->addRigidBody(new_physics_object->fallRigidBody);
	return new_physics_object;
}

void add_rigid_body(struct physics_object * physics_object_2add)
{
	dynamicsWorld->addRigidBody(physics_object_2add->fallRigidBody);
}

void remove_rigid_body(struct physics_object * physics_object_2remove)
{
	dynamicsWorld->removeRigidBody(physics_object_2remove->fallRigidBody);
}

void zero_joint_from_obj(void * joint, struct physics_object * object)
{
	if(object == NULL || joint == NULL)
		return;
	unsigned int i;
	struct physics_joints * joints = &object->joints;
	for (i = 0; i < joints->size; i++)
	{
		if(joints->joint[i] == joint) {
			joints->joint[i] = NULL;
			joints->type[i] = NULL;
			zero_joint_from_obj(joint, joints->connected[i]->physics_obj);
			joints->connected[i] = NULL;
			if (i < (joints->size-1)) {// the joint is not at the end of array
				joints->joint[i] = joints->joint[joints->size-1];
				joints->type[i] = joints->type[joints->size-1];
				joints->connected[i] = joints->connected[joints->size-1];
				joints->joint[joints->size-1] = NULL;
				joints->type[joints->size-1] = 0;
				joints->connected[joints->size-1] = NULL;
			}
			joints->size--;
			break;
		}
	}
}

//not deleting collision shape!!
void delete_physics_object(struct physics_object * physics2delete)
{
	int i;
	unsigned int j;
	//first remove any joints on this object:
	void * del_joint;
	for (j = 0; j < physics2delete->joints.size; j++) {
		del_joint = physics2delete->joints.joint[j];
		zero_joint_from_obj(del_joint, physics2delete);
		dynamicsWorld->removeConstraint((btGeneric6DofSpring2Constraint*)del_joint);
	}


	for (i=dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		if (physics2delete->fallRigidBody == btRigidBody::upcast(obj)) {
			if (physics2delete->fallRigidBody && physics2delete->fallRigidBody->getMotionState())
			{
				delete physics2delete->fallRigidBody->getMotionState();
			}
			dynamicsWorld->removeCollisionObject( obj );
			delete obj;
			break;
		} 
	}
	//delete fallShape;
}

void phx_step(float hertz, struct character_kinematic * charactor)
{
	unsigned int i;
	struct d3_object * raw_object;
	dynamicsWorld->stepSimulation(1 / hertz, 10);
	std::vector<d3_object*> list_objects = get_d3objects();
	for (i = 0; i < list_objects.size(); i++)
	{
		raw_object = list_objects[i];
		if (raw_object == NULL)
			continue;
		if (raw_object->flags & D3_OBJECT_STATIC)
			continue;
		btTransform trans;
		if (raw_object->physics_obj == NULL)
			continue;
		raw_object->physics_obj->fallRigidBody->getMotionState()->getWorldTransform(trans);
		//struct d3_object *  falling_box = find_object_by_name("some_box");
		glm::vec3 new_position = glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
		GLfloat bt_m[16];
		trans.getOpenGLMatrix(bt_m);
		raw_object->position = new_position;
		raw_object->model_matrix = glm::make_mat4(bt_m);//glm::mat4(bt_m);
		//printf("upating physics %s\n", raw_object->name);
	}
	move_player_character(charactor);
}

void transport_character(struct character_kinematic * charactor, glm::vec3 position)
{
	if (charactor != NULL)
		charactor->m_character->warp (btVector3(position.x, position.y, position.z));
}

glm::vec3 get_character_world_transform(struct character_kinematic * charactor)
{
	if (charactor == NULL)
		return (glm::vec3(0.0f,0.0f,0.0f));
	glm::vec3 position;
	btTransform trans;
	trans = charactor->m_ghostObject->getWorldTransform();
	position = glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
	return position;
}

void weld_2_objects(struct d3_object * object1, struct d3_object * object2)
{
	btTransform trA;
	btTransform trB;
	btTransform globalFrame;
	globalFrame.setIdentity();
	globalFrame.setOrigin(btVector3(object1->position.x, object1->position.y, object1->position.z));
	trA.setIdentity();
	trB.setIdentity();
	trA = object1->physics_obj->fallRigidBody->getWorldTransform().inverse()*globalFrame;
	trB = object2->physics_obj->fallRigidBody->getWorldTransform().inverse()*globalFrame;
	//trA.setOrigin(btVector3(btScalar(computer_case->position.x), btScalar(computer_case->position.y), btScalar(computer_case->position.z)));
	//trB.setOrigin(btVector3(btScalar(computerscreen->position.x), btScalar(computerscreen->position.y), btScalar(computerscreen->position.z)));
	btFixedConstraint * fixed = new btFixedConstraint(*(object1->physics_obj->fallRigidBody),//btRigidBody& rbA,
                  *(object2->physics_obj->fallRigidBody),//btRigidBody& rbB,
                  trA,//const btTransform& frameInA,
                  trB);//const btTransform& frameInB);
	add_fixed_constraint(fixed, object1, object2);
}