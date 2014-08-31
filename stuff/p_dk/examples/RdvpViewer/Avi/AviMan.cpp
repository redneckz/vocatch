#include "stdafx.h"
#include "AviMan.h"

/////////////////////////////////////////////////////////////
// class CAviMan
//


	/////////////////////////////////////////////////////////
	//contructor - init avi library
	/////////////////////////////////////////////////////////
	CAviMan::CAviMan()
	{
		m_aviFile = NULL;
		m_aviVideoStream = NULL;
		m_aviGetFrame = NULL;
		m_dwNumStreams = MAX_AVI_STREAM-1;

		for (DWORD i = 0; i < MAX_AVI_STREAM; i++)
				m_aviStream[i] = NULL;


		AVIFileInit();			//init avi lib

	}

	/////////////////////////////////////////////////////////
	//destructir - free avi library
	/////////////////////////////////////////////////////////
	CAviMan::~CAviMan()
	{
		AVIFileExit();			//close avi lib

	}

	/////////////////////////////////////////////////////////
	//open avi file. preapre avi streams
	/////////////////////////////////////////////////////////
	CAviMan::OpenAviFile(LPTSTR szFileName)
	{
		LONG hr;

	    hr = AVIFileOpen(&m_aviFile, szFileName, 0, 0L);			//open AVI file

		if(hr!=0)
		{
			return 1;
		}

		PAVISTREAM	aviStream;

		//
		// Get all the streams from the new file
		//
		for (DWORD i = 0; i <= MAX_AVI_STREAM; i++)
		{
			if (AVIFileGetStream(m_aviFile, &aviStream, 0L, i) != AVIERR_OK)
			break;

			if (i == MAX_AVI_STREAM) {
				AVIStreamRelease(aviStream);
				break;
			}

			m_aviStream[i] = aviStream;				//save all streams
		}


		m_dwNumStreams = i;					//save real number of streams

		//
		// Couldn't get any streams out of this file
		//
		if (i == 0 && i != MAX_AVI_STREAM)
		{
	   //   ErrMsg(TEXT("Unable to open %s"), lpszFile);
			if (m_aviFile)
				AVIFileRelease(m_aviFile);
			return 1;
		}



		AVISTREAMINFO     aviStreamInfo;


		m_aviVideoStream = NULL;

		//
		// Walk through and init all streams loaded
		//
		for (i = 0; i < m_dwNumStreams; i++)
		{

			AVIStreamInfo(m_aviStream[i], &aviStreamInfo, sizeof(aviStreamInfo));

			//
			// Initialize video streams for getting decompressed frames to display
			//
			if (aviStreamInfo.fccType == streamtypeVIDEO) 
			{

		
				if (m_aviVideoStream == NULL) 
				{

					m_dwScale=aviStreamInfo.dwScale;
					m_dwRate =aviStreamInfo.dwRate;

					//
					// Remember the first video stream --- treat it specially
					//
					m_aviVideoStream = m_aviStream[i];

					m_aviGetFrame = AVIStreamGetFrameOpen(m_aviStream[i], NULL);

				}

			}//if aviStreamInfo==streamVideo 

		}//for i=0..dwNumStreams


		m_dwCurrentFrame = 0;
		m_dwStartFrame = 0;
		m_dwTotalFrame  = 0;

		return 0;

	}

	/////////////////////////////////////////////////////////
	//close Avi file. Close all avi streams
	/////////////////////////////////////////////////////////
	CAviMan::CloseAviFile()
	{
		if(m_aviGetFrame)
			AVIStreamGetFrameClose(m_aviGetFrame);			//close get frame info

		for (DWORD i = 0; i < m_dwNumStreams; i++)
		{
			if(m_aviStream[i])
				AVIStreamRelease(m_aviStream[i]);
		}

		if (m_aviFile) AVIFileRelease(m_aviFile);

		m_aviFile = NULL;
		m_aviVideoStream = NULL;
		m_aviGetFrame = NULL;

		m_dwNumStreams = MAX_AVI_STREAM-1;


	}

	/////////////////////////////////////////////////////////
	//get bitmpainfoheader information
	/////////////////////////////////////////////////////////
	CAviMan::GetBmpHeader(BITMAPINFO *pBmpHeader)
	{
		LONG bmp_size = sizeof( BITMAPINFO ); 

		AVIStreamReadFormat(m_aviVideoStream, 0, &(pBmpHeader->bmiHeader), &bmp_size); // Read format 

		memcpy(&m_bmpHeader,pBmpHeader,bmp_size);

		m_dwStartFrame = AVIStreamStart(m_aviVideoStream);
		m_dwTotalFrame = AVIStreamEnd(m_aviVideoStream);				//get number of frames

		m_dwImageSize = m_bmpHeader.bmiHeader.biWidth * m_bmpHeader.bmiHeader.biHeight;

		switch(m_bmpHeader.bmiHeader.biBitCount)
		{
		case(24):m_dwImageSize*=3;break;
		case(16):m_dwImageSize*=2;break;			
		}

	}

	DWORD	CAviMan::GetImageSize()
	{
		return	m_dwImageSize;
	}

	/////////////////////////////////////////////////////////
	//go to the start of film
	/////////////////////////////////////////////////////////
	CAviMan::ToStart()
	{
		m_dwCurrentFrame = m_dwStartFrame;
	}

	/////////////////////////////////////////////////////////
	//go to th end of film
	/////////////////////////////////////////////////////////
	CAviMan::ToEnd()
	{
		m_dwCurrentFrame = m_dwTotalFrame;
	}

	/////////////////////////////////////////////////////////
	//get frame - we not give frame number- get next frame
	/////////////////////////////////////////////////////////
	DWORD CAviMan::GetFrame(LPBYTE pImageData,DWORD dwNumFrame)
	{
		if(dwNumFrame==-1) dwNumFrame = (++m_dwCurrentFrame);				//if frame number not set 

		if(m_dwCurrentFrame>=m_dwTotalFrame) return 1;

		LPBYTE pAviFrame = (BYTE*)AVIStreamGetFrame(m_aviGetFrame,dwNumFrame);

		if((pAviFrame) && (pImageData))
		{
			DWORD dwWidth = m_bmpHeader.bmiHeader.biWidth;
			DWORD dwHeight = m_bmpHeader.bmiHeader.biHeight;

			switch(m_bmpHeader.bmiHeader.biBitCount)
			{
			case(24):dwWidth*=3;break;
			case(16):dwWidth*=2;break;			
			}

			LPBYTE	pFrom = (pAviFrame+sizeof( BITMAPINFO )) + m_dwImageSize - dwWidth-1;
			LPBYTE	pTo = pImageData;

			for(DWORD i=0;i<dwHeight;i++,pFrom-=dwWidth,pTo+=dwWidth)
				memcpy(pTo,pFrom,dwWidth);

//			memcpy(pImageData,(pAviFrame+sizeof( BITMAPINFO )),m_dwImageSize);

		}
		return 0;

	}

