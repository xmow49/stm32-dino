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
    move_manager_move_element(ID_CACTUS_1, C_X_TARGET, C_Y_TARGET, CACTUS_SPEED);

    elements_manager_move_element(ID_CLOUD_0, CLOUD_X_START, CLOUD_Y_START);
    move_manager_move_element(ID_CLOUD_0, CLOUD_X_TARGET, CLOUD_Y_TARGET, CLOUD_SPEED);

    move_manager_move_element(ID_ROCK_0, ROCK_X_TARGET, ROCK_Y_TARGET, CACTUS_SPEED);
    move_manager_move_element(ID_ROCK_1, ROCK_X_TARGET, ROCK_Y_TARGET, CACTUS_SPEED);
    move_manager_move_element(ID_ROCK_2, ROCK_X_TARGET, ROCK_Y_TARGET, CACTUS_SPEED);

    move_manager_move_element(ID_HOLE_0, HOLE_X_TARGET, HOLE_Y_TARGET, CACTUS_SPEED);
    move_manager_move_element(ID_HOLE_1, HOLE_X_TARGET, HOLE_Y_TARGET, CACTUS_SPEED);

    // elements_manager_move_element(ID_CLOUD_1, abs(CLOUD_X_START - CLOUD_X_TARGET) * 2, CLOUD_Y_START);
    // move_manager_move_element(ID_CLOUD_1, CLOUD_X_TARGET, CLOUD_Y_TARGET, CLOUD_SPEED / 2);

    return 0;
}

int move_manager_move_element(element_id_t element_id, int x, int y, int in_n_frame)
{
    element_t *element = elements_manager_find_element(element_id);
    if (element == NULL)
    {
        return -1;
    }
    element->move.target_x = x;
    element->move.target_y = y;
    element->move.speed = in_n_frame;

    int dx = abs(element->x - x);
    int dy = abs(element->y - y);

    if (dx > dy)
    {
        element->move.px_per_frame = dx / in_n_frame;
    }
    else
    {
        element->move.px_per_frame = dy / in_n_frame;
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
            float fspeed;
            if (dx > dy)
            {
                fspeed = fabs(element->x - x) / (float)in_n_frame;
            }
            else
            {
                fspeed = fabs(element->y - y) / (float)in_n_frame;
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
        elements_manager_move_element(ID_CACTUS_1, C_X_START, C_Y_START);
        move_manager_move_element(ID_CACTUS_1, C_X_TARGET, C_Y_TARGET, CACTUS_SPEED);
        break;
    case ID_CLOUD_0:
    case ID_CLOUD_1:
        elements_manager_move_element(element_id, CLOUD_X_START, CLOUD_Y_START);
        move_manager_move_element(element_id, CLOUD_X_TARGET, CLOUD_Y_TARGET, CLOUD_SPEED);
        break;
    case ID_DINO:
    {
        dino_process_jump();
        break;
    }
    case ID_ROCK_0:
    case ID_ROCK_1:
    case ID_ROCK_2:
    {
        elements_manager_move_element(element_id, ROCK_X_START, ROCK_Y_START);
        move_manager_move_element(element_id, ROCK_X_TARGET, ROCK_Y_TARGET, CACTUS_SPEED);
        break;
    }

    case ID_HOLE_0:
    case ID_HOLE_1:
    case ID_HOLE_2:
    {
        elements_manager_move_element(element_id, HOLE_X_START, HOLE_Y_START);
        move_manager_move_element(element_id, HOLE_X_TARGET, HOLE_Y_TARGET, CACTUS_SPEED);
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
        if (element->move.status != MOVE_IN_PROGRESS)
        {
            continue;
        }
        if (element->x == element->move.target_x && element->y == element->move.target_y)
        {
            element->move.status = MOVE_NO;
            elements_manager_move_element(element->id, element->move.target_x, element->move.target_y);
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

        uint16_t dx = abs(element->x - element->move.target_x);
        uint16_t dy = abs(element->y - element->move.target_y);
        if (element->x < element->move.target_x)
        {
            new_x += dx > element->move.px_per_frame ? element->move.px_per_frame : dx;
        }
        else if (element->x > element->move.target_x)
        {
            new_x -= dx > element->move.px_per_frame ? element->move.px_per_frame : dx;
        }

        if (element->y < element->move.target_y)
        {
            new_y += dy > element->move.px_per_frame ? element->move.px_per_frame : dy;
        }
        else if (element->y > element->move.target_y)
        {
            new_y -= dy > element->move.px_per_frame ? element->move.px_per_frame : dy;
        }
        if (element->x == element->move.target_x && element->y == element->move.target_y)
        {
            element->move.status = MOVE_NO;
            elements_manager_move_element(element->id, element->move.target_x, element->move.target_y);
            move_manager_finish_cb(element->id);
            continue;
        }
        elements_manager_move_element(element->id, new_x, new_y);
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
