# Cross-Fading Image Processing Project

This project implements cross-fading between a color image and its grayscale version to create a video transition effect. It includes sequential, MPI (Message Passing Interface), and OpenMP parallel implementations.

## Project Structure

```
cross-fading/
├── README.md (original assignment)
├── README_PROJECT.md (this file)
├── Makefile
├── create_video.py
├── convert_images.py
├── performance_analysis.cpp
├── cross_fading_sequential.cpp
├── cross_fading_mpi.cpp
├── cross_fading_openmp.cpp
├── images/
│   ├── image1.png
│   ├── image2.png
│   └── (converted BMP files will be here)
├── sequential/
│   └── cross_fading_sequential.cpp (backup)
├── mpi/
│   └── cross_fading_mpi.cpp (backup)
└── openmp/
    └── cross_fading_openmp.cpp (backup)
```

## Prerequisites

### Python Dependencies
Install Python packages using:
```bash
pip install -r requirements.txt
```

### System Dependencies
- **C++ Compiler**: GCC 7+ or Clang 5+ with C++17 support
- **MPI Library**: OpenMPI or MPICH (for MPI version)
- **OpenMP Support**: Compiler with OpenMP support (most modern compilers)
- **Python 3**: For video creation script
- **Input Images**: PNG format (supported natively by OpenCV)

See `requirements.txt` for detailed installation instructions for different operating systems.

## Step-by-Step Execution Guide

### Step 1: Prepare Your Environment

1. Ensure you have a C++ compiler installed:
   ```bash
   g++ --version
   ```

2. Install MPI if not already installed:
   - On Ubuntu/Debian: `sudo apt install openmpi-bin openmpi-doc libopenmpi-dev`
   - On CentOS/RHEL: `sudo yum install openmpi openmpi-devel`
   - On macOS: `brew install openmpi`

3. Install Python dependencies:
   ```bash
   pip install opencv-python numpy
   ```

### Step 2: Prepare Input Images

1. Place your PNG images in the `images/` directory (already done - you have image1.png and image2.png)

2. The PNG files are ready for processing directly (OpenCV supports PNG format natively)

### Step 3: Compile the Programs

1. Navigate to the project directory:
   ```bash
   cd /path/to/cross-fading
   ```

2. Compile all versions:
   ```bash
   make all
   ```

   If you encounter OpenCV linking issues, try the alternative compilation:
   ```bash
   make sequential_alt
   make mpi_alt
   make openmp_alt
   ```

   To check your OpenCV installation:
   ```bash
   make check_opencv
   ```

    Or compile individual versions:
   ```bash
   make sequential  # Compile sequential version
   make mpi         # Compile MPI version
   make openmp      # Compile OpenMP version
   ```

3. Compile performance analysis tool:
   ```bash
   g++ -std=c++17 -O2 performance_analysis.cpp -o performance_analysis
   ```

### Step 4: Run the Cross-Fading Programs

#### Sequential Version
```bash
./cross_fading_sequential images/image1.png
```
- Uses `images/image1.png` directly (PNG format supported)
- This will generate frames named `frame_sequential_0.png`, `frame_sequential_1.png`, etc.

#### MPI Version
```bash
mpirun -np 4 ./cross_fading_mpi images/image1.png
```
- `-np 4`: Use 4 MPI processes
- Uses `images/image1.png` directly (PNG format supported)
- This will generate frames named `frame_mpi_0.png`, `frame_mpi_1.png`, etc.

#### OpenMP Version
```bash
./cross_fading_openmp images/image1.png 4
```
- Example: `./cross_fading_openmp images/image1.png 4`
- Uses `images/image1.png` directly (PNG format supported)
- This will generate frames named `frame_openmp_4_0.png`, `frame_openmp_4_1.png`, etc.

**Note**: You can use `image2.bmp` instead of `image1.bmp` for the second image

### Step 5: Create the Video

After generating frames, create an MP4 video:

```bash
python create_video.py frame1 output_video.mp4 --fps 24
```

This creates `output_video.mp4` from the `frame1_*.bmp` files.

