# Makefile for Cross-Fading Project

CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall

# Sequential version with OpenCV
sequential: cross_fading_sequential.cpp
	$(CXX) $(CXXFLAGS) `pkg-config --cflags opencv4` -o cross_fading_sequential cross_fading_sequential.cpp `pkg-config --libs opencv4` -lopencv_core -lopencv_imgcodecs -lopencv_imgproc

# MPI version (requires MPI library and OpenCV)
mpi: cross_fading_mpi.cpp
	mpicxx $(CXXFLAGS) `pkg-config --cflags opencv4` -o cross_fading_mpi cross_fading_mpi.cpp `pkg-config --libs opencv4` -lopencv_core -lopencv_imgcodecs -lopencv_imgproc

# OpenMP version with OpenCV
openmp: cross_fading_openmp.cpp
	$(CXX) $(CXXFLAGS) -fopenmp `pkg-config --cflags opencv4` -o cross_fading_openmp cross_fading_openmp.cpp `pkg-config --libs opencv4` -lopencv_core -lopencv_imgcodecs -lopencv_imgproc

# Alternative compilation if pkg-config doesn't work (try different library paths)
sequential_alt: cross_fading_sequential.cpp
	$(CXX) $(CXXFLAGS) -I/usr/include/opencv4 -L/usr/lib/x86_64-linux-gnu -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -o cross_fading_sequential cross_fading_sequential.cpp

mpi_alt: cross_fading_mpi.cpp
	mpicxx $(CXXFLAGS) -I/usr/include/opencv4 -L/usr/lib/x86_64-linux-gnu -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -o cross_fading_mpi cross_fading_mpi.cpp

openmp_alt: cross_fading_openmp.cpp
	$(CXX) $(CXXFLAGS) -fopenmp -I/usr/include/opencv4 -L/usr/lib/x86_64-linux-gnu -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -o cross_fading_openmp cross_fading_openmp.cpp

# Check OpenCV installation
check_opencv:
	@echo "Checking pkg-config for OpenCV4:"
	@pkg-config --exists opencv4 && echo "OpenCV4 found via pkg-config" || echo "OpenCV4 NOT found via pkg-config"
	@echo "Checking for OpenCV libraries:"
	@find /usr/lib* -name "*opencv*" 2>/dev/null | head -10 || echo "No OpenCV libraries found in /usr/lib*"
	@echo "Checking for OpenCV headers:"
	@find /usr/include -name "*opencv*" 2>/dev/null | head -5 || echo "No OpenCV headers found in /usr/include"

# Performance analysis tool
performance: performance_analysis.cpp
	$(CXX) $(CXXFLAGS) -o performance_analysis performance_analysis.cpp

# Build all versions
all: sequential mpi openmp performance

# Clean build artifacts
clean:
	rm -f cross_fading_sequential
	rm -f cross_fading_mpi
	rm -f cross_fading_openmp
	rm -f performance_analysis
	rm -f *.bmp *.png

.PHONY: all clean sequential mpi openmp performance