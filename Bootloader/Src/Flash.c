#include "Flash.h"


/* Private define ------------------------------------------------------------*/
#define FLASH_USER_START_ADDR   ADDR_FLASH_PAGE_12   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR     ADDR_FLASH_PAGE_16 + FLASH_PAGE_SIZE   /* End @ of user Flash area */

#define DATA_32                 ((uint32_t)0x12345678)

/* Private variables ---------------------------------------------------------*/
uint32_t Address = 0, PageError = 0;
__IO uint32_t data32 = 0 , MemoryProgramStatus = 0;

/*Variable used for Erase procedure*/
static FLASH_EraseInitTypeDef EraseInitStruct;

void FLASH_Init(void)
{
  /* Unlock the Program memory */
  HAL_FLASH_Unlock();

  /* Clear all FLASH flags */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_WRPERR );/*| FLASH_FLAG_PGAERR | FLASH_FLAG_SIZERR |
                         FLASH_FLAG_OPTVERR | FLASH_FLAG_RDERR | FLASH_FLAG_FWWERR |
                         FLASH_FLAG_NOTZEROERR);*/
  /* Unlock the Program memory */
  HAL_FLASH_Lock();
}

void Flash_Erase(void)
{
/* Erase the user Flash area
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

  /* Fill EraseInit structure*/
  EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.PageAddress = FLASH_USER_START_ADDR;
  EraseInitStruct.NbPages = (FLASH_USER_END_ADDR - FLASH_USER_START_ADDR) / FLASH_PAGE_SIZE;

  if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
  {
    /*
      Error occurred while page erase.
      User can add here some code to deal with this error.
      PageError will contain the faulty page and then to know the code error on this page,
      user can call function 'HAL_FLASH_GetError()'
    */
    /* Infinite loop */
    while (1)
    {
      BSP_LED_On(LED5);
    }
  }
}
  
/* Program the user Flash area word by word
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/
void Flash_ProgramWordByWord(uint8_t* data_8)
{
    DATA_32_8 data; 
    Address = FLASH_USER_START_ADDR;

  while (Address < FLASH_USER_END_ADDR)
  {
	  memcpy(data.data_8, data_8, COUNTOF(data_8));
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, data.data_32) == HAL_OK)
    {
		data_8 += 4;
		Address = Address + 4;
    }
    else
    {
      /* Error occurred while writing data in Flash memory.
         User can add here some code to deal with this error */
      while (1)
      {
        BSP_LED_On(LED5);
      }
    }
  }
}


/* Check if the programmed data is OK
      MemoryProgramStatus = 0: data programmed correctly
      MemoryProgramStatus != 0: number of words not programmed correctly ******/
void Flash_CheckProgramData(void)
{
  Address = FLASH_USER_START_ADDR;
  MemoryProgramStatus = 0x0;

  while (Address < FLASH_USER_END_ADDR)
  {
    data32 = *(__IO uint32_t *)Address;

    if (data32 != DATA_32)
    {
      MemoryProgramStatus++;
    }
    Address = Address + 4;
  }

  /*Check if there is an issue to program data*/
  if (MemoryProgramStatus == 0)
  {
    /* No error detected. Switch on LED3*/
    BSP_LED_On(LED3);
  }
  else
  {
    /* Error detected. Switch on LED4*/
    BSP_LED_On(LED4);
  }
}
