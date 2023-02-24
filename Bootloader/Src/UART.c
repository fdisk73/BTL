
#include "UART.h"

UART_HandleTypeDef UartHandle;
__IO ITStatus UartReadyFlag = RESET;
__IO uint32_t UserButtonStatus = 0;  /* set to 1 after User Button interrupt  */
__IO ITStatus UartRxFlag = RESET;


uint8_t aTxBuffer[200] = {0};
uint8_t aRxBuffer[RXBUFFERSIZE];

void UART_Init(void)
{
	memset(aTxBuffer, 0, 200);
	
/*##-1- Configure the UART peripheral ######################################*/
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* UART configured as follows:
      - Word Length = 8 Bits
      - Stop Bit = One Stop bit
      - Parity = None
      - BaudRate = 9600 baud
      - Hardware flow control disabled (RTS and CTS signals) */
  UartHandle.Instance        = USARTx;

  UartHandle.Init.BaudRate   = 9600;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits   = UART_STOPBITS_1;
  UartHandle.Init.Parity     = UART_PARITY_NONE;
  //UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode       = UART_MODE_TX_RX;
  UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_AUTOBAUDRATE_INIT | UART_ADVFEATURE_RXOVERRUNDISABLE_INIT;
	
  if(HAL_UART_DeInit(&UartHandle) != HAL_OK)
  {
    Error_Handler();
  }  
  if(HAL_UART_Init(&UartHandle) != HAL_OK)
  {
    Error_Handler();
  }
  
  __HAL_UART_ENABLE_IT(&UartHandle, UART_IT_IDLE); 
}



  
  /* The board sends the message and expects to receive it back */
  /* DMA is programmed for reception before starting the transmission, in order to
     be sure DMA Rx is ready when board 2 will start transmitting */

  /*##-2- Program the Reception process #####################################*/  
void UART_Rx()
{

	memset(aRxBuffer, 0, RXBUFFERSIZE);	
	HAL_UART_IRQHandler(&UartHandle);
	HAL_UART_DMAStop(&UartHandle);  //need it, or huart->RxState wont be ready.
	
	//UART_Rx_ResetIndex();
	
    if(HAL_UART_Receive_DMA(&UartHandle, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK)
    {
      Error_Handler();
    }	
    
	if(aRxBuffer[0])
	{
		UART_SetUartRxFlag(SET);
		return;
	}
	
	__HAL_UART_ENABLE_IT(&UartHandle, UART_IT_IDLE); 
	
    while (HAL_UART_GetState(&UartHandle) != HAL_UART_STATE_READY)
    {
    }
	
	
}

  /*##-3- Start the transmission process #####################################*/  
  /* While the UART in reception process, user can transmit data through 
     "aTxBuffer" buffer */
void UART_Tx(uint8_t* buffer, uint8_t len)
{
  
	
  if(HAL_UART_Transmit_DMA(&UartHandle, buffer, len)!= HAL_OK)
  {
    Error_Handler();
  }
  
  while (UartReadyFlag != SET)
  {
  }
  
  UartReadyFlag = RESET;
  
  while (HAL_UART_GetState(&UartHandle) != HAL_UART_STATE_READY)
  {
  }
  
  //UART_Tx_ResetIndex();
}


  





  /*##-6- Compare the sent and received buffers ##############################*/
  /*if(Buffercmp((uint8_t*)aTxBuffer,(uint8_t*)aRxBuffer,RXBUFFERSIZE))
  {
    Error_Handler();
  }*/

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /* Set transmission flag: trasfer complete*/
  UartReadyFlag = SET;

  /* Turn LED3 on: Transfer in transmission process is correct */
  BSP_LED_On(LED3); 
  
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /* Set transmission flag: trasfer complete*/
  UartReadyFlag = SET;

  /* Turn LED5 on: Transfer in reception process is correct */
  BSP_LED_On(LED5);
  
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
  /* Turn LED6 on: Transfer error in reception/transmission process */
  BSP_LED_On(LED6); 
}

FlagStatus UART_GetUartRxFlag(void)
{
	return UartRxFlag;
}

void UART_SetUartRxFlag(FlagStatus en)
{
	UartRxFlag = en;
}

void USAR_UART_IDLECallback(void)
{
	//HAL_UART_DMAResume(&UartHandle);
	HAL_UART_DMAStop(&UartHandle);  
	UART_SetUartRxFlag(SET);                 
}

void UART_Rx_IRQ(void)
{
	if(__HAL_UART_GET_FLAG(&UartHandle, UART_FLAG_IDLE) == SET)   
	{
		
		__HAL_UART_CLEAR_IDLEFLAG(&UartHandle);  
		
		HAL_UART_DMAStop(&UartHandle);  
		UART_Rx_ResetIndex();
		UART_SetUartRxFlag(SET);
		//UART_Rx_ResetIndex();
	}			
}

void UART_Rx_ResetIndex(void)
{	
	__HAL_DMA_DISABLE(UartHandle.hdmarx);
	UartHandle.hdmarx->Instance->CNDTR = RXBUFFERSIZE; 
	__HAL_DMA_ENABLE(UartHandle.hdmarx);
}

void UART_Tx_ResetIndex(void)
{
	__HAL_DMA_DISABLE(UartHandle.hdmatx);
	UartHandle.hdmatx->Instance->CNDTR = TXBUFFERSIZE; 
	__HAL_DMA_ENABLE(UartHandle.hdmatx);
}



