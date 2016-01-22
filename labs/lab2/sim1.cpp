//Modified by Shilpa Chirackel, 861060176
// CSC 270 simulation example
// Adapted March 1996 by J. Clarke from Turing original by M. Molle

// This version edited slightly for the fall 1997 course notes.

// What system are we on?
//
// The system matters only because random-number generating functions
// tend to be system-dependent. To set the system, comment out the
// irrelevant choices and make sure the one you're using is not
// commented out. (A sensible alternative would be to comment out
// all the system #defines and use a compiler flag instead.)
//
// If your system is not mentioned here, look up its random number
// generator and add it to the list. The GNU random number generators
// are certainly preferable to the TURBO (Turbo C++) ones.
#define GNU
//#define TURBO

//#include <iostream.h>	//modified: excluded since this header file is depreciated
#include <iostream>		//added
//#include <iomanip.h>	//modified: excluded since this header file si depreciated
#include <iomanip>		//added
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <queue>		//added for STL queue

#define BOOLEAN int
#define TRUE 1
#define FALSE 0

using namespace std;	//added 

//-----------------------------------------------------------------------
// The time, a "keep going" control, and some constants of the model.
//
// There are several other global variables. Should we use them?
// Could we get rid of them? Where would they appear if we broke the
// program up into several separately-compiled modules? We certainly
// ought to do that!
//-----------------------------------------------------------------------

double simulationTime; // A major global variable: the current time.
BOOLEAN simulating; // Are we still going?

const double profit = .025; // dollars per litre
const double cost = 20;
// the incremental cost in dollars to operate one pump for a day

//-----------------------------------------------------------------------
// Generating random numbers.
//-----------------------------------------------------------------------

// Prototypes for system-provided random number functions.
#if defined(GNU)
double erand48(unsigned short xsubi[3]);
#elif defined(TURBO)
int rand ();
#endif

class randStream {
// Manage one stream of random numbers.
//
// "nextrand" gives the floating-point number obtained from a
// new value in this stream. "xsubi" stores that latest value.
private:
	unsigned short xsubi[3];

public:
	randStream (unsigned short seed = 0);
	double nextrand ();
};

randStream * arrivalStream; // auto arrival times
randStream * litreStream; // number of litres needed
randStream * balkingStream; // balking probability
randStream * serviceStream; // service times

double randStream::nextrand ()
// Returns the next random number in this stream.
{
#if defined(GNU)
	return erand48 (xsubi);
#elif defined(TURBO)
	return ((double) rand())/((double) RAND_MAX);
#endif
}

randStream::randStream (unsigned short seed)
// Initialize the stream of random numbers.
{
#if defined(GNU)
	xsubi[0] = 0;
	xsubi[1] = 0;
	xsubi[2] = seed;
#elif defined(TURBO)
	// Turbo C doesn't provide a random-number generator with
	// explicit seeds. We will get along with just one stream.
	return;
#endif
}

//-----------------------------------------------------------------------
// Events
//
// Remember that events are not entities in the same sense as cars and
// pumps are, and the event queue does not have the same reality as the
// car queue. The event queue is a data structure without a real-world
// equivalent, while the car queue is real and you can see it. Events are
// not quite so imaginary, but they are certainly less "real" than cars.
//-----------------------------------------------------------------------

//----------------------------------------------------------------------
// Events--the general class on which the specific event types are based
//----------------------------------------------------------------------

class eventClass {
private:
	double localTime;

public:
	eventClass (double T = 0.0) {localTime = T;};
	inline double whatTime () {return localTime;}; // when it happens
	inline void setTime (const double newTime = 0) {localTime = newTime;};
	virtual void makeItHappen () {}; // the event routine
};

//----------------------------------------------------------------------
// Add a comparator struct for priority queue
//----------------------------------------------------------------------
struct CompareTime
{
	bool operator()(eventClass * e1, eventClass *e2){
		return e1->whatTime() > e2->whatTime();
	}

};

//--------------------------------------------------
// The event list and utilities for manipulating it.
//--------------------------------------------------

class eventListClass {
private:
	priority_queue<eventClass *, vector<eventClass *>, CompareTime> q1;	//added 

