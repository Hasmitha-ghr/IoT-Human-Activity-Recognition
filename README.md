# IoT Sensor-based Human Activity Recognition

## Overview

This project implements a simplified Human Activity Recognition (HAR) preprocessing pipeline in C using smartphone sensor data. The objective is to demonstrate the concepts of dimensionality reduction and feature extraction for IoT sensor data.

The implementation was developed as a learning project inspired by work carried out during a Summer Research Internship at IIT Indore.

---

## Features

- Reads smartphone sensor data from a CSV file.
- Normalizes sensor readings using z-score normalization.
- Processes each motion window as a 450-dimensional feature vector.
- Applies Johnson–Lindenstrauss (JL) random projection to reduce the feature dimension from **450 to 50**.
- Performs simplified SVD-inspired feature extraction using power iteration on the covariance matrix.
- Saves the reduced feature vectors to an output CSV file.

---

## Project Workflow

```text
Sensor Data (CSV)
        │
        ▼
Data Normalization
        │
        ▼
450-Dimensional Motion Windows
        │
        ▼
Johnson–Lindenstrauss Random Projection
(450 → 50)
        │
        ▼
Covariance Matrix
        │
        ▼
Power Iteration
(SVD-inspired Feature Extraction)
        │
        ▼
Reduced Feature Vectors
```

---

## Input

The program expects a CSV file named `sensor_data.csv`.

Each row represents one motion window containing **450 numerical values** corresponding to preprocessed accelerometer and gyroscope measurements.

### Example

```text
0.12,0.34,-0.21,...,0.98
0.15,0.29,-0.18,...,1.02
```

---

## Output

The program generates:

```text
reduced_features.csv
```

Each row contains the corresponding **50-dimensional feature vector** obtained after JL projection.

The console output also displays:

- Number of motion windows processed
- Original feature dimension
- Reduced feature dimension
- Extracted principal feature strengths

---

## Algorithms Used

### 1. Data Normalization

Each feature is normalized using z-score normalization:

```text
x = (x − mean) / standard deviation
```

This ensures that all features contribute equally during dimensionality reduction.

### 2. Johnson–Lindenstrauss Random Projection

A random Gaussian projection matrix is generated to reduce feature dimensionality from **450** to **50** while approximately preserving pairwise distances.

### 3. Simplified SVD-inspired Feature Extraction

A covariance matrix is computed from the projected data. Power iteration is then used to estimate the dominant principal directions, providing a simplified feature extraction step suitable for educational purposes.

---

## Compilation

Compile the program using GCC:

```bash
gcc har_project.c -o har_project -lm
```

The `-lm` option links the C math library required for functions such as `sqrt()`, `log()`, `cos()`, and `fabs()`.

---

## Running

Linux/macOS:

```bash
./har_project
```

Make sure `sensor_data.csv` is in the same directory as the executable.

---

## Sample Console Output

```text
Reading sensor data from CSV...
Normalizing sensor readings...
Applying Johnson-Lindenstrauss random projection...

Reduced Data Summary
--------------------
Input dimension      : 450
Reduced dimension    : 50
Windows processed    : 10

Extracting Principal Motion Features...
Feature 1 strength: 8.14
Feature 2 strength: 6.37
Feature 3 strength: 5.29

Reduced features saved to reduced_features.csv

Human Activity Recognition preprocessing completed.
```
## Key Parameters

| Constant | Description |
|----------|-------------|
| `ORIGINAL_DIM = 450` | Number of features in each input motion window. |
| `PROJECTED_DIM = 50` | Feature dimension after Johnson–Lindenstrauss random projection. |
| `MAX_WINDOWS = 100` | Maximum number of motion windows the program processes from the input CSV file. |
| `SVD_FEATURES = 5` | Number of principal motion features extracted using the simplified SVD-inspired method. |

...
## Technologies

- C 
- GCC
- Standard C Libraries
  - `stdio.h`
  - `stdlib.h`
  - `math.h`
  - `string.h`
  - `time.h`

---

