#ifndef HMI_DRV_H
#define HMI_DRV_H

#include "RingBuffer.h"

extern void HMIDrvInit(void);
extern void HMIDrvEnableSend(void);
extern RingBuffer *HMIDrvTxRingBuffer(void);
extern RingBuffer *HMIDrvRxRingBuffer(void);

#endif
