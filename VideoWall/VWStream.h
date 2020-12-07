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


#define VWSTREAM_ERROR	-100

#define ERROR_CONNECTION_FAILED	VWSTREAM_ERROR-1
#define ERROR_NO_STREAM_INFO	VWSTREAM_ERROR-2
#define ERROR_NO_VIDEO_STREAM	VWSTREAM_ERROR-3
#define ERROR_NO_MEMORY			VWSTREAM_ERROR-4
#define ERROR_AV_CODEC			VWSTREAM_ERROR-5
#define ERROR_NO_MORE_DATA		VWSTREAM_ERROR-6
#define ERROR_CONNECTION_UNAUTHORIZED	VWSTREAM_ERROR-7



class VWStream
{
	public:
		VWStream();
		~VWStream();

		int Connect(int nCameraID, std::string sURL); // return error code
		int ReadFrame(AVPacket *pPacket); // return content of Packet
		int Close(void); // return error code
		int Destruct(void); // return error code

	private:
		int CleanUP();

		int SetURL(int, std::string);
		void PrintStatus();
	
	public:
		enum {Instant, Wait} ReadType = Wait;
		//Global data
		const static int gi_Buffer_Size = 100;

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
			SDL_Thread * pReadFrame_Thread = NULL;

			bool Decoding_Thread_Exit = false;

			//Status parameter
			bool bDecoding_Started = false;
		} m_Decoder;

		void Init_Decoder_Data();
	
		int m_CameraID = -1;
		std::string m_URL;

		FILE *fp = NULL;
		void OpenOutputFile();
		void CloseOutputFile();
		void WriteOutputFile(AVPacket *packet);

		AVBSFContext * h264bsfc;
		const AVBitStreamFilter * filter;

		void Init_BitStream_Filter();
		void BitStream_Filter(AVPacket * packet);
		void Close_BitStream_Filter();
};
