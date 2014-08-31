// GeneralPage.cpp : implementation file
//

#include "stdafx.h"
#include "RvcViewer.h"
#include "GeneralPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGeneralPage dialog

// Video formats
const CGeneralPage::SFormat CGeneralPage::s_aVideoFormats[] = 
{	
	{RVCL_VF_NTSCM,		IDS_VF_NTSC_M},
	{RVCL_VF_NTSCJ,		IDS_VF_NTSC_J},
	{RVCL_VF_PALBDGHI,	IDS_VF_PAL_BDGHI},
	{RVCL_VF_PALM,		IDS_VF_PAL_M},
	{RVCL_VF_PALN,		IDS_VF_PAL_N},
	{RVCL_VF_SECAM,		IDS_VF_SECAM}
};

// Color formats
const CGeneralPage::SFormat CGeneralPage::s_aColorFormats[] = 
{	
	{RDN_CF_RGB32,			IDS_CF_RGB32},
	{RDN_CF_RGB24,			IDS_CF_RGB24},
	{RDN_CF_RGB15,			IDS_CF_RGB15},
/*
	{RDN_CF_YUV422_PACKED,	IDS_CF_YUV422_PACKED},
	{RDN_CF_YUV411_PACKED,	IDS_CF_YUV411_PACKED},
	{RDN_CF_YUV422_PLANAR,	IDS_CF_YUV422_PLANAR},
	{RDN_CF_YUV411_PLANAR,	IDS_CF_YUV411_PLANAR},
	{RDN_CF_YUV420_PLANAR,  IDS_CF_YUV420_PLANAR},
*/
	{RDN_CF_Y8,				IDS_CF_Y8}
};

const CGeneralPage::SVideoAdjustments CGeneralPage::s_aVideoAdjustmentsDefault[] = 
{	
	{RVCL_BT_DEFAULT_BRIGHTNESS, RVCL_BT_DEFAULT_CONTRAST, RVCL_BT_DEFAULT_SATURATION, RVCL_BT_DEFAULT_HUE, 0},
	{RVCL_CX_DEFAULT_BRIGHTNESS, RVCL_CX_DEFAULT_CONTRAST, RVCL_CX_DEFAULT_SATURATION, RVCL_CX_DEFAULT_HUE, RVCL_CX_DEFAULT_COMBRANGE},
};

const CGeneralPage::SVideoAdjustments CGeneralPage::s_aVideoAdjustmentsMax[] = 
{	
	{RVCL_BT_MAX_BRIGHTNESS, RVCL_BT_MAX_CONTRAST, RVCL_BT_MAX_SATURATION, RVCL_BT_MAX_HUE, 0},
	{RVCL_CX_MAX_BRIGHTNESS, RVCL_CX_MAX_CONTRAST, RVCL_CX_MAX_SATURATION, RVCL_CX_MAX_HUE, RVCL_CX_MAX_COMBRANGE},
};

CGeneralPage::CGeneralPage(CControlPanelDlg* pControlPanel)
	: CControlPageBase(pControlPanel, 2000)
{
	//{{AFX_DATA_INIT(CGeneralPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CGeneralPage::~CGeneralPage()
{
}

void CGeneralPage::DoDataExchange(CDataExchange* pDX)
{
	CControlPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGeneralPage)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGeneralPage, CControlPageBase)
	//{{AFX_MSG_MAP(CGeneralPage)
	ON_CBN_SELCHANGE(IDC_COMBO_COLOR_FORMAT, OnSelChange_ComboColorFormat)
	ON_CBN_SELCHANGE(IDC_COMBO_VIDEO_FORMAT, OnSelChange_ComboVideoFormat)
	ON_CBN_SELCHANGE(IDC_COMBO_VIDEO_SOURCE, OnSelChange_ComboVideoSource)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_RESET_ADJUSTMENTS, OnButton_ResetAdjustments)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGeneralPage message handlers

void CGeneralPage::OnCancel() 
{
}

void CGeneralPage::OnOK() 
{
}

/////////////////////////////////////////////////////////////////////////////

