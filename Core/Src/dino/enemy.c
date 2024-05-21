/*
 * enemy.c
 *
 *  Created on: May 21, 2024
 *      Author: dbenech
 */

#include "dino/enemy.h"
#include "dino/sprites.h"
#include "dino/elements_manager.h"
#include "dino/move_manager.h"
#include "dino/dino.h"

element_id_t current_enemy = ID_CACTUS;

void enemy_init()
{
    elements_manager_move_element(ID_CACTUS, CACTUS_X_START, CACTUS_Y_START);
    move_manager_move_element_with_const_speed(ID_CACTUS, CACTUS_X_TARGET, CACTUS_Y_TARGET, CACTUS_SPEED);
}

void enemy_update()
{
    element_t *enemy = elements_manager_find_element(current_enemy);
    if (enemy == NULL)
    {
        return;
    }
}