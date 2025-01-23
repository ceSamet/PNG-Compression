#include "ImageCompressor.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

/**
 * @file ImageCompressor.cpp
 * @brief Implementation of ImageCompressor class
 * @author Samet Aydın
 * @date 2025
 */

bool ImageCompressor::saveCompressed(const PNGImage& image, const std::string& filename) {
    if (image.getWidth() == 0 || image.getHeight() == 0) {
        std::cout << "Error: No image data to compress" << std::endl;
        return false;
    }

    std::cout << "Starting compression..." << std::endl;
    std::cout << "Image size: " << image.getWidth() << "x" << image.getHeight() 
              << " with " << (int)image.getChannels() << " channels" << std::endl;

    std::ofstream file(filename + ".samet", std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Error: Cannot create compressed file" << std::endl;
        return false;
    }

    file << image.getWidth() << " " 
         << image.getHeight() << " " 
         << (int)image.getChannels() << " "
         << image.getData().size() << "\n";

    const std::vector<uint8_t>& pngData = image.getData();
    file.write(reinterpret_cast<const char*>(pngData.data()), pngData.size());

    file.close();

    std::cout << "Compression completed!" << std::endl;
    std::cout << "Original PNG data size: " << pngData.size() << " bytes" << std::endl;

    return true;
}

bool ImageCompressor::loadCompressed(const std::string& filename, PNGImage& image) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Error: Cannot open file" << std::endl;
        return false;
    }

    std::string header;
    std::getline(file, header);
    
    std::istringstream iss(header);
    int width, height, channels;
    size_t dataSize;
    
    if (!(iss >> width >> height >> channels >> dataSize)) {
        std::cout << "Error: Failed to parse header values" << std::endl;
        return false;
    }

    std::cout << "Image info from header:" << std::endl;
    std::cout << "Width: " << width << std::endl;
    std::cout << "Height: " << height << std::endl;
    std::cout << "Channels: " << channels << std::endl;
    std::cout << "Data size: " << dataSize << " bytes" << std::endl;

    image.setWidth(width);
    image.setHeight(height);
    image.setChannels(channels);

    std::vector<uint8_t> pngData(dataSize);
    file.read(reinterpret_cast<char*>(pngData.data()), dataSize);

    if (file.gcount() != dataSize) {
        std::cout << "Error: Could not read all PNG data" << std::endl;
        std::cout << "Expected: " << dataSize << " bytes" << std::endl;
        std::cout << "Read: " << file.gcount() << " bytes" << std::endl;
        return false;
    }

    image.setData(pngData);
    
    std::cout << "Decompression successful!" << std::endl;
    std::cout << "Image dimensions: " << width << "x" << height << " with " 
              << channels << " channels" << std::endl;
    std::cout << "PNG data size: " << pngData.size() << " bytes" << std::endl;

    return true;
}

std::string ImageCompressor::compressData(const std::vector<unsigned char>& data) {
    std::string compressed;
    size_t i = 0;
    size_t totalSize = data.size();
    size_t lastProgress = 0;
    
    compressed.reserve(data.size());
    
    while (i < data.size()) {
        size_t progress = (i * 100) / totalSize;
        if (progress > lastProgress) {
            std::cout << "\rCompressing: " << progress << "%" << std::flush;
            lastProgress = progress;
        }

        const size_t BLOCK_SIZE = 1024;
        size_t remainingBytes = data.size() - i;
        size_t blockSize = std::min(BLOCK_SIZE, remainingBytes);
        
        size_t j = 0;
        while (j < blockSize) {
            unsigned char currentByte = data[i + j];
            size_t count = 1;
            
            while (j + count < blockSize && 
                   count < 255 && 
                   data[i + j + count] == currentByte) {
                count++;
            }
            
            if (count >= 4) {
                compressed.push_back('\xFF');
                compressed.push_back(currentByte);
                compressed.push_back(static_cast<char>(count));
                j += count;
            } else {
                size_t literalCount = 1;
                while (j + literalCount < blockSize && 
                       literalCount < 255 && 
                       (literalCount < 3 || 
                        data[i + j + literalCount] != data[i + j + literalCount - 1] ||
                        data[i + j + literalCount] != data[i + j + literalCount - 2])) {
                    literalCount++;
                }
                
                compressed.push_back(static_cast<char>(literalCount - 1));
                for (size_t k = 0; k < literalCount; k++) {
                    compressed.push_back(static_cast<char>(data[i + j + k]));
                }
                j += literalCount;
            }
        }
        i += blockSize;
    }
    
    std::cout << "\rCompressing: 100%" << std::endl;
    return compressed;
}

std::vector<unsigned char> ImageCompressor::decompressData(const std::string& compressed) {
    std::vector<unsigned char> result;
    size_t i = 0;
    
    std::cout << "Starting decompression..." << std::endl;
    size_t totalSize = compressed.length();
    size_t lastProgress = 0;
    
    result.reserve(totalSize * 2);
    
    while (i < compressed.length()) {
        size_t progress = (i * 100) / totalSize;
        if (progress > lastProgress) {
            std::cout << "\rDecompressing: " << progress << "%" << std::flush;
            lastProgress = progress;
        }
        
        unsigned char control = static_cast<unsigned char>(compressed[i++]);
        
        if (control == 0xFF) {
            if (i + 1 >= compressed.length()) break;
            
            unsigned char value = static_cast<unsigned char>(compressed[i++]);
            unsigned char count = static_cast<unsigned char>(compressed[i++]);
            
            result.insert(result.end(), count, value);
        } else {
            unsigned char literalCount = control + 1;
            if (i + literalCount > compressed.length()) break;
            
            result.insert(result.end(), 
                         compressed.begin() + i, 
                         compressed.begin() + i + literalCount);
            i += literalCount;
        }
    }
    
    std::cout << "\rDecompressing: 100%" << std::endl;
    std::cout << "Decompressed size: " << result.size() << " bytes" << std::endl;
    
    return result;
} 