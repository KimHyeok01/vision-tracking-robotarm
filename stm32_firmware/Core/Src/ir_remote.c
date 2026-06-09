#include "ir_remote.h"

/**
 * @brief  RAW 펄스 버퍼의 데이터 구역을 분석하여 중복 연산 없이 키값을 반환합니다.
 */
uint8_t IR_Decode_Packet(volatile uint32_t* raw_buf, uint8_t length)
{
    // 최소 32개 비트 완결성 확인
    if (length < 32) return 0xFF;

    uint8_t key_byte = 0;

    /* * \\
     * 배열의 [16]번 부터 [23]번 까지가 실제 리모컨의 데이터 바이트 영역입니다.
     * 다른 비트는 검사할 필요도 없이, 딱 이 8개 방만 돌면서 8비트 변수에 밀어 넣습니다.
     */
    for (int i = 0; i < 8; i++)
    {
        if (raw_buf[16 + i] > IR_THRESHOLD)
        {
            key_byte |= (1 << (7 - i)); // 배열 순서대로 상위 비트부터 적재
        }
    }

    return key_byte;
}
