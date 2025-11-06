#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include <cmath>
#include <iomanip>

// Simple BMP image structure (assuming 24-bit RGB)
struct BMPImage {
    int width;
    int height;
    std::vector<std::vector<std::vector<unsigned char>>> pixels; // [y][x][rgb]

    BMPImage(int w, int h) : width(w), height(h), pixels(h, std::vector<std::vector<unsigned char>>(w, std::vector<unsigned char>(3, 0))) {}
};

// Function to load BMP image (simplified, assumes 24-bit BMP without headers)
BMPImage loadBMP(const std::string& filename, int width, int height) {
    BMPImage img(width, height);
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return img;
    }

    // Skip BMP header (54 bytes for standard BMP)
    file.seekg(54, std::ios::beg);

    // Read pixel data (bottom-up)
    for (int y = height - 1; y >= 0; --y) {
        for (int x = 0; x < width; ++x) {
            file.read(reinterpret_cast<char*>(&img.pixels[y][x][2]), 1); // B
            file.read(reinterpret_cast<char*>(&img.pixels[y][x][1]), 1); // G
            file.read(reinterpret_cast<char*>(&img.pixels[y][x][0]), 1); // R
        }
        // Skip padding if any (assuming 4-byte alignment)
        int padding = (4 - (width * 3) % 4) % 4;
        file.seekg(padding, std::ios::cur);
    }

    return img;
}

// Convert RGB to grayscale
BMPImage rgbToGrayscale(const BMPImage& img) {
    BMPImage gray(img.width, img.height);
    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {
            unsigned char r = img.pixels[y][x][0];
            unsigned char g = img.pixels[y][x][1];
            unsigned char b = img.pixels[y][x][2];
            unsigned char grayValue = static_cast<unsigned char>(0.299 * r + 0.587 * g + 0.114 * b);
            gray.pixels[y][x][0] = gray.pixels[y][x][1] = gray.pixels[y][x][2] = grayValue;
        }
    }
    return gray;
}

// Cross-fading function
BMPImage crossFade(const BMPImage& img1, const BMPImage& img2, double p) {
    BMPImage result(img1.width, img1.height);
    for (int y = 0; y < img1.height; ++y) {
        for (int x = 0; x < img1.width; ++x) {
            for (int c = 0; c < 3; ++c) {
                double val = img1.pixels[y][x][c] * p + img2.pixels[y][x][c] * (1.0 - p);
                result.pixels[y][x][c] = static_cast<unsigned char>(std::round(val));
            }
        }
    }
    return result;
}

// Performance measurement function
double measurePerformance(int size, const std::string& inputFile) {
    BMPImage original = loadBMP(inputFile, size, size);
    if (original.width != size || original.height != size) {
        std::cerr << "Error: Image size does not match specified size" << std::endl;
        return -1.0;
    }

    BMPImage grayscale = rgbToGrayscale(original);

    int totalFrames = 96;
    auto start = std::chrono::high_resolution_clock::now();

    for (int frame = 0; frame < totalFrames; ++frame) {
        double p = 1.0 - (double)frame / (totalFrames - 1);
        BMPImage faded = crossFade(original, grayscale, p);
        // Note: Not saving images in performance test to focus on computation time
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    return elapsed.count();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_image.bmp>" << std::endl;
        std::cerr << "Note: The input image should be available in different sizes (800x800, 2000x2000, 5000x5000)" << std::endl;
        return 1;
    }

    std::string baseInputFile = argv[1];

    std::vector<int> sizes = {800, 2000, 5000};
    std::vector<double> sequentialTimes;

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Performance Analysis for Cross-Fading Algorithm" << std::endl;
    std::cout << "=================================================" << std::endl << std::endl;

    // Measure sequential performance for different sizes
    std::cout << "Sequential Implementation:" << std::endl;
    for (int size : sizes) {
        std::string inputFile = baseInputFile + "_" + std::to_string(size) + "x" + std::to_string(size) + ".bmp";
        double time = measurePerformance(size, inputFile);
        if (time > 0) {
            sequentialTimes.push_back(time);
            std::cout << "Size " << size << "x" << size << ": " << time << " seconds" << std::endl;
        } else {
            std::cout << "Size " << size << "x" << size << ": Error loading image" << std::endl;
            sequentialTimes.push_back(0.0);
        }
    }

    std::cout << std::endl;
    std::cout << "Expected Parallel Performance:" << std::endl;
    std::cout << "(Run MPI and OpenMP versions separately and compare)" << std::endl;
    std::cout << std::endl;

    std::cout << "Speedup = Sequential_Time / Parallel_Time" << std::endl;
    std::cout << "Efficiency = Speedup / Number_of_Processors" << std::endl;
    std::cout << std::endl;

    std::cout << "For MPI version, test with different numbers of processes." << std::endl;
    std::cout << "For OpenMP version, test with different numbers of threads." << std::endl;
    std::cout << std::endl;

    std::cout << "Analysis Tips:" << std::endl;
    std::cout << "- Measure time for grayscale conversion separately" << std::endl;
    std::cout << "- Measure time for cross-fading separately" << std::endl;
    std::cout << "- Consider communication overhead in MPI" << std::endl;
    std::cout << "- Consider thread overhead in OpenMP" << std::endl;
    std::cout << "- Test on different hardware configurations" << std::endl;

    return 0;
}