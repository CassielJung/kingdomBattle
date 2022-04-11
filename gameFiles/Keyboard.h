#include <stdio.h>

#ifndef _KBDH_
#define _KBDH_

/* Prototypes for functions used to access physical memory addresses */
unsigned int getKeycode(volatile int * KEYCODE_ptr, volatile int * KEYCODE_RST_ptr);

unsigned int getKeycode(volatile int * KEYCODE_ptr, volatile int * KEYCODE_RST_ptr)
{
   unsigned int keycode = *KEYCODE_ptr;
   if (keycode & 0x01000000) {
      printf("keycode:%x \n", keycode - 0x01000000);
      *KEYCODE_RST_ptr = 0x00000001;
      *KEYCODE_RST_ptr = 0x00000000;
      return keycode - 0x01000000;
   }
}

#endif