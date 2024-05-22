/*
 * enemy.c
 *
 *  Created on: May 21, 2024
 *      Author: dbenech
 */

#include "stm32f1xx_hal.h"

#include "dino/enemy.h"
#include "dino/sprites.h"
#include "dino/elements_manager.h"
#include "dino/move_manager.h"
#include "dino/dino.h"

element_id_t current_enemy = ID_CACTUS;
uint32_t start_cactus_count = 0;

void enemy_init()
{
    enemy_set_current(ID_CACTUS);
    start_cactus_count = 0;
}

element_id_t enemy_get_current()
{
    return current_enemy;
}

void enemy_set_current(element_id_t id)
{
    switch (id)
    {
    case ID_BIRD:
        elements_manager_set_visible(ID_CACTUS, false);
        elements_manager_set_visible(ID_BIRD, true);

        move_manager_stop_element(ID_CACTUS);
        elements_manager_move_element(ID_CACTUS, C_X_START, C_Y_START);

        elements_manager_move_element(ID_BIRD, B_X_START, B_Y_START);
        move_manager_move_element_with_const_speed(ID_BIRD, B_X_TARGET, B_Y_TARGET, CACTUS_SPEED);
        break;
    case ID_CACTUS:
        elements_manager_set_visible(ID_BIRD, false);
        elements_manager_set_visible(ID_CACTUS, true);

        move_manager_stop_element(ID_BIRD);
        elements_manager_move_element(ID_BIRD, B_X_START, B_Y_START);

        elements_manager_move_element(ID_CACTUS, C_X_START, C_Y_START);
        move_manager_move_element_with_const_speed(ID_CACTUS, C_X_TARGET, C_Y_TARGET, CACTUS_SPEED);
        break;
    default:
        return;
        break;
    }

    current_enemy = id;
}

void enemy_update()
{
    element_t *enemy = elements_manager_find_element(current_enemy);
    if (enemy == NULL)
    {
        return;
    }

    element_id_t enemy_id;
    if (start_cactus_count < 10)
    {
        enemy_id = ID_CACTUS;
        start_cactus_count++;
    }
    else
    {
        uint32_t random = HAL_GetTick() % 100;
        random = random % 2;
        enemy_id = random == 0 ? ID_CACTUS : ID_BIRD;
    }

    enemy_set_current(enemy_id);
}
