#include "hmi_drv.h"
#include "stm32f4xx_hal.h"

UART_HandleTypeDef gUart1;

#define RAW_BYTE_BUFFER_SIZE    5120
static uint8_t gTxRawByteBuffer[RAW_BYTE_BUFFER_SIZE];
static uint8_t gRxRawByteBuffer[RAW_BYTE_BUFFER_SIZE];
RingBuffer gTxRingBuffer;
RingBuffer gRxRingBuffer;

void HMIDrvInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    __GPIOD_CLK_ENABLE();
    __USART3_CLK_ENABLE();

    GPIO_InitStructure.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.Alternate = GPIO_AF7_USART3 ;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);

    UART_HandleTypeDef *handle = &gUart1;

    handle->Instance = USART3;
    handle->Init.BaudRate = 9600;
    handle->Init.WordLength = UART_WORDLENGTH_8B;
    handle->Init.StopBits = UART_STOPBITS_1;
    handle->Init.Parity = UART_PARITY_NONE;
    handle->Init.Mode = UART_MODE_TX_RX;
    handle->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    handle->Init.OverSampling = UART_OVERSAMPLING_16;

    HAL_UART_DeInit(handle);
    HAL_UART_Init(handle);

    HAL_NVIC_SetPriority(USART3_IRQn, 3, 1);
    HAL_NVIC_EnableIRQ(USART3_IRQn);

    __HAL_USART_ENABLE_IT(handle, USART_IT_RXNE);


    RingBufferInit(&gTxRingBuffer, gTxRawByteBuffer, RAW_BYTE_BUFFER_SIZE, sizeof(uint8_t));
    RingBufferInit(&gRxRingBuffer, gRxRawByteBuffer, RAW_BYTE_BUFFER_SIZE, sizeof(uint8_t));
}

void HMIDrvEnableSend(void)
{
    __HAL_USART_ENABLE_IT(&gUart1, USART_IT_TXE);
}

RingBuffer *HMIDrvTxRingBuffer(void)
{
    return &gTxRingBuffer;
}

RingBuffer *HMIDrvRxRingBuffer(void)
{
    return &gRxRingBuffer;
}



extern "C" void USART3_IRQHandler(void)
{
    uint32_t tmp1 = 0, tmp2 = 0;
    UART_HandleTypeDef *handle = &gUart1;

    tmp1 = __HAL_UART_GET_FLAG(handle, UART_FLAG_RXNE);
    tmp2 = __HAL_UART_GET_IT_SOURCE(handle, UART_IT_RXNE);

    /* UART in mode Receiver ---------------------------------------------------*/
    if ((tmp1 != RESET) && (tmp2 != RESET)) {
        uint8_t data = (uint8_t)(handle->Instance->DR & (uint8_t)0x00FF);

        if (RingBufferIsFull(&gRxRingBuffer) == false) {
            RingBufferEnqueue(&gRxRingBuffer, &data);
        }
    }

    tmp1 = __HAL_UART_GET_FLAG(handle, UART_FLAG_TXE);
    tmp2 = __HAL_UART_GET_IT_SOURCE(handle, UART_IT_TXE);

    /* UART in mode Transmitter ------------------------------------------------*/
    if ((tmp1 != RESET) && (tmp2 != RESET)) {
        if (RingBufferIsEmpty(&gTxRingBuffer) == false) {
            uint8_t data;
            RingBufferDequeue(&gTxRingBuffer, &data);
            handle->Instance->DR = data;
        } else {
            __HAL_USART_DISABLE_IT(&gUart1, USART_IT_TXE);
        }
    }
}
