   SensorTux

   Copyright (C) 2009-2010:
         Thomas Lars Widmer and Daniel Roggen
         Wearable Computing Laboratory, ETH Zurich
         

What is it
----------

This software is part of an educational and research kit for activity recognition using on-body sensors:
	D. Roggen, M. B�chlin, J. Schumm, T. Holleczek, C. Lombriser, G. Tr�ster, L. Widmer, D. Majoe, J. Gutknecht. 
	"An educational and research kit for activity and context recognition from on-body sensors", 
	Proc. IEEE Int. Conf. on Body Sensor Networks (BSN). 277--282, 2010



License
-------

This software is licensed under the terms of the GPL v.2 license.


README SensorTux
================

Dear User

No installation of the game itself is needed. 
Just compile and run ./src/supertux. 

But some packages are mandatory to compile supertux. 
Run ./installpackages.sh in order to automatically install those I found to be required. 
Afterwards run ./configure - it should run through without an error. 
At the end it displays a summary what is supported, like: 

Features:
=========
 Profile Mode:   no
 Debug Mode:     no
 OpenGL Support: yes

If there are still some packages missing use apt-get install or synaptic to install them. 

Now connect the sensor using the bluetooth-connection-wizard of your desktop. 
Use the code 0005. 
This has only to be done once per computer. 

Afterwards run prepare.sh to configure the connection. 
prepare.sh is needed everytime after you switched of your computer or one of the sensors. 

Now run compile.sh
Finally the following executables should be ready in the src folder: 
- printrawsensors 
- configureclassification
- recordgestures 
- testclassification 
- supertux 

printrawsensors:
================
The simple program printrawsensors shows you the data coming from the sensors. 
If this works fine, you know that prepare.sh has been successful in connecting the sensors. 

configureclassification:
========================
With configureclassification you can adapt various project settings. 
For a start the default settings should work fine. 

Configure is a generic tool to set up, show, read and change XML based configuration files. 
For the application at hand it offers the following options: 
- Hardware ID numbers of the used sensors
- On which position in the sensor data the ID can be found
- The number of takes when recording a single gesture
- The indices of the sensor data which should be used to extract features
- The window size to do the energy based segmentation
- The verbosity of the software
- The filename where the sensor activity information is stored
- The format string to extract the sensor data with the frame parser
- The files on disk representing the sensors in a Linux OS. 
- The names of the classification results (e.g. "jump", "fire", ...)
- If mean should be used as a feature
- In how many chunks the time window of a gesture should be cut for the mean
- If standard deviation should be used as a feature
- In how many chunks the time window of a gesture should be cut for the standard deviation
- If mean crossing rate should be used as a feature
- In how many chunks the time window of a gesture should be cut for the mean crossing rate
- If the gesture length should be considered when classifying gestures captured with energy based segmentation
- How many percent is the tolerance when checking the gesture length
- How many percent of the biggest distance in feature space between two cluster centers should be the threshold for outliners
- Should continous classification be used (no energy based segmentation for classification)

recordgestures: 
===============
The program recordgestures  is used to record training gestures and store them to XML files. 
After recording it also allows to automatically determine which sensor is used for which gesture. 
So every gesture uses only one sensor. 
We came up with this idea to allow more than one classification in parallel. 
This is mandatory for playing a game by gestures. 
As an example it should be possible to walk an fire at the same time. 

The activity of the sensors gets stored in another XML file. 
Beside automatic recognition the activity can also be set by hand. 
In some cases it might come handy to store a single pose as a gesture. 
Because a pose doesn't include any activity the automatic recognition will fail and the data has to be set by hand. 

Finally a test mode for the classification can be run in order to see from the 
console output if the right classifications are established. 

testclassification:
===================
This application offers the test mode for the classification can be run in order 
to see from the console output if the right classifications are established. 

supertux:
=========
Given a complete set of XML files the classification can start without further interaction, all is stored persitently. 
Therefore SuperTux can be ran without a special interface. 
Gesture recognition within SuperTux can automatically be used to control the penguin inside of the levels. 

Used XML-Files:
===============
The configuration for the whole software is stored in a file named src/config.xml. 

Another XML file contains the activity settings. 
Activity means to map the gestures to the sensors. 
Each gesture can only use one sensor. 

Further XML files store the recorded training data. 
For each take a single file gets stored. 

Class Diagram:
==============
GestureControl is sort of a main class. 
It combines the sensing and learning algorithm to the desired functionality. 
Configuration allows to configure the software by a file. 
The Sensor class encapsulates the handling of a single sensor. 
If more than one sensor from a single source is desired SensorFusion can be used instead of Sensor. 
Other alternatives are DataExtension and FileData. 
DataExtension adds two more sensor measurements to the existing ones. 
In DataExtension the two existing gyroscope measurements get centered around zero and amplified. 
FileData allows to exchange the live sensor data by data from a file. 
FileData gets used to read the gestures from files in order to train the learning algorithm. 
Buffer is used to keep a queue of recent data for each sensor.  
FeatureExtraction extracts features from the buffer which can be fed to a learning algorithm like NCC. 

Learning Algorithm:
===================
In the current version the NCC algorithm gets used. 
NCC stands for Nearest Cluster Center. 
For every gesture there is a cluster center in feature space. 
Features from a live gesture are compared and the nearest cluster center determines the classification. 
To avoid matching outliners we use a threshold distance. 
If a gestures is matched too far away from any trained gesture, it is classified as outliner. 

Features: 
=========
Every value in the supplied sensor data can be used as a feature source. 
As features we can choose among mean, standard deviation and mean crossing rate. 
For each of the three we can seperatly choose how many time chunks of them we want to consider within a single gesture. 

Segmentation:
=============
Segmentation is used to determine the start and end of a gesture. 
For training energy based segmentation or manual input can be used. 
Energy based segmentation starts the recording as soon as the sensor gets moved and stops it afterwards. 
For still gestures (poses) alternatively the space key can be used to start and stop the recording. 

When playing energy based segmentation can be used as well. 
But in this case every gesture has to be kept as a single instance followed by a proper rest state of the used sensor. 
This might be really difficult when playing a (more or less) fast game like SuperTux. 
To overcome this continous recognition can be used. 
Continous recognition assumes the live gestures to have the same length as the trained ones. 
Doing so it continously checks the buffer of sensor data if a match can be found. 

Adding Source Files:
====================
If you happen to add source files to the project, you need to mention them in the src/Makefile.am.
Note that Makefile.am has a separate list for each of the final executables. 
After modifying src/Makefile.am you have to run the script installnew.sh in order to generate adapted Makefiles for the project. 

Platform:
=========
The very most of the sources shouldn't only compile under Linux. 
With MinGW it should possible to run it all under Windows. 
Nevertheless the sources have yet only been tested in Ubuntu Linux 9.10. 

Special Thanks to: 
==================
Dr. Daniel Roggen
Dr. Dennis Majoe

Finally:
========
Have a lot of fun! 

Contact:
========
Lars Widmer
lars.widmer@inf.ethz.ch
+41 44 632 96 87
www.lawi.ch

