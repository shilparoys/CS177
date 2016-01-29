// Shilpa Chirackel & 861060176

#include "cpp.h"
#include <string.h>

//constants
const double profit = .025; // dollars per litre
const double cost = 20;

//variable declarations
double simulationTime;
int arrivalStream, litreStream, balkingStream, serviceStream;
int totalArrival = 0;
int balkingCustomers = 0;
double TotalLitresMissed = 0.0;
double totalEmptyQueueTime = 0.0;
int customersServed = 0;
double TotalLitresSold = 0.0;
double TotalWaitingTime = 0.0;
double TotalServiceTime = 0.0;
int numPumps;
double ReportInterval, endingTime;
bool needDiesel; //yes 1 no 0
double maxPumpDieselQLength =0.0;
double maxPumpQLength = 0.0;

facility_ms * pumpMachine;
facility * pumpDiesel;

//function declaration
void departures();
void dep_cust();
void printStatus();
double serviceTime(double litres){
    double howLong = -6;
    for (int i = 21; i <= 12; i++){
        howLong += uniform(0, serviceStream);
    }
    return 150 + 0.5 * litres + 30 * howLong;
}
bool doesCarBalk (double litres, double  queueLength){
    return queueLength > 0 && uniform(0, balkingStream) > (40 + litres) / (25 * (3+ queueLength));
}

extern "C" void sim()		// main process
{
	create("sim");

    scanf("%lf", &ReportInterval);
    scanf("%lf", &endingTime);

    scanf("%d", &numPumps);
    pumpMachine = new facility_ms("pumpMachine", numPumps);
    pumpDiesel = new facility("pumpDiesel");

    printf("The simiulation run uses %d", numPumps);
    printf(" pumps and the following random number seeds:\n");
    scanf("%d", &arrivalStream);
    scanf("%d", &litreStream);
    scanf("%d", &balkingStream);
    scanf("%d", &serviceStream);
    
    printf("ArrivalStream is %d", arrivalStream);
    printf("\nLitreStream is %d", litreStream);
    printf("\nBalkingStream is %d", balkingStream);
    printf("\nServiceStream is %d", serviceStream);
    printf("\n");

	departures();
		
    printf ("%9s%7s%8s%9s%8s%7s%9s%7s%8s%7s%7s\n", " Current", "Total ",
                "NoQueue", "Car->Car", "Average", "Number", "Average", "Pump ",
                "Total", " Lost ", "Max");
    printf ("%9s%7s%8s%9s%8s%7s%9s%7s%8s%7s%7s\n", "   Time ", "Cars ",
                "Fraction", "  Time  ", " Litres ", "Balked", "  Wait ",
                "Usage ", "Profit", "Profit", "Size");
    for (int i = 0; i < 87; i++)
         printf( "-");
    printf("\n");
    printStatus();
    hold(endingTime + ReportInterval);
}


void departures()	
{
	create("departures");

	while(clock < endingTime)	
	{
		hold(uniform(0, arrivalStream));
		dep_cust();
	}
}


void dep_cust()
{
	create("dept_car");
    totalArrival++;
    double pumpQLength = pumpMachine->qlength();
    double pumpDieselQLength = pumpDiesel->qlength();
    
    double simulationTime = clock;
    double litresNeeded = 10 + uniform(0, litreStream) * 50;
    
    double  B = uniform(0, 10);
    if (B <= 0.5){
        needDiesel = 1; //diesel
    }
    else{
        needDiesel = 0; //regular
    }
    double arrTime = simulationTime;
    if( !doesCarBalk(litresNeeded, pumpQLength)){
        if(pumpQLength == 0 && pumpDieselQLength == 0){
            totalEmptyQueueTime += simulationTime;
        }
        if(needDiesel || (pumpDieselQLength < pumpQLength)){
            pumpDiesel->reserve();
            pumpDieselQLength = pumpDiesel->qlength();
            if(pumpDieselQLength > maxPumpDieselQLength){
                maxPumpDieselQLength = pumpDieselQLength;
            }
            hold(uniform(0, serviceStream));
            pumpDiesel -> release();
            if(pumpQLength == 0 && pumpDieselQLength == 0){
                totalEmptyQueueTime -= simulationTime;
            }
        }
        else{
            pumpMachine ->reserve();
            pumpQLength = pumpMachine->qlength();
            if(pumpQLength > maxPumpQLength){
                maxPumpQLength = pumpQLength;
            }
            for(int i = 0; i < numPumps; ++i){
                if(pumpMachine[i].status() != BUSY){
                    pumpMachine[i].status() == BUSY;
                    hold(uniform(0, serviceStream));
                    pumpMachine->release();
                    if(pumpQLength == 0 && pumpDieselQLength == 0){
                         totalEmptyQueueTime -= simulationTime;
                    }
                    pumpMachine[i].status() == FREE;
                    break;

                }
            }
        }
        double waitTime = clock - arrTime;
        TotalLitresSold += litresNeeded;
        customersServed++;
        TotalWaitingTime += waitTime;
        TotalServiceTime += serviceTime(litresNeeded);
    }
    else{
        balkingCustomers ++;
        TotalLitresMissed += litresNeeded;
    }
}

void printStatus(){
    create("stats");
    while(clock <= endingTime){
        hold(ReportInterval);
        double simulationTime = clock; 
        double maxSize = 0.0;
        if(maxPumpQLength < maxPumpDieselQLength){
            maxSize = maxPumpDieselQLength;
        }
        else{   
            maxSize = maxPumpQLength;
        }
         printf("%8.0f%7i", simulationTime, totalArrival);
         printf("%8.3f",  totalEmptyQueueTime/simulationTime);
         if(totalArrival > 0){
            printf("%9.3f%8.3f", simulationTime/totalArrival,
            (TotalLitresSold + TotalLitresMissed) / totalArrival);
         }
        else{
            printf ("%9s%8s", "Unknown", "Unknown");
        }
        printf ("%7i", balkingCustomers);
         if (customersServed > 0)
            printf ("%9.3f", TotalWaitingTime / customersServed);
        else
            printf ("%9s", "Unknown");
        printf ("%7.3f", TotalServiceTime/ (numPumps * simulationTime));
        printf ("%9.2f", TotalLitresSold * profit- cost * numPumps);
        printf ("%7.2f", TotalLitresMissed * profit);
        printf("%5i\n", maxSize);


    }

}

