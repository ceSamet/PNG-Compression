#ifndef IMAGE_COMPRESSOR_H
#define IMAGE_COMPRESSOR_H

#include <string>
#include <vector>
#include "PNGImage.h"

/**
 * @file ImageCompressor.h
 * @brief Contains ImageCompressor class for image compression operations
 * @author Samet Aydın
 * @date 2025
 */

class ImageCompressor {
private:
    /**
     * @brief Compresses raw image data
     * @param data Raw image data to compress
     * @return Compressed data as string
     */
    std::string compressData(const std::vector<unsigned char>& data);

    /**
     * @brief Decompresses compressed image data
     * @param compressed Compressed image data
     * @return Decompressed data as vector
     */
    std::vector<unsigned char> decompressData(const std::string& compressed);

public:
    /**
     * @brief Saves image in compressed format
     * @param image PNGImage object to compress
     * @param filename Output filename (without extension)
     * @return true if successful, false otherwise
     */
    bool saveCompressed(const PNGImage& image, const std::string& filename);

    /**
     * @brief Loads compressed image
     * @param filename Input filename
     * @param image PNGImage object to store decompressed data
     * @return true if successful, false otherwise
     */
    bool loadCompressed(const std::string& filename, PNGImage& image);
};

#endif // IMAGE_COMPRESSOR_H 