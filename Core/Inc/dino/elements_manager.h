#ifndef ELEMENTS_MANAGER_H
#define ELEMENTS_MANAGER_H

typedef enum
{
    TYPE_FILL,
    TYPE_SPRITE,
    TYPE_TEXT,
} type_t;

typedef enum
{
    ID_SKY,
    ID_GROUND,

    ID_CACTUS_0,
    ID_CACTUS_1,
    ID_CACTUS_2,

    ID_CLOUD_0,
    ID_CLOUD_1,

    ID_DINO,
    ID_BIRD,

    ID_COPYRIGHT,
    ID_GAME_OVER,

} element_id_t;

typedef struct
{
    type_t type;
    element_id_t id;
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
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

} element_t;


#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

int elements_manager_update_full_screen();

int elements_manager_move_element(element_id_t id, int16_t target_x, int16_t target_y);

#endif // ELEMENTS_MANAGER_H
