#include "stdafx.h"
#include "capture.h"

extern CCaptureApp	theApp;

///////////////////////////////////////////////////////
//	Initialize bt878 and set Compress routine pointer

BOOL __stdcall D2CInitialize(PDATA2COMPRESS_ROUTINE pRoutineAddress,LPVOID	lpParam)
{
	return theApp.Initialize(pRoutineAddress,lpParam);	
}


///////////////////////////////////////////////////////
// Set Grab Color Format

BOOL __stdcall D2CSetColorFormat(DWORD dwColorFormat)
{
	theApp.SetColorFormat(dwColorFormat);

	return 0;
}

///////////////////////////////////////////////////////
// Set Grab Resolution

BOOL __stdcall D2CSetResolution(SIZE sizeResolution)
{
	theApp.SetResolution(sizeResolution);

	return 0;
}

///////////////////////////////////////////////////////
// Close Grab thread

BOOL __stdcall D2CExitCapture()
{
	theApp.ExitCapture();

	return 0;
}

///////////////////////////////////////////////////////
// Start - empty function becouse bt878 always grab

BOOL __stdcall D2CStart()
{
	return 0;
}

///////////////////////////////////////////////////////
// Stop - empty function becouse bt878 always grab

BOOL __stdcall D2CStop()
{
	return 0;
}