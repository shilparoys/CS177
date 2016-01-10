#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <iomanip>
#include <fstream>
using namespace std;

const double PI  =3.14159;
/*x^2 + y^2 = r^2*/
double circleEquaution(double x, double y){
	return sqrt(pow(x, 2) + pow(y, 2));
}
/*check if a pair of coordinates are within the boundary of the quarter-circle*/
bool in_circle(double& x, double&y){
	if(x < 0 || y < 0 || x > 1 || y > 1 ){
		return false;
	}
	double a = circleEquaution(x, y);
	if( a <= 1){
		return true;
	}
	else{
		return false;
	}

}
double estimate_pi(const int & n){
	double x, y;
	double counter = 0;
	for (int i = 0; i < n; i++){
		 x = (rand() % 10000 + 1) / 10000.0;
         y = (rand() % 10000 + 1) / 10000.0;
         if(in_circle(x,y)){
         	++counter;
         }
	}
	return (counter / n) * 4.0;
}
void print(){
	int n[8] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000};
	cout << "N 			Estimate 			Difference" << endl;
	double x, estimate;
	for (int i = 0; i < 8; i++){
		x = estimate_pi(n[i]);
		estimate = abs(PI - x);
		cout << n[i] << "		 	" << x << "					"<<estimate<<endl;

	}
}

double* pi_sequence(const int &n){
	/*double a[500];
	double x, y;
	int counter = 0;
	for(int i = 0; i < 500; i++){
		for(int j = 0; j < i; j++){
			x = estimate_pi(j);
			//a[counter++] = x;

    	}
	}*/
    static double a[500];
    int counter =0;
    double x;
    for(int i = 0; i < 500; i++){
    	x = estimate_pi(i);
    	a[counter++] = x;
    }
    return a;

}

void printCSV(double *a){
	ofstream myfile;
    myfile.open("test.csv");
    for(int i = 0; i < 500; i++){
    	myfile << *(a + i) << endl;
    }
    myfile.close();
}
int main(){
	srand(time(NULL));
	//print();
	double * x;
	for(int i = 0; i < 6; i++){
		cout << i << endl;
		x = pi_sequence(500);
		printCSV(x);
	}

	return 0;
}

