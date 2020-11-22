#ifndef __PAD_H
#define __PAD_H

#include "libpad.h"

void pad_init(void);
void buttonStatts(int port, int slot);
void checkPadConnected(void);

char padBuf[256] __attribute__((aligned(64)));
u32 old_pad;
u32 new_pad;
int port, slot;

#endif
