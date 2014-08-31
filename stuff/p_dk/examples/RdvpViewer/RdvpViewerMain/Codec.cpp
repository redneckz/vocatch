// Codec.cpp: implementation of the CCodec class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MainApp.h"
#include "Codec.h"
#include "data2compress.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//maxmimal neede buffer for (Full Screen RGB32 Image)
#define BUFFER_SIZE (768*576*4)


CCodec::CCodec(HWND hwndParent)
{
	m_hwndParent = hwndParent;

	// Create Compress thread (in a suspended state)
	DWORD dwThreadID;		

	//thread for compress/decompress routine
	m_hCompressThread = CreateThread(NULL,0,CompressThread,this,0,&dwThreadID);

	// Create "stop Compress" auto-reset event
	m_hCompressEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	m_fStop = CODEC_STOP | CODEC_SHOW;

	// Create  "show Compress" auto-reset event
	m_hExitEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	m_hOnlyShowEvent = CreateEvent(NULL,FALSE,FALSE,NULL);

	m_pCompressData = new BYTE[BUFFER_SIZE];
	m_pDecompressData = new BYTE[BUFFER_SIZE];

	//////////Create Codec Streams//////////////////////
	// initialize library
	SRdvpInitialize rdvp_init;

	rdvp_init.dwSize = sizeof(rdvp_init);

	// Use Demo Key passwords
	rdvp_init.pdwCustomerPassword[0] = 0x9592CB4F;
	rdvp_init.pdwCustomerPassword[1] = 0xF3B62AC0;
	rdvp_init.pdwCustomerPassword[2] = 0xFCB8802F;
	rdvp_init.pdwCustomerPassword[3] = 0x9CE9CEF9;

	RdvpInitialize(&rdvp_init);
	

	//create two streams - compress and decompress
	RdvpCreateVideoStream(DVPACK_1_41,&m_dwCoderID);
	RdvpCreateVideoStream(DVPACK_1_41,&m_dwDecoderID);

	//get memeory
	m_sCompress.pQuality = new SRdvpQualityParam;


	//Fill m_sCompress and m_sDecompress structure
	m_sCompress.dwSize = sizeof(m_sCompress);
	m_sCompress.dwSrcBufferSize = BUFFER_SIZE;
	m_sCompress.pDestBuffer = m_pCompressData;
	m_sCompress.dwDestBufferSize = BUFFER_SIZE;

	m_sDecompress.dwFlags = 0;
	m_sDecompress.dwSize = sizeof(m_sDecompress);
	m_sDecompress.pSrcBuffer = m_pCompressData;
	m_sDecompress.dwDestBufferSize = BUFFER_SIZE;
	m_sDecompress.pDestBuffer = m_pDecompressData;


	//create critical section
	InitializeCriticalSection(&m_cs);
	InitializeCriticalSection(&m_csChar);
	

	CString	strMyClass;
	
	strMyClass = AfxRegisterWndClass (CS_VREDRAW | CS_HREDRAW,
						::LoadCursor(NULL, IDC_ARROW),
						(HBRUSH) ::GetStockObject(WHITE_BRUSH),
						0);


	m_wndSourceImage.CreateEx(0 , strMyClass, "Source Image",	WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX |
		WS_OVERLAPPED |  WS_VISIBLE | WS_POPUP, CRect(0,0,20,20),CWnd::GetDesktopWindow(),NULL);

	m_wndDecompressImage.CreateEx(0 , strMyClass, "Decompressed Image",	WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX |
		WS_OVERLAPPED |  WS_VISIBLE | WS_POPUP, CRect(0,0,20,20),CWnd::GetDesktopWindow(),NULL);

	m_wndSourceImage.SetWindowPos(0,0,0,192,164,  SWP_NOZORDER |
			 SWP_SHOWWINDOW | SWP_NOACTIVATE );

	m_wndDecompressImage.SetWindowPos(0,500,0 ,192,164,  SWP_NOZORDER |
			 SWP_SHOWWINDOW | SWP_NOACTIVATE );

}


