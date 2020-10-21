#pragma once
#include <string>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "libavutil/hwcontext.h"
}

#define SDL_MAIN_HANDLED
#include "SDL/SDL.h"
#include "SDL/SDL_syswm.h"


//Global data
const int gi_Buffer_Size = 100;


class VWStream
{
	public:
		VWStream();
		int Connect(int nCameraID, std::string sURL);
		int CleanUP();

		int SetURL(int, std::string);
		void PrintStatus();

		struct VideoDecoder
		{
			//ffmpeg
			//char *pFilepath = NULL;
			AVFormatContext	*pFormatCtx = NULL;
			AVCodecContext	*pCodecCtx;
			AVCodec			*pCodec;
			AVFrame	*pFrame;
			AVFrame *pFrameYUV;
			unsigned char *out_buffer;
			AVPacket *packet;
			int got_picture;
			int videoindex;
			struct SwsContext *img_convert_ctx;

			//SDL
			SDL_mutex* BufferLock = NULL;

			//own data
			AVPacket Package_Buffer[gi_Buffer_Size];
			int Buffer_Head = -1;
			int Buffer_End = 0;
			bool HeadCatchEnd = true;

			bool bAvailable = true;

			//First ready
			bool First_Ready = false;
			int Screen_Mask = 0x0000;

			//Commanding parameter
			//Thread
			bool ReadFram_Thread_Exit = false;

			bool Decoding_Thread_Exit = false;

			//Status parameter
			bool bDecoding_Started = false;
		} m_Decoder;

	
		int m_CameraID = -1;
		std::string m_URL;
};