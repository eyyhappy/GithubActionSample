/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * mbedTLS SHA performance test
 */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "unity.h"
#include "sdkconfig.h"
#include "esp_heap_caps.h"
#include "test_utils.h"
#include "ccomp_timer.h"
#include "test_mbedtls_utils.h"
#include "esp_rom_crc.h"

TEST_CASE("mbedtls CRC32 test", "[crc]")
{
    // Input data buffer
    unsigned char buf[60] =
    {
        0xe9, 0x48, 0x6f, 0x6e, 0x65, 0x79, 0x77, 0x65, 0x6c, 0x6c, 0x47, 0x57, 0x50, 0x40, 0x00, 0x00,
        0x00, 0x44, 0x00, 0x6b, 0x21, 0x3c, 0x8a, 0xef, 0x48, 0xe9, 0xf0, 0xc4, 0x88, 0x6c, 0xe3, 0xac,
        0x96, 0x14, 0x29, 0xef, 0x13, 0x18, 0xf6, 0x7b, 0x88, 0x77, 0x2c, 0x11, 0x31, 0x73, 0x57, 0x67,
        0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    uint32_t crc = 0;
    size_t len = sizeof(buf);
    // Expected CRC32 value
    uint32_t expected_crc = 0x5A1B1C13;
    // Calculate CRC32 checksum
    uint32_t actual_crc =  esp_rom_crc32_le(crc, buf, len);
    printf("le result value is :%02x\r\n", actual_crc);
    // actual_crc=actual_crc^0xffffffff;
    // Check if actual CRC32 matches expected value
    TEST_ASSERT_EQUAL_HEX32(expected_crc, actual_crc);
}