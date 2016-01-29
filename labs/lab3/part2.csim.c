//Timothy Lim 861048365

#include <iostream>
#include "cpp.h"
#include <string.h>

using namespace std;


#define REGULAR 0
#define DISEL 1

facility_ms* reg_pumps;
facility_ms* disel_pumps;

int reg_pumps_num = 0;
int disel_pumps_num = 0;
const double profit = .025; // dollars per litre
const double cost = 20;
double reportInterval, endingTime = 0.0;


int totalArrivals = 0;
double totalLitresSold = 0.0;
int customersServed = 0;
int balkingCustomers = 0;
double totalLitresMissed = 0.0;
double totalWaitingTime = 0.0;
double totalServiceTime = 0.0;
int max_queue = 0;
double totalEmptyQueueTime = 0.0;
double lastCustomerTime = 0.0;

int total_pumps;

stream *arrivalStream;
stream *litreStream;
stream *balkingStream;
stream *serviceStream;
stream *diselStream;
stream *choiceStream;


void departures();
void dep_cust();
void shuttle();
long group_size();
bool will_balk(long qlength, double litres);
void getStatus();
void queueMonitor();

extern "C" void sim()		// main process
{
	create("sim");
	
	long seed;

	arrivalStream = new stream();
	litreStream = new stream();
	balkingStream = new stream();
	serviceStream = new stream();
	diselStream = new stream();
	choiceStream = new stream();

	cout << "Enter report interval and ending time: ";
	cin >> reportInterval >> endingTime;
	cout << endl;

	cout << "Enter number of regular pumps: ";
	cin >> reg_pumps_num;
	cout << endl;

	cout << "Enter number of regular and disel pumps: ";
	cin >> disel_pumps_num;
	cout << endl;

	total_pumps = reg_pumps_num + disel_pumps_num;

	cout << "Enter 6 seed values: ";
	cin >> seed;
	arrivalStream->reseed(seed);
	cin >> seed;
	litreStream->reseed(seed);
	cin >> seed;
	balkingStream->reseed(seed);
	cin >> seed;
	serviceStream->reseed(seed);
	cin >> seed;
	diselStream->reseed(seed);
	cin >> seed;
	choiceStream->reseed(seed);

	reg_pumps = new facility_ms("reg_pumps", reg_pumps_num);
	disel_pumps = new facility_ms("disel_pumps", disel_pumps_num);


	printf ("%9s%7s%8s%9s%8s%7s%9s%7s%8s%7s%5s\n", " Current", "Total ",
		"NoQueue", "Car->Car", "Average", "Number", "Average", "Pump ",
		"Total", " Lost ", "Max");
	printf ("%9s%7s%8s%9s%8s%7s%9s%7s%8s%7s%6s\n", "   Time ", "Cars ",
		"Fraction", "  Time  ", " Litres ", "Balked", "  Wait ",
		"Usage ", "Profit", "Profit", "Size");
	for (int i = 0; i < 85; i++)
		cout << "-";
	cout << "\n";

	departures();		// start a stream of departing customers
	//queueMonitor();
	getStatus();
	hold (endingTime);		// wait for a whole day (in minutes) to pass
}


// Model segment 2a: generate groups of customers at car lot, heading to airport

void departures()		// this model segment spawns departing customers
{
	create("departures");

	while(clock < endingTime)
	{
		hold(arrivalStream->expntl(50));
		dep_cust();
	}
}

// Model segment 2b: activities followed by individual car lot customers

