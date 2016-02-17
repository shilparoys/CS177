// Shilpa Chirackel & 861060176

#include "cpp.h"
#include <string.h>

#define TINY 1.e-20
#define CellNum 120

//variable declarations
facility_set * road;
double *D;              //contains departure times

//function declarations
void createCar();

extern "C" void sim()		// main process
{
	create("sim");
    //initialize
    road = new facility_set("road", CellNum);
    D = new double[CellNum];
    for(int i = 0; i < CellNum; i++){
        D[i] = 0.0;
    }
    createCar();        //start a stream of cars
    hold(1440);         //wait for a whole day (in minutes) to pass
    report();
}

void createCar(){
    create("createCar");
}

