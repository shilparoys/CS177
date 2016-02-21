// Shilpa Chirackel & 861060176

#include <iostream>
#include "cpp.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace std;

//defintions
#define TINY 1.e-20
#define CellNum 120
#define TotalTime 1440
#define CarLength 2

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
    void createCar(int i){
        i++;        //need to make sure that tail does not become a negative no
        carId = i;
        head = 2* i;
        tail = 2*i - 1;
        speed = 1;
        travelDistance = 0;
        hold_time = 1;
    }
    void snapshot(){
        cout << "Car Information: \n";
        cout << "carId: " << carId << "\thead: " << head << "\t tail:" << tail << endl;
        cout << "speed: " << speed << "\ttravelDistance: " << travelDistance << "\thold_time: " << hold_time << endl << endl;
    }
};

//variable declarations
facility_set * road;
int numCars = 0;        //number of cars in simulation
vector<Car> carObj;

extern "C" void sim()		// main process
{
	create("sim");
    //initialize
    road = new facility_set("road", CellNum);
    cout << "Please enter number of cars : ";
    cin >> numCars;
    Car c;
    //initialize numCars object and make the zombie cars to move
    for(int i = 0; i < numCars; i++){
        carObj.push_back(c);
        carObj.at(i).createCar(i);
    }    
    hold(TotalTime);         //wait for a whole day (in minutes) to pass
    //print out snapshot
    for(int i = 0; i < numCars; i++){
        carObj.at(i).snapshot();
    }
}


