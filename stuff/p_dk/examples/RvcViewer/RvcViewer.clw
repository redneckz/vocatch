; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CVideoView
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "rvcviewer.h"
LastPage=0

ClassCount=9
Class1=CControlPanelDlg
Class2=CMainFrame
Class3=CRvcViewerApp
Class4=CAboutDlg
Class5=CVideoDoc
Class6=CVideoView

ResourceCount=12
Resource1=IDD_TYPE_CHOICE (English (U.S.))
Resource2=IDR_MAINFRAME (English (U.S.))
Resource3=IDD_VFW_SOURCE (English (U.S.))
Resource4=IDD_FINAL (English (U.S.))
Resource5=IDD_GPIO_PAGE (English (U.S.))
Resource6=IDD_WELCOME (English (U.S.))
Resource7=IDD_VFW_DEVICES (English (U.S.))
Resource8=IDD_FORMAT (English (U.S.))
Resource9=IDD_ABOUTBOX (English (U.S.))
Class7=CGeneralPage
Class8=CControlPageBase
Resource10=IDD_CONTROL_PANEL (English (U.S.))
Class9=CGpioPage
Resource11=IDD_GENERAL_PAGE (English (U.S.))
Resource12=IDD_LOGIN (English (U.S.))

[CLS:CControlPanelDlg]
Type=0
BaseClass=CDialog
HeaderFile=ControlPanelDlg.h
ImplementationFile=ControlPanelDlg.cpp
LastObject=IDC_TAB_PAGES
Filter=D
VirtualFilter=dWC

