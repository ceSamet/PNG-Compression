#include <iostream>
#include <fstream>
#include <limits>
#include <iomanip>
#include "PNGImage.h"
#include "ImageCompressor.h"

/**
 * @file main.cpp
 * @brief Main program file for PNG image compression application
 * @author Samet Aydın
 * @date 2025
 */

/**
 * @brief Displays the main menu options
 */
void displayMenu() {
    std::cout << "\nImage Compression Menu\n";
    std::cout << "1. Compress Image\n";
    std::cout << "2. Decompress Image\n";
    std::cout << "3. Exit\n";
    std::cout << "Enter your choice (1-3): ";
}

int main() {
    PNGImage image;
    std::string input;
    bool running = true;

    while (running) {
        displayMenu();
        std::cin >> input;

        if (input == "1") {
            std::string filename;
            std::cout << "Enter PNG filename to compress (with or without .png): ";
            std::cin >> filename;
            
            filename = image.checkPNGExtension(filename);
            
            if (!image.readPNG(filename)) {
                std::cout << "Failed to load PNG image!" << std::endl;
                continue;
            }
            
            std::string outFilename; 
            std::cout << "Enter output filename (without extension): ";
            std::cin >> outFilename;
            
            ImageCompressor compressor;
            std::cout << "\nCompressing image..." << std::endl;
            
            if (compressor.saveCompressed(image, outFilename)) {
                std::cout << "\nCompression completed successfully!" << std::endl;
                std::cout << "Image compressed and saved as " << outFilename << ".samet" << std::endl;
                std::cout << "Original size: " << image.getData().size() << " bytes" << std::endl;
                
                std::ifstream compressedFile(outFilename + ".samet", std::ios::binary | std::ios::ate);
                if (compressedFile.is_open()) {
                    size_t compressedSize = compressedFile.tellg();
                    compressedFile.close();
                    std::cout << "Compressed size: " << compressedSize << " bytes" << std::endl;
                    std::cout << "Compression ratio: " << std::fixed << std::setprecision(2) 
                             << (100.0 * compressedSize / image.getData().size()) << "%" << std::endl;
                }
                std::cout << "\nPress Enter to continue...";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cin.get();
            }
            else {
                std::cout << "Failed to compress image!" << std::endl;
                std::cout << "\nPress Enter to continue...";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cin.get();
            }
        }
        else if (input == "2") {
            std::string inFilename;
            std::cout << "Enter compressed file name (.samet): ";
            std::cin >> inFilename;
            
            if (inFilename.length() < 6 || inFilename.substr(inFilename.length() - 6) != ".samet") {
                inFilename += ".samet";
            }
            
            std::ifstream checkFile(inFilename);
            if (!checkFile) {
                std::cout << "Error: File '" << inFilename << "' not found!" << std::endl;
                continue;
            }
            checkFile.close();
            
            ImageCompressor compressor;
            if (compressor.loadCompressed(inFilename, image)) {
                std::string outFilename = inFilename.substr(0, inFilename.length() - 6) + "_decompressed.png";
                
                if (image.savePNG(outFilename, image.getData(), image.getWidth(), 
                                image.getHeight(), image.getChannels())) {
                    std::cout << "Image decompressed and saved as " << outFilename << std::endl;
                } 
                else {
                    std::cout << "Error saving decompressed image!" << std::endl;
                }
            } 
            else {
                std::cout << "Failed to decompress image." << std::endl;
            }
        }
        else if (input == "3") {
            std::cout << "Exiting...\n";
            running = false;
        }
        else {
            std::cout << "Invalid choice! Please enter a number between 1-3." << std::endl;
        }
    }

    return 0;
}