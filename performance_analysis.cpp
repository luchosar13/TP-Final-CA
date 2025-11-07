#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <opencv2/opencv.hpp>

using namespace cv;

void convertToGrayscale(const Mat& colorImg, Mat& grayImg) {
    for (int i = 0; i < colorImg.rows; ++i) {
        for (int j = 0; j < colorImg.cols; ++j) {
            Vec3b pixel = colorImg.at<Vec3b>(i, j);
            grayImg.at<uchar>(i, j) = 0.299 * pixel[2] + 0.587 * pixel[1] + 0.114 * pixel[0];
        }
    }
}

void crossFade(const Mat& colorImg, const Mat& gray3, Mat& result, double p) {
    result = Mat(colorImg.rows, colorImg.cols, CV_8UC3);
    for (int i = 0; i < colorImg.rows; ++i) {
        for (int j = 0; j < colorImg.cols; ++j) {
            Vec3b colorPixel = colorImg.at<Vec3b>(i, j);
            Vec3b grayPixel = gray3.at<Vec3b>(i, j);
            result.at<Vec3b>(i, j) = Vec3b(
                saturate_cast<uchar>(colorPixel[0] * p + grayPixel[0] * (1 - p)),
                saturate_cast<uchar>(colorPixel[1] * p + grayPixel[1] * (1 - p)),
                saturate_cast<uchar>(colorPixel[2] * p + grayPixel[2] * (1 - p))
            );
        }
    }
}


double measurePerformance(int size, const std::string& inputFile) {

    Mat original = imread(inputFile, IMREAD_COLOR);
    if (original.empty()) {
        std::cerr << "Error al abrir el archivo: " << inputFile << std::endl;
        return -1.0;
    }

    if (original.cols != size || original.rows != size) {
        cv::resize(original, original, cv::Size(size, size));
    }

    Mat grayImg(original.rows, original.cols, CV_8UC1);
    Mat gray3;

    auto start = std::chrono::high_resolution_clock::now();

    convertToGrayscale(original, grayImg);

    cvtColor(grayImg, gray3, COLOR_GRAY2BGR);


    int totalFrames = 96;
    std::vector<Mat> results;
    results.reserve(totalFrames);

    for (int frame = 0; frame < totalFrames; ++frame) {
        double p = 1.0 - (double)frame / (totalFrames - 1);
        Mat faded;
        crossFade(original, gray3, faded, p);
        results.push_back(faded);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    return elapsed.count();
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Uso: " << argv[0] << " <tamaño_imagen> <ruta_imagen>" << std::endl;
        std::cerr << "Para calcular speedup y eficiencia, proveer también:" << std::endl;
        std::cerr << "Uso: " << argv[0] << " <tamaño_imagen> <ruta_imagen> <tiempo_paralelo_segundos> <num_procesadores>" << std::endl;
        return 1;
    }

    int size = std::stoi(argv[1]);
    std::string inputFile = argv[2];

    std::cout << std::fixed << std::setprecision(4);

    std::cout << "Calculando tiempo secuencial para tamaño " << size << "x" << size << "..." << std::endl;
    double sequentialTime = measurePerformance(size, inputFile);

    if (sequentialTime < 0) {
        return 1;
    }

    std::cout << "Tiempo Secuencial: " << sequentialTime << " segundos" << std::endl;

    if (argc == 5) {
        double parallelTime = std::stod(argv[3]);
        int numProcessors = std::stoi(argv[4]);

        if (parallelTime > 0 && numProcessors > 0) {
            double speedup = sequentialTime / parallelTime;
            double efficiency = speedup / numProcessors;

            std::cout << "----------------------------------" << std::endl;
            std::cout << "Métricas de Rendimiento:" << std::endl;
            std::cout << "Tiempo Paralelo:   " << parallelTime << " segundos" << std::endl;
            std::cout << "Num Procesadores:  " << numProcessors << std::endl;
            std::cout << "Speedup:         " << speedup << std::endl;
            std::cout << "Eficiencia:      " << efficiency << std::endl;
        }
    } else {
        std::cout << "\nPara calcular Speedup y Eficiencia, ejecute de nuevo con el tiempo paralelo y el número de procesadores." << std::endl;
    }

    return 0;
}