#include "stdafx.h"
#include "tools.h"
//glm::vec3 old_position;
void (*character_move_callback) ( glm::vec3 position);
float walkspeed = 1.0f;


bool check_if_character_moved(struct character_kinematic * charactor, glm::vec3 * position)
{
	bool diditmove = false;
	//glm::vec3 position;
	btTransform trans;
	trans = charactor->m_ghostObject->getWorldTransform();
	*position = glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
	//if (abs(position->x - charactor->old_position.x) >0.01 || position->y != charactor->old_position.y || position->z != charactor->old_position.z)
	if (abs(position->x - charactor->old_position.x) >0.01 || abs(position->y - charactor->old_position.y) >0.01 || abs(position->z - charactor->old_position.z) >0.01)
		diditmove = true;
	charactor->old_position = *position;
	return diditmove;
}

void register_character_move_callback(void (*character_move_callback_fn) (glm::vec3 position))
{
	character_move_callback = character_move_callback_fn;
}

void init_character()
{
	//old_position = glm::vec3(0.0f, 0.0f, 0.0f);
	character_move_callback = NULL;
}

void move_player_character(struct character_kinematic * charactor)
{
	glm::vec3 prewalkdir = get_camerafront();
	glm::vec3 cameraup = glm::vec3(0.0, 1.0, 0.0);
	glm::vec3 walkdir = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 character_position;
	walkdir += (get_updown(false)) * prewalkdir;
	walkdir += glm::normalize(glm::cross(prewalkdir, cameraup)) * (get_leftright(false));
	if (get_noclip())
		transport_character(charactor, get_camerapos());
	else if (charactor != NULL)
		charactor->m_character->setWalkDirection(btVector3(walkdir.x, walkdir.y, walkdir.z)*walkspeed);
	if (check_if_character_moved(charactor, &character_position) && character_move_callback != NULL){
		(*character_move_callback)(character_position);
	}
}