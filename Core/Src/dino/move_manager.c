#include "dino/move_manager.h"
#include "dino/dino.h"
int move_manager_init()
{
     move_manager_move_element(ID_CACTUS_1, 20, 140, CACTUS_SPEED);
     move_manager_move_element(ID_CLOUD_0, 0, 43, 2);
     move_manager_move_element(ID_CLOUD_1, 0, 43, 2);

    return 0;
}

int move_manager_move_element(element_id_t element_id, int x, int y, int speed)
{
    element_t *element = elements_manager_find_element(element_id);
    element->move.target_x = x;
    element->move.target_y = y;
    element->move.speed = speed;

    int dx = abs(element->x - x);
    int dy = abs(element->y - y);

    if(dx > dy){
    element->move.px_per_frame = dx / speed;
    }else {
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
        	float fspeed = fabs(element->x - x) / (float)speed;
                    element->move.speed = (int)(1.0 / fspeed);
        }
    }

    element->move.status = MOVE_IN_PROGRESS;
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
        elements_manager_move_element(element_id, 280, 43);
        move_manager_move_element(element_id, 0, 43, 1000);
        break;
    case ID_DINO:
    {
        static int jump = 0;
        if (!jump)
        {
            move_manager_move_element(ID_DINO, 82, 124, DINO_GRAVITY_SPEED);
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

uint32_t frame_count = 0;

int move_manager_loop()
{
    for (int i = 0; i < elements_count; i++)
    {
        element_t *element = &elements_list[i];
        if (element->move.status == MOVE_IN_PROGRESS)
        {

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
