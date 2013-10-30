/*
 * main.c 
 * 
 * Program contains main methods for RasPi operations. At the moment this is nothing but a testing ground.
 * 
 * Depends on: libjpeg8-dev, and an openVG api included in this set: git://github.com/ajstarks/openvg
 * Compile: gcc -Wall -I../h -I../lib/openvg -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/usr/include -c "%f" ../lib/openvg/libshapes.o ../lib/openvg/oglinit.o -L/opt/vc/lib -lGLESv2 -ljpeg
 * Build: gcc -Wall -I../h -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I../lib/openvg -I/usr/include -o "%e" "%f" ../lib/openvg/libshapes.o ../lib/openvg/oglinit.o -L/opt/vc/lib -lGLESv2 -ljpeg
 * 
 */
#include <stdlib.h>
#include "RPi_general.h"

#define GPS_USBID 0
struct Thermistor thermistor1, thermistor2;
struct Humistor humistor1;
struct GPSdata GPS_1;
long record_count;

int main() {	
	RPi_construct
	(GPS_USBID); //general function for starting tasks
	do {
		printf("\nRECORD: %ld\n", record_count);
		/**********************************************************************************/
		/**************** READ THE DATA ***************************************************/
		/**********************************************************************************/
		RPi_USBGPSread(GPS_USBID, &GPS_1); //obtain the GPS fix
		RPi_ADCread_sensors(&thermistor1, &thermistor2, &humistor1);
		
		/**********************************************************************************/
		/**************** OUTPUT THE DATA *************************************************/
		/**********************************************************************************/
		printf("T1: %.2fC   T2: %.2fC\n", thermistor1.temperature, thermistor2.temperature);
		
		/**********************************************************************************/
		/**************** FINAL STEPS *****************************************************/
		/**********************************************************************************/
		sleep(2);
		record_count++;
		
	} while (!kbhit());
	
	RPi_destruct(); //general function for ending tasks
	usleep(200000);
	exit(0);
}
 
