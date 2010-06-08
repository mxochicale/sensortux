/**
 * TestClassification.cpp
 * Very simple application. 
 * It uses the same interface as superTux. 
 * But the control events just trigger a screen output. 
 * Use this program to test the classification and post processing. 
 * TestClassification uses TuxControlSingleton to get the ClassificationTux object. 
 * ClassificationTux evaluates the results from the classification and does some post processing. 
 */

#include "../src/Helper.h"
#include "../src/TuxControlSingleton.h"

#include <iostream>

using namespace std;

int main (int argc, char *argv[]) {
	cout << "Start Test" << endl;
	while (!Helper::kbhit()) {
		usleep(10000);
		ControlEvent controlEvent = TuxControlSingleton::getInstance()->removeControlEvent();
		if (controlEvent.valid)	{
			cout << "Key " << controlEvent.key << "; Event " << controlEvent.event << endl;
		}
	}
	Helper::getch();
	cout << "Ende Test" << endl;
	return 0;
}