void dep_cust()
{
	create("dep_cust");
	
	totalArrivals += 1;

	double disel = diselStream->uniform(0, 1);
	long qlength_disel = 0;
	long qlength_reg = 0;
	double serviceTime = 0.0;
	double litresNeeded = litreStream->uniform(10, 60);

	qlength_disel = disel_pumps->qlength();
	qlength_reg = reg_pumps->qlength();

	double start_wait = clock;
	double service_time = 0.0;

	if(qlength_disel == 0 && qlength_reg == 0){
		totalEmptyQueueTime += clock - lastCustomerTime;
	}

	if(disel <= 0.05){

		if(will_balk(qlength_disel, litresNeeded)){
			balkingCustomers += 1;
			totalLitresMissed += litresNeeded;
			return;
		}else{
			totalLitresSold += litresNeeded;
			disel_pumps->reserve();
			totalWaitingTime += clock - start_wait;
			service_time = clock;
			hold(150 + serviceStream->normal(litresNeeded/2, 30));
			totalServiceTime += clock - service_time;
			disel_pumps->release();
			customersServed += 1;
			lastCustomerTime = clock;
		}
		
	}else{
		
		if(qlength_disel < qlength_reg){

			if(will_balk(qlength_disel, litresNeeded)){
				balkingCustomers += 1;
				totalLitresMissed += litresNeeded;
				return;
			}else{
				totalLitresSold += litresNeeded;
				disel_pumps->reserve();
				totalWaitingTime += clock - start_wait;
				service_time = clock;
				hold(150 + serviceStream->normal(litresNeeded/2, 30));
				totalServiceTime += clock - service_time;
				disel_pumps->release();
				customersServed += 1;
				lastCustomerTime = clock;
			}

		}else if(qlength_disel > qlength_reg){

			if(will_balk(qlength_reg, litresNeeded)){
				balkingCustomers += 1;
				totalLitresMissed += litresNeeded;
				return;
			}else{
				totalLitresSold += litresNeeded;
				reg_pumps->reserve();
				totalWaitingTime += clock - start_wait;
				service_time = clock;
				hold(150 + serviceStream->normal(litresNeeded/2, 30));
				totalServiceTime += clock - service_time;
				reg_pumps->release();
				customersServed += 1;
				lastCustomerTime = clock;
			}

		}else{

			if(choiceStream->uniform_int(0, 1) == REGULAR){

				if(will_balk(qlength_reg, litresNeeded)){
					balkingCustomers += 1;
					totalLitresMissed += litresNeeded;
					return;
				}else{
					totalLitresSold += litresNeeded;
					reg_pumps->reserve();
					totalWaitingTime += clock - start_wait;
					service_time = clock;
					hold(150 + serviceStream->normal(litresNeeded/2, 30));
					totalServiceTime += clock - service_time;
					reg_pumps->release();
					customersServed += 1;
					lastCustomerTime = clock;
				}

			}else{

				if(will_balk(qlength_disel, litresNeeded)){
					balkingCustomers += 1;
					totalLitresMissed += litresNeeded;
					return;
				}else{
					totalLitresSold += litresNeeded;
					disel_pumps->reserve();
					totalWaitingTime += clock - start_wait;
					service_time = clock;
					hold(150 + serviceStream->normal(litresNeeded/2, 30));
					totalServiceTime += clock - service_time;
					disel_pumps->release();
					customersServed += 1;
					lastCustomerTime = clock;
				}

			}

		}
		
	}

	qlength_disel = disel_pumps->qlength();
	qlength_reg = reg_pumps->qlength();

	if(qlength_reg > max_queue){
		max_queue = qlength_reg;
	}

	if(qlength_disel > max_queue){
		max_queue = qlength_disel;
	}

}

bool will_balk(long qlength, double litres){
	return qlength > 0 && balkingStream->uniform(0,1)
		> (40 + litres) / (25 * (3 + qlength));
}

void getStatus(){
	create("status");
	while(clock <= endingTime){
		hold(reportInterval);

		printf("%8.0f%7i", clock, totalArrivals);

		printf("%8.3f", totalEmptyQueueTime/clock);
	
		if (totalArrivals > 0) {
			printf("%9.3f%8.3f", clock/totalArrivals,
				(totalLitresSold + totalLitresMissed) / totalArrivals);
		}
		else
			printf ("%9s%8s", "Unknown", "Unknown");

		printf ("%7i", balkingCustomers);
		if (customersServed > 0)
			printf ("%9.3f", totalWaitingTime / customersServed);
		else
			printf ("%9s", "Unknown");
		printf ("%7.3f", totalServiceTime / clock);
		printf ("%9.2f", totalLitresSold * profit - cost * total_pumps);
		printf ("%7.2f", totalLitresMissed * profit);
		printf ("%4d\n", max_queue);
	}

}
