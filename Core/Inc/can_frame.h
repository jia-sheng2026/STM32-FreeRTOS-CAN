#ifndef CAN_FRAME_H
#define CAN_FRAME_H

#include <stdint.h>   // 确保 uint32_t, uint8_t 被定义

typedef struct {
    uint32_t id;
    uint8_t data[8];
    // 如果你还有 dlc 字段，也一起搬过来
} CAN_Frame_t;

#endif /* CAN_FRAME_H */