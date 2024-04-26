/*
 * flash.h
 *
 *  Created on: Apr 26, 2024
 *      Author: dbenech
 */

#ifndef INC_DINO_FLASH_H_
#define INC_DINO_FLASH_H_

#include <stdint.h>

uint32_t Flash_Write_Data(uint32_t StartPageAddress, uint32_t *Data, uint16_t numberofwords);
void Flash_Read_Data(uint32_t StartPageAddress, uint32_t *RxBuf, uint16_t numberofwords);

#endif /* INC_DINO_FLASH_H_ */
