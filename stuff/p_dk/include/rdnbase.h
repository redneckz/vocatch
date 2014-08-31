/*
rdnbase.h

This software is supplied under the terms of a license agreement or
nondisclosure agreement with PENTACON Corporation and may not be copied
or disclosed except in accordance with the terms of that agreement.

Copyright (c) PENTACON Corporation, 1995-2006. All Rights Reserved.

Version:	3.0
January 2006
*/

#if !defined __RDNBASE_H__
#define __RDNBASE_H__

#pragma pack(push,8)

#ifndef size_including_member
#define size_including_member(s,m) (size_t)((size_t)&(((s *)0)->m)+sizeof(((s*)0)->m))
#endif

/* Color Formats */
#define RDN_CF_RGB32					0x00000001	/* RGB32 */
#define RDN_CF_RGB24					0x00000002	/* RGB24 */
#define RDN_CF_RGB16					0x00000004	/* RGB16 (5:6:5) */
#define RDN_CF_RGB15					0x00000008	/* RGB15 (5:5:5) */
#define RDN_CF_Y8						0x00000010	/* grayscale */
#define RDN_CF_GRAYSCALE				RDN_CF_Y8
#define RDN_CF_YUV422_PLANAR			0x00000020	/* YUV 4:2:2 Planar */
#define RDN_CF_YUV420_PLANAR			0x00000040	/* YUV 4:2:0 Planar */
#define RDN_CF_YUV411_PLANAR			0x00000080	/* YUV 4:1:1 Planar */
#define RDN_CF_YUV422_PACKED			0x00000100	/* YUV 4:2:2 Packed */
#define RDN_CF_YUV411_PACKED			0x00000200	/* YUV 4:1:1 Packed */

#pragma pack(pop)

#endif /* __RDNBASE_H__ */
