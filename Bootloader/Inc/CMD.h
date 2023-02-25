#include "main.h"

typedef  void (*pFunction)(void);

#ifndef   __ASM
  #define __ASM                                  __asm
#endif

void CMD_Write(void);
void CMD_DeCommand(void);
void CMD_Jump(void);