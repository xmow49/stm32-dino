#ifndef ELEMENTS_MANAGER_H
#define ELEMENTS_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    TYPE_FILL,
    TYPE_SPRITE,
    TYPE_BITMAP,
    TYPE_TEXT,
} type_t;

typedef enum
{
    ID_SKY,
    ID_GROUND,
    ID_CROWN,

    ID_ROCK_0,
    ID_ROCK_1,
    ID_ROCK_2,

    ID_HOLE_0,
    ID_HOLE_1,
    ID_HOLE_2,

    ID_CACTUS,
    ID_BIRD,

    ID_CLOUD_0,
    ID_CLOUD_1,
    ID_MOON,

    ID_DINO_STAND,
    ID_DINO_SIT,

    ID_COPYRIGHT,
    ID_GAME_OVER,
    ID_PRESS_RETRY,
    ID_PRESS_START,

} element_id_t;

typedef enum
{
    SPEED_N_FRAME,
    SPEED_DELTA_FRAME,
} speed_unit_t;

typedef enum
{
    MOVE_NO,
    MOVE_IN_PROGRESS,
} move_manager_status_t;

typedef struct
{
    type_t type;
    element_id_t id;
    int x;
    int y;
    int width;
    int height;
    union
    {
        struct
        {
            uint16_t color;
        } fill;
        struct
        {
            uint16_t scale;
            const uint16_t *sprite;
        } sprite;
        struct
        {
            uint16_t scale;
            const char *text;
        } text;
    } data;
    struct
    {
        move_manager_status_t status;
        int target_x;
        int target_y;
        int speed;
        int px_per_frame;
        speed_unit_t speed_unit;
        uint32_t start_frame_index;
    } move;
    bool visible;
} element_t;

extern const uint16_t elements_count;
extern element_t elements_list[];
extern uint32_t frame_count;

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

int elements_manager_update_full_screen();

int elements_manager_move_element(element_id_t id, int16_t target_x, int16_t target_y);

element_t *elements_manager_find_element(element_id_t id);

int elements_manager_update_element(element_id_t id);
int elements_manager_set_visible(element_id_t id, bool visible);
int elements_manager_set_dark_mode(bool state);
bool elements_manager_get_dark_mode();

#endif // ELEMENTS_MANAGER_H
