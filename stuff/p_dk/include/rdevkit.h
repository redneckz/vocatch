/*
rdevkit.h

This software is supplied under the terms of a license agreement or
nondisclosure agreement with ROSSI-SP JSC and may not be copied
or disclosed except in accordance with the terms of that agreement.

Copyright (c) PENTACON Corporation, 1995-2006. All Rights Reserved.

Version:	3.0
Jule 2006
*/

#if !defined __RDEVKIT_H__
#define __RDEVKIT_H__

#include <windows.h>
#include "rdnbase.h"

#pragma pack(push,8)

#ifdef __cplusplus
extern "C" {
#endif

/*
	Structures
*/

typedef struct _SRdkitKeyData
{
	DWORD	dwSize;
	DWORD	pdwCustomerPassword[4];
	DWORD	dwDataOffset;
	DWORD	dwDataSize;
	BYTE*	pDataPtr;

} SRdkitKeyData;

#define SRdkitKeyData_V1 size_including_member(SRdkitKeyData,pDataPtr)


/*
	Functions
*/

BOOL __stdcall RdkitReadKeyData(const SRdkitKeyData* pKeyData);
BOOL __stdcall RdkitWriteKeyData(const SRdkitKeyData* pKeyData);

#ifdef __cplusplus
}
#endif

#pragma pack(pop)

#endif /* __RDEVKIT_H__ */
