//Shilpa Chirackel & 861060176

#include "cpp.h"
#include <string.h>

#define TINY 1.e-20		// a very small time period

//variables
facility_set *vendingMachines;
int number;

//function
void departures();
void dep_cust();
long group_size();


extern "C" void sim()		// main process
{
	printf("Enter number of vending machines: ");
	scanf("%d", &number);
	vendingMachines = new facility_set("vendingMachines", number);

	create("sim");
	departures();		
	hold (1440);	
	report();
}

void departures()	
{
	create("departures");
	while(clock < 1440.)
	{
		hold(expntl(10)); // exponential interarrivals, mean 10 minutes
		long group = group_size();
		for (long i=0;i<group;i++)
			dep_cust();	// new customer appears at the car lot
	}
}


void dep_cust()
{
	create("dep_cust");
	
	double B = uniform(10,60);
	long minQ = (*vendingMachines)[0].qlength();
	int index = 0;
	for (int i=0; i<number; i++) {
		if ( (*vendingMachines)[i].qlength() < minQ ) {
			minQ = (*vendingMachines)[i].qlength();
			index = i;
		}
	}
	long Q = minQ; 
    double R = uniform(0,1);
    double C = (40+B)/(25*(3+Q));
	if (R <  C) {
		(*vendingMachines)[index].reserve();
		hold(uniform(0,150+normal(B/2,30)));
		(*vendingMachines)[index].release();
	}
}

long group_size()	// function gives the number of customers in a group
{
	double x = prob();
	if (x < 0.3) return 1;
	else
	{
		if (x < 0.7) return 2;
		else
			return 4;
	}
}
