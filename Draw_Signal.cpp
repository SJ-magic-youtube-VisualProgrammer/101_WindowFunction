/************************************************************
************************************************************/
#include "Draw_Signal.h"

/************************************************************
************************************************************/

/******************************
******************************/
DRAW_SIGNAL::DRAW_SIGNAL(){
}

/******************************
******************************/
DRAW_SIGNAL::~DRAW_SIGNAL(){
}

/******************************
******************************/
void DRAW_SIGNAL::setup(const FFT& fft){
	/********************
	********************/
	print_separatoin();
	printf("> setup : Draw ArtSin 2D\n");
	fflush(stdout);
	
	/********************
	********************/
	VboSet_Audio.setup(AUDIO_BUF_SIZE);
	VboSet_Hanning.setup(AUDIO_BUF_SIZE);
	VboSet_Tukey.setup(AUDIO_BUF_SIZE);
	
	Refresh_vboVerts(fft);
	Refresh_vboColor();
	VboSet_Audio.update_vertex_color();
	
	LastCol_Audio = Gui_Global->col_Audio;
	LastCol_Window = Gui_Global->col_Window;
	
	/********************
	********************/
	img_Back.load("img/img.png");
}

/******************************
1--2
|  |
0--3
******************************/
void DRAW_SIGNAL::Refresh_vboVerts(const FFT& fft)
{
	/********************
	********************/
	const ofVec2f ofs = ofVec2f(129, ofGetHeight()/2);
	const float space = 2;
	const float BarHeight = Gui_Global->DrawAudioSignal_H;
	
	/********************
	********************/
	if(Gui_Global->b_update_signal){
		for(int i = 0; i < AUDIO_BUF_SIZE; i++){
			float _y = ofs.y - fft.get_fftX(i) * BarHeight;
			VboSet_Audio.set_vboVerts( i, ofs.x + i * space, _y );
		}
		VboSet_Audio.update_vertex();
	}
	
	/********************
	********************/
	for(int i = 0; i < AUDIO_BUF_SIZE; i++){
		float _y = ofs.y - fft.get_Hanning(i) * BarHeight;
		VboSet_Hanning.set_vboVerts( i, ofs.x + i * space, _y );
	}
	VboSet_Hanning.update_vertex();
	
	/********************
	********************/
	for(int i = 0; i < AUDIO_BUF_SIZE; i++){
		float _y = ofs.y - fft.get_Tukey(i) * BarHeight;
		VboSet_Tukey.set_vboVerts( i, ofs.x + i * space, _y );
	}
	VboSet_Tukey.update_vertex();
}

/******************************
******************************/
void DRAW_SIGNAL::Refresh_vboColor()
{
	VboSet_Audio.set_singleColor(Gui_Global->col_Audio);
	VboSet_Audio.update_color();
	
	VboSet_Hanning.set_singleColor(Gui_Global->col_Window);
	VboSet_Hanning.update_color();
	
	VboSet_Tukey.set_singleColor(Gui_Global->col_Window);
	VboSet_Tukey.update_color();
	
	return;
}


/******************************
******************************/
void DRAW_SIGNAL::update(const FFT& fft){
	Refresh_vboVerts(fft);
	
	ofColor gui_col_Audio = Gui_Global->col_Audio;
	ofColor gui_col_Window = Gui_Global->col_Window;
	if( (gui_col_Audio != LastCol_Audio) || (gui_col_Window != LastCol_Window) ){
		Refresh_vboColor();
		LastCol_Audio = Gui_Global->col_Audio;
		LastCol_Window = Gui_Global->col_Window;
	}
}

/******************************
******************************/
void DRAW_SIGNAL::draw(){
	/********************
	********************/
	ofEnableAlphaBlending();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	ofEnableSmoothing();
	
	/********************
	********************/
	ofSetColor(255, 255, 255, Gui_Global->alpha_of_BackImg);
	img_Back.draw(0, 0);
	
	ofSetColor(255, 255, 255, 255);
	glLineWidth(Gui_Global->Draw_LineWidth); // glPointSize();
	VboSet_Audio.draw(GL_LINE_STRIP);
	
	/********************
	********************/
	if(Gui_Global->b_Draw_Window){
		ofSetColor(255, 255, 255, 255);
		if(Gui_Global->b_window_Hanning)	VboSet_Hanning.draw(GL_LINE_STRIP);
		else if(Gui_Global->b_window_Tukey)	VboSet_Tukey.draw(GL_LINE_STRIP);
	}
}

