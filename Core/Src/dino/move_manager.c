#include <stdio.h>
#include "stm32f1xx_hal.h"

#include "dino/move_manager.h"
#include "dino/dino.h"
#include "dino/sprites.h"
#include "dino/sound.h"
#include <math.h>
typedef enum
{
    JUMP_NO,
    JUMP_START,
    JUMP_UP_FAST,
    JUMP_UP_SLOW,
    JUMP_DOWN_SLOW,
    JUMP_DOWN_FAST,
    JUMP_DOWN_FALL,
    JUMP_WAIT_GROUND,
} jump_status_t;

jump_status_t jump_status = JUMP_NO;
volatile uint8_t dino_new_position = 0; // 0 = no move, 1 = set to sit down, 2 = set to stand up

uint32_t frame_count = 0;

int move_manager_init()
{
    jump_status = JUMP_NO;
    // move_manager_move_element_with_const_speed(ID_CACTUS, C_X_TARGET, C_Y_TARGET, CACTUS_SPEED);

    elements_manager_move_element(ID_CLOUD_0, CLOUD_X_START, CLOUD_Y_START);
    move_manager_move_element(ID_CLOUD_0, CLOUD_X_TARGET, CLOUD_Y_TARGET, CLOUD_SPEED);

    elements_manager_move_element(ID_CLOUD_1, CLOUD_X_START / 2, CLOUD_Y_START);
    move_manager_move_element(ID_CLOUD_1, CLOUD_X_TARGET, CLOUD_Y_TARGET, CLOUD_SPEED);

    move_manager_move_element_with_const_speed(ID_ROCK_0, ROCK_X_TARGET, ROCK_Y_TARGET, CACTUS_SPEED);
    move_manager_move_element_with_const_speed(ID_ROCK_1, ROCK_X_TARGET, ROCK_Y_TARGET, CACTUS_SPEED);
    move_manager_move_element_with_const_speed(ID_ROCK_2, ROCK_X_TARGET, ROCK_Y_TARGET, CACTUS_SPEED);

    move_manager_move_element_with_const_speed(ID_HOLE_0, HOLE_X_TARGET, HOLE_Y_TARGET, CACTUS_SPEED);
    move_manager_move_element_with_const_speed(ID_HOLE_1, HOLE_X_TARGET, HOLE_Y_TARGET, CACTUS_SPEED);

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
    element->move.start_frame_index = frame_count;

    return 0;
}

int move_manager_move_element_with_const_speed(element_id_t element_id, int x, int y, float px_per_frame)
{
    element_t *element = elements_manager_find_element(element_id);
    if (element == NULL)
    {
        return -1;
    }
    int px = (int)px_per_frame;
    float decimal = px_per_frame - px;

    element->move.target_x = x;
    element->move.target_y = y;

    // if (px_per_frame < 1)
    // {
    //     element->move.px_per_frame = 1;
    // }
    // else
    // {
    //     element->move.px_per_frame = px;
    // }

    // if (decimal == 0)
    // {
    //     element->move.speed = 0;
    // }
    // else
    // {
    //     element->move.speed = 1 / decimal;
    // }
    // element->move.speed_unit = SPEED_N_FRAME;
    element->move.speed_unit = SPEED_DELTA_FRAME;
    element->move.px_per_frame = px;
    element->move.status = MOVE_IN_PROGRESS;
    element->move.start_frame_index = frame_count;
    return 0;
}
int move_manager_stop_element(element_id_t element_id)
{
    element_t *element = elements_manager_find_element(element_id);
    if (element == NULL)
    {
        return 0;
    }
    element->move.status = MOVE_NO;
    return 0;
}

