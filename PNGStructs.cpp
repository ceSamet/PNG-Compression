#include "PNGStructs.h"

/**
 * @file PNGStructs.cpp
 * @brief Implementation of PNG structures
 * @author Samet Aydın
 * @date 2025
 */

// Initialize PNG signature data
const uint8_t PNGSignature::data[8] = {137, 80, 78, 71, 13, 10, 26, 10};

uint32_t PNGChunk::calculateCRC(const std::vector<uint8_t>& data) {
    static const uint32_t crcTable[256] = {
        0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
    };

    uint32_t crc = 0xFFFFFFFF;
    for (uint8_t byte : data) {
        crc = crcTable[(crc ^ byte) & 0xFF] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFF;
} 