#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double** initialize_matrix(int n, bool random = true) {
    double **matrix = (double **)malloc(n * sizeof(double *));
    for (int i = 0; i < n; i++) {
        matrix[i] = (double *)malloc(n * sizeof(double));
    }

    // Get random values for each elements in the matrix
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if(random) 
                matrix[i][j] = rand() % 10;
            else 
                matrix[i][j] = 0;
        }
    }
    
    return matrix;
}

void free_matrix(double **matrix, int n) {
    for(int i = 0; i < n; i++) 
        free(matrix[i]);

    free(matrix);
}

double** multiply_2_matrix(double **a, double **b, int n) {
    double **result = initialize_matrix(n, false);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }

    return result;
}

double measure_time(int n) {
    double **a = initialize_matrix(n);
    double **b = initialize_matrix(n);

    // Start time
    clock_t start = clock();
    
    double **result = multiply_2_matrix(a, b, n);
    
    // End time
    clock_t end = clock();

    // Free memory
    free_matrix(a, n);
    free_matrix(b, n);
    free_matrix(result, n);
    
    return (double)(end - start) / CLOCKS_PER_SEC;
}

int main() {
    int sizes[] = {1, 100, 500, 1000, 2500, 3500, 4500, 5000};
    int num_tests = sizeof(sizes) / sizeof(sizes[0]); 
    
    for (int i = 0; i < num_tests; i++) {
        int n = sizes[i];

        printf("\nMatrix %d x %d\n", n, n);

        double time_taken = measure_time(n);

        printf("Time to multiply 2 matrix: %f seconds\n\n", time_taken);
    }

    return 0;
}
