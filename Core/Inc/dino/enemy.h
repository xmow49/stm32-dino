/*
 * enemy.h
 *
 *  Created on: May 21, 2024
 *      Author: dbenech
 */

#ifndef INC_DINO_ENEMY_H_
#define INC_DINO_ENEMY_H_

#include "elements_manager.h"

void enemy_update();
element_id_t enemy_get_current();
void enemy_set_current(element_id_t id);

#endif /* INC_DINO_ENEMY_H_ */
