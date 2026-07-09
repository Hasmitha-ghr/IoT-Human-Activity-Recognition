#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define ORIGINAL_DIM 450
#define PROJECTED_DIM 50
#define MAX_WINDOWS 100
#define SVD_FEATURES 5
#define LINE_SIZE 12000

double sensorWindow[MAX_WINDOWS][ORIGINAL_DIM];
double reducedWindow[MAX_WINDOWS][PROJECTED_DIM];
double projectionMatrix[ORIGINAL_DIM][PROJECTED_DIM];

int totalWindows = 0;

double randNormal() {
    double u = ((double) rand() + 1) / ((double) RAND_MAX + 1);
    double v = ((double) rand() + 1) / ((double) RAND_MAX + 1);

    return sqrt(-2 * log(u)) * cos(2 * M_PI * v);
}

int loadCsvData(const char *fileName) {
    FILE *file = fopen(fileName, "r");

    if (file == NULL) {
        printf("Could not open input file: %s\n", fileName);
        return 0;
    }

    char line[LINE_SIZE];

    while (fgets(line, sizeof(line), file) && totalWindows < MAX_WINDOWS) {
        char *token = strtok(line, ",");
        int col = 0;

        while (token != NULL && col < ORIGINAL_DIM) {
            sensorWindow[totalWindows][col] = atof(token);
            token = strtok(NULL, ",");
            col++;
        }

        if (col == ORIGINAL_DIM) {
            totalWindows++;
        }
    }

    fclose(file);
    return totalWindows;
}

void normalizeData() {
    for (int col = 0; col < ORIGINAL_DIM; col++) {
        double mean = 0.0;
        double variance = 0.0;

        for (int row = 0; row < totalWindows; row++) {
            mean += sensorWindow[row][col];
        }

        mean /= totalWindows;

        for (int row = 0; row < totalWindows; row++) {
            double diff = sensorWindow[row][col] - mean;
            variance += diff * diff;
        }

        double stdDev = sqrt(variance / totalWindows);

        if (stdDev < 1e-9) {
            stdDev = 1.0;
        }

        for (int row = 0; row < totalWindows; row++) {
            sensorWindow[row][col] = (sensorWindow[row][col] - mean) / stdDev;
        }
    }
}

void buildProjectionMatrix() {
    double scale = 1.0 / sqrt(PROJECTED_DIM);

    for (int i = 0; i < ORIGINAL_DIM; i++) {
        for (int j = 0; j < PROJECTED_DIM; j++) {
            projectionMatrix[i][j] = randNormal() * scale;
        }
    }
}

void reduceSensorWindows() {
    for (int row = 0; row < totalWindows; row++) {
        for (int col = 0; col < PROJECTED_DIM; col++) {
            double sum = 0.0;

            for (int k = 0; k < ORIGINAL_DIM; k++) {
                sum += sensorWindow[row][k] * projectionMatrix[k][col];
            }

            reducedWindow[row][col] = sum;
        }
    }
}

void saveReducedData(const char *fileName) {
    FILE *file = fopen(fileName, "w");

    if (file == NULL) {
        printf("Could not create output file: %s\n", fileName);
        return;
    }

    for (int i = 0; i < totalWindows; i++) {
        for (int j = 0; j < PROJECTED_DIM; j++) {
            fprintf(file, "%.6f", reducedWindow[i][j]);

            if (j < PROJECTED_DIM - 1) {
                fprintf(file, ",");
            }
        }

        fprintf(file, "\n");
    }

    fclose(file);
}

void printReducedSummary() {
    printf("\nReduced Data Summary\n");
    printf("--------------------\n");
    printf("Input dimension      : %d\n", ORIGINAL_DIM);
    printf("Reduced dimension    : %d\n", PROJECTED_DIM);
    printf("Windows processed    : %d\n", totalWindows);

    for (int i = 0; i < totalWindows; i++) {
        double min = reducedWindow[i][0];
        double max = reducedWindow[i][0];
        double sum = 0.0;

        for (int j = 0; j < PROJECTED_DIM; j++) {
            double value = reducedWindow[i][j];

            if (value < min) {
                min = value;
            }

            if (value > max) {
                max = value;
            }

            sum += value;
        }

        printf("Window %d -> mean: %.4f, min: %.4f, max: %.4f\n",
               i + 1, sum / PROJECTED_DIM, min, max);
    }
}

void covarianceMatrix(double cov[PROJECTED_DIM][PROJECTED_DIM]) {
    for (int i = 0; i < PROJECTED_DIM; i++) {
        for (int j = 0; j < PROJECTED_DIM; j++) {
            double sum = 0.0;

            for (int k = 0; k < totalWindows; k++) {
                sum += reducedWindow[k][i] * reducedWindow[k][j];
            }

            cov[i][j] = sum / totalWindows;
        }
    }
}

double vectorLength(double vector[], int size) {
    double sum = 0.0;

    for (int i = 0; i < size; i++) {
        sum += vector[i] * vector[i];
    }

    return sqrt(sum);
}

void multiplyByVector(double matrix[PROJECTED_DIM][PROJECTED_DIM],
                      double vector[PROJECTED_DIM],
                      double result[PROJECTED_DIM]) {
    for (int i = 0; i < PROJECTED_DIM; i++) {
        result[i] = 0.0;

        for (int j = 0; j < PROJECTED_DIM; j++) {
            result[i] += matrix[i][j] * vector[j];
        }
    }
}

void extractMotionFeatures() {
    double cov[PROJECTED_DIM][PROJECTED_DIM];
    double eigenVector[PROJECTED_DIM];
    double temp[PROJECTED_DIM];

    covarianceMatrix(cov);

    printf("\nSVD-style Motion Features\n");
    printf("-------------------------\n");

    for (int feature = 0; feature < SVD_FEATURES; feature++) {
        for (int i = 0; i < PROJECTED_DIM; i++) {
            eigenVector[i] = randNormal();
        }

        for (int iter = 0; iter < 80; iter++) {
            multiplyByVector(cov, eigenVector, temp);

            double norm = vectorLength(temp, PROJECTED_DIM);

            if (norm < 1e-9) {
                break;
            }

            for (int i = 0; i < PROJECTED_DIM; i++) {
                eigenVector[i] = temp[i] / norm;
            }
        }

        multiplyByVector(cov, eigenVector, temp);

        double eigenValue = 0.0;
        for (int i = 0; i < PROJECTED_DIM; i++) {
            eigenValue += eigenVector[i] * temp[i];
        }

        printf("Feature %d strength: %.4f\n",
               feature + 1, sqrt(fabs(eigenValue)));

        for (int i = 0; i < PROJECTED_DIM; i++) {
            for (int j = 0; j < PROJECTED_DIM; j++) {
                cov[i][j] -= eigenValue * eigenVector[i] * eigenVector[j];
            }
        }
    }
}

int main() {
    srand((unsigned int) time(NULL));

    const char inputFile[] = "sensor_data.csv";
    const char outputFile[] = "reduced_sensor_data.csv";

    printf("Reading sensor data from CSV...\n");

    if (loadCsvData(inputFile) == 0) {
        printf("No valid sensor windows found.\n");
        return 1;
    }

    printf("Normalizing sensor readings...\n");
    normalizeData();

    printf("Applying Johnson-Lindenstrauss random projection...\n");
    buildProjectionMatrix();
    reduceSensorWindows();

    printReducedSummary();

    printf("\nSaving reduced data to %s...\n", outputFile);
    saveReducedData(outputFile);

    extractMotionFeatures();

    printf("\nProcessing completed successfully.\n");

    return 0;
}