CCodec::~CCodec()
{

		//delete compress tread and events
		CloseHandle(m_hCompressThread);

		CloseHandle(m_hCompressEvent);
		CloseHandle(m_hExitEvent);
		CloseHandle(m_hOnlyShowEvent);


		m_wndSourceImage.DestroyWindow();
		m_wndDecompressImage.DestroyWindow();

		//delete Coder and Decoder////////////////

		if(m_pCompressData)
			delete []m_pCompressData;

		if(m_pDecompressData)
			delete []m_pDecompressData;

		while(!m_lstReadyImages.IsEmpty())
		{
			SData2Compress *p = m_lstReadyImages.RemoveHead();
			delete p;
		}


		//create critical section
		DeleteCriticalSection(&m_cs);
		DeleteCriticalSection(&m_csChar);

		memset(&m_sCharacter,0,sizeof(m_sCharacter));

		RdvpFreeVideoStream(m_dwCoderID);
		RdvpFreeVideoStream(m_dwDecoderID);

		delete m_sCompress.pQuality;
}

///////////////////////////////////////////////////////////////////
// function calculate bites per pixel accroding color format
///////////////////////////////////////////////////////////////////
DWORD	CCodec::GetImageSize(SIZE	szRes,DWORD	dwCF)
{

	DWORD	dwSize = szRes.cx*szRes.cy;

	switch(dwCF)
	{
	case(RDN_CF_RGB24):return dwSize*3;
	case(RDN_CF_RGB32):return dwSize*3;
	case(RDN_CF_Y8):return dwSize;
	case(RDN_CF_YUV411_PACKED):return dwSize*3/2;		
	}

	return 1;


}

///////////////////////////////////////////////////////////////////
// set parameters of codec (Quality and Sense of Compresor)
// dwIntraFrame set distance between Inter Frame
///////////////////////////////////////////////////////////////////
void	CCodec::SetParams(BYTE	bQuality,BYTE	bSense,DWORD  dwIntraFrame,SIZE	szSrcRes,DWORD	dwSrcCF,SIZE	szDstRes,DWORD	dwDstCF)
{
	EnterCriticalSection(&m_cs);
	
	//set destination resolution and Color Format
	m_szDstRes = szDstRes;
	m_dwDstCF = dwDstCF;
	
	m_szSrcRes = szSrcRes;
	m_dwSrcCF = dwSrcCF;
	
	m_sCompress.pQuality->dwQuality = bQuality;
	m_sCompress.pQuality->dwSense = bSense;
	
	RdvpResetStream(m_dwCoderID);
	RdvpResetStream(m_dwDecoderID);
	
	
	LeaveCriticalSection(&m_cs);
	
	memset(&m_sCharacter,0,sizeof(m_sCharacter));
}

///////////////////////////////////////////////////////////////////
//call from VideoCapturer for compress live video
///////////////////////////////////////////////////////////////////
void CCodec::ProcessImage(const SData2Compress	*pSData,LPVOID pParam)
{

	CCodec *pThis = (CCodec*)pParam;		//get this of class
	pThis->ProcessImageEngine(pSData);
}


///////////////////////////////////////////////////////////////////
// function add new image to the Image List and set Event for compress
///////////////////////////////////////////////////////////////////
CCodec::ProcessImageEngine(const SData2Compress	*pSData)
{

	if(m_fStop & CODEC_START)						//compress/decompress, show result
	{
		EnterCriticalSection(&m_cs);
		//Duplicate Data
		SData2Compress *pSFrame = new SData2Compress;;
		memcpy(pSFrame,pSData,sizeof(SData2Compress));

		//add in list
		m_lstReadyImages.AddTail(pSFrame);

		//set event for compress
		SetEvent(m_hCompressEvent);

		LeaveCriticalSection(&m_cs);

	}
	else if(m_fStop & CODEC_SHOW)						//show captured video(without compress)
	{
		EnterCriticalSection(&m_cs);
		//Duplicate Data
		SData2Compress *pSFrame = new SData2Compress;
		memcpy(pSFrame,pSData,sizeof(SData2Compress));

		//add in list
		m_lstReadyImages.AddTail(pSFrame);

		//set event for compress
		SetEvent(m_hOnlyShowEvent);

		LeaveCriticalSection(&m_cs);

	}
	else
	{
		if(pSData->hSemaphore) ReleaseSemaphore(pSData->hSemaphore,1,NULL);
		else SetEvent(pSData->hReadyEvent);
	}


}


