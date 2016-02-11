// Shilpa Chirackel & 861060176

#include "cpp.h"
#include <string.h>
#include <math.h>
#define ROW 10
#define COLUMN 10000
#define C4 10e3
#define C5 10e6
#define C6 10e9

//variable declarations
double U[ROW][COLUMN];
//function declarations
void reportStep2(double * U);
double sampleMean(double *U);
double sampleVarTwice(double *U);
double sampleVarOnce(double *U);
extern "C" void sim()		// main process
{
    //Step One
    printf("Step One Results\n");
    for(int i = 0; i < ROW; i++){
        for(int j = 0; j < COLUMN; j++){
            U[i][j] = uniform(0,1);
        }
    }
    for(int i = 0; i < ROW; i++){
        printf("Row %d\n", i+1);
        reportStep2(U[i]);
    }

    printf("\nStep Four Results\n");
    for(int i = 0; i < ROW; i++){
        for(int j = 0; j < COLUMN; j++){
            U[i][j] = uniform(0,1) + C4;
        }
    }
    for(int i = 0; i < ROW; i++){
        printf("Row %d\n", i+1);
        reportStep2(U[i]);
    }
    printf("\nStep Five Results\n");
    for(int i = 0; i < ROW; i++){
        for(int j = 0; j < COLUMN; j++){
            U[i][j] = uniform(0,1) + C5;
        }
    }
    for(int i = 0; i < ROW; i++){
        printf("Row %d\n", i+1);
        reportStep2(U[i]);
    } 
    printf("\nStep Six Results\n");
    for(int i = 0; i < ROW; i++){
        for(int j = 0; j < COLUMN; j++){
            U[i][j] = uniform(0,1) + C6;
        }
    }
    for(int i = 0; i < ROW; i++){
        printf("Row %d\n", i+1);
        reportStep2(U[i]);
    }
}

void reportStep2(double * U){
    double sMean = sampleMean(U);
    double sVarTwice = sampleVarTwice(U);
    double sVarOnce = sampleVarOnce(U);
    printf("Mean: %d\n", sMean);
    printf("Variance Twice: %d\n", sVarTwice);
    printf("Variance Once: %d\n", sVarOnce);
}

double sampleMean(double * U){
    double sum = 0;
    for(int i = 0; i < COLUMN; i++){
        sum += U[i];
    }
    return sum/COLUMN;
}

double sampleVarTwice(double *U){
    double mean = sampleMean(U);
    double sum = 0;
    double temp = 0;
    for(int i = 0; i < COLUMN; i++){
        temp = U[i] - mean;
        sum += pow(temp, 2);
    }
    return sum/(COLUMN-1);
}

double sampleVarOnce(double *U){
    double mean = sampleMean(U);
    double term2 = COLUMN * pow(mean, 2);
    double term1 = 0;
    for (int i = 0; i < COLUMN; i++){
        term1 += pow(U[i], 2);
    }
    return ((term1-term2)/(COLUMN-1));
}
