/*
rvcl.h

This software is supplied under the terms of a license agreement or
nondisclosure agreement with PENTACON Corporation and may not be copied
or disclosed except in accordance with the terms of that agreement.

Copyright (c) PENTACON Corporation, 1995-2006. All Rights Reserved.

Version: 3.0
July 2006
*/

#if !defined __RVCL_H__
#define __RVCL_H__

#include <windows.h>
#include "rdnbase.h"

#pragma pack(push,8)

#ifdef __cplusplus
extern "C" {
#endif

/* Video Formats */
#define RVCL_VF_NTSCM					0x00000001	/* NTSC (M) */
#define RVCL_VF_NTSCJ					0x00000002	/* NTSC (Japan) */
#define RVCL_VF_PALBDGHI				0x00000004	/* PAL (BDGHI) */
#define RVCL_VF_PALM					0x00000008	/* PAL (M) */
#define RVCL_VF_PALN					0x00000010	/* PAL (N) */
#define RVCL_VF_SECAM					0x00000020	/* SECAM */

/* Status Flags */
#define RVCL_STATUS_VIDEOPRESENT		0x00000001
#define RVCL_STATUS_VIDEOSOURCE			0x00000002
#define RVCL_STATUS_VIDEOFORMAT			0x00000004
#define RVCL_STATUS_RESOLUTION			0x00000008

/* Adjustments flags */
#define RVCL_VADJUST_BRIGHTNESS			0x00000001
#define RVCL_VADJUST_CONTRAST			0x00000002
#define RVCL_VADJUST_SATURATION			0x00000004
#define RVCL_VADJUST_HUE				0x00000008
#define RVCL_VADJUST_COMBRANGE          0x00000010


/* Maximum values */
#define RVCL_BT_MAX_BRIGHTNESS			255
#define RVCL_BT_MAX_HUE					255
#define RVCL_BT_MAX_CONTRAST			511
#define RVCL_BT_MAX_SATURATION			437

#define RVCL_CX_MAX_BRIGHTNESS			255
#define RVCL_CX_MAX_HUE					255
#define RVCL_CX_MAX_CONTRAST			255
#define RVCL_CX_MAX_SATURATION			255
#define RVCL_CX_MAX_COMBRANGE			1023

/* Default values */
#define RVCL_BT_DEFAULT_BRIGHTNESS		128
#define RVCL_BT_DEFAULT_HUE				0
#define RVCL_BT_DEFAULT_CONTRAST		216
#define RVCL_BT_DEFAULT_SATURATION		180

#define RVCL_CX_DEFAULT_BRIGHTNESS		128
#define RVCL_CX_DEFAULT_HUE				0
#define RVCL_CX_DEFAULT_CONTRAST		60
#define RVCL_CX_DEFAULT_SATURATION		110
#define RVCL_CX_DEFAULT_COMBRANGE		38



/* Grab Frame Flags */
#define RVCL_GRAB_FIELD_ODD				0x00000001
#define RVCL_GRAB_FIELD_EVEN			0x00000002
#define RVCL_GRAB_FIELD_NEAREST			0x00000004
#define RVCL_GRAB_NOSIGNAL				0x00000010
#define RVCL_GRAB_BOTTOM_UP				0x00000020
#define RVCL_GRAB_AFTER_SWITCH			0x00000040
#define RVCL_GRAB_SVIDEO				0x00000080

/*
   Color Formats 

   Note: 
   All RVCL_CF_XXX codes are obsolete.
   Use RDN_CF_XXX codes instead.
*/
#define RVCL_CF_RGB32					RDN_CF_RGB32
#define RVCL_CF_RGB24					RDN_CF_RGB24
#define RVCL_CF_RGB16					RDN_CF_RGB16
#define RVCL_CF_RGB15					RDN_CF_RGB15
#define RVCL_CF_Y8						RDN_CF_Y8
#define RVCL_CF_GRAYSCALE				RDN_CF_Y8
#define RVCL_CF_YUV422_PACKED			RDN_CF_YUV422_PACKED
#define RVCL_CF_YUV411_PACKED			RDN_CF_YUV411_PACKED
#define RVCL_CF_YUV422_PLANAR			RDN_CF_YUV422_PLANAR
#define RVCL_CF_YUV411_PLANAR			RDN_CF_YUV411_PLANAR

/* GPIO control */
#define RVCL_GPIO_SET_OUTPUT_ENABLE		0x00000001
#define RVCL_GPIO_SET_DATA				0x00000002
#define RVCL_GPIO_GET_OUTPUT_ENABLE		0x00000004
#define RVCL_GPIO_GET_DATA				0x00000008

/* Base audio frequency */
#define RVCL_BT8XX_AUDIO_FREQUENCY		119318
#define RVCL_CX2388X_AUDIO_FREQUENCY	48000

//0.2666666666 sec (Stereo buffer or Mono (for one chanel will be filled only half buffer))
#define RVCL_CX_AUDIO_BUFF_SIZE			51200 

/* Audio control mask */
#define RVCL_AUDIO_SET_FLAGS			0x00000001
#define RVCL_AUDIO_SET_GAIN				0x00000002
#define RVCL_AUDIO_GET_FLAGS			0x00010000
#define RVCL_AUDIO_GET_GAIN				0x00020000

/* Audio control flags */
#define RVCL_BT8XX_AUDIO_GAINBOOST		0x00000001

/* Audio block flags */
#define RVCL_CX2388X_AUDIO_STEREO		0x00000001
#define RVCL_CX2388X_AUDIO_RIGHT		0x00000002
#define RVCL_CX2388X_AUDIO_LEFT			0x00000004

/*
	Structures
*/
typedef struct _SRvclInitialize
{
	DWORD	dwSize;
	DWORD	pdwCustomerPassword[4];
	DWORD	dwUserDataSize;

} SRvclInitialize;

#define SRvclInitialize_V1 size_including_member(SRvclInitialize,dwUserDataSize)


typedef struct _SRvclVersionInfo
{
	DWORD	dwSize;					/* size of this struct */
	WORD	wMajor;					/* major version number */
	WORD	wMinor;					/* minor version number */
	WORD	wBuild;					/* build number */
} SRvclVersionInfo;

#define SRvclVersionInfo_V1 size_including_member(SRvclVersionInfo,wBuild)

enum EDeviceType
{
	E_DEVICE_BT8XX   = 0,
	E_DEVICE_CX2388X = 1,
};
typedef struct _SRvclDeviceInfo
{
	DWORD	dwSize;					/* size of this struct */
	WORD	wPciDeviceID;			/* PCI device ID */
	DWORD	dwPciBusNumber;			/* PCI bus number */
	DWORD	dwPciSlotNumber;		/* PCI slot number */
	DWORD	dwDeviceType;			/* Device type, look for EDeviceType*/
	CHAR	szDeviceDesc[16];		/* Device Name*/
} SRvclDeviceInfo;

#define SRvclDeviceInfo_V1 size_including_member(SRvclDeviceInfo, dwPciSlotNumber)
#define SRvclDeviceInfo_V2 size_including_member(SRvclDeviceInfo, dwDeviceType)

typedef struct _SRvclDeviceStatus
{
	DWORD	dwSize;
	DWORD	dwMask;
	BOOL	fVideoPresent;
	DWORD	dwVideoSource;
	DWORD	dwVideoFormat;
	SIZE	sizeMaxResolution;
	SIZE	sizeMinResolution;
	SIZE	sizeResolutionAlignment;
} SRvclDeviceStatus;

#define SRvclDeviceStatus_V1 size_including_member(SRvclDeviceStatus, sizeResolutionAlignment)

typedef struct _SRvclVideoAdjustments
{
	DWORD	dwSize;
	DWORD	dwMask;
	DWORD	dwBrightness;
	DWORD	dwContrast;
	DWORD	dwSaturation;
	DWORD	dwHue;
	DWORD	dwCombRange;
} SRvclVideoAdjustments;

#define SRvclVideoAdjustments_V1 size_including_member(SRvclVideoAdjustments,dwHue)
#define SRvclVideoAdjustments_V2 size_including_member(SRvclVideoAdjustments,lCombRange)


typedef struct _SRvclFrame
{
	DWORD	dwSize;
	DWORD	dwFlags;
	SIZE	sizeResolution;
	DWORD	dwColorFormat;
	void*	pBufferPtr;
	DWORD	dwBufferSize;

	/* V2 */
	DWORD	dwVideoSource;

} SRvclFrame;

#define SRvclFrame_V1 size_including_member(SRvclFrame,dwBufferSize)
#define SRvclFrame_V2 size_including_member(SRvclFrame,dwVideoSource)


typedef struct _SRvclGpioControl
{
	DWORD	dwSize;
	DWORD	dwMask;
	DWORD	dwOutputEnableMask;
	DWORD	dwOutputEnable;
	DWORD	dwDataMask;
	DWORD	dwData;

} SRvclGpioControl;

#define SRvclGpioControl_V1 size_including_member(SRvclGpioControl,dwData)


typedef struct _SRvclKeyData
{
	DWORD	dwSize;
	DWORD	dwDataOffset;
	DWORD	dwDataSize;
	BYTE*	pDataPtr;

} SRvclKeyData;

#define SRvclKeyData_V1 size_including_member(SRvclKeyData,pDataPtr)


typedef struct _SRvclAudioControl
{
	DWORD	dwSize;
	DWORD	dwMask;
	DWORD	dwFlags;
	DWORD	dwGain;

} SRvclAudioControl;

#define SRvclAudioControl_V1 size_including_member(SRvclAudioControl,dwGain)


typedef struct _SRvclAudioBlock
{
	DWORD	dwSize;
	DWORD	dwFlags;
	DWORD	dwBitsPerSample;
	DWORD	dwSamples;
	void*	pBufferPtr;
	DWORD	dwBufferSize;

} SRvclAudioBlock;

#define SRvclAudioBlock_V1 size_including_member(SRvclAudioBlock,dwBufferSize)


/*
	Functions
*/

BOOL __stdcall RvclInitialize();
BOOL __stdcall RvclInitializeEx(SRvclInitialize* pInit);
BOOL __stdcall RvclGetVersion(SRvclVersionInfo* pVersionInfo);
BOOL __stdcall RvclGetDevicesCount(int* pnDevicesCount);
BOOL __stdcall RvclGetDeviceInfo(int nDevice,SRvclDeviceInfo* pDeviceInfo);
BOOL __stdcall RvclSetVideoSource(int nDevice,DWORD dwVideoSource);
BOOL __stdcall RvclSetVideoFormat(int nDevice,DWORD dwVideoFormat);
BOOL __stdcall RvclGetDeviceStatus(int nDevice,SRvclDeviceStatus* pDeviceStatus);
BOOL __stdcall RvclSetVideoAdjustments(int nDevice,const SRvclVideoAdjustments* pVideoAdjustments);
BOOL __stdcall RvclGetVideoAdjustments(int nDevice,SRvclVideoAdjustments* pVideoAdjustments);
BOOL __stdcall RvclGrabFrame(int nDevice,const SRvclFrame* pFrame,OVERLAPPED* pOverlapped);
BOOL __stdcall RvclGetGrabFrameResult(int nDevice,OVERLAPPED* pOverlapped,DWORD* pdwBytesWritten,BOOL bWait);
BOOL __stdcall RvclCancelVideoCapture(int nDevice);
BOOL __stdcall RvclAudioControl(int nDevice,SRvclAudioControl* pAudioCtrl);
BOOL __stdcall RvclCaptureAudioBlock(int nDevice,const SRvclAudioBlock* pAudioBlock,OVERLAPPED* pOverlapped);
BOOL __stdcall RvclGetAudioCaptureResult(int nDevice,OVERLAPPED* pOverlapped,DWORD* pdwBytesWritten,BOOL bWait);
BOOL __stdcall RvclCancelAudioCapture(int nDevice);
BOOL __stdcall RvclScaleAudioBlock(void* pUnscaledDataPtr,void* pScaledDataPtr,DWORD dwUnscaledSamples,DWORD dwScaledSamples,DWORD dwBitsPerSample);
BOOL __stdcall RvclCancelIo(int nDevice);
BOOL __stdcall RvclGpioControl(int nDevice,SRvclGpioControl* pGpioControl);
BOOL __stdcall RvclActivateWatchdog(int nDevice,BOOL fActivate);
BOOL __stdcall RvclReadKeyData(const SRvclKeyData* pKeyData);
BOOL __stdcall RvclDeinterlaceFilter(VOID* pDataPtr, SIZE sizeRes, DWORD dwColorFormat);

#ifdef __cplusplus
}
#endif

#pragma pack(pop)

#endif /* __RVCL_H__ */
