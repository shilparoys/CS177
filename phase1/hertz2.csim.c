// Shilpa Chirackel & 861060176

#include <iostream>
#include "cpp.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
using namespace std;

#define TINY 1.e-20
#define CellNum 120

//variable declarations
facility_set * road;
double *D;              //contains departure times
static int driverNum = 0;
string driverId = "car";
//function declarations
void createTraffic();
void addCar(int startingCell);

extern "C" void sim()		// main process
{
	create("sim");
    //initialize
    road = new facility_set("road", CellNum);
    D = new double[CellNum];
    for(int i = 0; i < CellNum; i++){
        D[i] = 0.0;
    }
    createTraffic();    //start a stream of traffic
    hold(1440);         //wait for a whole day (in minutes) to pass
    report();
}

void createTraffic(){
    create("createTraffic");
    addCar(1);          //placing car at position 1 because a stopped car takes 2 cells
}

void addCar(int startingCell){
    create("addCar");
}

