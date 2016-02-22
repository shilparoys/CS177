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
#define TotalTime 10
#define CarLength 2

facility_set * road;
int numCars = 0;        //number of cars in simulation

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
    //initialize a car object
    void createCar(int i){
        i++;        //need to make sure that tail does not become a negative no
        carId = i;
        head = 2* i;
        tail = 2*i - 1;
        speed = 1;
        travelDistance = 0;
        hold_time = 1;
    }

    bool empty(){
        if(carId == 0){
            return true;
        }
        else{
            return false;
        }
    }
    //print out info about car
    void snapshot(){
        cout << "Car Information: \n";
        cout << "carId: " << carId << "\thead: " << head << "\t tail:" << tail << endl;
        cout << "speed: " << speed << "\ttravelDistance: " << travelDistance << "\thold_time: " << hold_time << endl << endl;
    }
    
    //function declaration
    void addCarToTraffic(int index);
    double calculateHoldTime(int index);    
    int calculateLookahead(int index);
};

vector<Car> carObj;

//moving the car
void Car::addCarToTraffic(int index){
    create("addCarToTraffic");
    //non-moving car is added to traffic, so it should reserve 2 cells for head and tail
    (*road)[carObj.at(index).head].reserve();
    (*road)[carObj.at(index).tail].reserve();
    
    while(clock < TotalTime){
        //cout << "Trying to move car " << index << endl;
        //calculate hold time for the car for given speed
        carObj.at(index).hold_time = calculateHoldTime(index);   
        int lookAhead = calculateLookahead(index);
        //we are going to do collision avoidance
        for(int i = 0; i < lookAhead; i++){
            Car temp = carObj.at(index + 1 + i);
            if(temp.empty()){
                cout << "potentional crash\n";
                if(carObj.at(index).speed -2 < 0){
                    carObj.at(index).speed = max(0, temp.speed);
                }
                else{
                    carObj.at(index).speed = max(carObj.at(index).speed-2, temp.speed);
                }
            }
    
        }

        //we are going to make the car move
        //sequences: release the head, reserve head +1, release the tail, reserve tail + 1
        (*road)[carObj.at(index).head].release();
        carObj.at(index).head++;
        (*road)[carObj.at(index).head].reserve();

        hold(carObj.at(index).hold_time);
        carObj.at(index).travelDistance++;

        (*road)[carObj.at(index).tail].release();
        carObj.at(index).tail++;
        (*road)[carObj.at(index).tail].reserve();
    }
}

//calculate the hold time
double Car::calculateHoldTime(int index){
    if(carObj.at(index).speed == 0){
        return 0;
    }
    else if(carObj.at(index).speed == 1){
        return (3.0/CarLength);
    }
    else if(carObj.at(index).speed == 2){
        return ((11.0/6)/CarLength);
    }
    else if(carObj.at(index).speed == 3){
        return (1.0/CarLength);
    }
    else if(carObj.at(index).speed == 4){
        return ((2.0/3.0)/CarLength);
    }
    else{
        return (.5 / CarLength);
    }
}

//calculate lookahead value
int Car::calculateLookahead(int index){
    if(carObj.at(index).speed == 0){
        return 1;
    }
    else if(carObj.at(index).speed == 1){
        return 1;
    }
    else if(carObj.at(index).speed== 2){
        return 4;
    }
    else if(carObj.at(index).speed == 3){
        return 4;
    }
    else if(carObj.at(index).speed == 4){
        return 6;
    }
    else{
        return 8;
    }
}

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
        carObj.at(i).addCarToTraffic(i);
    }    
    hold(TotalTime);         //wait for a whole day (in minutes) to pass
    //print out snapshot
    for(int i = 0; i < numCars; i++){
        carObj.at(i).snapshot();
    }
}


