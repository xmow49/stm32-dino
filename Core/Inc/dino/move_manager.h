
#ifndef MOVE_MANAGER_H
#define MOVE_MANAGER_H

#include "dino/elements_manager.h"

int move_manager_init(void);

int move_manager_move_element(element_id_t element_id, int x, int y, int speed);
int move_manager_move_element_with_const_speed(element_id_t element_id, int x, int y, float px_per_frame);

int move_manager_loop(void);

int move_manager_stop_element(element_id_t element_id);

void dino_process_jump();

#endif // MOVE_MANAGER_H
