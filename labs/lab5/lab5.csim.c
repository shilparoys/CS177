// Shilpa Chirackel & 861060176
/*ANSWER TO QUESTION 7:
In theory, adding the constant C to every data item in steps 4-6 makes the one-pass sample variance numerically unstable because of the cancellation error when we subtract two very large numbers to produce a small final answer. 
In practice, I observed that the theory holds true. As I add the constant C, the one-pass sample variance answers become large whole numbers rather than the desire answer of an samller value answer.
*/
#include "cpp.h"
#include <string.h>
#include <math.h>
#define ROW 10
#define COLUMN 10000
#define C4 10e3
#define C5 10e6
#define C6 10e9
#define t  2.262 //using t distribution, for 95% confidence, df = 9 and t = 2.262
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
    printf("Step One,Two, and Three Results\n");
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
    printf("Mean: %.2f\n", sMean);
    printf("Variance Twice: %.2f\n", sVarTwice);
    printf("Variance Once: %.2f\n", sVarOnce);
    double stdDerivationTwice = sqrt(sVarTwice);
    double stdDerivationOnce = sqrt(sVarOnce);
    double upperLimit2 = sMean + t * (stdDerivationTwice/(sqrt(COLUMN)));
    double lowerLimit2 = sMean - t * (stdDerivationTwice/(sqrt(COLUMN)));
    double upperLimit1 = sMean + t * (stdDerivationOnce/(sqrt(COLUMN)));
    double lowerLimit1 = sMean - t * (stdDerivationOnce/(sqrt(COLUMN)));
    printf("Confidence Level For sample variance one: [%.2f , %.2f]\n", lowerLimit1, upperLimit1);
    printf("Confidence Level for sample variance two: [%.2f , %.2f]\n\n", lowerLimit2, upperLimit2);
}

double sampleMean(double * U){
    double sum = 0.0;
    for(int i = 0; i < COLUMN; i++){
        sum += U[i];
    }
    return sum/COLUMN;
}

double sampleVarTwice(double *U){
    double mean = sampleMean(U);
    double sum = 0.0;
    double temp = 0.0;
    for(int i = 0; i < COLUMN; i++){
        temp = U[i] - mean;
        sum += pow(temp, 2);
    }
    return sum/(COLUMN-1);
}

double sampleVarOnce(double *U){
    double mean = sampleMean(U);
    double term2 = COLUMN * pow(mean, 2);
    double term1 = 0.0;
    for (int i = 0; i < COLUMN; i++){
        term1 += pow(U[i], 2);
    }
    double temp2 = term1 - term2;
    double temp3 = double(COLUMN-1);
    return temp2/temp3;
}
