/* GPS Control
 * This header file contains commands to interact with the GPS on a USB port. 
 * The functions in this file do the same basic tasks that a user can do 
 * from the command prompt to control the GPIO.
 * Contains the following functions:
 *  GPIOexport: sets up the GPIO pin so that it can be used. 
 *  GPIOunexport: closes the GPIO pin after use is done.
 *  GPIOdirection: sets whether the pin is an in or out pin.
 *  GPIOread: reads the status of the pin.
 *  GPIOwrite: sends a high or low signal out the GPIO.
 * 
 * Version 0.1
 * GCC: gcc -Wall -I../h -I/opt/vc/include -I/usr/include -o RPI_USBGPS.o
 */

#include "RPi_USBGPS.h"
#include "RPi_generror.h"
#include <string.h>

int RPi_USBGPSset_time(char USBdev){
	char path[30];
	char gpsbuffer[1024] = {0};
	char cmd[30];
	char time[9];
	char date[9];
	char *toks = NULL;
	
	snprintf(path, 30, "/dev/ttyUSB%d", USBdev);
	FILE *fstream = fopen(path, "r");
	while(!(fgets(gpsbuffer, 1024, fstream) == NULL)) {
		printf(".");
		if (gpsbuffer[0] == '$') {
			if (memcmp(gpsbuffer+1, "GPRMC", 5) == 0) break;
		}
	}
	toks = strtok(gpsbuffer, ","); //tokenize the string. First token is the GPS flag
	toks = strtok(NULL, ","); //next token is Zulu time. Process it!
	if (toks != NULL) {
		time[0] = toks[0];
		time[1] = toks[1];
		time[2] = ':';
		time[3] = toks[2];
		time[4] = toks[3];
		time[5] = ':';
		time[6] = toks[4];
		time[7] = toks[5];
		time[8] = '\0';
	}
	else return 0;
	
	toks = strtok(NULL, ","); //next token is vakuduty
	toks = strtok(NULL, ","); //next token is latitude may be of use later for first fix
	toks = strtok(NULL, ","); //next token is N/S
	toks = strtok(NULL, ","); //next token is longitude
	toks = strtok(NULL, ","); //next token is E/W
	toks = strtok(NULL, ","); //next token is knots
	toks = strtok(NULL, ","); //next token is course
	toks = strtok(NULL, ","); //next token is date
	if (toks != NULL) {
		date[0] = '2';
		date[1] = '0';
		date[2] = toks[4];
		date[3] = toks[5];
		date[4] = toks[2];
		date[5] = toks[3];
		date[6] = toks[0];
		date[7] = toks[1];
		date[8] = '\0';
	}
	else return 0;
	printf("\n==============SET==============\nDATE:\n");
	sprintf(cmd, "date +%%Y%%m%%d -s \"%s\"", date);
	system(cmd);
	printf("TIME:\n");
	sprintf(cmd, "date +%%H:%%M:%%S -s \"%s\"", time);
	system(cmd);
	printf("===============================\n");
	fclose(fstream);
	return 1;
}

void RPi_USBGPSread(char USBdev, struct GPSdata *GPS_dev) {
	char path[30];
	char gpsbuffer[1024] = {0};
	char *toks = NULL;
	
	snprintf(path, 30, "/dev/ttyUSB%d", USBdev);
	FILE *fstream = fopen(path, "r");
	
	if (fstream == NULL) {
		ge_warn(1, "RPI-_general.h, GPS", "Unable to open GPS port for reading! Retrying...");
		return;
	}
	
	while(!(fgets(gpsbuffer, 1024, fstream) == NULL)) {
		if (gpsbuffer[0] == '$') {
			if (memcmp(gpsbuffer+1, "GPGGA", 5) == 0) break;
		}
	}
	toks = strtok(gpsbuffer, ","); //tokenize the string. First token is the GPS flag
	toks = strtok(NULL, ","); //next token is time
	toks = strtok(NULL, ","); //next token is latitude
	if (toks != NULL) {
		(*GPS_dev).latitude_d = (int)(atoi(toks) / 100); //truncate the minutes off the degrees
		(*GPS_dev).latitude_m = (atof(toks) - (*GPS_dev).latitude_d * 100); //remove the degrees off the minutes
	}
	toks = strtok(NULL, ","); //next token is latitude direction char
	if (toks != NULL) (*GPS_dev).latitude_dir = toks[0];
	toks = strtok(NULL, ","); //next token is longitude
	if (toks != NULL) {
		(*GPS_dev).longitude_d = (int)(atoi(toks) / 100); //truncate the minutes off the degrees
		(*GPS_dev).longitude_m = (atof(toks) - (*GPS_dev).longitude_d * 100); //remove the degrees off the minutes
	}
	toks = strtok(NULL, ","); //next token is longitude direction char
	if (toks != NULL) (*GPS_dev).longitude_dir = toks[0];
	toks = strtok(NULL, ","); //next token is quality
	if (toks != NULL) (*GPS_dev).fix_quality = atoi(toks);
	toks = strtok(NULL, ","); //next token is number of satalites
	if (toks != NULL) (*GPS_dev).num_sats = atoi(toks);
	toks = strtok(NULL, ","); //next token is HDOP
	if (toks != NULL) (*GPS_dev).HDOP_acc = atoi(toks);
	toks = strtok(NULL, ","); //next token is Raw Altitude
	if (toks != NULL) (*GPS_dev).altitude_raw = atof(toks);
	toks = strtok(NULL, ","); //next token is Raw Altitude unit
	if (toks != NULL) (*GPS_dev).altitude_rawUNIT = toks[0];
	toks = strtok(NULL, ","); //next token is Geoidal Separation
	if (toks != NULL) (*GPS_dev).geoidal_sep = atof(toks);
	toks = strtok(NULL, ","); //next token is Geoidal separation unit
	if (toks != NULL) (*GPS_dev).geoidal_sepUNIT = toks[0];
	
	(*GPS_dev).actual_alt = ((*GPS_dev).altitude_raw + (*GPS_dev).geoidal_sep);
	printf("|| GPS1: LT:%dd%.2f'%c LG:%dd%.2f'%c AT:%.2f%c SAT:%d QUA:%d HDOP:%.2f\n", (*GPS_dev).latitude_d, (*GPS_dev).latitude_m, 
	(*GPS_dev).latitude_dir, (*GPS_dev).longitude_d, (*GPS_dev).longitude_m, (*GPS_dev).longitude_dir, (*GPS_dev).actual_alt, 
	(*GPS_dev).altitude_rawUNIT, (*GPS_dev).num_sats, (*GPS_dev).fix_quality, (*GPS_dev).HDOP_acc);
	fclose(fstream);
}
