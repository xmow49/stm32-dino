#include <stdio.h>

#include "dino/move_manager.h"
#include "dino/dino.h"
#include <math.h>
typedef enum
{
    JUMP_NO,
    JUMP_UP_FAST,
    JUMP_UP_SLOW,
    JUMP_DOWN_SLOW,
    JUMP_DOWN_FAST,
} jump_status_t;

jump_status_t jump_status = JUMP_NO;

int move_manager_init()
{
	jump_status = JUMP_NO;
    move_manager_move_element(ID_CACTUS_1, 20, 140, CACTUS_SPEED);
    move_manager_move_element(ID_CLOUD_0, 0, 43, CLOUD_SPEED);
    move_manager_move_element(ID_CLOUD_1, 0, 43, CLOUD_SPEED);

    return 0;
}

int move_manager_move_element(element_id_t element_id, int x, int y, int speed)
{
    element_t *element = elements_manager_find_element(element_id);
    if (element == NULL)
    {
        return -1;
    }
    element->move.target_x = x;
    element->move.target_y = y;
    element->move.speed = speed;

    int dx = abs(element->x - x);
    int dy = abs(element->y - y);

    if (dx > dy)
    {
        element->move.px_per_frame = dx / speed;
    }
    else
    {
        element->move.px_per_frame = dy / speed;
    }

    element->move.speed_unit = SPEED_DELTA_FRAME;
    if (element->move.px_per_frame == 0)
    {
        element->move.px_per_frame = 1;
        element->move.speed_unit = SPEED_N_FRAME;
        if (element->move.speed == 0)
        {
            printf("Speed can't be 0\n\r");
        }
        else
        {
        	float fspeed ;
        	 if (dx > dy){
        		 fspeed = fabs(element->x - x) / (float)speed;
        	 }else {
        		 fspeed = fabs(element->y - y) / (float)speed;

        	 }

            element->move.speed = (int)(1.0 / fspeed);
        }
    }

    element->move.status = MOVE_IN_PROGRESS;
    return 0;
}

int move_manager_stop_element(element_id_t element_id)
{
    element_t *element = elements_manager_find_element(element_id);
    element->move.status = MOVE_NO;
    return 0;
}

int move_manager_finish_cb(element_id_t element_id)
{

    switch (element_id)
    {
    case ID_CACTUS_1:
        elements_manager_move_element(ID_CACTUS_1, 290, 140);
        move_manager_move_element(ID_CACTUS_1, 20, 140, CACTUS_SPEED);
        break;
    case ID_CLOUD_0:
    case ID_CLOUD_1:
        elements_manager_move_element(element_id, 260, 43);
        move_manager_move_element(element_id, 0, 43, CLOUD_SPEED);
        break;
    case ID_DINO:
    {
        dino_process_jump();
        break;
    }

    default:
        break;
    }

    return 0;
}

uint32_t frame_count = 0;

int move_manager_loop()
{
    for (int i = 0; i < elements_count; i++)
    {
        element_t *element = &elements_list[i];
        if (element->move.status == MOVE_IN_PROGRESS)
        {

            if (element->x == element->move.target_x && element->y == element->move.target_y)
            {
                element->move.status = MOVE_NO;
                move_manager_finish_cb(element->id);
                continue;
            }

            switch (element->move.speed_unit)
            {
            case SPEED_DELTA_FRAME:
                break;

            case SPEED_N_FRAME:
                if (frame_count % element->move.speed != 0)
                {
                    continue;
                }
                break;

            default:
                break;
            }

            uint16_t new_x = element->x;
            uint16_t new_y = element->y;

            if (element->x < element->move.target_x)
            {
                new_x += abs(new_x - element->move.target_x) > element->move.px_per_frame ? element->move.px_per_frame : abs(new_x - element->move.target_x);
            }
            else if (element->x > element->move.target_x)
            {
                new_x -= abs(new_x - element->move.target_x) > element->move.px_per_frame ? element->move.px_per_frame : abs(new_x - element->move.target_x);
            }
            if (element->y < element->move.target_y)
            {
                new_y += abs(new_y - element->move.target_y) > element->move.px_per_frame ? element->move.px_per_frame : abs(new_y - element->move.target_y);
            }
            else if (element->y > element->move.target_y)
            {
                new_y -= abs(new_y - element->move.target_y) > element->move.px_per_frame ? element->move.px_per_frame : abs(new_y - element->move.target_y);
            }
            if (element->x == element->move.target_x && element->y == element->move.target_y)
            {
                element->move.status = MOVE_NO;
                move_manager_finish_cb(element->id);
                continue;
            }
            elements_manager_move_element(element->id, new_x, new_y);
        }
    }
    frame_count++;
    return 0;
}

void dino_process_jump()
{
    switch (jump_status)
    {
    case JUMP_NO:
        jump_status = JUMP_UP_FAST;
        move_manager_move_element(ID_DINO, 82, 85, DINO_GRAVITY_SPEED);
        break;
    case JUMP_UP_FAST:
        jump_status = JUMP_UP_SLOW;
        move_manager_move_element(ID_DINO, 82, 70, DINO_GRAVITY_SPEED);
        break;
    case JUMP_UP_SLOW:
        jump_status = JUMP_DOWN_SLOW;
        move_manager_move_element(ID_DINO, 82, 85, DINO_GRAVITY_SPEED);
        break;
    case JUMP_DOWN_SLOW:
        jump_status = JUMP_DOWN_FAST;
        move_manager_move_element(ID_DINO, 82, 124, DINO_GRAVITY_SPEED);
        break;
    case JUMP_DOWN_FAST:
        jump_status = JUMP_NO;
        break;
    default:
        break;
    }
}
