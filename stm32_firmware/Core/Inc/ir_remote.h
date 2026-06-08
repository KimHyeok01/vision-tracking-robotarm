#ifndef INC_IR_REMOTE_H_
#define INC_IR_REMOTE_H_

#include "main.h"
#include <stdint.h>

#define IR_THRESHOLD  1700

#define IR_KEY_POWER  0x28
#define IR_KEY_UP     0xC0
#define IR_KEY_DOWN   0x40
#define IR_KEY_RIGHT  0x70
#define IR_KEY_LEFT   0x78

uint8_t IR_Decode_Packet(volatile uint32_t* raw_buf, uint8_t length);

#endif /* INC_IR_REMOTE_H_ */
