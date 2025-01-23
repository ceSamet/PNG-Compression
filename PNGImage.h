#ifndef PNG_IMAGE_H
#define PNG_IMAGE_H

#include <vector>
#include <string>
#include "PNGStructs.h"

/**
 * @file PNGImage.h
 * @brief Contains PNGImage class for handling PNG image operations
 * @author Samet Aydın
 * @date 2025
 */

class PNGImage {
private:
    std::vector<uint8_t> data;
    uint32_t width;
    uint32_t height;
    uint8_t channels;
    uint8_t bitDepth;
    ColorType colorType;

    bool writeChunk(std::ofstream& file, uint32_t type, const std::vector<uint8_t>& chunkData);
    bool readChunk(std::ifstream& file, PNGChunk& chunk);
    std::vector<uint8_t> createIHDR();
    bool processIHDR(const std::vector<uint8_t>& data);
    std::vector<uint8_t> compressData();
    std::vector<uint8_t> decompressData(const std::vector<uint8_t>& compressed);

public:
    /**
     * @brief Default constructor
     */
    PNGImage();

    // Getters
    const std::vector<uint8_t>& getData() const { return data; }
    uint32_t getWidth() const { return width; }
    uint32_t getHeight() const { return height; }
    uint8_t getChannels() const { return channels; }

    // Setters
    void setWidth(uint32_t w) { width = w; }
    void setHeight(uint32_t h) { height = h; }
    void setChannels(uint8_t c) { channels = c; }
    void resizeData(size_t size) { data.resize(size); }
    void setData(const std::vector<uint8_t>& newData) { data = newData; }

    /**
     * @brief Reads a PNG file
     * @param filename Path to the PNG file
     * @return true if successful, false otherwise
     */
    bool readPNG(const std::string& filename);

    /**
     * @brief Saves data as a PNG file
     * @param filename Output file path
     * @param newData Image data to save
     * @param newWidth Image width
     * @param newHeight Image height
     * @param newChannels Number of color channels
     * @return true if successful, false otherwise
     */
    bool savePNG(const std::string& filename, const std::vector<uint8_t>& newData,
                 uint32_t newWidth, uint32_t newHeight, uint8_t newChannels);

    /**
     * @brief Ensures filename has .png extension
     * @param filename Input filename
     * @return Filename with .png extension
     */
    std::string checkPNGExtension(const std::string& filename);

    friend class ImageCompressor;
};

#endif // PNG_IMAGE_H 