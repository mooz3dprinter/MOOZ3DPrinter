#include "hmi.h"
#include <stdio.h>
#include "hmi_drv.h"
#include <stdarg.h>
#include "stm32f4xx_hal.h"
#include <math.h>

#define PROTOCOL_TERMINATOR     0xFF
#define PROTOCOL_TERMINATOR_LEN 3
static HMICallbackFunc gHMICallbackFunc = NULL;

void HMIInit(void)
{
    HMIDrvInit();
}

void HMIRegisterCallback(HMICallbackFunc hmiCallback)
{
    if (hmiCallback) {
        gHMICallbackFunc = hmiCallback;
    }
}

void HMISendCmd(const char *fmt, ...)
{
    char string[128];
    const char *strIterator = string;
    va_list ap;
    RingBuffer *txRingBuffer = HMIDrvTxRingBuffer();

    va_start(ap, fmt);
    vsprintf(string, fmt, ap);
    va_end(ap);

    while (*strIterator != '\0' && RingBufferIsFull(txRingBuffer) == false) {
        RingBufferEnqueue(txRingBuffer, (void *)strIterator);
        ++strIterator;
    }

    //printf("Send to HMI:%s\r\n", string);
    char terminator = PROTOCOL_TERMINATOR;

    for (uint32_t i = 0; i < PROTOCOL_TERMINATOR_LEN && RingBufferIsFull(txRingBuffer) == false; ++i) {
        RingBufferEnqueue(txRingBuffer, &terminator);
    }
}

static void HMIWriteProcess(void)
{
    RingBuffer *txRingBuffer = HMIDrvTxRingBuffer();

    if (RingBufferIsEmpty(txRingBuffer) == false) {
        HMIDrvEnableSend();
    }
}

static void HMIReadProcess(void)
{
    uint8_t id;
    uint8_t data[127];
    uint32_t dataLen;
    RingBuffer *rxRingBuffer = HMIDrvRxRingBuffer();
    static bool firstEnterFlag = true;
    static uint32_t systick;

    // First enter and refresh the systick
    if (firstEnterFlag) {
        firstEnterFlag = false;
        systick = HAL_GetTick();
    }

    if (RingBufferIsEmpty(rxRingBuffer)) {
        // When no data, refresh the systick
        systick = HAL_GetTick();
        return;
    } else {
        // Data available, but timeout!
        if (HAL_GetTick() - systick > 100) {
            // Clear the ring buffer and refresh the systick
            systick = HAL_GetTick();
            printf("%s:Clear the ring buffer!\r\n", __FUNCTION__);
            RingBufferClear(rxRingBuffer);
            return;
        }
    }

    // Check data
    while (1) {
        uint32_t packetLen = RingBufferGetCount(rxRingBuffer);

        // At least PROTOCOL_TERMINATOR_LEN bytes
        if (packetLen < PROTOCOL_TERMINATOR_LEN) {
            break;
        }

        // Check terminator bytes
        bool terminated = false;
        uint32_t terminatedAddr = 0;

        for (uint32_t i = 0; i < packetLen - PROTOCOL_TERMINATOR_LEN + 1; ++i) {
            terminated = true;

            for (uint32_t j = 0; j < PROTOCOL_TERMINATOR_LEN; j++) {
                if ((*(uint8_t *)RingBufferDataAt(rxRingBuffer, i + j)) != PROTOCOL_TERMINATOR) {
                    // Three consecutive terminator
                    terminated = false;
                    break;
                }
            }

            if (terminated) {
                terminatedAddr = i;
                break;
            }
        }

        // Terminator found!
        if (terminated) {
            // Refresh the time
            systick = HAL_GetTick();
            uint32_t len = terminatedAddr;

            if (len == 0) {
                RingBufferDequeue(rxRingBuffer, NULL);
                continue;
            } else if (len > sizeof(data) + 1) {
                RingBufferDequeue(rxRingBuffer, NULL);
                continue;
            }

            // ID
            RingBufferDequeue(rxRingBuffer, &id);
            // Data
            dataLen = len - 1;

            for (uint32_t i = 0; i < dataLen; ++i) {
                RingBufferDequeue(rxRingBuffer, (void *)&data[i]);
            }

            // Terminators
            for (uint32_t i = 0; i < PROTOCOL_TERMINATOR_LEN; ++i) {
                RingBufferDequeue(rxRingBuffer, NULL);
            }

            // Callback
            if (gHMICallbackFunc) {
                gHMICallbackFunc(id, data, dataLen);
            }
        } else {
            break;
        }
    }
}

void HMIProcess(void)
{
    HMIWriteProcess();
    HMIReadProcess();
#if 0
    static uint32_t systick = HAL_GetTick();
    static uint32_t count = 0;
    static uint32_t count2 = 0;
    static uint32_t state = 0;
    uint32_t tickInterval = HAL_GetTick() - systick;

    switch (state) {
    case 0:
        if (tickInterval > 100) {
            systick = HAL_GetTick();
            HMISendCmd("add 1,0,%d", (int32_t)(50 + 50 * sin(count / 5 / 3.1415926535f)));
            HMISendCmd("add 1,1,%d", (int32_t)(50 + 50 * cos(count / 5 / 3.1415926535f)));
            count++;
            state = 1;
        }

        break;

    case 1:
        if (tickInterval > 100) {
            systick = HAL_GetTick();
            HMISendCmd("j0.val=%d", count2);
            HMISendCmd("t2.txt=\"%d\"", count2);
            count2++;

            if (count2 > 100) {
                count2 = 0;
            }

            state = 0;
        }

        break;
    }

#endif
}
