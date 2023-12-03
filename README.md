# QSORT_CILK
## Результаты
par qsort: 14892 (ms)


seq qsort: 45657 (ms)


acceleration: 3.06587x

## Build and Run
clang++ -fopencilk -O3 qsort.cpp -o qsort

CILK_NWORKERS=4 ./qsort