**Example workflow:**
1. Run sequential version: `./cross_fading_sequential images/image1.png`
2. Create video: `python create_video.py frame_sequential image1_sequential.mp4 --fps 24`
3. Run MPI version: `mpirun -np 4 ./cross_fading_mpi images/image1.png`
4. Create video: `python create_video.py frame_mpi image1_mpi.mp4 --fps 24`
5. Run OpenMP version: `./cross_fading_openmp images/image1.png 4`
6. Create video: `python create_video.py frame_openmp_4 image1_openmp.mp4 --fps 24`

### Step 6: Performance Analysis

1. Create multiple sizes of your images (800x800, 2000x2000, 5000x5000) and place them in the images/ directory with names like:
   - `image1_800x800.bmp`
   - `image1_2000x2000.bmp`
   - `image1_5000x5000.bmp`

2. Run performance analysis:
   ```bash
   ./performance_analysis images/image1
   ```
   (This expects files like `images/image1_800x800.bmp`, etc.)

3. Compare execution times between versions

4. Calculate metrics:
   - **Speedup** = Sequential_Time / Parallel_Time
   - **Efficiency** = Speedup / Number_of_Processors

### Step 7: Clean Up

Remove generated files:
```bash
make clean  # Removes executables
rm frame_*.bmp output_video.mp4  # Remove frames and video
```

## Requirements

- C++17 compatible compiler
- For MPI version: MPI library (e.g., OpenMPI or MPICH)
- For OpenMP version: OpenMP support in compiler
- Input images in PNG format (supported natively by OpenCV)

## Compilation

Use the provided Makefile to build all versions:

```bash
make all
```

Or build individual versions:

```bash
make sequential
make mpi
make openmp
```

## Usage

### Sequential Version

```bash
./sequential/cross_fading_sequential <input_image.bmp> <output_prefix> <size>
```

### MPI Version

```bash
mpirun -np <num_processes> ./mpi/cross_fading_mpi <input_image.bmp> <output_prefix> <size>
```

### OpenMP Version

```bash
./openmp/cross_fading_openmp <input_image.bmp> <output_prefix> <size> <num_threads>
```

## Parameters

- `input_image.bmp`: Input BMP image file
- `output_prefix`: Prefix for output frame files (e.g., "frame" will create frame_0.bmp, frame_1.bmp, etc.)
- `size`: Image size (width and height, assumes square images)
- `num_processes`: Number of MPI processes (MPI version only)
- `num_threads`: Number of OpenMP threads (OpenMP version only)

## Output

Each program generates 96 BMP frames (4 seconds at 24 fps) showing the cross-fading transition from color to grayscale.

## Performance Analysis

The project includes timing measurements for each implementation. To analyze performance:

1. Run each version with different image sizes (800x800, 2000x2000, 5000x5000)
2. Measure execution times
3. Calculate speedup and efficiency metrics

### Speedup Formula

Speedup = Sequential_Time / Parallel_Time

### Efficiency Formula

Efficiency = Speedup / Number_of_Processors

## Creating the Video

After generating the frames, you can create a video using the provided Python script:

```bash
python create_video.py <frame_prefix> <output_video.mp4> [--fps 24]
```

For example:
```bash
python create_video.py frame output_video.mp4
```

This will create an MP4 video from frames named frame_0.bmp, frame_1.bmp, etc.

**Requirements for video creation:**
- Python 3
- OpenCV: `pip install opencv-python`
- NumPy: `pip install numpy`

Alternatively, use a video editor like CapCut, Inshot, or Canva to combine the BMP frames into a video file.

## Notes

- The BMP loading/saving functions are simplified and assume 24-bit RGB format
- Images are processed in RGB color space
- Grayscale conversion uses standard luminance formula: 0.299*R + 0.587*G + 0.114*B
- Cross-fading is performed per pixel across all color channels

## Troubleshooting

- Ensure input BMP files are uncompressed 24-bit RGB
- For MPI: Make sure MPI is properly installed and configured
- For OpenMP: Ensure compiler supports OpenMP (most modern compilers do)
- Large images may require significant memory and processing time