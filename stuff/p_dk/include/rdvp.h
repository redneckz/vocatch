/*
rdvp.h

This software is supplied under the terms of a license agreement or
nondisclosure agreement with PENTACON Corporation and may not be copied
or disclosed except in accordance with the terms of that agreement.

Copyright (c) PENTACON Corporation, 1995-2006. All Rights Reserved.

Version:	3.0
Jule 2006
*/

#if !defined __RVDP_H
#define __RVDP_H

#include <wtypes.h>
#include "rdnbase.h"

/* Possible codec Types */
enum   EDVPCodecType
{
	DVPACK_1_41 = 0x00000001
};

/* Initialization parameters */
typedef struct _SRdvpInitialize
{
	DWORD	dwSize;
	DWORD	pdwCustomerPassword[4];
	DWORD	dwUserDataSize;

} SRdvpInitialize;

#define SRdvpInitialize_V1 size_including_member(SRdvpInitialize,dwUserDataSize)

/* Library version information */
typedef struct _SRdvpVersionInfo
{
	DWORD	dwSize;						/* size of structure */
	DWORD	wMajor;						/* Major version of library */
	DWORD	wMinor;						/* Minor version of library */
	DWORD	wBuild;						/* Build number of library */

} SRdvpVersionInfo;

#define SRdvpVersionInfo_V1 size_including_member(SRdvpVersionInfo,wBuild)


/* Codec Capabilities Masks */
enum ERdvpCapsMask
{
	RDVP_CODEC_CAPSMASK_NAME			= 0x00000001,			/*  Get codec Name */
	RDVP_CODEC_CAPSMASK_VERSION			= 0x00000002,			/*  Get Version (Minor and Major) */
	RDVP_CODEC_CAPSMASK_RESOLUTION		= 0x00000004,			/*  Get Maximum and minimus resolution and vertical and horizontal picture alignment*/
	RDVP_CODEC_CAPSMASK_COLORFORMAT		= 0x00000008,			/*  Get possible input,output and compressed color formats */
	RDVP_CODEC_CAPSMASK_ALLINFO			= 0xffffffff
};


/* Codec capabilities */
typedef struct _SRdvpCodecCaps
{
	DWORD	dwSize;
	DWORD	dwMask;								/* mask of capabilities (set of EDVPCaps Mask) */
	BSTR	bstrName;							/* name of codec */
	DWORD	dwVersionMajor;						/* major version */
	DWORD	dwVersionMinor;						/* minor version */
	SIZE	sizeResolutionAlignment;				/* vertical and horizontal picture alignment */
	SIZE	sizeMinResolution;					/* minimal possible resolution */
	SIZE	sizeMaxResolution;					/* maximal possible resolution */
	DWORD	dwInputCFMask;						/* possible input color formats */
	DWORD	dwCompressedCFMask;					/* possible compressed color format */
	DWORD	dwOutputCFMask;						/* possible output formats */

} SRdvpCodecCaps;

/* size of SRdvpCodecCaps structure ver 1.*/
#define SRdvpCodecCaps_V1 size_including_member(SRdvpCodecCaps,dwOutputCFMask)

/* Stream Info */
typedef struct _SRdvpStreamInfo
{
	DWORD	dwSize;
	DWORD	dwInterFrameCount;						/* Difference frame count */
	DWORD	dwCodecID;

} SRdvpStreamInfo;


#define SRdvpStreamInfo_V1 size_including_member(SRdvpStreamInfo,dwCodecID)


/* Quality parameters */
typedef struct	_SRdvpQualityParam
{
	DWORD	dwSize;						/* size of structure */
	DWORD	dwQuality;					/* quality of compression (1..10) */
	DWORD	dwSense;					/* sense	(0..8) */

} SRdvpQualityParam;

#define SRdvpQualityParam_V1 size_including_member(SRdvpQualityParam,dwSense)

/* Compress parameters */
typedef struct _SRdvpCompress
{

	DWORD		dwSize;
	SIZE		szSrcResolution;		/*  Source Image Size */
	SIZE		szDestResolution;		/*  Destimation image size (must be less or equal of source image size) */
	DWORD		dwSrcColorFormat;		/*  Source color format */
	DWORD		dwDestColorFormat;		/*  Destination color format */
	LPVOID		pSrcBuffer;				/*  Source data buffer (contain image data) */
	DWORD		dwSrcBufferSize;		/*  Size of source buffer in bytes */
	LPVOID		pDestBuffer;			/*  Destination data buffer (contain compressed image) */
	DWORD		dwDestBufferSize;		/*  Size of destination buffer */
	SRdvpQualityParam	*pQuality;		/*  Quality paramaters */

}  SRdvpCompress;

