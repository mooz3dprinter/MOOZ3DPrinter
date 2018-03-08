#ifndef HMI_H
#define HMI_H

#include <stdint.h>

typedef void (*HMICallbackFunc)(uint8_t id, void *buf, uint32_t len);

void HMIInit(void);
void HMIRegisterCallback(HMICallbackFunc hmiCallback);
void HMISendCmd(const char *fmt, ...);
void HMIProcess(void);

#endif
