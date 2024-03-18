
#ifndef MOVE_MANAGER_H
#define MOVE_MANAGER_H

#include "dino/elements_manager.h"

int move_manager_init(void);

int move_manager_move_element(element_id_t element_id, int x, int y, float speed);

int move_manager_loop(void);

#endif // MOVE_MANAGER_H