///////////////////////////////////////////////////////////////////
// thread for decompress/compress video
///////////////////////////////////////////////////////////////////
DWORD WINAPI CCodec::CompressThread(LPVOID pParam)
{
	CCodec *pThis = (CCodec*)pParam;		//get this of class
	pThis->CompressEngine();

	return 0;
}


///////////////////////////////////////////////////////////////////
// CompressEngine - the hurt of compress/decompress operation
//
///////////////////////////////////////////////////////////////////
CCodec::CompressEngine()
{

	BOOL	hCompress;
	BOOL	hDecompress;

	LARGE_INTEGER	tStart,tStop;			//star and stop time
	LARGE_INTEGER	tFreq;					//internal system frequency

	QueryPerformanceFrequency(&tFreq);		//get frequency of the system


	DWORD dwBytesWritten;				//number of bytes after decompress
	HANDLE hEvents[3];					//events

	// Events
	hEvents[0] =  m_hExitEvent;			//exit event
	hEvents[1] =  m_hCompressEvent;		//compress event
	hEvents[2] =  m_hOnlyShowEvent;		//pnly show capture event (without compress)


	while(true)							//infinity cycle
	{

		//wait for 2 events - 'Exit' or 'Compress'
		DWORD dwWait = WaitForMultipleObjects(3,hEvents,FALSE,INFINITE);


		if(dwWait == WAIT_OBJECT_0)			//we get Exit Event -> Terminate thread
		{
			break;
		}
		else if(dwWait == WAIT_OBJECT_0+1)	//we get Compress Event -> Compress/Decompress Data and Show it	
		{

				EnterCriticalSection(&m_cs);

				SData2Compress *pImage = m_lstReadyImages.RemoveHead();


				////////////////////////////////////////////////////////////////////
				// Compressor

				m_sCompress.pSrcBuffer = pImage->pDataPtr;				 //set Source Data
				m_sCompress.szSrcResolution = pImage->sizeResolution;	 //set Captured Resolution
				m_sCompress.dwSrcColorFormat = pImage->dwColorFormat;	 //Set Captured Format
				m_sCompress.dwSrcBufferSize = pImage->dwDataSize;

				m_sCompress.szDestResolution = m_szDstRes;

				// select proper destination color format
				if(m_sCompress.dwSrcColorFormat == RDN_CF_Y8)
					m_sCompress.dwDestColorFormat = RDN_CF_Y8;
				else
					m_sCompress.dwDestColorFormat = m_dwDstCF == RDN_CF_Y8 ? RDN_CF_Y8 : RDN_CF_YUV411_PACKED;

				QueryPerformanceCounter(&tStart);						//Calc Start Time of COmpress

				hCompress = RdvpCompressFrame(m_dwCoderID, &m_sCompress, &dwBytesWritten); //Compress Image


				QueryPerformanceCounter(&tStop);						//Calc Start Time
				tStop.QuadPart -= tStart.QuadPart;						//Total time

				EnterCriticalSection(&m_csChar);

				m_sCharacter.dwCompressTime = (DWORD)(tStop.QuadPart*10000/tFreq.QuadPart);	//Time in ms
				
				////////////////////////////////////////////////////////////////////
				// Decompressor

				m_sDecompress.dwSrcBufferSize = dwBytesWritten;			//how many bytes we write
				m_sDecompress.dwColorFormat = m_dwDstCF;

				//set dest buffer

				QueryPerformanceCounter(&tStart);						//Calc Start Time of Decompress

				if(hCompress)
					hDecompress =  RdvpDecompressFrame(m_dwDecoderID, &m_sDecompress);	//decompress
				else
					hDecompress = 0;


				QueryPerformanceCounter(&tStop);						//Calc Start Time
				tStop.QuadPart -= tStart.QuadPart;						//Total time
				m_sCharacter.dwDecompressTime = (DWORD)(tStop.QuadPart*10000/tFreq.QuadPart);	//Time in ms

				m_sCharacter.dwFrameNum++;								//increase frame counter
				m_sCharacter.dwTotalSize += dwBytesWritten;				//calculate total size

				if(dwBytesWritten)
				m_sCharacter.dwAvgFrameSize = m_sCharacter.dwTotalSize/m_sCharacter.dwFrameNum;	//calculate avrage frame size


				m_dwSourceSize = GetImageSize(m_szDstRes,m_dwDstCF);	//calculate surce size of image

				m_sCharacter.fCompression = ((float)m_dwSourceSize*m_sCharacter.dwFrameNum/m_sCharacter.dwTotalSize);	//calc compression ratio

				LeaveCriticalSection(&m_cs);

				LeaveCriticalSection(&m_csChar);

				//send message for print statical data on the dialog bar
				if(m_hwndParent && IsWindow(m_hwndParent))
					::PostMessage(m_hwndParent,WM_USER_PRINTDATA,NULL,NULL);

				////////////////////////////////////////////////////////////////////
				// Show Read Image and Compress/Decompress Image	
				
				if(hCompress)
				m_wndSourceImage.SetParams(		pImage->sizeResolution.cx,			//source iW
												pImage->sizeResolution.cy,			//source iH
												pImage->dwColorFormat,				//source Color Format
												(LPBYTE)pImage->pDataPtr);			//source Data

				if(hDecompress)
				m_wndDecompressImage.SetParams( m_szDstRes.cx,						//dest	iW
												m_szDstRes.cy,						//dest  iH
												m_dwDstCF,							//dest  Color Format
												m_pDecompressData);					//dest  Data


				if(pImage->hSemaphore) ReleaseSemaphore(pImage->hSemaphore,1,NULL);
				else SetEvent(pImage->hReadyEvent);

				delete	pImage;

				if(!hDecompress || !hCompress)
					::PostMessage(m_hwndParent, WM_USER_CODEC_ERROR, 0,0);
		}
		else if(dwWait == WAIT_OBJECT_0+2)	//we get Only Show Event -> Show Captured Video
		{

				EnterCriticalSection(&m_cs);

				SData2Compress *pImage = m_lstReadyImages.RemoveHead();				//extract element

				m_wndSourceImage.SetParams( pImage->sizeResolution.cx,				//source iW
											pImage->sizeResolution.cy,				//source iH
											pImage->dwColorFormat,					//source Color Format
											(LPBYTE)pImage->pDataPtr);				//source Data

				SetEvent(pImage->hReadyEvent);

				delete pImage;														//delete extracted element

				LeaveCriticalSection(&m_cs);

		}

	}//while(true)


	return 0;
}


