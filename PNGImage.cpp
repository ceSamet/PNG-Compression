#include "PNGImage.h"
#include <iostream>
#include <fstream>
#include <algorithm>

/**
 * @file PNGImage.cpp
 * @brief Implementation of PNGImage class
 * @author Samet Aydın
 * @date 2025
 */

PNGImage::PNGImage() : width(0), height(0), channels(3), bitDepth(8), 
                       colorType(ColorType::RGB) {}

bool PNGImage::readPNG(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cout << "Error: Cannot open file " << filename << std::endl;
        return false;
    }

    file.seekg(0, std::ios::end);
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    if (fileSize < 8) {
        std::cout << "Error: File is too small to be a PNG" << std::endl;
        return false;
    }

    uint8_t signature[8];
    file.read(reinterpret_cast<char*>(signature), 8);

    if (!std::equal(std::begin(signature), std::end(signature), PNGSignature::data)) {
        std::cout << "Error: Invalid PNG signature - File is not a PNG image" << std::endl;
        return false;
    }
        
    PNGChunk chunk;
    bool foundIHDR = false;
    std::vector<uint8_t> imageData;

    while (file) {
        if (!readChunk(file, chunk)) {
            break;
        }

        switch (chunk.type) {
            case static_cast<uint32_t>(ChunkType::IHDR):
                if (!processIHDR(chunk.data)) {
                    std::cout << "Error: Failed to process IHDR chunk" << std::endl;
                    return false;
                }
                foundIHDR = true;
                break;

            case static_cast<uint32_t>(ChunkType::IDAT):
                imageData.insert(imageData.end(), chunk.data.begin(), chunk.data.end());
                break;

            case static_cast<uint32_t>(ChunkType::IEND):
                if (foundIHDR && !imageData.empty()) {
                    data = imageData;
                    return true;
                }
                break;
        }
    }

    if (!foundIHDR) {
        std::cout << "Error: No IHDR chunk found" << std::endl;
        return false;
    }

    if (imageData.empty()) {
        std::cout << "Error: No image data found (no IDAT chunks)" << std::endl;
        return false;
    }

    data = imageData;
    return true;
}

bool PNGImage::savePNG(const std::string& filename, const std::vector<uint8_t>& newData,
                       uint32_t newWidth, uint32_t newHeight, uint8_t newChannels) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cout << "Error: Cannot create PNG file" << std::endl;
        return false;
    }

    file.write(reinterpret_cast<const char*>(PNGSignature::data), 8);

    width = newWidth;
    height = newHeight;
    channels = newChannels;
    colorType = channels == 1 ? ColorType::GRAYSCALE : 
                channels == 3 ? ColorType::RGB : ColorType::RGBA;

    if (!writeChunk(file, static_cast<uint32_t>(ChunkType::IHDR), createIHDR())) {
        return false;
    }

    std::vector<uint8_t> idat_data = newData;

    if (!writeChunk(file, static_cast<uint32_t>(ChunkType::IDAT), idat_data)) {
        std::cout << "Error: Failed to write IDAT chunk" << std::endl;
        return false;
    }

    if (!writeChunk(file, static_cast<uint32_t>(ChunkType::IEND), std::vector<uint8_t>())) {
        std::cout << "Error: Failed to write IEND chunk" << std::endl;
        return false;
    }

    std::cout << "PNG file saved successfully:" << std::endl;
    std::cout << "Width: " << width << std::endl;
    std::cout << "Height: " << height << std::endl;
    std::cout << "Channels: " << (int)channels << std::endl;
    std::cout << "Total pixels: " << (width * height) << std::endl;
    std::cout << "Data size: " << newData.size() << " bytes" << std::endl;

    return true;
}

std::string PNGImage::checkPNGExtension(const std::string& filename) {
    if (filename.length() >= 4 && filename.substr(filename.length() - 4) == ".png") {
        return filename;
    }
    return filename + ".png";
}

bool PNGImage::readChunk(std::ifstream& file, PNGChunk& chunk) {
    unsigned char lenBytes[4];
    file.read(reinterpret_cast<char*>(lenBytes), 4);
    if (file.eof()) return false;
    
    chunk.length = (lenBytes[0] << 24) | (lenBytes[1] << 16) | 
                  (lenBytes[2] << 8) | lenBytes[3];

    unsigned char typeBytes[4];
    file.read(reinterpret_cast<char*>(typeBytes), 4);
    chunk.type = (typeBytes[0] << 24) | (typeBytes[1] << 16) | 
                 (typeBytes[2] << 8) | typeBytes[3];
    
    chunk.data.resize(chunk.length);
    if (chunk.length > 0) {
        file.read(reinterpret_cast<char*>(chunk.data.data()), chunk.length);
    }
    
    unsigned char crcBytes[4];
    file.read(reinterpret_cast<char*>(crcBytes), 4);
    chunk.crc = (crcBytes[0] << 24) | (crcBytes[1] << 16) | 
                (crcBytes[2] << 8) | crcBytes[3];
    
    return file.good();
}

