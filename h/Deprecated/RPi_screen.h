/* Screen Control Header
 * Screen functions, macros etc.
 * Applies openVG, uses an API developed by Anthony Starks.
 * API available from: https://github.com/ajstarks/openvg
 * oglint.c has been modified as there as an issue with the resolution it was returning.
 */
#include <stdlib.h>
#include "RPi_generror.h"
#include <stdio.h>
#include <unistd.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "fontinfo.h"
#include "shapes.h"

#define SCALE_X 32
#define SCALE_Y 32
#define BARSCALE_X 150

struct ScreenData {
	int width;
	int height;
	int graph_display_Ox;
	int graph_display_Oy;
	VGfloat Data1_Yposi[200];
	VGfloat Data3_Yposi[200];
};

void RPi_GenerateScreen(struct ScreenData *Screen_Main, int records, struct Thermistor *thermistor1, struct Thermistor *thermistor2, struct GasSensor *MQ7_1) {
	int i;
	char fstr[30];
	VGfloat X_Coords[200];
	if (records >= 200) records = 199;
	
	for (i =0 ; i < records; i++) {
		X_Coords[i] = (*Screen_Main).width - SCALE_X - BARSCALE_X - (i * 3);
	}
	for (i = records; (i > 0); i--) { //shift the data in records by 1
		(*Screen_Main).Data1_Yposi[i] = (*Screen_Main).Data1_Yposi[i-1];
		(*Screen_Main).Data3_Yposi[i] = (*Screen_Main).Data3_Yposi[i-1];
	}
	//record new data
	(*Screen_Main).Data1_Yposi[0] = (((*thermistor1).temperature + (*thermistor2).temperature) / 2.0 ) * 5 + SCALE_Y + 150;
	(*Screen_Main).Data3_Yposi[0] = 1/(*MQ7_1).ratio * 100 + SCALE_Y + 30;
		
	Start((*Screen_Main).width, (*Screen_Main).height);                   // Start the picture 720*480 but actual is 656x416 32 pixel padding?
	Background(0, 0, 0);                    // Black background
	StrokeWidth(3);
	
	//graph display
	(*Screen_Main).graph_display_Ox = (*Screen_Main).width - SCALE_X - BARSCALE_X;
	(*Screen_Main).graph_display_Oy = SCALE_Y + 20;
	//cheap chinese displays cannot show lines. Odd. Anything smaller than width 3 just chnages colour
	Stroke(255,255,255, 1);
	Line((*Screen_Main).graph_display_Ox, (*Screen_Main).graph_display_Oy, (*Screen_Main).graph_display_Ox, (*Screen_Main).height - SCALE_Y + 5);
	Line((*Screen_Main).graph_display_Ox, (*Screen_Main).graph_display_Oy, SCALE_X + 10, (*Screen_Main).graph_display_Oy);
	
	Stroke(255,0,0, 1);
	Polyline(X_Coords, (*Screen_Main).Data1_Yposi, records);
	Stroke(0,255,0, 1);
	Polyline(X_Coords, (*Screen_Main).Data3_Yposi, records);
	
	Fill(255, 0, 0, 1);
	sprintf(fstr, "%.1fC", ((*thermistor1).temperature + (*thermistor2).temperature) / 2);
	Text((*Screen_Main).graph_display_Ox + 10, (*Screen_Main).Data1_Yposi[0], fstr, SansTypeface, 15);
	Fill(0, 255, 0, 1);
	sprintf(fstr, "%.1fPPM", (*MQ7_1).PPM);
	Text((*Screen_Main).graph_display_Ox + 10, (*Screen_Main).Data3_Yposi[0], fstr, SansTypeface, 15);
	End();
}
void RPi_Graphics_Init(int *width, int *height) {
	init(width, height); // Graphics initialization	
}

void RPi_Graphics_Finish() {
	finish();
	
}