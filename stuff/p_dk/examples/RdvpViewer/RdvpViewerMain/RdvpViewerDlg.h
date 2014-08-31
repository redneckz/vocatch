// RdvpViewerDlg.h : header file
//

#if !defined(AFX_RdvpViewerDLG_H__3E95288E_3610_49B3_A9DF_8D6C31478F92__INCLUDED_)
#define AFX_RdvpViewerDLG_H__3E95288E_3610_49B3_A9DF_8D6C31478F92__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VideoParams.h"
#include "Codec.h"



/////////////////////////////////////////////////////////////////////////////
// CRdvpViewerDlg dialog

enum
{
	AVI = 0x00000001,
	LIVE = 0x00000002
};

class CRdvpViewerDlg : public CDialog
{
// Construction
public:
	CRdvpViewerDlg(CWnd* pParent = NULL);	// standard constructor


	BOOL  ControlResolution();

// Dialog Data
	//{{AFX_DATA(CRdvpViewerDlg)
	enum { IDD = IDD_RDVPVIEWER_DIALOG };
	CStatic	m_stFrameSize;
	CStatic	m_stDecTime;
	CStatic	m_stCompTime;
	CStatic	m_stCompression;
	CStatic	m_stSense;
	CStatic	m_stQual;
	CButton	m_btnOptions;
	CButton	m_btnStart;
	CButton	m_btnStop;
	CSliderCtrl	m_slSense;
	CSliderCtrl	m_slQual;
	CComboBox	m_cmbDstCF;
	CComboBox	m_cmbSrcCF;
	CComboBox	m_cmbDstRez;
	CComboBox	m_cmbSrcRez;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRdvpViewerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Struct for video & color formats
	struct SCFormat
	{
		DWORD	dwRvclFlag;		// RVCL value
		long	lStringID;		// resource string ID
	};

	struct SVFormat
	{
		SIZE	szRes;		// RVCL value
//		long	lStringID;		// resource string ID
	};

	const static SVFormat s_aVideoFormats[];
	const static SCFormat s_aColorFormats[];
	HICON m_hIcon;

	SVideoParams	m_sSourceParams;			//src resolution an color
	SVideoParams	m_sDestParams;				//dst resolution and color

	BYTE	m_bSense;
	BYTE	m_bQuality;

	CCodec	*m_codecDVPack;						//Codec (DV Pack AKA ADVC AKA HiPEr AKA  VSE AKA MPSE AKA DSE)
	SCharacter	m_sChar;


	DWORD	m_dwIntraFrame;						//Intra Frame every "n" frames

	DWORD	m_dwFlagSource;
	DWORD	m_lastCmbDstRez;

public:

	HINSTANCE m_hCaptureDll;

	BOOL(__stdcall* lpfInitialize) (PDATA2COMPRESS_ROUTINE,LPVOID);
	BOOL(__stdcall* lpfSetColorFormat ) (DWORD);
	BOOL(__stdcall* lpfSetResolution ) (SIZE);
	BOOL(__stdcall* lpfStop ) ();
	BOOL(__stdcall* lpfStart ) ();
	BOOL(__stdcall* lpfExitCapture ) ();
	BOOL(__stdcall* lpfGetResolution ) (SIZE	*);


	void CRdvpViewerDlg::DisableControls();
	void CRdvpViewerDlg::EnableControls();
	void CRdvpViewerDlg::SetAviResolution();

	LRESULT OnPrintData(WPARAM wParam, LPARAM lParam);
	LRESULT OnCodecError(WPARAM wParam, LPARAM lParam);

private:

	// Generated message map functions
	//{{AFX_MSG(CRdvpViewerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelChange_ComboSrcRez();
	afx_msg void OnSelChange_ComboDstRez();
	afx_msg void OnSelChange_ComboSrcCF();
	afx_msg void OnSelChange_ComboDstCF();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnButtonOptions();
	afx_msg void OnDestroy();
	afx_msg void OnButtonStart();
	afx_msg void OnButtonStop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()



};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RdvpViewerDLG_H__3E95288E_3610_49B3_A9DF_8D6C31478F92__INCLUDED_)
