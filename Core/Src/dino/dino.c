#include "stm32f1xx_hal.h"
#include "lib/stm/stm32f1_uart.h"
#include "macro_types.h"
#include "lib/stm/stm32f1_gpio.h"
#include <stdint.h>
#include <lib/tft_ili9341/stm32f1_ili9341.h>

#include "dino/dino.h"
#include "dino/sprites.h"
#include "dino/framebuffer.h"
#include "dino/elements_manager.h"

void writeLED(bool_e b)
{
	HAL_GPIO_WritePin(LED_GREEN_GPIO, LED_GREEN_PIN, b);
}

bool_e readButton(void)
{
	return HAL_GPIO_ReadPin(BLUE_BUTTON_GPIO, BLUE_BUTTON_PIN);
}

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
	// ILI9341_Fill(0x769f);
	// ILI9341_DrawFilledRectangle(0, 168, 320, 240, 0xf6b2);
	// ILI9341_putBitmap(77, 124, 20, 22, 2, sprite_dino_stand, 20 * 22);

	elements_manager_update_full_screen();

	while (1)
	{

		for (int i = 320; i > 0; i--)
		{
			elements_manager_move_element(ID_CACTUS_1, i, 140);
		}
		//
		//		elements_manager_move_element(ID_DINO, 82, 124);

		// elements_manager_update_full_screen();

		if (!readButton())
		{
			for (int i = 124; i > 80; i -= 2)
			{
				elements_manager_move_element(ID_DINO, 82, i);
			}
			for (int i = 80; i < 124; i += 2)
			{
				elements_manager_move_element(ID_DINO, 82, i);
			}
		}
	}
}
int a = 0;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == BLUE_BUTTON_PIN)
	{
		a++;
	}
	else
	{
		__NOP();
	}
}
