#include "dino/move_manager.h"

int move_manager_init()
{
    move_manager_move_element(ID_CACTUS_1, 20, 140, 1);
    move_manager_move_element(ID_CLOUD_0, 0, 43, 1);

    return 0;
}

int move_manager_move_element(element_id_t element_id, int x, int y, float speed)
{
    element_t *element = elements_manager_find_element(element_id);
    element->move.target_x = x;
    element->move.target_y = y;
    element->move.speed = speed;
    element->move.status = MOVE_IN_PROGRESS;
    return 0;
}

int move_manager_finish_cb(element_id_t element_id)
{

    switch (element_id)
    {
    case ID_CACTUS_1:
        elements_manager_move_element(ID_CACTUS_1, 290, 140);
        move_manager_move_element(ID_CACTUS_1, 20, 140, 2);
        break;
    case ID_DINO:
    {
        static int jump = 0;
        if (!jump)
        {
            move_manager_move_element(ID_DINO, 82, 124, 3);
            jump++;
        }
        else
        {
            jump = 0;
        }
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
        if (element->move.status == MOVE_IN_PROGRESS)
        {
            uint16_t new_x = element->x;
            uint16_t new_y = element->y;
            if (element->x < element->move.target_x)
            {
                new_x += abs(new_x - element->move.target_x) > element->move.speed ? element->move.speed : abs(new_x - element->move.target_x);
            }
            else if (element->x > element->move.target_x)
            {
                new_x -= abs(new_x - element->move.target_x) > element->move.speed ? element->move.speed : abs(new_x - element->move.target_x);
            }
            if (element->y < element->move.target_y)
            {
                new_y += abs(new_y - element->move.target_y) > element->move.speed ? element->move.speed : abs(new_y - element->move.target_y);
            }
            else if (element->y > element->move.target_y)
            {
                new_y -= abs(new_y - element->move.target_y) > element->move.speed ? element->move.speed : abs(new_y - element->move.target_y);
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

    return 0;
}
