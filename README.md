# Proyecto: Cross-Fading de Imágenes en Paralelo

Este proyecto implementa y analiza el rendimiento de un algoritmo de cross-fading de imágenes utilizando tres enfoques: secuencial, paralelo con OpenMP y paralelo con MPI.

## Requisitos

Para compilar y ejecutar este proyecto, necesitarás:
*   Un compilador de C++ (g++)
*   Una implementación de MPI (por ejemplo, OpenMPI)
*   Soporte para OpenMP (generalmente incluido en g++)
*   La biblioteca de desarrollo de OpenCV 4 (`libopencv-dev`)

## 1. Compilación

El proyecto utiliza un `Makefile` que simplifica la compilación de todos los ejecutables. Para compilar todo, simplemente ejecuta:

```bash
make all
```

Esto generará cuatro ejecutables:
*   `cross_fading_sequential`
*   `cross_fading_openmp`
*   `cross_fading_mpi`
*   `performance_analysis`

Si necesitas limpiar los archivos compilados, puedes usar:
```bash
make clean
```

## 2. Ejecución para Generar Frames

Puedes ejecutar cada algoritmo individualmente para generar los 96 frames del video de cross-fading.

### a. Versión Secuencial
```bash
./cross_fading_sequential <ruta_a_tu_imagen>
```
Ejemplo:
```bash
./cross_fading_sequential images/image1.png
```

### b. Versión OpenMP
Necesitas especificar el número de hilos a utilizar.
```bash
./cross_fading_openmp <ruta_a_tu_imagen> <num_hilos>
```
Ejemplo con 8 hilos:
```bash
./cross_fading_openmp images/image1.png 8
```

### c. Versión MPI
Necesitas usar `mpirun` para especificar el número de procesos.
```bash
mpirun -np <num_procesos> --oversubscribe ./cross_fading_mpi <ruta_a_tu_imagen>
```
Ejemplo con 8 procesos:
```bash
mpirun -np 8 --oversubscribe ./cross_fading_mpi images/image1.png
```
*Nota: El flag `--oversubscribe` se usa para asegurar que MPI pueda ejecutar el número de procesos solicitado, incluso si la configuración del sistema reporta menos "slots" disponibles.*

## 3. Análisis de Rendimiento

Para facilitar el análisis de rendimiento, se proporciona la herramienta `performance_analysis`. Esta herramienta calcula el tiempo de ejecución secuencial y, si se le proporcionan los datos de una ejecución paralela, calcula el Speedup y la Eficiencia.

### Paso a Paso del Análisis

1.  **Obtener el tiempo de la versión paralela:** Ejecuta la versión OpenMP o MPI como se describió en el paso anterior y anota el "Tiempo de cómputo" que se imprime en la consola.

    Por ejemplo, al ejecutar la versión OpenMP para una imagen de 2000x2000:
    ```bash
    ./cross_fading_openmp images/image2.png 8
    ```
    Salida de ejemplo: `Tiempo de computo de OpenMP (8 hilos): 1338 ms` -> **1.338 segundos**

2.  **Ejecutar la herramienta de análisis:** Ahora, ejecuta `performance_analysis` con el tamaño de la imagen, la ruta, el tiempo que anotaste y el número de procesadores/hilos.

    ```bash
    ./performance_analysis <tamaño> <ruta_imagen> <tiempo_paralelo_en_segundos> <num_procesadores>
    ```
    Usando el ejemplo anterior:
    ```bash
    ./performance_analysis 2000 images/image2.png 1.338 8
    ```

3.  **Interpretar los resultados:** La herramienta imprimirá el tiempo secuencial de referencia y las métricas de rendimiento calculadas.

    ```
    Calculando tiempo secuencial para tamaño 2000x2000...
    Tiempo Secuencial: 6.9262 segundos
    ----------------------------------
    Métricas de Rendimiento:
    Tiempo Paralelo:   1.3380 segundos
    Num Procesadores:  8
    Speedup:         5.1765
    Eficiencia:      0.6471
    ```
