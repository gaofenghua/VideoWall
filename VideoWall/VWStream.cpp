#include "VWStream.h"


#define   LIBPATH(p,f)   p##f 
#pragma comment(lib,LIBPATH(__FILE__,   "\\..\\lib\\avcodec.lib")) 
#pragma comment(lib,LIBPATH(__FILE__,   "\\..\\lib\\avformat.lib")) 
#pragma comment(lib,LIBPATH(__FILE__,   "\\..\\lib\\avutil.lib")) 
#pragma comment(lib,LIBPATH(__FILE__,   "\\..\\lib\\avdevice.lib")) 
#pragma comment(lib,LIBPATH(__FILE__,   "\\..\\lib\\avfilter.lib")) 
#pragma comment(lib,LIBPATH(__FILE__,   "\\..\\lib\\swscale.lib")) 
#pragma comment(lib,LIBPATH(__FILE__,   "\\..\\lib\\swresample.lib")) 

#pragma comment(lib,LIBPATH(__FILE__,   "\\..\\lib\\SDL2main.lib")) 
#pragma comment(lib,LIBPATH(__FILE__,   "\\..\\lib\\SDL2.lib")) 

int ReadFrame_Thread(void *opaque)
{
	VWStream* pStreamObj = (VWStream *)opaque;

	VWStream::VideoDecoder* pDecoder = (VWStream::VideoDecoder*)&(pStreamObj->m_Decoder);

	//pDecoder->packet = (AVPacket *)av_malloc(sizeof(AVPacket));
	pDecoder->packet = av_packet_alloc();

	while (!pDecoder->ReadFram_Thread_Exit && av_read_frame(pDecoder->pFormatCtx, pDecoder->packet) >= 0)
	{
		if (pDecoder->packet->stream_index == pDecoder->videoindex)
		{
			//Lock
			SDL_LockMutex(pDecoder->BufferLock);

			if (pDecoder->Buffer_Head == pDecoder->Buffer_End && !pDecoder->HeadCatchEnd) // read too fast
			{
				//Throw one package
				pDecoder->Buffer_Head = (pDecoder->Buffer_Head + 1 >= gi_Buffer_Size) ? 0 : pDecoder->Buffer_Head + 1;
			}

			//av_copy_packet(&(pDecoder->Package_Buffer[pDecoder->Buffer_End]), pDecoder->packet);
			//av_free_packet(pDecoder->packet);
			av_packet_ref(&(pDecoder->Package_Buffer[pDecoder->Buffer_End]), pDecoder->packet);
			av_packet_unref(pDecoder->packet);

			pDecoder->Buffer_End = (pDecoder->Buffer_End + 1 >= gi_Buffer_Size) ? 0 : pDecoder->Buffer_End + 1;

			if (pDecoder->Buffer_Head == -1) // initial first package
			{
				pDecoder->Buffer_Head = 0;
			}
			if (pDecoder->Buffer_End == pDecoder->Buffer_Head) // End catach Head
			{
				pDecoder->HeadCatchEnd = false;
			}

			SDL_UnlockMutex(pDecoder->BufferLock);

			printf("ReadFrame_Thread, buffer head = %d, end = %d\r\n", pDecoder->Buffer_Head, pDecoder->Buffer_End);
			//SDL_Delay(40);
		}
	}

	printf("Read Threat %d: Loop ends -------------------------------- buffer head = %d, end = %d\r\n", pStreamObj->m_CameraID, pDecoder->Buffer_Head, pDecoder->Buffer_End);

	return 0;
}

VWStream::VWStream()
{

}

int VWStream::CleanUP()
{
	VideoDecoder *pDecoder = &(m_Decoder);

	avformat_close_input(&(pDecoder->pFormatCtx));

	if (pDecoder->pFormatCtx != NULL)
	{
		avformat_free_context(pDecoder->pFormatCtx);
		pDecoder->pFormatCtx = NULL;
	}
	return true;
}

