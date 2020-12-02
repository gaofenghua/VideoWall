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

	pDecoder->packet = av_packet_alloc();

	while (!pDecoder->ReadFram_Thread_Exit && av_read_frame(pDecoder->pFormatCtx, pDecoder->packet) >= 0)
	{
		if (pDecoder->packet->stream_index == pDecoder->videoindex)
		{
			//BYTE* pb =(BYTE*) pDecoder->packet->data;
			//printf("AV_Read_Frame Addr=%p, size=%d, Key=%d,  %.2x %.2x %.2x %.2x %.2x\r\n",pb, pDecoder->packet->size, pDecoder->packet->flags, *pb, *(pb + 1), *(pb + 2), *(pb + 3), *(pb + 4));

			
			//Lock
			SDL_LockMutex(pDecoder->BufferLock);

			if (pDecoder->Buffer_Head == pDecoder->Buffer_End && !pDecoder->HeadCatchEnd) // read too fast
			{
				//Throw one package
				pDecoder->Buffer_Head = (pDecoder->Buffer_Head + 1 >= VWStream::gi_Buffer_Size) ? 0 : pDecoder->Buffer_Head + 1;

				printf("Read Frame Warnning: Throw one package, Head=End = %d, HeadCatchEnd = %d\r\n", pDecoder->Buffer_Head, pDecoder->HeadCatchEnd);
			}

			if (NULL != (pDecoder->Package_Buffer[pDecoder->Buffer_End]).buf)
			{
				av_packet_unref(&(pDecoder->Package_Buffer[pDecoder->Buffer_End]));
			}
			av_packet_ref(&(pDecoder->Package_Buffer[pDecoder->Buffer_End]), pDecoder->packet);
			av_packet_unref(pDecoder->packet);

			pDecoder->Buffer_End = (pDecoder->Buffer_End + 1 >= VWStream::gi_Buffer_Size) ? 0 : pDecoder->Buffer_End + 1;

			if (pDecoder->Buffer_Head == -1) // initial first package
			{
				pDecoder->Buffer_Head = 0;
			}
			if (pDecoder->Buffer_End == pDecoder->Buffer_Head) // End catach Head
			{
				pDecoder->HeadCatchEnd = false;
			}

			SDL_UnlockMutex(pDecoder->BufferLock);

			//AV_PKT_FLAG_KEY
			//printf("ReadFrame_Thread, buffer head = %d, end = %d, flag=%d\r\n", pDecoder->Buffer_Head, pDecoder->Buffer_End, (pDecoder->Package_Buffer[pDecoder->Buffer_End-1]).flags);
			//SDL_Delay(40);
		}
	}

	av_packet_free(&(pDecoder->packet));
	pDecoder->ReadFram_Thread_Exit = false;
	pDecoder->pReadFrame_Thread = NULL;

	printf("Read Threat %d: Loop ends.  buffer head = %d, end = %d\r\n", pStreamObj->m_CameraID, pDecoder->Buffer_Head, pDecoder->Buffer_End);

	return 0;
}

VWStream::VWStream()
{
	Init_Decoder_Data();
	OpenOutputFile();
}
VWStream::~VWStream()
{
	CloseOutputFile();
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

	avcodec_free_context(&(pDecoder->pCodecCtx));

	return true;
}

void VWStream::Init_Decoder_Data()
{
	for (int i = 0; i < gi_Buffer_Size; i++)
	{
		av_init_packet(&(m_Decoder.Package_Buffer[i]));
		m_Decoder.Package_Buffer[i].data = NULL;
		m_Decoder.Package_Buffer[i].size = 0;
	}

}