BOOL CGeneralPage::OnInitDialog() 
{
	int		i,j;
	CString	strMsg,strCap,str;
		
	CControlPageBase::OnInitDialog();
	
	// Video sources
	CComboBox *pSourcesCombo = (CComboBox*) GetDlgItem(IDC_COMBO_VIDEO_SOURCE);
	ASSERT(pSourcesCombo && ::IsWindow(pSourcesCombo->GetSafeHwnd()));

	for(i = 0; i < 4; i++)
	{
		str.Format(IDS_VIDEO_SOURCE, i);
		j = pSourcesCombo->AddString(str);
		
		// application-defined item data = source number
		pSourcesCombo->SetItemData(j, i);
	}

	// Video formats
	CComboBox *pFormatsCombo = (CComboBox*) GetDlgItem(IDC_COMBO_VIDEO_FORMAT);
	ASSERT(pFormatsCombo && ::IsWindow(pFormatsCombo->GetSafeHwnd()));

	for(i = 0; i < sizeof(s_aVideoFormats) / sizeof(SFormat); i++)
	{
		// Load format name from resource
		str.LoadString(s_aVideoFormats[i].lStringID);
		j = pFormatsCombo->AddString(str);
	
		// application-defined item data = RVCL video format flag
		pFormatsCombo->SetItemData(j, s_aVideoFormats[i].dwRvclFlag);
	}

	// Color formats
	CComboBox *pColorsCombo = (CComboBox*) GetDlgItem(IDC_COMBO_COLOR_FORMAT);
	ASSERT(pColorsCombo && ::IsWindow(pColorsCombo->GetSafeHwnd()));

	for(i=0; i<sizeof(s_aColorFormats)/sizeof(SFormat); i++)
	{
		// Load color format name from resource
		str.LoadString(s_aColorFormats[i].lStringID);
		j = pColorsCombo->AddString(str);

		// Select RGB24 as default
		if(s_aColorFormats[i].dwRvclFlag == RDN_CF_RGB24)
			pColorsCombo->SetCurSel(j);
	
		// Application-defined item data = RVCL color format flag
		pColorsCombo->SetItemData(j, s_aColorFormats[i].dwRvclFlag);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////

DWORD CGeneralPage::GetColorFormat()
{
	CComboBox *pColorsCombo = (CComboBox*) GetDlgItem(IDC_COMBO_COLOR_FORMAT);
	ASSERT(pColorsCombo && ::IsWindow(pColorsCombo->GetSafeHwnd()));

	int j = pColorsCombo->GetCurSel();
	return (j != CB_ERR) ? pColorsCombo->GetItemData(j) : 0;
}

/////////////////////////////////////////////////////////////////////////////

// CBN_SELCHANGE for color formats combo
void CGeneralPage::OnSelChange_ComboColorFormat() 
{
	// Update view settings
	UpdateView();
}

/////////////////////////////////////////////////////////////////////////////

// CBN_SELCHANGE for video formats combo
void CGeneralPage::OnSelChange_ComboVideoFormat() 
{
	CComboBox *pFormatsCombo = (CComboBox*)GetDlgItem(IDC_COMBO_VIDEO_FORMAT);
	ASSERT(pFormatsCombo && ::IsWindow(pFormatsCombo->GetSafeHwnd()));

	int i = pFormatsCombo->GetCurSel();

	if(i == CB_ERR)
		return;

	// Video format = item's DWORD data
	DWORD dwFormat = pFormatsCombo->GetItemData(i);
		
	// Set new format
	if(RvclSetVideoFormat(GetCurrentDevice(),dwFormat))
	{
		// Ok, update view settings
		UpdateView();
	}
	else
	{
		// Show error message
		for(i=0; i<sizeof(s_aVideoFormats)/sizeof(SFormat); i++)
		{
			if(s_aVideoFormats[i].dwRvclFlag == dwFormat)
			{
				DWORD	dwError = GetLastError();
				CString	strFormat,strMsg,strCap;
				
				// Load format name from resource
				strFormat.LoadString(s_aVideoFormats[i].lStringID);

				strCap.LoadString(IDS_ERROR);
				strMsg.Format(IDS_ERR_CANT_SET_VIDEO_FORMAT,strFormat,GetCurrentDeviceName(),ERROR_MESSAGE(dwError),dwError);

				MessageBox(strMsg,strCap,MB_OK|MB_ICONSTOP|MB_TOPMOST);
				break;
			}
		}

		// Show current device status
		ShowCurrentState();
	}
}

/////////////////////////////////////////////////////////////////////////////

// CBN_SELCHANGE for sources combo
void CGeneralPage::OnSelChange_ComboVideoSource() 
{
	CComboBox *pSourcesCombo = (CComboBox*) GetDlgItem(IDC_COMBO_VIDEO_SOURCE);
	ASSERT(pSourcesCombo && ::IsWindow(pSourcesCombo->GetSafeHwnd()));

	int i = pSourcesCombo->GetCurSel();

	if(i == CB_ERR)
		return;

	// Video source = item's DWORD data
	DWORD dwVideoSource = pSourcesCombo->GetItemData(i);
	
	// Set new source
	if(!RvclSetVideoSource(GetCurrentDevice(),dwVideoSource))
	{
		// Show error message
		DWORD	dwError = GetLastError();
		CString	strMsg,strCap;

		strCap.LoadString(IDS_ERROR);
		strMsg.Format(IDS_ERR_CANT_SET_VIDEO_SOURCE,dwVideoSource,GetCurrentDeviceName(),ERROR_MESSAGE(dwError),dwError);

		MessageBox(strMsg,strCap,MB_OK|MB_ICONSTOP|MB_TOPMOST);

		// Show current device status
		ShowCurrentState();
	}
}

/////////////////////////////////////////////////////////////////////////////

void CGeneralPage::OnHScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar) 
{
	SRvclVideoAdjustments rvclva;
	CSliderCtrl* pSlider = (CSliderCtrl*) pScrollBar;
	
	// Find slider, prepare RVCL struct & update static text
	switch(pScrollBar->GetDlgCtrlID())
	{
		case IDC_SLIDER_BRIGHTNESS:	
				rvclva.dwMask = RVCL_VADJUST_BRIGHTNESS;
				rvclva.dwBrightness = (DWORD)pSlider->GetPos();
				SetDlgItemInt(IDC_STATIC_BRIGHTNESS,rvclva.dwBrightness);
				break;
		
		case IDC_SLIDER_CONTRAST:	
				rvclva.dwMask = RVCL_VADJUST_CONTRAST;
				rvclva.dwContrast = (DWORD)pSlider->GetPos();
				SetDlgItemInt(IDC_STATIC_CONTRAST,rvclva.dwContrast);
				break;
		
		case IDC_SLIDER_SATURATION:	
				rvclva.dwMask = RVCL_VADJUST_SATURATION;
				rvclva.dwSaturation = (DWORD)pSlider->GetPos();
				SetDlgItemInt(IDC_STATIC_SATURATION,rvclva.dwSaturation);
				break;
		
		case IDC_SLIDER_HUE:	
				rvclva.dwMask = RVCL_VADJUST_HUE;
				rvclva.dwHue = (DWORD)pSlider->GetPos();
				SetDlgItemInt(IDC_STATIC_HUE,rvclva.dwHue);
				break;

		case IDC_SLIDER_COMBRANGE:
				rvclva.dwMask = RVCL_VADJUST_COMBRANGE;
				rvclva.dwCombRange = (DWORD)pSlider->GetPos();
				SetDlgItemInt(IDC_STATIC_COMBRANGE, rvclva.dwCombRange);
				break;

		default:
				return;
	}

	rvclva.dwSize = sizeof(rvclva);

	if(!RvclSetVideoAdjustments(GetCurrentDevice(), &rvclva))
		ShowCurrentState();

	//CControlPageBase::OnHScroll(nSBCode, nPos, pScrollBar);
}

/////////////////////////////////////////////////////////////////////////////

// Resets adjaustment
void CGeneralPage::OnButton_ResetAdjustments() 
{
	SRvclDeviceInfo rvcldi = {0};
	rvcldi.dwSize = sizeof(rvcldi);
	if(!RvclGetDeviceInfo(GetCurrentDevice(), &rvcldi))
		return;

	SRvclVideoAdjustments rvclva;
	
	rvclva.dwSize = sizeof(rvclva);
	rvclva.dwMask =	RVCL_VADJUST_BRIGHTNESS |
					RVCL_VADJUST_CONTRAST |
					RVCL_VADJUST_SATURATION |
					RVCL_VADJUST_HUE |
					RVCL_VADJUST_COMBRANGE;

	rvclva.dwBrightness = s_aVideoAdjustmentsDefault[rvcldi.dwDeviceType].dwBrightness;
	rvclva.dwContrast   = s_aVideoAdjustmentsDefault[rvcldi.dwDeviceType].dwContrast;
	rvclva.dwSaturation = s_aVideoAdjustmentsDefault[rvcldi.dwDeviceType].dwSaturation;
	rvclva.dwHue        = s_aVideoAdjustmentsDefault[rvcldi.dwDeviceType].dwHue;
	rvclva.dwCombRange  = s_aVideoAdjustmentsDefault[rvcldi.dwDeviceType].dwCombRange;
	
	RvclSetVideoAdjustments(GetCurrentDevice(),&rvclva);
	ShowCurrentState();
}

/////////////////////////////////////////////////////////////////////////////

void CGeneralPage::ShowCurrentState()
{
	SRvclDeviceStatus rvclds;
	int i,nDevice = GetCurrentDevice();

	// Get current device status: source number & video format
	rvclds.dwSize = sizeof(rvclds);
	rvclds.dwMask = RVCL_STATUS_VIDEOSOURCE | RVCL_STATUS_VIDEOFORMAT;
		
	if(RvclGetDeviceStatus(nDevice,&rvclds))
	{
		// Show current video source
		CComboBox *pSourcesCombo = (CComboBox*) GetDlgItem(IDC_COMBO_VIDEO_SOURCE);
		ASSERT(pSourcesCombo && ::IsWindow(pSourcesCombo->GetSafeHwnd()));		

		// Find combo item to select
		for(i = 0; i < pSourcesCombo->GetCount(); i++)
		{
			if(pSourcesCombo->GetItemData(i) == rvclds.dwVideoSource)
			{
				// Select this item
				pSourcesCombo->SetCurSel(i);
				break;
			}
		}

		if(i == pSourcesCombo->GetCount())
			pSourcesCombo->SetCurSel(-1);

		// Show current video format
		CComboBox *pFormatsCombo = (CComboBox*) GetDlgItem(IDC_COMBO_VIDEO_FORMAT);
		ASSERT(pFormatsCombo && ::IsWindow(pFormatsCombo->GetSafeHwnd()));

		// Find combo item to select
		for(i = 0; i < pFormatsCombo->GetCount(); i++)
		{
			if(pFormatsCombo->GetItemData(i) == rvclds.dwVideoFormat)
			{
				// Select this item
				pFormatsCombo->SetCurSel(i);
				break;
			}
		}

		if(i == pFormatsCombo->GetCount())
			pFormatsCombo->SetCurSel(-1);
	}

	// Get adjustments
	SRvclVideoAdjustments rvclva = {0};

	rvclva.dwSize = sizeof(rvclva);
	rvclva.dwMask =	RVCL_VADJUST_BRIGHTNESS |
					RVCL_VADJUST_CONTRAST |
					RVCL_VADJUST_SATURATION |
					RVCL_VADJUST_HUE |
					RVCL_VADJUST_COMBRANGE;

	if(!RvclGetVideoAdjustments(nDevice, &rvclva))
		return;

	// Get device static info
	SRvclDeviceInfo rvcldi = {0};
	rvcldi.dwSize = sizeof(rvcldi);
	if(!RvclGetDeviceInfo(nDevice, &rvcldi))
		return;

	CSliderCtrl	*pBrightness = (CSliderCtrl*) GetDlgItem(IDC_SLIDER_BRIGHTNESS),
				*pContrast = (CSliderCtrl*) GetDlgItem(IDC_SLIDER_CONTRAST),
				*pSaturation = (CSliderCtrl*) GetDlgItem(IDC_SLIDER_SATURATION),
				*pHue = (CSliderCtrl*) GetDlgItem(IDC_SLIDER_HUE),
				*pCombRange = (CSliderCtrl*) GetDlgItem(IDC_SLIDER_COMBRANGE);

	// Set up sliders & static text
	pBrightness->SetPos(rvclva.dwBrightness);
	pBrightness->SetRange(0, s_aVideoAdjustmentsMax[rvcldi.dwDeviceType].dwBrightness);
	SetDlgItemInt(IDC_STATIC_BRIGHTNESS, rvclva.dwBrightness);

	pContrast->SetPos(rvclva.dwContrast);
	pContrast->SetRange(0, s_aVideoAdjustmentsMax[rvcldi.dwDeviceType].dwContrast);
	SetDlgItemInt(IDC_STATIC_CONTRAST, rvclva.dwContrast);
	
	pSaturation->SetPos(rvclva.dwSaturation);
	pSaturation->SetRange(0, s_aVideoAdjustmentsMax[rvcldi.dwDeviceType].dwSaturation);
	SetDlgItemInt(IDC_STATIC_SATURATION,rvclva.dwSaturation);

	pHue->SetPos(rvclva.dwHue);
	pHue->SetRange(0, s_aVideoAdjustmentsMax[rvcldi.dwDeviceType].dwHue);
	SetDlgItemInt(IDC_STATIC_HUE, rvclva.dwHue);

	pCombRange->SetPos(rvclva.dwCombRange);
	pCombRange->SetRange(0, s_aVideoAdjustmentsMax[rvcldi.dwDeviceType].dwCombRange);
	SetDlgItemInt(IDC_STATIC_COMBRANGE, rvclva.dwCombRange);
}
