// Codec.h: interface for the CCodec class.
//
//////////////////////////////////////////////////////////////////////

#include "..\..\..\include\rdvp.h"

#include "DrawWindow.h"
#include "data2compress.h"
#include <afxtempl.h>


#if !defined(AFX_CODEC_H__A76FE149_D45F_4051_9F5E_B96E43064690__INCLUDED_)
#define AFX_CODEC_H__A76FE149_D45F_4051_9F5E_B96E43064690__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


enum	
{
	CODEC_STOP = 0x00000001,
	CODEC_START = 0x00000002,
	CODEC_SHOW = 0x00000004
};



typedef struct  SCharacter_str
{
	DWORD	dwDecompressTime;			//average decompressed tim in ms
	DWORD	dwCompressTime;			//compressed time
	DWORD	dwAvgFrameSize;			//avrage frame size in Kb
	float	fCompression;				//compression ratio
	DWORD	dwFrameNum;				//number of processed frames
	DWORD	dwTotalSize;				//total size of compressed data

} SCharacter;



class CCodec  
{
public:

	CCodec(HWND hwndParent);								//constructor - init codec,thread,internal buffer
	virtual ~CCodec();										//destructor - delete codec,thread,buffers,events



	void	CCodec::SetParams(BYTE	bQuality,BYTE	bSense,DWORD  dwInraFrame,
								SIZE	szSrcRes,DWORD	dwSrcCF,
								SIZE	szDstRes,DWORD	dwDstCF);

	//start compress thread
	void CCodec::Start(void);				

	//start compress thread
	void CCodec::Stop(void);				

	void	Destroy(void);

	//get charcteristic of the codec ( compression time, average frame size..)
	void GetCharacteristics(SCharacter *pCharacter);

	void	GetInputResolution(SIZE	szRes);

	void	CCodec::Reset();

private:

	static DWORD WINAPI  CompressThread(LPVOID pParam);		//threads for comperss/decompress
	CompressEngine();								//Engine for CompressThread


	ProcessImageEngine(const SData2Compress	*pSData);

	DWORD	GetImageSize(SIZE	szRes,DWORD	dwCF);

public:

	//call from VideoCapturer for compress live video
	static void _stdcall ProcessImage(const SData2Compress	*pSData,LPVOID pParam);

private:

	CDrawWindow		m_wndSourceImage;			//window show source image
	CDrawWindow		m_wndDecompressImage;		//window show decompressed image
	DWORD			m_dwSrcBitCount;
	DWORD			m_dwDstBitCount;

	///////////Codec Variables/////////////////////////////
	DWORD			m_dwCoderID;
	DWORD			m_dwDecoderID;

	SRdvpCompress   m_sCompress;
	SRdvpDecompress m_sDecompress;

	///////////Multithreading variables////////////////////
	HANDLE	m_hCompressThread;			//Compress/Decompress Thread

private:

	HWND			m_hwndParent;

	DWORD				m_fStop;		//flag (Suspend,Compress,Show);

	HANDLE			m_hCompressEvent;	//Compress/Decompress Events
	CList <SData2Compress*,SData2Compress*>	m_lstReadyImages;

	CRITICAL_SECTION	m_cs;
	CRITICAL_SECTION	m_csChar;


	HANDLE	m_hExitEvent;				//Exit Events
	HANDLE	m_hOnlyShowEvent;			//Exit Events

	SCharacter	m_sCharacter;				//charactristics of the codec

	DWORD	m_dwSourceSize;
	//////////Internal variables///////////////////////////
	LPBYTE	m_pCompressData;			//buffer for compressed data
	LPBYTE	m_pDecompressData;			//buffer with compress/decompress data
	LPBYTE	m_pSourceData;				//buffer with source image data

	SIZE	m_szSrcRes;					//source resoulution (iW x iH)
	SIZE	m_szDstRes;					//destination resolution (must be equal or less than source)
	DWORD	m_dwSrcCF;					//Source Color Format (RGB24,RGB32,YUV411 Packed,Y8 Grayscale)
	DWORD	m_dwDstCF;					//Destination Color Format (RGB24,RGB32,Y8 Grayscale)

	BYTE	m_bQuality;					//Quality of Compress
	BYTE	m_bSense;					//Static Sense of Codec ( for detect static MacroBlocks )

};

#endif // !defined(AFX_CODEC_H__A76FE149_D45F_4051_9F5E_B96E43064690__INCLUDED_)
