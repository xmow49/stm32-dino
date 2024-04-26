#include "stm32f1xx_hal.h"
#include "lib/stm/stm32f1_uart.h"
#include "macro_types.h"
#include "lib/stm/stm32f1_gpio.h"
#include <stdint.h>
#include <lib/tft_ili9341/stm32f1_ili9341.h>


static uint32_t GetPage(uint32_t Address)
{
 for (int indx=0; indx<128; indx++)
 {

 if((Address < (0x08000000 + (FLASH_PAGE_SIZE *(indx+1))) ) && (Address >= (0x08000000 + FLASH_PAGE_SIZE*indx)))
 {
 return (0x08000000 + FLASH_PAGE_SIZE*indx);
 }
 }
 return 0;
}

uint32_t Flash_Write_Data(uint32_t StartPageAddress, uint32_t *Data, uint16_t numberofwords)
{

    static FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PAGEError;
    int sofar = 0;

    /* Unlock the Flash to enable the flash control register access *************/
    HAL_FLASH_Unlock();

    /* Erase the user Flash area*/

    uint32_t StartPage = GetPage(StartPageAddress);
    uint32_t EndPageAdress = StartPageAddress + numberofwords * 4;
    uint32_t EndPage = GetPage(EndPageAdress);

    /* Fill EraseInit structure*/
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = StartPage;
    EraseInitStruct.NbPages = ((EndPage - StartPage) / FLASH_PAGE_SIZE) + 1;

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
    {
        /*Error occurred while page erase.*/
        return HAL_FLASH_GetError();
    }

    /* Program the user Flash area word by word*/

    while (sofar < numberofwords)
    {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, StartPageAddress, Data[sofar]) == HAL_OK)
        {
            StartPageAddress += 4; // use StartPageAddress += 2 for half word and 8 for double word
            sofar++;
        }
        else
        {
            /* Error occurred while writing data in Flash memory*/
            return HAL_FLASH_GetError();
        }
    }

    /* Lock the Flash to disable the flash control register access (recommended
       to protect the FLASH memory against possible unwanted operation) *********/
    HAL_FLASH_Lock();

    return 0;
}

void Flash_Read_Data(uint32_t StartPageAddress, uint32_t *RxBuf, uint16_t numberofwords)
{
    while (1)
    {

        *RxBuf = *(__IO uint32_t *)StartPageAddress;
        StartPageAddress += 4;
        RxBuf++;
        if (!(--numberofwords))
            break;
    }
}
