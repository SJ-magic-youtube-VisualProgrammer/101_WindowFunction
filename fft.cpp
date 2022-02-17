/************************************************************
************************************************************/
#include "fft.h"

/************************************************************
************************************************************/

/******************************
******************************/
FFT::FFT()
{
}

/******************************
******************************/
FFT::~FFT()
{
}

/******************************
******************************/
void FFT::threadedFunction()
{
	while(isThreadRunning()) {
		lock();
		
		unlock();
		
		sleep(THREAD_SLEEP_MS);
	}
}

/******************************
******************************/
void FFT::setup()
{
	/********************
	********************/
	print_separatoin();
	printf("> setup : FFT\n");
	fflush(stdout);
	
	/********************
	********************/
	if(!Is_Factorial_of_2(AUDIO_BUF_SIZE)) { ERROR_MSG(); std::exit(1); }
	
	/********************
	■std::vectorのresizeとassignの違い (C++)
		https://minus9d.hatenablog.com/entry/2021/02/07/175159
	********************/
	vol_l.assign(AUDIO_BUF_SIZE, 0.0);
	vol_r.assign(AUDIO_BUF_SIZE, 0.0);
	
	/**************************
	**************************/
	N = AUDIO_BUF_SIZE;
	sintbl.resize(N + N/4);
	bitrev.resize(N);
	Hanning_window.resize(N);
	Tukey_window.resize(N);
	fft_x.assign(N, 0.0);
	fft_y.assign(N, 0.0);
	/*
	Gain.assign(N/2, 0.0);
	Gain_smoothed.assign(N/2, 0.0);
	Gain_smoothed_2.assign(N/2, 0.0);
	phase_rad.assign(N/2, 0.0);
	phase_deg.assign(N/2, 0.0);
	phase_rad_madeFromGain.assign(N/2, 0.0);
	artSin_x1.assign(N, 0.0);
	artSin_y1.assign(N, 0.0);
	artSin_x2.assign(N, 0.0);
	artSin_y2.assign(N, 0.0);
	artSin_x3.assign(N, 0.0);
	artSin_y3.assign(N, 0.0);
	*/
	
	// table
	make_sintbl();
	make_bitrev();
	
	// 窓関数
	make_Hanning_window();
	make_tukey_window();
	
	// 補正係数
	/*
	float sum_window = 0;
	for(int i = 0; i < N; i++){
		sum_window += Hanning_window[i];
	}
	acf = sum_window / N;
	printf("> acf = %f\n", acf);
	fflush(stdout);
	*/
}

/******************************
******************************/
void FFT::make_Hanning_window()
{
	/********************
	********************/
	for(int i = 0; i < N; i++)	Hanning_window[i] = 0.5 - 0.5 * cos(2 * _PI * i / N); // Hanning.
	
	/********************
	********************/
	/*
	static bool b_1st = true;
	if(b_1st){
		fprintf(fp_Log, ",Hanning\n");
		for(int i = 0; i < N; i++){
			fprintf(fp_Log, "%d,%f\n", i,Hanning_window[i]);
		}
		fprintf(fp_Log, "\n\n");
		
		b_1st = false;
	}
	*/
}

/******************************
******************************/
void FFT::make_tukey_window()
{
	/********************
	********************/
	float TukeyAlpha = Gui_Global->Tukey_alpha;
	if(TukeyAlpha <= 0) TukeyAlpha = 1e-4;
	
	for(int i = 0; i < N; i++){
		if(i <= float(N) * TukeyAlpha / 2){
			Tukey_window[i] = 0.5 - 0.5 * cos( 2 * _PI / (N * TukeyAlpha) * i );
		}else if( float(N) - float(N) * TukeyAlpha / 2 <= i ){
			Tukey_window[i] = 0.5 - 0.5 * cos( 2 * _PI / (N * TukeyAlpha) * (i - N + N * TukeyAlpha) );
		}else{
			Tukey_window[i] = 1.0;
		}
	}
	
	/********************
	********************/
	/*
	static bool b_1st = true;
	if(b_1st){
		fprintf(fp_Log, ",Tukey\n");
		for(int i = 0; i < N; i++){
			fprintf(fp_Log, "%d,%f\n", i,Tukey_window[i]);
		}
		fprintf(fp_Log, "\n\n");
		
		b_1st = false;
	}
	*/
}

/******************************
******************************/
bool FFT::Is_Factorial_of_2(double val)
{
	double ret = log2(val);
	
	if( ret - (int)ret  == 0 )	return true;
	else						return false;
}

/******************************
******************************/
void FFT::update()
{
	/********************
	********************/
	int now = ofGetElapsedTimeMillis();
	int dt = now - t_LastUpdate;
	
	/********************
	********************/
	copy_vol_to_analyzeArray();
	
	make_tukey_window();
	
	if(Gui_Global->b_window_Hanning)	multiply_HanningWindow(fft_x);
	else if(Gui_Global->b_window_Tukey)	multiply_TukeyWindow(fft_x);
	
	/********************
	********************/
	t_LastUpdate = now;
}

/******************************
******************************/
double FFT::get_fftX(int id) const
{
	if( (id < 0) || (fft_x.size() <= id) )	return 0;
	else									return fft_x[id];
}