int VWStream::Connect(int nCameraID, std::string sURL)
{
	m_CameraID = nCameraID;
	m_URL = sURL;

	av_log_set_level(AV_LOG_QUIET);

	VideoDecoder *pDecoder = &(m_Decoder);

	pDecoder->pFormatCtx = avformat_alloc_context();

	AVDictionary* options = NULL;
	av_dict_set(&options, "rtsp_transport", "tcp", 0); //tcp udp
	av_dict_set(&options, "stimeout", "6000000", 0); //µ¥Î»ÊÇÎ¢Ãë£¨us£©

	AVInputFormat* iformat = NULL;
	//iformat = av_find_input_format("h264");

	//int iRet = avformat_open_input(&(pDecoder->pFormatCtx), pDecoder->pFilepath, iformat, &options);
	int iRet = avformat_open_input(&(pDecoder->pFormatCtx), m_URL.data(), iformat, &options);
	
	av_dict_free(&options);

	if (iRet != 0)
	{
		char strerror_buf[1024];
		av_strerror(iRet, strerror_buf, 1024);
		//printf("Couldn't open input stream. avformat_open_input = %d error = %s\n", iRet,strerror_buf);
		return ERROR_CONNECTION_FAILED;
	}



	if (avformat_find_stream_info(pDecoder->pFormatCtx, NULL) < 0)
	{
		//fprintf(stderr, "Cannot find input stream information.\n");
		return ERROR_NO_STREAM_INFO;
	}

	/* find the video stream information */
	int ret = av_find_best_stream(pDecoder->pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &(pDecoder->pCodec), 0);
	if (ret < 0)
	{
		//fprintf(stderr, "Cannot find a video stream in the input file\n");
		return ERROR_NO_VIDEO_STREAM;
	}
	pDecoder->videoindex = ret;


	if (!(pDecoder->pCodecCtx = avcodec_alloc_context3(pDecoder->pCodec)))
	{
		return ERROR_NO_MEMORY;//AVERROR(ENOMEM)
	}

	AVStream* video = pDecoder->pFormatCtx->streams[pDecoder->videoindex];
	if (avcodec_parameters_to_context(pDecoder->pCodecCtx, video->codecpar) < 0)
	{
		return ERROR_AV_CODEC;
	}

	if ((ret = avcodec_open2(pDecoder->pCodecCtx, pDecoder->pCodec, NULL)) < 0)
	{
		//fprintf(stderr, "Failed to open codec for stream #%u\n", pDecoder->videoindex);
		return ERROR_AV_CODEC;
	}


	//pDecoder->pFrame = av_frame_alloc();

	//pDecoder->pFrameYUV = av_frame_alloc();
	//pDecoder->out_buffer = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pDecoder->pCodecCtx->width, pDecoder->pCodecCtx->height, 1));
	////av_image_fill_arrays(pDecoder->pFrameYUV->data, pDecoder->pFrameYUV->linesize, pDecoder->out_buffer, AV_PIX_FMT_YUV420P, pDecoder->pCodecCtx->width, pDecoder->pCodecCtx->height, 1);
	//av_image_fill_arrays(pDecoder->pFrameYUV->data, pDecoder->pFrameYUV->linesize, pDecoder->out_buffer, AV_PIX_FMT_YUV420P, 270, 180, 1);

	////pDecoder->img_convert_ctx = sws_getContext(pDecoder->pCodecCtx->width, pDecoder->pCodecCtx->height, pDecoder->pCodecCtx->pix_fmt, pDecoder->pCodecCtx->width, pDecoder->pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
	//pDecoder->img_convert_ctx = sws_getContext(pDecoder->pCodecCtx->width, pDecoder->pCodecCtx->height, pDecoder->pCodecCtx->pix_fmt, 270, 180, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

	////Output Info-----------------------------
	//printf("--------------- File Information ----------------\n");
	//av_dump_format(pDecoder->pFormatCtx, 0, m_URL.data(), 0);
	//printf("-------------------------------------------------\n");

	//Init_BitStream_Filter();

	m_Decoder.pReadFrame_Thread = SDL_CreateThread(ReadFrame_Thread, NULL, (void*)this);
}

