/************************************************************
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include <ofMain.h>
#include <ofxVboSet.h>

#include "sj_common.h"
#include "fft.h"

/************************************************************
************************************************************/
class DRAW_SIGNAL{
private:
	/****************************************
	****************************************/
	ofColor LastCol_Audio;
	ofColor LastCol_Window;
	ofx__VBO_SET VboSet_Audio;
	ofx__VBO_SET VboSet_Hanning;
	ofx__VBO_SET VboSet_Tukey;
	
	ofImage img_Back;
	
	/****************************************
	****************************************/
	void Refresh_vboVerts(const FFT& fft);
	void Refresh_vboColor();

public:
	/****************************************
	****************************************/
	DRAW_SIGNAL();
	~DRAW_SIGNAL();
	void setup(const FFT& fft);
	void update(const FFT& fft);
	void draw();
};