[CLS:CMainFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
LastObject=CMainFrame

[CLS:CRvcViewerApp]
Type=0
BaseClass=CWinApp
HeaderFile=RvcViewer.h
ImplementationFile=RvcViewer.cpp
LastObject=CRvcViewerApp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=RvcViewer.cpp
ImplementationFile=RvcViewer.cpp
LastObject=CAboutDlg

[CLS:CVideoDoc]
Type=0
BaseClass=CDocument
HeaderFile=VideoDoc.h
ImplementationFile=VideoDoc.cpp
LastObject=CVideoDoc

[CLS:CVideoView]
Type=0
BaseClass=CView
HeaderFile=VideoView.h
ImplementationFile=VideoView.cpp
LastObject=CVideoView
Filter=C
VirtualFilter=VWC

[DLG:IDD_CONTROL_PANEL]
Type=1
Class=CControlPanelDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg

[ACL:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_WINDOW_MAIN
Command2=ID_WINDOW_CONTROL_PANEL
CommandCount=2

[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_WINDOW_FULL_FRAME
Command2=ID_WINDOW_CIF
Command3=ID_WINDOW_QCIF
Command4=ID_IMAGE_TOP_DOWN
Command5=ID_IMAGE_BOTTOM_UP
Command6=ID_WINDOW_DEINTERLACINGFILTER
Command7=ID_APP_ABOUT
CommandCount=7

[DLG:IDD_TYPE_CHOICE (English (U.S.))]
Type=1
Class=?
ControlCount=8
Control1=IDC_STATIC,static,1342308352
Control2=IDC_BUTTON_ROSSI,button,1342242880
Control3=IDC_STATIC_ROSSI_HDR,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_BUTTON_MULTIMEDIA,button,1342242880
Control6=IDC_STATIC_MULTIMEDIA_HDR,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDCANCEL,button,1342242816

[DLG:IDD_WELCOME (English (U.S.))]
Type=1
Class=?
ControlCount=7
Control1=IDC_STATIC,static,1342179854
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_BUTTON_PREV,button,1476460544
Control6=IDC_BUTTON_NEXT,button,1342242816
Control7=IDCANCEL,button,1342242816

[DLG:IDD_VFW_DEVICES (English (U.S.))]
Type=1
Class=?
ControlCount=7
Control1=IDC_STATIC,static,1342179854
Control2=IDC_STATIC,static,1342308352
Control3=IDC_LIST_DEVICES,listbox,1352728835
Control4=IDC_STATIC,static,1342308352
Control5=IDC_BUTTON_PREV,button,1342242816
Control6=IDC_BUTTON_NEXT,button,1342242816
Control7=IDCANCEL,button,1342242816

[DLG:IDD_VFW_SOURCE (English (U.S.))]
Type=1
Class=?
ControlCount=9
Control1=IDC_STATIC,static,1342179854
Control2=IDC_STATIC,static,1342308352
Control3=IDC_BUTTON_SOURCE,button,1342242816
Control4=IDC_BUTTON_PREV,button,1342242816
Control5=IDC_BUTTON_NEXT,button,1342242816
Control6=IDCANCEL,button,1342242816
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=?
ControlCount=4
Control1=IDOK,button,1342373889
Control2=IDC_STATIC,static,1342177283
Control3=IDC_STATIC,static,1342177296
Control4=IDC_STATIC,static,1342308480

[DLG:IDD_CONTROL_PANEL (English (U.S.))]
Type=1
Class=?
ControlCount=3
Control1=IDC_STATIC,static,1342308352
Control2=IDC_COMBO_DEVICES,combobox,1344339971
Control3=IDC_TAB_PAGES,SysTabControl32,1342242816

[DLG:IDD_FORMAT (English (U.S.))]
Type=1
Class=?
ControlCount=20
Control1=IDC_STATIC_ICON1,static,1342177283
Control2=IDC_STATIC_SIZE1,static,1342308352
Control3=IDC_STATIC_ICON2,static,1342177283
Control4=IDC_STATIC_SIZE2,static,1342308352
Control5=IDC_STATIC_ICON3,static,1342177283
Control6=IDC_STATIC_SIZE3,static,1342308352
Control7=IDC_STATIC_ICON4,static,1342177283
Control8=IDC_STATIC_SIZE4,static,1342308352
Control9=IDC_STATIC_CUSTOM,static,1342308352
Control10=IDC_EDIT_WIDTH,edit,1350639744
Control11=IDC_STATIC_X2,static,1342308865
Control12=IDC_EDIT_HEIGHT,edit,1350639744
Control13=IDC_BUTTON_TEST_CUSTOM,button,1342242816
Control14=IDC_CHECK_43,button,1342242819
Control15=IDC_STATIC,static,1342308352
Control16=IDC_BUTTON_FORMATS,button,1342242816
Control17=IDC_BUTTON_PREV,button,1342242816
Control18=IDC_BUTTON_NEXT,button,1476460544
Control19=IDCANCEL,button,1342242816
Control20=IDC_STATIC,static,1342177297

[DLG:IDD_FINAL (English (U.S.))]
Type=1
Class=?
ControlCount=5
Control1=IDOK,button,1342242816
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342179854
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC_RESOLUTIONS,static,1342308352

[DLG:IDD_GENERAL_PAGE (English (U.S.))]
Type=1
Class=CGeneralPage
ControlCount=21
Control1=IDC_STATIC,button,1342177287
Control2=IDC_STATIC,static,1342308352
Control3=IDC_COMBO_VIDEO_SOURCE,combobox,1344339971
Control4=IDC_STATIC,static,1342308352
Control5=IDC_COMBO_VIDEO_FORMAT,combobox,1344339971
Control6=IDC_STATIC,button,1342177287
Control7=IDC_STATIC,static,1342308352
Control8=IDC_COMBO_COLOR_FORMAT,combobox,1344339971
Control9=IDC_STATIC_BRIGHTNESS_HDR,static,1342308352
Control10=IDC_SLIDER_BRIGHTNESS,msctls_trackbar32,1342242840
Control11=IDC_STATIC_BRIGHTNESS,static,1342308354
Control12=IDC_STATIC_CONTRAST_HDR,static,1342308352
Control13=IDC_SLIDER_CONTRAST,msctls_trackbar32,1342242840
Control14=IDC_STATIC_CONTRAST,static,1342308354
Control15=IDC_STATIC_SATURATION_HDR,static,1342308352
Control16=IDC_SLIDER_SATURATION,msctls_trackbar32,1342242840
Control17=IDC_STATIC_SATURATION,static,1342308354
Control18=IDC_STATIC_HUE_HDR,static,1342308352
Control19=IDC_SLIDER_HUE,msctls_trackbar32,1342242840
Control20=IDC_STATIC_HUE,static,1342308354
Control21=IDC_BUTTON_RESET_ADJUSTMENTS,button,1342242816

[CLS:CGeneralPage]
Type=0
HeaderFile=GeneralPage.h
ImplementationFile=GeneralPage.cpp
BaseClass=CDialog
Filter=D
LastObject=CGeneralPage

[CLS:CControlPageBase]
Type=0
HeaderFile=ControlPageBase.h
ImplementationFile=ControlPageBase.cpp
BaseClass=generic CWnd
Filter=W
LastObject=CControlPageBase

[DLG:IDD_GPIO_PAGE (English (U.S.))]
Type=1
Class=CGpioPage
ControlCount=6
Control1=IDC_STATIC,static,1342308352
Control2=IDC_LIST_GPIO,SysListView32,1350664205
Control3=IDC_BUTTON_INPUT,button,1342242816
Control4=IDC_BUTTON_OUTPUT,button,1342242816
Control5=IDC_BUTTON_SET,button,1342242816
Control6=IDC_BUTTON_RESET,button,1342242816

[CLS:CGpioPage]
Type=0
HeaderFile=GpioPage.h
ImplementationFile=GpioPage.cpp
BaseClass=CControlPageBase
Filter=D
LastObject=CGpioPage
VirtualFilter=WC

[DLG:IDD_LOGIN (English (U.S.))]
Type=1
Class=?
ControlCount=10
Control1=IDC_STATIC,button,1342177287
Control2=IDC_RADIO_PASSWORDS1,button,1342308361
Control3=IDC_RADIO_PASSWORDS2,button,1342177289
Control4=IDC_EDIT_PASSWORD1,edit,1350631552
Control5=IDC_EDIT_PASSWORD2,edit,1350631552
Control6=IDC_EDIT_PASSWORD3,edit,1350631552
Control7=IDC_EDIT_PASSWORD4,edit,1350631552
Control8=IDOK,button,1342242817
Control9=IDCANCEL,button,1342242816
Control10=IDC_STATIC,static,1342308352

