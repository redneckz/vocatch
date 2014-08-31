/*
data2compress.h
*/

#if !defined __DATA_2_COMPRESS_H__
#define __DATA_2_COMPRESS_H__

#include <windows.h>

#pragma pack(push,8)

#ifdef __cplusplus
extern "C" {
#endif


/*
	Structures
*/

typedef struct _SData2Compress
{
	DWORD	dwSize;
	SIZE	sizeResolution;
	DWORD	dwColorFromat;
	DWORD	dwDataSize;
	BYTE*	pDataPtr;
	HANDLE	hReadyEvent;
	HANDLE	hSemaphore;

} SData2Compress;


typedef void (__stdcall *PDATA2COMPRESS_ROUTINE)(const SData2Compress* pData2Compress,LPVOID pParam);

/*
	Functions
*/

BOOL __stdcall D2CInitialize(PDATA2COMPRESS_ROUTINE pRoutineAddress,LPVOID pParam);
BOOL __stdcall D2CSetColorFormat(DWORD dwColorFormat);
BOOL __stdcall D2CSetResolution(SIZE sizeResolution);
BOOL __stdcall D2CStop();
BOOL __stdcall D2CExitCapture();
BOOL __stdcall D2CStart();



#ifdef __cplusplus
}
#endif

#pragma pack(pop)

#endif /* __DATA_2_COMPRESS_H__ */
