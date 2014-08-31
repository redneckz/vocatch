#if !defined(AFX_CONTROLPANELDLG_H__BA79E258_9907_11D3_9584_00A0244E460B__INCLUDED_)
#define AFX_CONTROLPANELDLG_H__BA79E258_9907_11D3_9584_00A0244E460B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ControlPanelDlg.h : header file
//

#include "ControlPageBase.h"
#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CControlPanelDlg dialog

class CVideoView;

class CControlPanelDlg : public CDialog
{
// Construction
public:
	CControlPanelDlg();
	~CControlPanelDlg();
	
	BOOL Create(CVideoView* pParentWnd);

	int		GetCurrentDevice() const;
	CString	GetCurrentDeviceName() const;
	void	UpdateView() const;

// Dialog Data
	//{{AFX_DATA(CControlPanelDlg)
	enum { IDD = IDD_CONTROL_PANEL };
	CTabCtrl	m_PagesTabCtrl;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CControlPanelDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
private:
	CVideoView*	m_pView;			// parent window

	CTypedPtrArray<CObArray,CControlPageBase*> m_aPages;

protected:
	void ShowCurrentState();
	
	// Generated message map functions
	//{{AFX_MSG(CControlPanelDlg)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChange_ComboDevices();
	afx_msg void OnSelChanging_TabPages(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelChange_TabPages(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

// Returns current device index
inline int CControlPanelDlg::GetCurrentDevice() const
{
	CComboBox* pDevicesCombo = (CComboBox*)GetDlgItem(IDC_COMBO_DEVICES);
	ASSERT(pDevicesCombo && ::IsWindow(pDevicesCombo->GetSafeHwnd()));

	int i = pDevicesCombo->GetCurSel();
	
	if(i != CB_ERR)
		// Device index = item's DWORD data
		return pDevicesCombo->GetItemData(i);
	else
		// Noone selected
		return -1;
}

/////////////////////////////////////////////////////////////////////////////

// Returns current device name
inline CString CControlPanelDlg::GetCurrentDeviceName() const
{
	SRvclDeviceInfo rvcldi;
	CString			str;
	int				nDevice = GetCurrentDevice();
	
	rvcldi.dwSize = sizeof(rvcldi);
	
	if(RvclGetDeviceInfo(nDevice,&rvcldi))
		str.Format(
			IDS_FULL_DEVICE_NAME,
			nDevice,rvcldi.wPciDeviceID,
			rvcldi.dwPciBusNumber,rvcldi.dwPciSlotNumber);

	return str;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTROLPANELDLG_H__BA79E258_9907_11D3_9584_00A0244E460B__INCLUDED_)