	/*struct eventListItem {
		eventClass * data;
		eventListItem * next;
	};
	eventListItem * firstEvent;*/

public:
	eventListClass () {/*firstEvent = NULL;*/};
	void insert (eventClass * event);
	eventClass * getNext ();
};

eventListClass * eventList;

void eventListClass::insert (eventClass * event)
{
	/*eventListItem * e = new eventListItem;
	e -> data = event;

	const double T = event -> whatTime();
	if (firstEvent == NULL || T < firstEvent -> data -> whatTime()) {
		e -> next = firstEvent;
		firstEvent = e;
	}
	else {
		eventListItem * behind = firstEvent;
		eventListItem * ahead = firstEvent -> next;
		while (ahead != NULL && ahead -> data -> whatTime() <= T) {
			behind = ahead;
			ahead = ahead -> next;
		}
		behind -> next = e;
		e -> next = ahead;
	}*/
	q1.push(event);	//added
}

eventClass * eventListClass::getNext ()
{
	// pre firstEvent not= NULL

	/*if (firstEvent == NULL) {
		cout << "Error! ran out of events\n";
		return NULL;
	}

	eventClass * eventToReturn = firstEvent -> data;

	eventListItem * restOfList = firstEvent -> next;
	delete firstEvent;
	firstEvent = restOfList;

	return eventToReturn;*/
	//added
	if(q1.empty()){
		cout << "Error! ran out of events\n";
		return NULL;
	}
	else{
		eventClass * temp = q1.top();
		q1.pop();
		return temp;
	}
}

//---------------------------------------------------------------------------
// Entities: cars and pumps
//---------------------------------------------------------------------------

class carClass {
private:
	double localArrivalTime;
	double localLitresNeeded;

public:
	carClass();
	void setTime (double T) {localArrivalTime = T;};
	double arrivalTime () {return localArrivalTime;};
	double litresNeeded () {return localLitresNeeded;};
};

carClass::carClass ()
// The number of litres required is a property of a car, so it
// belongs in this class. It is also something the car "knows"
// when it arrives, so it should be calculated in the constructor.
//
// The distribution of litres required is uniform between 10 and 60.
{
	localLitresNeeded = 10 + litreStream -> nextrand() * 50;
};

class pumpClass {
private:
	carClass * localCarInService;
	double serviceTime ();

public:
	carClass * carInService () {return localCarInService;};
	void startService (carClass *);
};

double pumpClass::serviceTime()
// Determine how long the service will take. This is a property of the
// pump-car combination.
//
// Service times will have a near Normal distribution, where the mean is 150
// seconds plus 1/2 second per litre, and the standard deviation is 30 seconds.
// (It can be shown that the sum of 12 random numbers from the
// uniform distribution on (0,1) has a near Normal distribution
// with mean 6 and standard deviation 1.)
{
	if (carInService() == NULL) {
		cout << "Error! no car in service when expected\n";
		return -1.0;
	}

	double howLong = -6;

	for (int i = 1; i <= 12; i++)
		howLong += serviceStream -> nextrand();

	return 150 + 0.5 * localCarInService -> litresNeeded() + 30 * howLong;
}

//----------------------------------------
// The actual pumps at the service station
//----------------------------------------

class pumpStandClass {
private:
	typedef pumpClass * pumpListItem;
	pumpListItem * pumps; // A stack of pumps
	int numPumps;
	int topPump;

public:
	pumpStandClass (int);
	int howManyPumps () {return numPumps;}; // needed for statistics
	BOOLEAN existsAvailablePump () {return topPump >= 0;};
	pumpClass * getAvailablePump ();
	void releasePump (pumpClass *&);
};

pumpStandClass * pumpStand;

pumpStandClass::pumpStandClass (int N)
// Create set of available pumps and put all N pumps into it.
{
	if (N < 1) {
		cout << "Error! pump stand needs more than 0 pumps\n";
		return;
	}

	numPumps = N;
	pumps = new pumpListItem [N];
	topPump = N - 1;

	for (int j = 0; j < N; j++)
		pumps[j] = new pumpClass;
}

