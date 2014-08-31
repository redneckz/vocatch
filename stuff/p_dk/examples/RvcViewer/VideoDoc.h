// VideoDoc.h : interface of the CVideoDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIDEODOC_H__BA79E24D_9907_11D3_9584_00A0244E460B__INCLUDED_)
#define AFX_VIDEODOC_H__BA79E24D_9907_11D3_9584_00A0244E460B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CVideoDoc : public CDocument
{
protected: // create from serialization only
	CVideoDoc();
	DECLARE_DYNCREATE(CVideoDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVideoDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVideoDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CVideoDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIDEODOC_H__BA79E24D_9907_11D3_9584_00A0244E460B__INCLUDED_)
