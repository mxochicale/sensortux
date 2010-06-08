/**
 * Program to prepare everything for the classification. 
 * SuperTux uses the classification but it's not able to record training data. 
 * RecordGestures is used to capture training data. 
 * Afterwards the activity has to be determined. 
 * Finally the recognition can be tested with a simple command line output. 
 */ 

#include "../src/GestureManagement.h"

//-------------------------------------------------------------------------------------------------
// main
//-------------------------------------------------------------------------------------------------
int main(int argc,char **argv) {
	GestureManagement * gm = new GestureManagement();
	gm->manageGestures();
	safeDelete(gm);
	return 0;
}
