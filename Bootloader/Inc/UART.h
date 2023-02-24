#include "main.h"


#define USARTx                           USART1
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE()
#define DMAx_CLK_ENABLE()                __HAL_RCC_DMA1_CLK_ENABLE()
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __HAL_RCC_USART1_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART1_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_9
#define USARTx_TX_GPIO_PORT              GPIOA
#define USARTx_TX_AF                     GPIO_AF1_USART1
#define USARTx_RX_PIN                    GPIO_PIN_10
#define USARTx_RX_GPIO_PORT              GPIOA
#define USARTx_RX_AF                     GPIO_AF1_USART1

/* Definition for USARTx's DMA */
#define USARTx_TX_DMA_CHANNEL             DMA1_Channel2
#define USARTx_RX_DMA_CHANNEL             DMA1_Channel3


/* Definition for USARTx's NVIC */
#define USARTx_DMA_TX_IRQn                DMA1_Channel2_3_IRQn
#define USARTx_DMA_RX_IRQn                DMA1_Channel2_3_IRQn
#define USARTx_DMA_TX_IRQHandler          DMA1_Channel2_3_IRQHandler
#define USARTx_DMA_RX_IRQHandler          DMA1_Channel2_3_IRQHandler

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      USART1_IRQn
#define USARTx_IRQHandler                USART1_IRQHandler

/* Size of Trasmission buffer */
#define TXBUFFERSIZE                      (COUNTOF(aTxBuffer) - 1)
/* Size of Reception buffer */
#define RXBUFFERSIZE                      4 //TXBUFFERSIZE
  

/* Exported functions ------------------------------------------------------- */


void UART_Init(void);
void UART_Tx(uint8_t* buffer, uint8_t len);
void UART_Rx(void);
FlagStatus UART_GetUartRxFlag(void);
void UART_SetUartRxFlag(FlagStatus en);
void UART_Rx_IRQ(void);
void UART_Rx_ResetIndex(void);
void UART_Tx_ResetIndex(void);
