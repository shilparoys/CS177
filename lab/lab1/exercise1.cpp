#include <iostream>
#include <cmath>
#include <stdlib.h>
using namespace std;

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
int main(){
	srand(time(NULL));
	double x, y;
	x = (rand() % 10000 + 1)/10000;
	y = (rand() % 10000 + 1)/10000;
	cout << x << " " << y << endl;
	if(in_circle(x,y)){
		cout << "Trune\n";
	}
	else{
		cout << "False\n";
	}

	return 0;
}