pumpClass * pumpStandClass::getAvailablePump ()
// Take a pump from the set of free pumps, and return a pointer to it.
{
	if (topPump < 0) {
		cout << "Error! no pump available when needed\n";
		return NULL;
	}

	return pumps[topPump--];
}

void pumpStandClass::releasePump (pumpClass *& P)
// Put pump P back on the available list.
// P is set to NULL, just to cause trouble if the caller tries
// to reuse this pump, which is now officially free.
{
	if (topPump >= numPumps) {
		cout << "Error! attempt to release a free pump\n";
		return;
	}

	pumps[++topPump] = P;
	P = NULL;
}

//---------------------------------------------------------
// Queue for waiting cars and utilities for manipulating it
//---------------------------------------------------------

class carQueueClass {
private:
	queue <carClass *> q2;
	/*struct queueItem {
		carClass * data;
		queueItem * next;
	};

	queueItem * firstWaitingCar, * lastWaitingCar;
	int localQueueSize;*/
	double totalEmptyQueueTime;
	int maxQueueSize;	//added to compute max queue size

public:
	carQueueClass ();
	int queueSize ();
	double emptyTime ();
	void insert (carClass * newestCar);
	carClass * getNext ();
	int maxSizeQueue () {return maxQueueSize;};	//added to return max queue size
};

carQueueClass * carQueue;

carQueueClass::carQueueClass ()
{
	/*firstWaitingCar = NULL;
	lastWaitingCar = NULL;
	localQueueSize = 0;*/
	totalEmptyQueueTime = 0.0;
	maxQueueSize = 0;
}

int carQueueClass::queueSize ()
{
	return q2.size();
	//return localQueueSize;
}

double carQueueClass::emptyTime ()
{
	if (/*localQueueSize > 0*/ !q2.empty())
		return totalEmptyQueueTime;
	else
		return totalEmptyQueueTime + simulationTime;
}

void carQueueClass::insert (carClass * newestCar)
{
	/*queueItem * newQueueItem;
	newQueueItem = new queueItem;
	newQueueItem -> data = newestCar;
	newQueueItem -> next = NULL;

	if (lastWaitingCar == NULL) {
		// assert queueSize = 0
		firstWaitingCar = newQueueItem;
		totalEmptyQueueTime += simulationTime;
	}
	else {
		// assert queueSize > 0
		lastWaitingCar -> next = newQueueItem;
	}

	lastWaitingCar = newQueueItem;
	localQueueSize += 1;*/
	if(q2.empty()){
		q2.push(newestCar);
		totalEmptyQueueTime += simulationTime;
	}
	else{
		q2.push(newestCar);
	}
	if(q2.size() > maxQueueSize){
		maxQueueSize = q2.size();
	}
}

carClass * carQueueClass::getNext ()
{
	// pre queueSize > 0 and firstWaitingCar not= NULL

	/*if (firstWaitingCar == NULL) {
		cout << "Error! car queue unexpectedly empty\n";
		return NULL;
	}

	carClass * carToReturn = firstWaitingCar -> data;

	localQueueSize -= 1;
	queueItem * restOfList = firstWaitingCar -> next;
	delete firstWaitingCar;
	firstWaitingCar = restOfList;

	if (firstWaitingCar == NULL) {
		// empty queue: update the pointer to the end of queue too!
		lastWaitingCar = NULL;
		totalEmptyQueueTime -= simulationTime;
	}

	return carToReturn;*/

	if(q2.empty()){
		cout << "Error! car queue unexpectedly empty\n";
		return NULL;
	}

	carClass * temp = q2.front();
	q2.pop();
	if(q2.empty()){
		totalEmptyQueueTime -= simulationTime;
	}
	return temp;

}

//-------------------------------------
// Collecting and displaying statistics
//-------------------------------------

class statsClass {
private:
	int TotalArrivals, customersServed, balkingCustomers;
	double TotalLitresSold, TotalLitresMissed, TotalWaitingTime,
		TotalServiceTime;

public:
	statsClass ();
	void countArrival () {TotalArrivals += 1;};
	void accumSale (double litres);
	void accumBalk (double litres);
	void accumWaitingTime (double interval) {TotalWaitingTime += interval;};
	void accumServiceTime (double interval) {TotalServiceTime += interval;};
	void snapshot ();
};