void CCodec::Start(void)
{


	if(!(m_fStop & CODEC_START))					//if we not alrady start
	{
		RdvpResetStream(m_dwCoderID);
		RdvpResetStream(m_dwDecoderID);

		m_sCharacter.dwFrameNum = 0;				//number of processed frames
		m_sCharacter.dwTotalSize = 0;				//total size of compressed data


		m_wndSourceImage.RedrawWindow();
		m_wndDecompressImage.RedrawWindow();

		m_fStop |= CODEC_START;
	}
}


void CCodec::Stop(void)
{
	m_fStop = (m_fStop^(m_fStop&CODEC_START));
}


void	CCodec::Destroy(void)
{
	m_fStop = CODEC_STOP;

	SetEvent(m_hExitEvent);

	// Stop Compress thread
	WaitForSingleObject(m_hCompressThread,INFINITE);

}


//////////////////////////////////////////////////////////////////
// copy codec charcterisic to the structure
//////////////////////////////////////////////////////////////////
void CCodec::GetCharacteristics(SCharacter *pCharacter)
{
	EnterCriticalSection(&m_csChar);
		memcpy(pCharacter,&m_sCharacter,sizeof(m_sCharacter));
	LeaveCriticalSection(&m_csChar);
}

//////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////
void	CCodec::Reset()
{
	EnterCriticalSection(&m_cs);

	while(!m_lstReadyImages.IsEmpty())
	{
		SData2Compress *p = m_lstReadyImages.RemoveHead();
//		if(p->hSemaphore) ReleaseSemaphore(p->hSemaphore,1,NULL);
		delete p;
	}

	LeaveCriticalSection(&m_cs);
}
