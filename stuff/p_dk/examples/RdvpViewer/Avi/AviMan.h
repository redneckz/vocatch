////////////////////////////////////////////////////////////////////
// CAviMan - AVI file manager - support read and write
// from AVI.
// octotber 2002
////////////////////////////////////////////////////////////////////



#ifndef _AVIMAN_H

#define _AVIMAN_H


#include <vfw.h>

#define MAX_AVI_STREAM 50

class CAviMan
{

	DWORD m_dwStartFrame;							//number of current frame
	DWORD m_dwCurrentFrame;							//number of current frame
	DWORD m_dwTotalFrame;							//total frame in avi

	////////Picture format members/////
	BITMAPINFO m_bmpHeader;
	DWORD	m_dwImageSize;


	/////////Avi members///////////////
	PAVIFILE	    m_aviFile;						// the current file
	PAVISTREAM      m_aviStream[MAX_AVI_STREAM];	// the current streams
	PGETFRAME	    m_aviGetFrame;					// data for decompressing video
	PAVISTREAM		m_aviVideoStream;			    //videostream
	AVISTREAMINFO	m_aviStreamInfo;				//information about videostream

	DWORD   m_dwScale;								//frame rate
	DWORD	m_dwRate;
	DWORD	m_dwNumStreams;							//number of streams in avi file



public:

	//contructor - init avi library
	CAviMan();

	//destructir - free avi library
	~CAviMan();

	//open avi file. preapre avi streams
	OpenAviFile(LPTSTR szFileName);

	//close Avi file. Close all avi streams
	CloseAviFile();

	//get bitmpainfoheader information
	GetBmpHeader(BITMAPINFO *pBmpHeader);

	//go to the start of film
	ToStart();

	//go to th end of film
	ToEnd();

	//get frame - we not give frame number- get next frame
	DWORD GetFrame(LPBYTE pImageData, DWORD dwNumFrame = -1);

	DWORD	CAviMan::GetImageSize();

};



#endif //_AVIMAN_H