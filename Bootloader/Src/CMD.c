//#include "stm32f0xx_hal_dma.h"
#include "CMD.h"
#include "Flash.h"
#include <stdio.h>
#include "UART.h"

pFunction JumpToApplication;
uint32_t JumpAddress;

extern uint8_t aRxBuffer[RXBUFFERSIZE];

void CMD_DeCommand(void)
{
	char temp[20];
	
	if(UART_GetUartRxFlag())
	{
		UART_SetUartRxFlag(RESET);
		
		switch(aRxBuffer[0])
		{   
			case 0x00:
				UART_Tx((uint8_t*)"\r\n\r\nNULL", strlen("\r\n\r\nNULL"));
				break;
				
			case 0x30: //info
				UART_Tx((uint8_t*)"\r\n\r\n[BTL]", strlen("\r\n\r\n[BTL]"));
				break;
			
			case 0x31: //receive BIN and check
				UART_Tx((uint8_t*)"\r\n\r\n01", strlen("\r\n\r\n01"));
				break;
			
			case 0x32: //write to flash amd check
				UART_Tx((uint8_t*)"\r\n\r\n02", strlen("\r\n\r\n02"));
				break;
			
			case 0x33: //jump
				UART_Tx((uint8_t*)"\r\n\r\nJump", strlen("\r\n\r\nJump"));
                //FLASH_Init();
				CMD_Jump();
				break;
			
			default: 
				UART_Tx((uint8_t*)"\r\n\r\nDefault", strlen("\r\n\r\nDefault"));
				
                //CMD_Write();
            
                //UART_Tx((uint8_t*)"\r\n\r\nDone", strlen("\r\n\r\nDone"));
            
			    //UART_Tx((uint8_t*)"\r\n\r\n", 4);
				//UART_Tx(aRxBuffer, strlen((char*)aRxBuffer));
				break;
		}
		
		UART_Rx();	
	}
}

void CMD_Write(void)
{
	/* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  /* Erase the user Flash area
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

  /* Fill EraseInit structure*/
  Flash_Erase();

  /* Program the user Flash area word by word
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/
	
  

  Flash_ProgramWordByWord(aRxBuffer);

  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();

  /* Check if the programmed data is OK
      MemoryProgramStatus = 0: data programmed correctly
      MemoryProgramStatus != 0: number of words not programmed correctly ******/
  Flash_CheckProgramData();
}

//__IO uint32_t VectorTable[48] __attribute__((at(0X20000100)));
//#define SYSCFG_MemoryRemap_SRAM                 ((uint8_t)0x03)
//#define VECT_TAB_OFFSET  0x6000

void SYSCFG_MemoryRemapConfig(uint32_t SYSCFG_MemoryRemap)
{
  uint32_t tmpctrl = 0;

  /* Check the parameter */
  assert_param(IS_SYSCFG_MEMORY_REMAP(SYSCFG_MemoryRemap));

  /* Get CFGR1 register value */
  tmpctrl = SYSCFG->CFGR1;

  /* Clear MEM_MODE bits */
  tmpctrl &= (uint32_t) (~SYSCFG_CFGR1_MEM_MODE);

  /* Set the new MEM_MODE bits value */
  tmpctrl |= (uint32_t) SYSCFG_MemoryRemap;

  /* Set CFGR1 register with the new memory remap configuration */
  SYSCFG->CFGR1 = tmpctrl;
}

void SetVectorTable(void)
{
    //__ASM("__Vectors  DCD  __initial_sp");
    //__ASM("__Vectors  DCD  msp");
}

    
void CMD_Jump(void)
{
    /*uint32_t i = 0;
    
	for(i = 0; i < 48; i++)
	{
		VectorTable[i] = *(__IO uint32_t*)((FLASH_BASE | VECT_TAB_OFFSET) + (i<<2));
	}*/
	
    
    HAL_RCC_DeInit();
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;
    /** * Step: Disable all interrupts */
    __disable_irq();
    /* ARM Cortex-M Programming Guide to Memory Barrier Instructions.*/
    __DSB();
    __HAL_SYSCFG_REMAPMEMORY_SYSTEMFLASH();
    
    /* Remap is bot visible at once. Execute some unrelated command! */
    __DSB();
    __ISB();

	JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
    /* Jump to user application */
    JumpToApplication = (pFunction) JumpAddress;
    /* Initialize user application's Stack Pointer */
    __set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
    //__enable_irq();
    //SetVectorTable();
//#define __VECTOR_TABLE  __Vectors
    //SYSCFG_MemoryRemapConfig(SYSCFG_MemoryRemap_SRAM);
	JumpToApplication();
}