statsClass * stats;

statsClass::statsClass ()
{
	TotalArrivals = 0;
	customersServed = 0;
	balkingCustomers = 0;
	TotalLitresSold = 0.0;
	TotalLitresMissed = 0.0;
	TotalWaitingTime = 0.0;
	TotalServiceTime = 0.0;
}

void statsClass::accumSale (double litres)
{
	customersServed += 1;
	TotalLitresSold += litres;
}

void statsClass::accumBalk (double litres)
{
	balkingCustomers += 1;
	TotalLitresMissed += litres;
}

void statsClass::snapshot ()
{
	printf("%8.0f%7i", simulationTime, TotalArrivals);
	printf("%8.3f", carQueue -> emptyTime()/simulationTime);
	if (TotalArrivals > 0) {
		printf("%9.3f%8.3f", simulationTime/TotalArrivals,
			(TotalLitresSold + TotalLitresMissed) / TotalArrivals);
	}
	else
		printf ("%9s%8s", "Unknown", "Unknown");

	printf ("%7i", balkingCustomers);
	if (customersServed > 0)
		printf ("%9.3f", TotalWaitingTime / customersServed);
	else
		printf ("%9s", "Unknown");
	printf ("%7.3f", TotalServiceTime
		/ (pumpStand -> howManyPumps() * simulationTime));
	printf ("%9.2f", TotalLitresSold * profit
		- cost * pumpStand -> howManyPumps());
	printf ("%7.2f", TotalLitresMissed * profit);
	printf ("%7i\n", carQueue->maxSizeQueue()); //added to print maxSizeQueue
}

//-----------------------------------------
// Classes for the specific kinds of events
//-----------------------------------------

class arrivalClass : public eventClass {
private:
	double interarrivalTime();
	BOOLEAN DoesCarBalk (double litres, int queueLength);

public:
	arrivalClass (double T) : eventClass (T) {};
	void makeItHappen ();
};

double arrivalClass::interarrivalTime ()
// The interarrival time of the next car is exponentially distributed.
{
	const int MEAN = 50;
	// seconds, because r is a rate in 1/sec, and this is 1/r
	return - MEAN * log (arrivalStream -> nextrand());
}

BOOLEAN arrivalClass::DoesCarBalk (double litres, int queueLength)
// Deciding whether to balk is an activity that forms part of the
// arrival event, so this procedure belongs among the event routines.
//
// The probability that a car leaves without buying gas (i.e., balks)
// grows larger as the queue length gets larger, and grows smaller
// when the car requries a greater number of litres of gas, such that:
// (1) there is no balking if the queueLength is zero, and
// (2) otherwise, the probability of *not* balking is
//       (40 + litres)/(25 * (3 + queueLength))
{
	return queueLength > 0 && balkingStream -> nextrand()
		> (40 + litres) / (25 * (3 + queueLength));
}

class departureClass : public eventClass {
private:
	pumpClass * whichPump;

public:
	departureClass (double T) : eventClass (T) {};
	void setPump (pumpClass * p) {whichPump = p;};
	void makeItHappen ();
};

class reportClass : public eventClass {
private:
	double intervalToNext; // time between regular reports

public:
	reportClass (double T) : eventClass (T) {};
	void setInterval (double interval) {intervalToNext = interval;};
	void makeItHappen ();
};

void reportClass::makeItHappen ()
{
	stats -> snapshot ();

	// schedule the next interim report.
	setTime (simulationTime + intervalToNext);
	eventList -> insert (this);
}
 
class allDoneClass : public eventClass {
public:
	allDoneClass (double T) : eventClass (T) {};
	void makeItHappen () {stats -> snapshot (); simulating = FALSE;};
};

//---------------
// Event routines
//---------------
    
void arrivalClass::makeItHappen ()
// The car arrival event routine
{
	// Create and initialize a new auto record
	carClass * arrivingCar = new carClass;
	stats -> countArrival ();
	const double L = arrivingCar -> litresNeeded();
	if (DoesCarBalk (L, carQueue->queueSize())) {
		stats -> accumBalk (L);
		delete arrivingCar;
	}
	else {
		arrivingCar -> setTime (simulationTime);
		if (pumpStand -> existsAvailablePump()) {
			pumpClass * Pump = pumpStand -> getAvailablePump ();
			Pump -> startService (arrivingCar);
		}
		else
			carQueue -> insert (arrivingCar);
	}

	// schedule the next arrival, reusing the current event object
	setTime (simulationTime + interarrivalTime());
	eventList -> insert (this);
}

