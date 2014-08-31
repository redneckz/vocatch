// VideoDoc.cpp : implementation of the CVideoDoc class
//

#include "stdafx.h"
#include "RvcViewer.h"

#include "VideoDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVideoDoc

IMPLEMENT_DYNCREATE(CVideoDoc, CDocument)

BEGIN_MESSAGE_MAP(CVideoDoc, CDocument)
	//{{AFX_MSG_MAP(CVideoDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVideoDoc construction/destruction

CVideoDoc::CVideoDoc()
{
	// TODO: add one-time construction code here

}

CVideoDoc::~CVideoDoc()
{
}

BOOL CVideoDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CVideoDoc serialization

void CVideoDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CVideoDoc diagnostics

#ifdef _DEBUG
void CVideoDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CVideoDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVideoDoc commands
