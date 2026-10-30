/* author: M. Amine Belkoura
   date:   03/04/2026
   purpose: CS3377
   description:
   Matrix multiplication benchmark using POSIX pthreads and pure C-style
   library calls only — no C++ STL headers. */

#include <stdio.h>      /* printf                          */
#include <stdlib.h>     /* srand, rand, malloc, free       */
#include <time.h>       /* clock_gettime, struct timespec  */
#include <unistd.h>     /* sysconf                         */
#include <pthread.h>    /* pthread_t, pthread_create/join  */

#define N 1000

/* Flat NxN matrices in static storage (row-major). */
static double A[N][N];
static double B[N][N];
static double C_single[N][N];
static double C_multi[N][N];

/* Arguments passed to each worker thread. */
typedef struct {
    double (*C)[N];
    int startRow;
    int endRow;
} WorkerArgs;

/* -----------------------------------------------------------------------
   Returns elapsed milliseconds between two timespec values.
   ----------------------------------------------------------------------- */
static double elapsedMs(struct timespec start, struct timespec end) {
    return (end.tv_sec  - start.tv_sec)  * 1000.0 +
           (end.tv_nsec - start.tv_nsec) / 1000000.0;
}

/* -----------------------------------------------------------------------
   Fill a flat NxN matrix with random doubles in [1, 10].
   ----------------------------------------------------------------------- */
static void initializeMatrix(double mat[][N]) {
    int i, j;
    for (i = 0; i < N; ++i)
        for (j = 0; j < N; ++j)
            mat[i][j] = 1.0 + (rand() / (double)RAND_MAX) * 9.0;
}

/* -----------------------------------------------------------------------
   Compute rows [startRow, endRow) of C = A * B.
   ----------------------------------------------------------------------- */
static void multiplyRows(double C[][N], int startRow, int endRow) {
    int i, j, k;
    for (i = startRow; i < endRow; ++i) {
        for (j = 0; j < N; ++j) {
            double sum = 0.0;
            for (k = 0; k < N; ++k)
                sum += A[i][k] * B[k][j];
            C[i][j] = sum;
        }
    }
}

/* -----------------------------------------------------------------------
   pthread worker — unpacks WorkerArgs and calls multiplyRows.
   ----------------------------------------------------------------------- */
static void* workerThread(void* arg) {
    WorkerArgs* wa = (WorkerArgs*)arg;
    multiplyRows(wa->C, wa->startRow, wa->endRow);
    return NULL;
}

/* -----------------------------------------------------------------------
   Single-threaded: run the whole multiplication on the calling thread.
   ----------------------------------------------------------------------- */
static void multiplySingleThreaded(void) {
    multiplyRows(C_single, 0, N);
}

/* -----------------------------------------------------------------------
   Multi-threaded: spawn numThreads pthreads, each handling a row slice.
   ----------------------------------------------------------------------- */
static void multiplyMultiThreaded(unsigned int numThreads) {
    pthread_t*  threads = (pthread_t*)  malloc(numThreads * sizeof(pthread_t));
    WorkerArgs* args    = (WorkerArgs*) malloc(numThreads * sizeof(WorkerArgs));
    int rowsPerThread   = N / (int)numThreads;
    // TO DO
    // Complete this method so that you can:
    // 1- Create "numThreads" threads
    // 2- Build the arguments list for each thread before setting "workerThread" as a starting routine.
    // Each thread will do a portion of the matrix calculation. 
    // Depending on the number of threads assigned, each thread will get ~ (100/number of threads) rows to calculate
    // See https://www.geeksforgeeks.org/maths/multiplying-matrices/ for more details
    // A, B and the resulting C= A x B matrices are all of size 100x100
}

/* -----------------------------------------------------------------------
   main
   ----------------------------------------------------------------------- */
int main(void) {
    // variables to store times used for compute time duration
    struct timespec startSingle, endSingle, startMulti, endMulti;
    double durationSingle, durationMulti, speedup;
    
 
    srand((unsigned int)time(NULL));

    printf("Initializing two %dx%d matrices...\n", N, N);
    initializeMatrix(A);
    initializeMatrix(B);

   
    // Number of threads used
    threadsToUse = 10u;
   
    /* --- Benchmark Single-Threaded --- */
    printf("Starting single-threaded matrix multiplication...\n");
    clock_gettime(CLOCK_MONOTONIC, &startSingle);
    multiplySingleThreaded();
    clock_gettime(CLOCK_MONOTONIC, &endSingle);
    durationSingle = elapsedMs(startSingle, endSingle);
    printf("Single-threaded took: %.3f ms\n\n", durationSingle);

    /* --- Benchmark Multi-Threaded --- */
    printf("Starting multi-threaded matrix multiplication using %u threads...\n",
           threadsToUse);
    clock_gettime(CLOCK_MONOTONIC, &startMulti);
    multiplyMultiThreaded(threadsToUse);
    clock_gettime(CLOCK_MONOTONIC, &endMulti);
    durationMulti = elapsedMs(startMulti, endMulti);
    printf("Multi-threaded took: %.3f ms\n\n", durationMulti);

    /* --- Speedup --- */
    speedup = durationSingle / durationMulti;
    printf("=========================================\n");
    printf("Performance Increase (Speedup): %.4fx faster!\n", speedup);
    printf("=========================================\n");

    return 0;
}