bool PNGImage::processIHDR(const std::vector<uint8_t>& data) {
    if (data.size() < 13) {
        std::cout << "Error: Invalid IHDR chunk size" << std::endl;
        return false;
    }

    width = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
    height = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7];
    bitDepth = data[8];
    uint8_t colorType = data[9];
    
    if (width == 0 || height == 0) {
        std::cout << "Error: Invalid dimensions" << std::endl;
        return false;
    }

    switch (colorType) {
        case 0:
            this->colorType = ColorType::GRAYSCALE;
            channels = 1;
            break;
        case 2:
            this->colorType = ColorType::RGB;
            channels = 3;
            break;
        case 6:
            this->colorType = ColorType::RGBA;
            channels = 4;
            break;
        default:
            this->colorType = ColorType::RGB;
            channels = 3;
            break;
    }

    return true;
}

std::vector<uint8_t> PNGImage::createIHDR() {
    std::vector<uint8_t> ihdr(13);
    
    ihdr[0] = (width >> 24) & 0xFF;
    ihdr[1] = (width >> 16) & 0xFF;
    ihdr[2] = (width >> 8) & 0xFF;
    ihdr[3] = width & 0xFF;
    
    ihdr[4] = (height >> 24) & 0xFF;
    ihdr[5] = (height >> 16) & 0xFF;
    ihdr[6] = (height >> 8) & 0xFF;
    ihdr[7] = height & 0xFF;
    
    ihdr[8] = bitDepth;
    ihdr[9] = static_cast<uint8_t>(colorType);
    ihdr[10] = 0; // Compression method
    ihdr[11] = 0; // Filter method
    ihdr[12] = 0; // Interlace method
    
    return ihdr;
}

std::vector<uint8_t> PNGImage::compressData() {
    std::vector<uint8_t> compressed;
    size_t i = 0;
    
    while (i < data.size()) {
        uint8_t currentByte = data[i];
        size_t count = 1;
        
        while (i + count < data.size() && data[i + count] == currentByte && count < 255) {
            count++;
        }
            
        if (count >= 4) {
            compressed.push_back(0xFF);
            compressed.push_back(currentByte);
            compressed.push_back(count);
            i += count;
        } else {
            size_t nonRepeatCount = 1;
            while (i + nonRepeatCount < data.size() && 
                   nonRepeatCount < 255 && 
                   (i + nonRepeatCount + 2 >= data.size() || 
                    data[i + nonRepeatCount] != data[i + nonRepeatCount + 1] || 
                    data[i + nonRepeatCount] != data[i + nonRepeatCount + 2])) {
                nonRepeatCount++;
            }
                
            compressed.push_back(nonRepeatCount - 1);
            for (size_t j = 0; j < nonRepeatCount; j++) {
                compressed.push_back(data[i + j]);
            }
            i += nonRepeatCount;
        }
    }
        
    return compressed;
}

std::vector<uint8_t> PNGImage::decompressData(const std::vector<uint8_t>& compressed) {
    std::vector<uint8_t> decompressed;
    size_t i = 0;
        
    while (i < compressed.size()) {
        uint8_t control = compressed[i++];
        
        if (control == 0xFF) {
            uint8_t value = compressed[i++];
            uint8_t count = compressed[i++];
            decompressed.insert(decompressed.end(), count, value);
        } else {
            uint8_t count = control + 1;
            decompressed.insert(decompressed.end(), 
                              compressed.begin() + i, 
                              compressed.begin() + i + count);
            i += count;
        }
    }
    
    return decompressed;
}

bool PNGImage::writeChunk(std::ofstream& file, uint32_t type, const std::vector<uint8_t>& chunkData) {
    // Write length
    uint32_t length = chunkData.size();
    unsigned char lenBytes[4] = {
        static_cast<unsigned char>((length >> 24) & 0xFF),
        static_cast<unsigned char>((length >> 16) & 0xFF),
        static_cast<unsigned char>((length >> 8) & 0xFF),
        static_cast<unsigned char>(length & 0xFF)
    };
    file.write(reinterpret_cast<char*>(lenBytes), 4);

    // Write type
    unsigned char typeBytes[4] = {
        static_cast<unsigned char>((type >> 24) & 0xFF),
        static_cast<unsigned char>((type >> 16) & 0xFF),
        static_cast<unsigned char>((type >> 8) & 0xFF),
        static_cast<unsigned char>(type & 0xFF)
    };
    file.write(reinterpret_cast<char*>(typeBytes), 4);

    // Write data
    if (!chunkData.empty()) {
        file.write(reinterpret_cast<const char*>(chunkData.data()), chunkData.size());
    }

    // Calculate and write CRC
    uint32_t crc = PNGChunk::calculateCRC(chunkData);
    unsigned char crcBytes[4] = {
        static_cast<unsigned char>((crc >> 24) & 0xFF),
        static_cast<unsigned char>((crc >> 16) & 0xFF),
        static_cast<unsigned char>((crc >> 8) & 0xFF),
        static_cast<unsigned char>(crc & 0xFF)
    };
    file.write(reinterpret_cast<char*>(crcBytes), 4);

    return file.good();
} 