int VWStream::ReadFrame(AVPacket *pPacket)
{
	AVPacket* packet;
	packet = pPacket;

	if (NULL != pPacket->buf)
	{
		av_packet_unref(packet);
	}
	

	VideoDecoder* pDecoder = &m_Decoder;

	SDL_LockMutex(pDecoder->BufferLock);

	if (ReadType == Instant)
	{
		if (pDecoder->Buffer_Head == -1 || (pDecoder->Buffer_Head == pDecoder->Buffer_End && pDecoder->HeadCatchEnd))
		{
			//Unlock
			SDL_UnlockMutex(pDecoder->BufferLock);

			//printf("VWStream ReadFrame Warning: No package, Head = %d, HeadCatchEnd = %d\r\n", pDecoder->Buffer_Head, pDecoder->HeadCatchEnd);

			return ERROR_NO_MORE_DATA;
		}
	}
	else if (ReadType == Wait)
	{
		int nWaitTimes = 1000;

		while (nWaitTimes >= 0 && ( pDecoder->Buffer_Head == -1 || (pDecoder->Buffer_Head == pDecoder->Buffer_End && pDecoder->HeadCatchEnd)))
		{
			//Unlock
			SDL_UnlockMutex(pDecoder->BufferLock);

			//printf("VWStream ReadFrame Warning: No package, Wait. Countdown = %d,   Head = %d, HeadCatchEnd = %d\r\n", nWaitTimes, pDecoder->Buffer_Head, pDecoder->HeadCatchEnd);
			Sleep(20);

			SDL_LockMutex(pDecoder->BufferLock);

			nWaitTimes--;
		}

		if (nWaitTimes < 0)
		{
			//Unlock
			SDL_UnlockMutex(pDecoder->BufferLock);

			printf("VWStream ReadFrame Warning: No package, Head = %d, HeadCatchEnd = %d\r\n", pDecoder->Buffer_Head, pDecoder->HeadCatchEnd);

			return ERROR_NO_MORE_DATA;
		}
	}


	av_packet_ref(packet, &(pDecoder->Package_Buffer[pDecoder->Buffer_Head]));
	av_packet_unref(&(pDecoder->Package_Buffer[pDecoder->Buffer_Head]));

	pDecoder->Buffer_Head = (pDecoder->Buffer_Head + 1 >= gi_Buffer_Size) ? 0 : pDecoder->Buffer_Head + 1;

	if (pDecoder->Buffer_Head == pDecoder->Buffer_End)
	{
		pDecoder->HeadCatchEnd = true;
	}

	//Unlock
	SDL_UnlockMutex(pDecoder->BufferLock);

	return 0;
}
int VWStream::Close(void)
{
	if (m_Decoder.pReadFrame_Thread != NULL)
	{
		m_Decoder.ReadFram_Thread_Exit = true;
		Sleep(100);
	}

	while (m_Decoder.ReadFram_Thread_Exit == true)
	{
		printf("Wait for close...\n");
		Sleep(100);
	}

	for (int i = 0; i < gi_Buffer_Size; i++)
	{
		if (NULL != m_Decoder.Package_Buffer[i].buf)
		{
			av_packet_unref(&(m_Decoder.Package_Buffer[i]));
		}
	}

	CleanUP();

	return 0;
}
int VWStream::Destruct(void)
{
	Close();

	return 0;
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

void VWStream::OpenOutputFile()
{
	fp = fopen("junktest.h264", "ab");
}

void VWStream::CloseOutputFile()
{
	fclose(fp);
}
void VWStream::WriteOutputFile(AVPacket *packet)
{
	if (packet->flags == 1)
	{
		fwrite(m_Decoder.pFormatCtx->streams[m_Decoder.videoindex]->codecpar->extradata, 1, m_Decoder.pFormatCtx->streams[m_Decoder.videoindex]->codecpar->extradata_size, fp);
	}
	
	fwrite(packet->data, packet->size, 1, fp);
}


void VWStream::Init_BitStream_Filter()
{
	filter = av_bsf_get_by_name("h264_mp4toannexb");
	int ret = av_bsf_alloc(filter, &h264bsfc);

	//m_Decoder.pFormatCtx->streams[m_Decoder.videoindex]->codecpar->codec_tag = MKTAG('a', 'v', 'c', '1');
	avcodec_parameters_copy(h264bsfc->par_in, m_Decoder.pFormatCtx->streams[m_Decoder.videoindex]->codecpar);
	h264bsfc->time_base_in = m_Decoder.pFormatCtx->streams[m_Decoder.videoindex]->time_base;
	ret = av_bsf_init(h264bsfc);
	ret = avcodec_parameters_copy(m_Decoder.pFormatCtx->streams[m_Decoder.videoindex]->codecpar, h264bsfc->par_out);
	m_Decoder.pFormatCtx->streams[m_Decoder.videoindex]->time_base = h264bsfc->time_base_out;
}

void VWStream::BitStream_Filter(AVPacket * packet)
{
	int ret = av_bsf_send_packet(h264bsfc, packet);
	if (ret < 0)
	{
		printf("av_bsf_send_packet error");
	}
	while ((ret = av_bsf_receive_packet(h264bsfc, packet)) == 0)
	{
		BYTE* pb = (BYTE*)packet->data;
		printf("+++++++++     addr=%p, size=%d,       %.2x %.2x %.2x %.2x %.2x\r\n", packet->data, packet->size, *pb, *(pb + 1), *(pb + 2), *(pb + 3), *(pb + 4));
		
	}

}

void VWStream::Close_BitStream_Filter()
{
	av_bsf_free(&h264bsfc);
}
