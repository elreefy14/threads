#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
///home/elreefy14/Desktop/6701-matmult/input1-matrix.txt
///home/elreefy14/Desktop/6701-matmult/input3-matrix.txt
pthread_mutex_t mutex;

typedef struct {
    int* A;
    int* B;
    int* C;
    int l;
    int m;
    int n;
} ThreadArgs;



// takes a specific row from Matrix A and a specific column from Matrix B
void* computeElement(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    int* A = args->A;
    int* B = args->B;
    int* C = args->C;
    int l = args->l;
    int m = args->m;
    int n = args->n;

    int sum = 0;
    for (int i = 0; i < m; i++) {
        sum += A[l * m + i] * B[i * n + n];
    }
   //for synchronization
    pthread_mutex_lock(&mutex);
    C[l * n + n] = sum;
    pthread_mutex_unlock(&mutex);
    //print first element of matrix c
    printf("C[%d][%d] = %d\n", l, n, sum);
    

    return NULL;
}

	// It takes a specific row from Matrix A and multiplies it with each column of Matrix B   
void* computeRow(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    int* A = args->A;
    int* B = args->B;
    int* C = args->C;
    int l = args->l;
    int m = args->m;
    int n = args->n;

    int row = l;

    for (int col = 0; col < n; col++) {
        int sum = 0;
        for (int i = 0; i < m; i++) {
            sum += A[row * m + i] * B[i * n + col];
        }

        pthread_mutex_lock(&mutex);
        C[row * n + col] = sum;
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

void printMatrix(int* matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", matrix[i * cols + j]);
        }
        printf("\n");
    }
}

double getTimeDifference(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
}

int main() {
    char filename[100];
    printf("Enter the file name: ");
    scanf("%s", filename);

    FILE* file = fopen(filename, "r");

    if (file == NULL) {
        printf("Failed to open the file.\n");
        return 1;
    }

    int l, m, n;
    fscanf(file, "%d %d", &l, &m);

    int* A = (int*)malloc(l * m * sizeof(int));

    for (int i = 0; i < l * m; i++)
        fscanf(file, "%d", &A[i]);

    fscanf(file, "%d %d", &m, &n);

    int* B = (int*)malloc(m * n * sizeof(int));

    for (int i = 0; i < m * n; i++)
        fscanf(file, "%d", &B[i]);

    fclose(file);

   //if row of A is not equal to column of B, then the multiplication is not possible
   

    int* C1 = (int*)malloc(l * n * sizeof(int));
    int* C2 = (int*)malloc(l * n * sizeof(int));
      //threads for synchronization 
    pthread_mutex_init(&mutex, NULL);

    struct timeval start, end;
    double elapsedTime1, elapsedTime2;

    // First Variation: Each element computed in a thread
    gettimeofday(&start, NULL);

    pthread_t tids1[l][n];
    ThreadArgs args1[l][n];

    for (int i = 0; i < l; i++) {
        for (int j = 0; j < n; j++) {
            args1[i][j].A = A;
            args1[i][j].B = B;
            args1[i][j].C = C1;
            args1[i][j].l = i;
            args1[i][j].m = m;
            args1[i][j].n = j;

             //tids1[i][j] is the thread id of the thread that will be created
              pthread_create(&tids1[i][j], NULL, computeElement, (void*)&args1[i][j]);
        }
    }

    for (int i = 0; i < l; i++) {
        for (int j = 0; j < n; j++) {
            pthread_join(tids1[i][j], NULL);
        }
    }

    gettimeofday(&end, NULL);
    elapsedTime1 = getTimeDifference(start, end);

    // Second Variation: Each row computed in a thread
    gettimeofday(&start, NULL);
    //create threads and pass the arguments to each thread
    pthread_t tids2[l];
    ThreadArgs args2[l];

    for (int i = 0; i < l; i++) {
        args2[i].A = A;
        args2[i].B = B;
        args2[i].C = C2;
        args2[i].l = i;
        args2[i].m = m;
        args2[i].n = n;

        pthread_create(&tids2[i], NULL, computeRow, (void*)&args2[i]);
    }

    for (int i = 0; i < l; i++) {
        pthread_join(tids2[i], NULL);
    }

    gettimeofday(&end, NULL);
    elapsedTime2 = getTimeDifference(start, end);
     //print the 2 input matrices
     printf("Matrix A:\n");
     printMatrix(A, l, m);
        printf("Matrix b:\n");
     printMatrix(B, m, n);          
    // Print the resulting matrices and elapsed times
    printf("Result matrix computed element-wise:\n");
    printMatrix(C1, l, n);
    printf("END1 %.6f\n", elapsedTime1);

    printf("Result matrix 2 computed row-wise:\n");
    printMatrix(C2, l, n);
    printf("END2 %.6f\n", elapsedTime2);
    pthread_mutex_destroy(&mutex);

    return 0;
}