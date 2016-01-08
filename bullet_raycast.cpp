#include "stdafx.h"
#include "tools.h"

void ray_test(glm::vec2 Mouse, glm::vec2 screen_size)
{
    //glm::vec2 Mouse = mouse_pos;
   Mouse.y = screen_size.y - Mouse.y;

    //glm::vec4 RayStart = glm::vec4(2*Mouse.x/screen_size.x - 1,2*Mouse.y/screen_size.y - 1, -1, 1);
    //glm::vec4 RayEnd = RayStart; RayEnd.z = 0;
	glm::vec4 RayStart(
		((float)Mouse.x/(float)screen_size.x - 0.5f) * 2.0f, // [0,1024] -> [-1,1]
		((float)Mouse.y/(float)screen_size.y - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
		-1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
		1.0f
	);
	glm::vec4 RayEnd = RayStart; RayEnd.z = 0.0; /*lRayEnd_NDC(
		((float)mouseX/(float)screenWidth  - 0.5f) * 2.0f,
		((float)mouseY/(float)screenHeight - 0.5f) * 2.0f,
		0.0,
		1.0f
	);*/
    //RayStart = glm::inverse( get_g_projection()*get_g_view() )*RayStart; RayStart /= RayStart.w;
    //RayEnd   = glm::inverse( get_g_projection()*get_g_view() )*RayEnd;   RayEnd   /= RayEnd.w;
	// Faster way (just one inverse)
	glm::mat4 M = glm::inverse(get_g_projection() * get_g_view());
	glm::vec4 lRayStart_world = M * RayStart; lRayStart_world/=lRayStart_world.w;
	glm::vec4 lRayEnd_world   = M * RayEnd  ; lRayEnd_world  /=lRayEnd_world.w;


    //glm::vec4 RayDirection = glm::normalize(RayEnd-RayStart);
	glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
	lRayDir_world = 1000.0f * glm::normalize(lRayDir_world);

    //btCollisionWorld::AllHitsRayResultCallback RayCallback(btVector3(lRayStart_world.x,lRayStart_world.y,lRayStart_world.z),
	btCollisionWorld::ClosestRayResultCallback RayCallback(btVector3(lRayStart_world.x,lRayStart_world.y,lRayStart_world.z),
                                                           btVector3(lRayDir_world.x,lRayDir_world.y,lRayDir_world.z));
	btDiscreteDynamicsWorld* TheWorld = get_physics_world();
    TheWorld->rayTest(btVector3(lRayStart_world.x,lRayStart_world.y,lRayStart_world.z),
                      btVector3(lRayDir_world.x,lRayDir_world.y,lRayDir_world.z),
                      RayCallback);
	int closest = 0;
    if ( RayCallback.hasHit() )
    {
        
        //for ( int i = 0; i < RayCallback.m_collisionObjects.size(); i++ )
       // {
           /* std::cout<<i<<": "<<glm::ToVec3(RayCallback.m_hitPointWorld[i])<<" "
                <<RayCallback.m_hitFractions[i]<<" "
                <<RayCallback.m_collisionObjects[i]->getCollisionShape()->getShapeType()<<std::endl;
				*/
			//printf("collisions %i distance %f ID: %i\n", i, RayCallback.m_hitPointWorld[i].getZ(), RayCallback.m_collisionObjects[i]->getUserIndex());
           // if (RayCallback.m_hitPointWorld[i].getZ() >
           //     RayCallback.m_hitPointWorld[closest].getZ() )
            //    closest = i;
      //  }
		//printf("closest %i\n", closest);
    } else{
		printf("no hits!!\n");
		return;
	}

	//struct d3_object * clicked_object = find_object_by_id(RayCallback.m_collisionObjects[closest]->getUserIndex());
	struct d3_object * clicked_object = find_object_by_id(RayCallback.m_collisionObject->getUserIndex());
	//printf("id trying to find: %i, ID GOT: %i\n", RayCallback.m_collisionObjects[closest]->getUserIndex(), clicked_object->id);
	//clicked_object->flags = clicked_object->flags | D3_OBJECT_NORENDER;
	printf("physics state: %i\n", clicked_object->physics_obj->fallRigidBody->getActivationState());
	//delete_object(clicked_object);
}
