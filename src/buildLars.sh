reset
echo "removing objects"
rm FrameParser2.o Helper.o IntSource.o LearningAlgorithm.o Mutex.o NearestClusterCenter.o PrintRawSensors.o RecordGestures.o RoggenBuffer.o RoggenDataExtension.o RoggenFeatureExtraction.o RoggenSensorFusion.o RoggenSensor.o TestClassification.o TuxControl.o TuxControlSingleton.o XmlFileHandling.o ClassificationTux.o Thread.o Configuration.o 
echo "removing executables"
rm printrawsensors
rm printrawsensors2
echo "building objects"
g++ -c FrameParser2.cpp Helper.cpp IntSource.cpp LearningAlgorithm.cpp Mutex.cpp NearestClusterCenter.cpp PrintRawSensors.cpp RecordGestures.cpp RoggenBuffer.cpp RoggenDataExtension.cpp RoggenFeatureExtraction.cpp RoggenSensorFusion.cpp RoggenSensor.cpp TestClassification.cpp TuxControl.cpp TuxControlSingleton.cpp XmlFileHandling.cpp ClassificationTux.cpp Thread.cpp Configuration.cpp
echo "linking applicaton"
g++ -lpthread FrameParser2.o Helper.o IntSource.o LearningAlgorithm.o Mutex.o NearestClusterCenter.o PrintRawSensors.o RoggenBuffer.o RoggenDataExtension.o RoggenFeatureExtraction.o RoggenSensorFusion.o RoggenSensor.o TuxControl.o TuxControlSingleton.o XmlFileHandling.o ClassificationTux.o Thread.o Configuration.o ../tinyxml/*.o -o printrawsensors2 
echo "done"




