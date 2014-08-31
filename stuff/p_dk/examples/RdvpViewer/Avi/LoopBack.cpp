#include "stdafx.h"
#include "Avi.h"
#include "data2compress.h"

extern CAviApp	theApp;

BOOL __stdcall D2CInitialize(PDATA2COMPRESS_ROUTINE pRoutineAddress,LPVOID	lpParam)
{
	return theApp.Initialize(pRoutineAddress,lpParam);	
}


BOOL __stdcall D2CSetColorFormat(DWORD dwColorFormat)
{
	return 0;
}

BOOL __stdcall D2CSetResolution(SIZE sizeResolution)
{
	return 0;
}

BOOL __stdcall D2CGetResolution(SIZE *psizeResolution)
{
	theApp.GetResolution(psizeResolution);
	return 0;
}


///////////////////////////////////////////////////////
// Close Avi file and close Read and Send Threads

BOOL __stdcall D2CExitCapture()
{
	theApp.ExitCapture();

	return 0;
}

///////////////////////////////////////////////////////
// Begin to read from avi file and send to compressor

BOOL __stdcall D2CStart()
{
	theApp.Start();

	return 0;
}

///////////////////////////////////////////////////////
// Finish read from avi file and send to compressor

BOOL __stdcall D2CStop()
{
	theApp.Stop();

	return 0;
}