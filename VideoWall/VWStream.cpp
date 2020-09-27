#include "VWStream.h"


#define   LIBPATH(p,f)   p##f 
#pragma comment(lib,LIBPATH(__FILE__,   "\\..\\lib\\avformat.lib")) 

VWStream::VWStream()
{
	avformat_network_init();
}