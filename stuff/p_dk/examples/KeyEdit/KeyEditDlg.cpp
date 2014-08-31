// KeyEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "KeyEdit.h"
#include "KeyEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define KEY_DATA_SIZE	16

/////////////////////////////////////////////////////////////////////////////
// CKeyEditDlg dialog

CKeyEditDlg::CKeyEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKeyEditDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKeyEditDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKeyEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKeyEditDlg)
	DDX_Control(pDX, IDC_LIST_DATA, m_listKeyData);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CKeyEditDlg, CDialog)
	//{{AFX_MSG_MAP(CKeyEditDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_READ, OnButtonRead)
	ON_BN_CLICKED(IDC_BUTTON_WRITE, OnButtonWrite)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnButtonEdit)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_LIST_DATA, OnBeginLabelEdit_ListData)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_DATA, OnEndLabelEdit_ListData)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_DATA, OnCustomDraw_ListData)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKeyEditDlg message handlers

BOOL CKeyEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	CString str;

	str.LoadString(IDS_DATA); m_listKeyData.InsertColumn(0, str);
	str.LoadString(IDS_OFFSET); m_listKeyData.InsertColumn(1, str);

	m_listKeyData.SetExtendedStyle(
		m_listKeyData.GetExtendedStyle() |
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	CRect rect;
	m_listKeyData.GetClientRect(&rect);

	m_listKeyData.SetColumnWidth(0, rect.Width()/2);
	m_listKeyData.SetColumnWidth(1, rect.Width()/2);

	TCHAR szTemp[8];

	for(int i=0; i<KEY_DATA_SIZE; i++)
	{
		m_listKeyData.InsertItem(i, NULL);
		m_listKeyData.SetItemText(i, 1, _itot(i, szTemp, 10));

		// application-defined item data = dirty flag
		m_listKeyData.SetItemData(i, false);
	}

	// Demo passwords
	SetDlgItemText(IDC_EDIT_PWD1, _T("9592CB4F"));
	SetDlgItemText(IDC_EDIT_PWD2, _T("F3B62AC0"));
	SetDlgItemText(IDC_EDIT_PWD3, _T("FCB8802F"));
	SetDlgItemText(IDC_EDIT_PWD4, _T("9CE9CEF9"));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CKeyEditDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CKeyEditDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CKeyEditDlg::OnOK() 
{
	//CDialog::OnOK();
}

void CKeyEditDlg::OnCancel() 
{
	CDialog::OnCancel();
}

bool CKeyEditDlg::GetPassword(DWORD dwPassword[4])
{
	static UINT nConstrols[] = { 
		IDC_EDIT_PWD1,
		IDC_EDIT_PWD2,
		IDC_EDIT_PWD3,
		IDC_EDIT_PWD4 };

	for(int i=0; i<4; i++)
	{
		CString str;
		
		if(GetDlgItemText(nConstrols[i], str) == 0)
			return false;

		// Convert HEX string into DWORD value
		LPTSTR endptr;
		dwPassword[i] = _tcstoul(str, &endptr, 16);

		if(	dwPassword[i] == ULONG_MAX ||
			dwPassword[i] == 0 )
			return false;
	}
	
	return true;
}

void CKeyEditDlg::OnButtonRead() 
{
	SRdkitKeyData	keydata;
	BYTE			bBuffer[KEY_DATA_SIZE];
	CString			strCaption,str;

	keydata.dwSize = sizeof(keydata);
	keydata.dwDataOffset = 0;
	keydata.dwDataSize = KEY_DATA_SIZE;
	keydata.pDataPtr = bBuffer;
		
	if(!GetPassword(keydata.pdwCustomerPassword))
	{
		strCaption.LoadString(IDS_ERROR);
		str.LoadString(IDS_ERROR_INVALID_PASSWORD);
		MessageBox(str, strCaption, MB_OK|MB_ICONSTOP);
		return;
	}
	
	if(!RdkitReadKeyData(&keydata))
	{
		DWORD dwError = GetLastError();

		strCaption.LoadString(IDS_ERROR);
		str.Format(IDS_ERROR_CANT_READ_KEY_DATA, ERROR_MESSAGE(dwError));
		MessageBox(str, strCaption, MB_OK|MB_ICONSTOP);
		return;
	}

	TCHAR szTemp[8];

	for(int i=0; i<KEY_DATA_SIZE; i++)
	{
		m_listKeyData.SetItemText(i, 0, _tcsupr(_ltot(bBuffer[i],szTemp,16)));
		m_listKeyData.SetItemData(i, false);
	}
}

void CKeyEditDlg::OnButtonWrite() 
{
	SRdkitKeyData	keydata;
	BYTE			bBuffer[KEY_DATA_SIZE];
	CString			strCaption,str;

	keydata.dwSize = sizeof(keydata);
	keydata.dwDataOffset = 0;
	keydata.dwDataSize = KEY_DATA_SIZE;
	keydata.pDataPtr = bBuffer;
		
	if(!GetPassword(keydata.pdwCustomerPassword))
	{
		strCaption.LoadString(IDS_ERROR);
		str.LoadString(IDS_ERROR_INVALID_PASSWORD);
		MessageBox(str, strCaption, MB_OK|MB_ICONSTOP);
		return;
	}
	
	LPTSTR endptr;

	for(int i=0; i<KEY_DATA_SIZE; i++)
	{
		str = m_listKeyData.GetItemText(i,0);

		ULONG ulValue = _tcstoul(str, &endptr, 16);
		ASSERT(ulValue != ULONG_MAX);

		bBuffer[i] = BYTE(ulValue);
	}

	strCaption.LoadString(IDS_WRITE);
	str.LoadString(IDS_WRITE_SURE);

	if(MessageBox(str, strCaption, MB_YESNO|MB_ICONQUESTION) != IDYES)
		return;

	if(!RdkitWriteKeyData(&keydata))
	{
		DWORD dwError = GetLastError();

		strCaption.LoadString(IDS_ERROR);
		str.Format(IDS_ERROR_CANT_WRITE_KEY_DATA, ERROR_MESSAGE(dwError));
		MessageBox(str, strCaption, MB_OK|MB_ICONSTOP);
		return;
	}

	// Clear dirty flags
	for(i=0; i<KEY_DATA_SIZE; i++)
	{
		m_listKeyData.SetItemData(i, false);
	}

	strCaption.LoadString(IDS_WRITE);
	str.LoadString(IDS_WRITE_SUCCESS);

	MessageBox(str, strCaption, MB_OK|MB_ICONINFORMATION);
}

void CKeyEditDlg::OnButtonEdit() 
{
	m_listKeyData.SetFocus();
	
	int iItem = m_listKeyData.GetNextItem(-1, LVIS_SELECTED);
	m_listKeyData.EditLabel((iItem >= 0) ? iItem : 0);
}

void CKeyEditDlg::OnBeginLabelEdit_ListData(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLVDISPINFO* pdi = (NMLVDISPINFO*)pNMHDR;

	m_listKeyData.GetEditControl()->SetLimitText(2);
	
	// To allow the user to edit the label, return FALSE
	*pResult = FALSE;
	
	// To prevent the user from editing the label, return TRUE.
	//*pResult = TRUE;
}

void CKeyEditDlg::OnEndLabelEdit_ListData(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLVDISPINFO* pdi = (NMLVDISPINFO*)pNMHDR;

	if(pdi->item.pszText == NULL)
	{
		// The user cancels editing, the return value is ignored
		*pResult = TRUE;
		return;
	}

	CString	strNewValue = pdi->item.pszText;

	strNewValue.TrimLeft();
	strNewValue.TrimRight();

	if(	strNewValue.IsEmpty() ||
		m_listKeyData.GetItemText(pdi->item.iItem, 0) == strNewValue)
	{
		*pResult = FALSE;
		return;
	}

	m_listKeyData.SetItemData(pdi->item.iItem, true);
	*pResult = TRUE;
}

void CKeyEditDlg::OnCustomDraw_ListData(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;

	switch(lplvcd->nmcd.dwDrawStage)
	{
		case CDDS_PREPAINT:
				*pResult = CDRF_NOTIFYITEMDRAW;
				break;

		case CDDS_ITEMPREPAINT:
				// application-defined item data = dirty flag
				if(lplvcd->nmcd.lItemlParam)
				{
					lplvcd->clrText = RGB(0xff,0,0);
					*pResult = CDRF_NEWFONT;
				}

				break;
	}
}