int VWStream::Connect(int nCameraID, std::string sURL)
{
	m_CameraID = nCameraID;
	m_URL = sURL;

	VideoDecoder *pDecoder = &(m_Decoder);

	//pDecoder->pFilepath = new char[strlen(path) + 1];
	//memcpy(pDecoder->pFilepath, path, strlen(path) + 1);

	pDecoder->pFormatCtx = avformat_alloc_context();

	AVDictionary* options = NULL;
	av_dict_set(&options, "rtsp_transport", "tcp", 0); //tcp udp
	av_dict_set(&options, "stimeout", "6000000", 0); //µ¥Î»ÊÇÎ¢Ãë£¨us£©

	AVInputFormat* iformat = NULL;
	//iformat = av_find_input_format("h264");

	//int iRet = avformat_open_input(&(pDecoder->pFormatCtx), pDecoder->pFilepath, iformat, &options);
	int iRet = avformat_open_input(&(pDecoder->pFormatCtx), m_URL.data(), iformat, &options);
	if (iRet != 0)
	{
		printf("Couldn't open input stream. avformat_open_input = %d \n", iRet);
		return -1;
	}

	//Output Info-----------------------------
	printf("--------------- File Information ----------------\n");
	av_dump_format(pDecoder->pFormatCtx, 0, m_URL.data(), 0);
	printf("-------------------------------------------------\n");

	if (avformat_find_stream_info(pDecoder->pFormatCtx, NULL) < 0)
	{
		fprintf(stderr, "Cannot find input stream information.\n");
		return -1;
	}

	/* find the video stream information */
	int ret = av_find_best_stream(pDecoder->pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &(pDecoder->pCodec), 0);
	if (ret < 0)
	{
		fprintf(stderr, "Cannot find a video stream in the input file\n");
		return -1;
	}
	pDecoder->videoindex = ret;


	//if (!(pDecoder->pCodecCtx = avcodec_alloc_context3(pDecoder->pCodec)))
	//{
	//	return AVERROR(ENOMEM);
	//}

	//AVStream* video = pDecoder->pFormatCtx->streams[pDecoder->videoindex];
	//if (avcodec_parameters_to_context(pDecoder->pCodecCtx, video->codecpar) < 0)
	//{
	//	return -1;
	//}

	//if ((ret = avcodec_open2(pDecoder->pCodecCtx, pDecoder->pCodec, NULL)) < 0)
	//{
	//	fprintf(stderr, "Failed to open codec for stream #%u\n", pDecoder->videoindex);
	//	return -1;
	//}


	//pDecoder->pFrame = av_frame_alloc();

	//pDecoder->pFrameYUV = av_frame_alloc();
	//pDecoder->out_buffer = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pDecoder->pCodecCtx->width, pDecoder->pCodecCtx->height, 1));
	////av_image_fill_arrays(pDecoder->pFrameYUV->data, pDecoder->pFrameYUV->linesize, pDecoder->out_buffer, AV_PIX_FMT_YUV420P, pDecoder->pCodecCtx->width, pDecoder->pCodecCtx->height, 1);
	//av_image_fill_arrays(pDecoder->pFrameYUV->data, pDecoder->pFrameYUV->linesize, pDecoder->out_buffer, AV_PIX_FMT_YUV420P, 270, 180, 1);

	////pDecoder->img_convert_ctx = sws_getContext(pDecoder->pCodecCtx->width, pDecoder->pCodecCtx->height, pDecoder->pCodecCtx->pix_fmt, pDecoder->pCodecCtx->width, pDecoder->pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
	//pDecoder->img_convert_ctx = sws_getContext(pDecoder->pCodecCtx->width, pDecoder->pCodecCtx->height, pDecoder->pCodecCtx->pix_fmt, 270, 180, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);


	SDL_Thread *p = SDL_CreateThread(ReadFrame_Thread, NULL, (void*)this);
}

int VWStream::SetURL(int nCameraID, std::string sURL)
{
	m_CameraID = nCameraID;
	m_URL = sURL;

	return true;
}

void VWStream::PrintStatus()
{
	printf("VWStream Obj CameraID = %d, buffer head = %d, end = %d\r\n", m_CameraID, m_Decoder.Buffer_Head, m_Decoder.Buffer_End);
}