int move_manager_finish_cb(element_id_t element_id)
{

    switch (element_id)
    {
    case ID_CACTUS:
    case ID_BIRD:
        enemy_update();
        break;
    case ID_CLOUD_0:
    case ID_CLOUD_1:
        elements_manager_move_element(element_id, CLOUD_X_START, CLOUD_Y_START);
        move_manager_move_element(element_id, CLOUD_X_TARGET, CLOUD_Y_TARGET, CLOUD_SPEED);
        break;
    case ID_DINO_STAND:
    {
        dino_process_jump();
        break;
    }
    case ID_ROCK_0:
    case ID_ROCK_1:
    case ID_ROCK_2:
    {
        elements_manager_move_element(element_id, ROCK_X_START, ROCK_Y_START);
        move_manager_move_element_with_const_speed(element_id, ROCK_X_TARGET, ROCK_Y_TARGET, CACTUS_SPEED);
        break;
    }

    case ID_HOLE_0:
    case ID_HOLE_1:
    case ID_HOLE_2:
    {
        elements_manager_move_element(element_id, HOLE_X_START, HOLE_Y_START);
        move_manager_move_element_with_const_speed(element_id, HOLE_X_TARGET, HOLE_Y_TARGET, CACTUS_SPEED);
        break;
    }

    default:
        break;
    }

    return 0;
}

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
            if ((frame_count - element->move.start_frame_index) % element->move.speed != 0)
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

    // bird animation
    element_t *bird = elements_manager_find_element(ID_BIRD);
    if (bird->visible)
    {
        if (frame_count % 8 == 0)
        {
            bird->data.sprite.sprite = bird->data.sprite.sprite == sprite_bird_down ? sprite_bird_up : sprite_bird_down;
            elements_manager_update_element(ID_BIRD);
        }
    }

    frame_count++;
    return 0;
}

void dino_trigger_jump()
{
    if (jump_status == JUMP_NO)
    {
        jump_status = JUMP_START;
        dino_process_jump();
        sound_play(SOUND_JUMP);
    }
}

void dino_process_jump()
{
    switch (jump_status)
    {
    case JUMP_NO:
        break;

    case JUMP_START:
        jump_status = JUMP_UP_FAST;
        move_manager_move_element(ID_DINO_STAND, 82, 85, DINO_GRAVITY_SPEED);
        break;
    case JUMP_UP_FAST:
        jump_status = JUMP_UP_SLOW;
        move_manager_move_element(ID_DINO_STAND, 82, 70, DINO_GRAVITY_SPEED);
        break;
    case JUMP_UP_SLOW:
        jump_status = JUMP_DOWN_SLOW;
        move_manager_move_element(ID_DINO_STAND, 82, 85, DINO_GRAVITY_SPEED);
        break;
    case JUMP_DOWN_SLOW:
        jump_status = JUMP_DOWN_FAST;
        move_manager_move_element(ID_DINO_STAND, 82, 124, DINO_GRAVITY_SPEED);
        break;
    case JUMP_DOWN_FAST:
        jump_status = JUMP_NO;
        break;
    case JUMP_DOWN_FALL:
        jump_status = JUMP_WAIT_GROUND;
        move_manager_move_element(ID_DINO_STAND, 82, 124, 1);
        break;
    case JUMP_WAIT_GROUND:
        jump_status = JUMP_NO;
        break;
    default:
        break;
    }
}

void dino_process_position()
{
    if (dino_new_position)
    {
        if (jump_status != JUMP_NO)
        {
            jump_status = JUMP_DOWN_FALL;
            dino_process_jump();
            return;
        }
        switch (dino_new_position)
        {
        case 2:
            // stand
            elements_manager_set_visible(ID_DINO_SIT, false);
            elements_manager_set_visible(ID_DINO_STAND, true);
            current_dino = ID_DINO_STAND;
            break;
        case 1:
            // sit
            elements_manager_set_visible(ID_DINO_STAND, false);
            elements_manager_set_visible(ID_DINO_SIT, true);
            current_dino = ID_DINO_SIT;
            break;
        default:
            break;
        }
        __disable_irq();
        dino_new_position = 0;
        __enable_irq();
    }
}

void dino_trigger_position(uint8_t position)
{
    dino_new_position = position;
}
