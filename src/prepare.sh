#!/bin/bash

# For Bluetooth acceleration and gyro
# Format: ID, count, Xcal, Ycal, Zcal, X, X45, Y, Y45, Vref

# Connect to an acceleration sensor, either bluetooth or usb
# parameter: ttyUSB or ttyRFCOMM of the device

# Put here the serial parameters: baudrate, flow control, etc.
# This is black magic.... 
# The full stty command is the equivalent of "raw"...
 
#stty -F $1 19200 -ignbrk -brkint -ignpar -parmrk -inpck -istrip -inlcr -igncr -icrnl  -ixon  -ixoff  -iuclc  -ixany -imaxbel -opost -isig -icanon -xcase min 1 time 0

reset

echo "----------------------------------------------------------------------------"
echo "Run this script after starting up your PC before using the sensors! "
echo ""
echo "1. Switch on the sensors"
echo "2. If it's the first time you use the sensors on this PC: "
echo "   Please connect the sensors via your GUI bluetooth menu."
echo "   Use the key 0005"
echo "3. Please type your super user password "
echo "   in order to bind the sensors an change the access rights "
echo "   on the sensor devices "
echo "----------------------------------------------------------------------------"

echo "Scanning for devices: "
hcitool scan
echo "done"
echo ""

echo "Before you enter the root password below:"
echo ""

echo "Binding the two sensors: "
sudo rfcomm bind 0 10:00:E8:9C:D3:B9
sudo rfcomm bind 1 10:00:E8:9C:EF:4D
echo "done"

echo "Sleeping for 2 seconds: "
sleep 2
echo "done"

echo "ls sensor devices: should show 2 lines"
ls /dev/rfcom*
echo "done"

echo "Changing access rights ..."
sudo chmod 777 /dev/rfcomm*
echo "... done"

echo "----------------------------------------------------------------------------"

# stty -F $1 19200 raw

echo "Setting baudrate and raw mode for the devices ..."
stty -F /dev/rfcomm0 38400 raw
stty -F /dev/rfcomm1 38400 raw
echo "... done"

echo "----------------------------------------------------------------------------"

echo "Display device settings ..."
echo "-- Sensor 0 --"
stty -F /dev/rfcomm0 
echo "-- Sensor 1 --"
stty -F /dev/rfcomm1 
echo "... done"

echo "----------------------------------------------------------------------------"