#define SRdvpCompress_V1 size_including_member(SRdvpCompress,pQuality)

enum ERdvpDecompressMode
{
	RDVP_DECOMPRESS_NORMAL		= 0x0000,	/* normal decompress */
	RDVP_DECOMPRESS_NODEST		= 0x0001,	/* no destination decompress */
};

/* Decompress Parameters */
typedef struct _SRdvpDecompress
{
	DWORD	dwSize;				
	LPVOID	pSrcBuffer;					/*  Source data buffer (contain compressed image) */
	DWORD	dwSrcBufferSize;			/*  Size of source buffer in bytes */
	LPVOID	pDestBuffer;				/*  Destination data buffer(contain decompressed image) */
	DWORD	dwDestBufferSize;			/*  Size of destination buffer */
	DWORD	dwColorFormat;				/*  Result image color format */
	DWORD	dwFlags;					/*  flags (see enum) */

} SRdvpDecompress;

#define SRdvpDecompress_V1 size_including_member(SRdvpDecompress,dwFlags)

/* Codec frame Types */
enum ERdvpFrameType
{
	RDVP_FRAME_REFERENCE	= 1,	/* Reference frame (Intra frame) */
	RDVP_FRAME_DIFFERENCE	= 2		/* Difference frame type */
};

/* Compressed frame info */
typedef struct _SRdvpFrameInfo
{
	DWORD	dwSize;
	DWORD	dwCodecID;		/* Used codec identifier */
	DWORD	dwFrameType;	/* Frame type (Difference or reference) */
	SIZE	szResolution;	/* Size of image */
	DWORD	dwColorFormat;	/* Color format of image */

} SRdvpFrameInfo;

#define SRdvpFrameInfo_V1 size_including_member(SRdvpFrameInfo,dwColorFormat)

/* Data from HASP user data area */
typedef struct _SRdvpKeyData
{
	DWORD	dwSize;			/* size of structure */
	DWORD	dwDataOffset;	/* Offset from the beginning of the user data area (in bytes) */
	DWORD	dwDataSize;		/* Number of bytes to be read from the user data area */
	BYTE*	pDataPtr;		/* Pointer to the buffer that receives the data read from the user data area */
} SRdvpKeyData;

#define SRdvpKeyData_V1 size_including_member(SRdvpKeyData,pDataPtr)


/* initialize library */
BOOL __stdcall RdvpInitialize(SRdvpInitialize* pInit);

/* return RDVL Library version description */
BOOL __stdcall RdvpGetVersion(SRdvpVersionInfo *pVersionInfo);

/* Enumerate all possible Codecs */
BOOL __stdcall RdvpEnumCodecs(CADWORD	*pcadwCodecs);

/* Get Codec capabilities */
BOOL __stdcall RdvpGetCodecCaps(DWORD dwCodecID, SRdvpCodecCaps *pCodecCaps);

/* Create new Stream with a specified Codec */
BOOL __stdcall RdvpCreateVideoStream(DWORD	dwCodecID, DWORD  *pdwStreamID);

/* Delete Stream */
BOOL __stdcall RdvpFreeVideoStream(DWORD	dwStreamID);

/* Enumarate all present streams */
BOOL __stdcall RdvpEnumVideoStreams(CADWORD	*pcadwStreams);

/* Get Stream Information */
BOOL __stdcall RdvpGetStreamInfo(DWORD dwStreamID, SRdvpStreamInfo *pStreamInfo);

/* Reset stream (to form a Reference frame) */
BOOL __stdcall RdvpResetStream(DWORD dwStreamID);

/* Compress Frame with a specified stream */
BOOL __stdcall RdvpCompressFrame(DWORD	dwStreamID, SRdvpCompress *pCompress, DWORD *dwBytesWritten);

/* Decompress Frame with a specified stream */
BOOL __stdcall RdvpDecompressFrame(DWORD dwStreamID, SRdvpDecompress *pDecompress);

/* Get Frame info */
BOOL __stdcall RdvpGetFrameInfo(LPVOID pCompressedFrame, DWORD dwCompressedFrameSize, SRdvpFrameInfo *pFrameInfo);

/* Read data from the HASP key user data area */
BOOL __stdcall RdvpReadKeyData(SRdvpKeyData* pKeyData);

#endif // __RVDP_H