void departureClass::makeItHappen ()
// The departure event routine
{
	// pre whichPump not= NULL and whichPump -> carInService not= NULL

	// Identify the departing car and collect statistics.
	carClass * departingCar = whichPump -> carInService();
	stats -> accumSale (departingCar -> litresNeeded());

	// The car vanishes and the pump is free; can we serve another car?
	delete departingCar;
	if (carQueue->queueSize() > 0)
		whichPump -> startService (carQueue -> getNext());
	else
		pumpStand -> releasePump (whichPump);

	// This departure event is all done.
	delete this;
}

void pumpClass::startService (carClass * Car)
// The start-of-service event routine:
// Connect the Car to this pump, and determine when the service will stop.
{
	// pre pumpStand -> existsAvailablePump();

	// Match the auto to an available pump
	localCarInService = Car;
	const double pumpTime = serviceTime ();

	// Collect statistics
	stats -> accumWaitingTime (simulationTime
		- localCarInService -> arrivalTime());
	stats -> accumServiceTime (pumpTime);

	// Schedule departure of car from this pump
	departureClass * Dep = new departureClass (simulationTime + pumpTime);
	Dep -> setPump (this);
	eventList -> insert (Dep);
}

int main()
{
	//---------------
	// Initialization
	//---------------

	simulationTime = 0.0;
	simulating = TRUE;

	double ReportInterval, endingTime;
	cin >> ReportInterval >> endingTime;

	int numPumps;
	cin >> numPumps;
	cout << "This simulation run uses " << numPumps << " pumps ";
	pumpStand = new pumpStandClass (numPumps);

	// Initialize the random-number streams.

	cout << "and the following random number seeds:\n";
	unsigned short seed;
	cin >> seed;
	cout << "         " << seed;
	arrivalStream = new randStream (seed);
	cin >> seed;
	cout << "         " << seed;
	litreStream = new randStream (seed);
	cin >> seed;
	cout << "         " << seed;
	balkingStream = new randStream (seed);
	cin >> seed;
	cout << "         " << seed;
	serviceStream = new randStream (seed);
	cout << "\n";

	// Create and initialize the event queue, the car queue,
	// and the statistical variables.

	eventList = new eventListClass;
	carQueue = new carQueueClass;
	stats = new statsClass;

	// Schedule the end-of-simulation and the first progress report.

	allDoneClass * lastEvent = new allDoneClass (endingTime);
	eventList -> insert (lastEvent);
	if (ReportInterval <= endingTime) {
		reportClass * nextReport = new reportClass (ReportInterval);
		nextReport -> setInterval (ReportInterval);
		eventList -> insert (nextReport);
	}

	// Schedule the first car to arrive at the start of the simulation

	arrivalClass * nextArrival = new arrivalClass (0);
		// Is 0 really the time for the first arrival?
	eventList -> insert (nextArrival);

	// Print column headings for periodic progress reports and final report

	printf ("%9s%7s%8s%9s%8s%7s%9s%7s%8s%7s%7s\n", " Current", "Total ",
		"NoQueue", "Car->Car", "Average", "Number", "Average", "Pump ",
		"Total", " Lost ", "Max");
	printf ("%9s%7s%8s%9s%8s%7s%9s%7s%8s%7s%7s\n", "   Time ", "Cars ",
		"Fraction", "  Time  ", " Litres ", "Balked", "  Wait ",
		"Usage ", "Profit", "Profit", "Size");
	for (int i = 0; i < 87; i++)
		cout << "-";
	cout << "\n";

	//------------------------
	// The "clock driver" loop
	//------------------------

	while (simulating) {
		eventClass * currentEvent = eventList -> getNext ();
		simulationTime = currentEvent -> whatTime();
		currentEvent -> makeItHappen();
	}

	return 0;
}