// Shilpa Chirackel & 861060176

#include <iostream>
#include "cpp.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
using namespace std;

//defintions
#define TINY 1.e-20
#define CellNum 120
#define TotalTime 1440
#define CarLength 2

//variable declarations
facility_set * road;
double *D;              //contains departure times
//function declarations
void createTraffic();
void addCar(int startingCell);

//struct for car class
/*car object will contain information such as 
id, head, tail, holdtme, etc for each object*/

struct Car{
    public:
    int carId;
    int head;
    int tail;
    int speed;
    int travelDistance;
    double hold_time;
    Car():carId(0), head(0), tail(0), speed(0), travelDistance(0), hold_time(0.0){};
};

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

