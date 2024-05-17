#include "stm32f1xx_hal.h"
#include "lib/stm/stm32f1_uart.h"
#include "macro_types.h"
#include "lib/stm/stm32f1_gpio.h"
#include <stdint.h>
#include <stdbool.h>
#include <lib/tft_ili9341/stm32f1_ili9341.h>

#include "main.h"
#include "dino/dino.h"
#include "dino/sprites.h"
#include "dino/framebuffer.h"
#include "dino/elements_manager.h"
#include "dino/move_manager.h"
#include "dino/score.h"

void writeLED(bool_e b)
{
	HAL_GPIO_WritePin(LED_GREEN_GPIO, LED_GREEN_PIN, b);
}

bool_e readButton(void)
{
	return HAL_GPIO_ReadPin(BUTTON_UP_PORT, BUTTON_UP_PIN);
}

volatile bool game_over = true;
volatile bool want_restart = false;
int dino_main(void)
{
	// Initialisation de l'UART2 � la vitesse de 115200 bauds/secondes (92kbits/s) PA2 : Tx  | PA3 : Rx.
	// Attention, les pins PA2 et PA3 ne sont pas reli�es jusqu'au connecteur de la Nucleo.
	// Ces broches sont redirig�es vers la sonde de d�bogage, la liaison UART �tant ensuite encapsul�e sur l'USB vers le PC de d�veloppement.
	//		UART_init(UART2_ID, 115200);

	//"Indique que les printf sortent vers le p�riph�rique UART2."
	//		SYS_set_std_usart(UART2_ID, UART2_ID, UART2_ID);

	// Initialisation du port de la led Verte (carte Nucleo)
	BSP_GPIO_PinCfg(LED_GREEN_GPIO, LED_GREEN_PIN, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH);

	// Initialisation du port du bouton bleu (carte Nucleo)
	BSP_GPIO_PinCfg(BLUE_BUTTON_GPIO, BLUE_BUTTON_PIN, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH);

	ILI9341_Init();
	ILI9341_Rotate(ILI9341_Orientation_Landscape_2);
	elements_manager_update_full_screen();
	move_manager_init();
	score_init();

	uint32_t last_time = HAL_GetTick();
	uint32_t fps = 0;
	uint32_t frame_time = 0;
	uint32_t count = 0;
	while (1)
	{

		if (want_restart)
		{
			printf("reinit\n\r");
			move_manager_stop_element(ID_CACTUS_1);
			move_manager_stop_element(ID_DINO);
			elements_manager_set_visible(ID_GAME_OVER, false);
			score_save();
			elements_manager_move_element(ID_CACTUS_1, C_X_TARGET, C_Y_TARGET);
			elements_manager_move_element(ID_DINO, D_X, D_Y);
			elements_manager_update_full_screen();
			score_reset();
			HAL_Delay(1000);
			move_manager_init();

			__disable_irq();
			game_over = false;
			want_restart = false;
			__enable_irq();
		}

		if (!game_over)
		{
			last_time = HAL_GetTick();
			move_manager_loop();
			frame_time = HAL_GetTick() - last_time;
			fps = 1000 / (frame_time == 0 ? 1 : frame_time);
			// cap 30 fps
			if (fps > 30)
			{
				HAL_Delay(1000 / 30 - frame_time);
			}
			count++;
			if (count % 100 == 0)
			{
				printf("Time: %ld, fps: %ld\n\r", frame_time, fps);
				count = 0;
			}
			score_update();

			if (collision_check())
			{
				__disable_irq();
				game_over = true;
				__enable_irq();
				printf("Game Over\n\r");
				score_save();
				elements_manager_set_visible(ID_GAME_OVER, true);
			}
		}
	}
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	static uint32_t start_time = 0;
	if (!readButton())
	{
		start_time = HAL_GetTick();
		if (game_over)
		{
			want_restart = true;
		}
		else
		{
			if (elements_manager_find_element(ID_DINO)->move.status == MOVE_NO)
			{
				dino_process_jump();
			}
		}
	}
	else
	{
		if (HAL_GetTick() - start_time > 3000)
		{
			score_erase();
		}
	}
}