/******************************
******************************/
double FFT::get_Hanning(int id) const
{
	if( (id < 0) || (Hanning_window.size() <= id) )	return 0;
	else											return Hanning_window[id];
}

/******************************
******************************/
double FFT::get_Tukey(int id) const
{
	if( (id < 0) || (Tukey_window.size() <= id) )	return 0;
	else											return Tukey_window[id];
}

/******************************
******************************/
void FFT::multiply_HanningWindow(vector <double>& _x)
{
	if(_x.size() != Hanning_window.size()) return;
	
	for(int i = 0; i < _x.size(); i++){
		_x[i] = _x[i] * Hanning_window[i];
	}
}

/******************************
******************************/
void FFT::multiply_TukeyWindow(vector <double>& _x)
{
	if(_x.size() != Tukey_window.size()) return;
	
	for(int i = 0; i < _x.size(); i++){
		_x[i] = _x[i] * Tukey_window[i];
	}
}

/******************************
******************************/
void FFT::copy_vol_to_analyzeArray()
{
	lock();
		for(int i = 0; i < N; i++){
			/********************
			********************/
			switch(AnalyzeCh){
				case ANALYZE_CH__STEREO:
					fft_x[i] = (vol_l[i] + vol_r[i]) / 2;
					break;
					
				case ANALYZE_CH__L:
					fft_x[i] = vol_l[i];
					break;
					
				case ANALYZE_CH__R:
					fft_x[i] = vol_r[i];
					break;
			}
			
			/********************
			********************/
			
			fft_y[i] = 0;
		}
	unlock();
}

/******************************
******************************/
void FFT::SetVol(ofSoundBuffer & buffer)
{
	static int c = 0;
	lock();
		
		if( (vol_l.size() < buffer.getNumFrames()) || (vol_r.size() < buffer.getNumFrames()) ){
			ERROR_MSG();
			std::exit(1);
		}else{
			for (size_t i = 0; i < buffer.getNumFrames(); i++){
				vol_l[i] = buffer[i*2 + 0];
				vol_r[i] = buffer[i*2 + 1];
			}
			if(c < 2){
				printf("Audio in  buffer = %p\n", &buffer[0]); // float & ofSoundBuffer::operator[](std::size_t pos) { return buffer[pos]; } // ofSoundBuffer.cpp
				c++;
			}
		}
	
	unlock();
}

/******************************
******************************/
void FFT::GetVol(ofSoundBuffer & buffer, bool b_EnableAudioOut)
{
	static int c = 0;
	lock();
	
		if( (vol_l.size() < buffer.getNumFrames()) || (vol_r.size() < buffer.getNumFrames()) ){
			ERROR_MSG();
			std::exit(1);
		}else{
			for (size_t i = 0; i < buffer.getNumFrames(); i++){
				if(b_EnableAudioOut){
					buffer[i*2 + 0]	= vol_l[i];
					buffer[i*2 + 1]	= vol_r[i];
				}else{
					buffer[i*2 + 0]	= 0;
					buffer[i*2 + 1]	= 0;
				}
			}
			if(c < 2){
				printf("Audio out buffer = %p\n", &buffer[0]);  // float & ofSoundBuffer::operator[](std::size_t pos) { return buffer[pos]; } // ofSoundBuffer.cpp
				c++;
			}
		}
	
	unlock();

}

/******************************
******************************/
int FFT::fft(vector <double>& x, vector <double>& y, int IsReverse)
{
	/*****************
		bit反転
	*****************/
	int i, j;
	for(i = 0; i < N; i++){
		j = bitrev[i];
		if(i < j){
			double t;
			t = x[i]; x[i] = x[j]; x[j] = t;
			t = y[i]; y[i] = y[j]; y[j] = t;
		}
	}

	/*****************
		変換
	*****************/
	int n4 = N / 4;
	int k, ik, h, d, k2;
	double s, c, dx, dy;
	for(k = 1; k < N; k = k2){
		h = 0;
		k2 = k + k;
		d = N / k2;

		for(j = 0; j < k; j++){
			c = sintbl[h + n4];
			if(IsReverse)	s = -sintbl[h];
			else			s = sintbl[h];

			for(i = j; i < N; i += k2){
				ik = i + k;
				dx = s * y[ik] + c * x[ik];
				dy = c * y[ik] - s * x[ik];

				x[ik] = x[i] - dx;
				x[i] += dx;

				y[ik] = y[i] - dy;
				y[i] += dy;
			}
			h += d;
		}
	}

	/*****************
	*****************/
	if(!IsReverse){
		for(i = 0; i < N; i++){
			x[i] /= N;
			y[i] /= N;
		}
	}

	return 0;
}

/******************************
******************************/
void FFT::make_bitrev(void)
{
	int i, j, k, n2;

	n2 = N / 2;
	i = j = 0;

	for(;;){
		bitrev[i] = j;
		if(++i >= N)	break;
		k = n2;
		while(k <= j)	{j -= k; k /= 2;}
		j += k;
	}
}

/******************************
******************************/
void FFT::make_sintbl(void)
{
	for(int i = 0; i < N + N/4; i++){
		sintbl[i] = sin(2 * _PI * i / N);
	}
}
