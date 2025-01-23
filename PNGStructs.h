#ifndef PNG_STRUCTS_H
#define PNG_STRUCTS_H

#include <cstdint>
#include <vector>

/**
 * @file PNGStructs.h
 * @brief Contains basic PNG-related structures and enums
 * @author Samet Aydın
 * @date 2025
 */

// PNG file signature structure
struct PNGSignature {
    static const uint8_t data[8];
};

// Chunk types in PNG file
enum class ChunkType {
    IHDR = 0x49484452,
    IDAT = 0x49444154,
    IEND = 0x49454E44
};

// Color types in PNG file
enum class ColorType {
    GRAYSCALE = 0,
    RGB = 2,
    RGBA = 6
};

// Structure representing a PNG chunk
struct PNGChunk {
    uint32_t length;
    uint32_t type;
    std::vector<uint8_t> data;
    uint32_t crc;

    /**
     * @brief Calculates CRC for chunk data
     * @param data Vector of bytes to calculate CRC for
     * @return Calculated CRC value
     */
    static uint32_t calculateCRC(const std::vector<uint8_t>& data);
};

#endif // PNG_STRUCTS_H 