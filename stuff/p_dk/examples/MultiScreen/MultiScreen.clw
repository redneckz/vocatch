; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CLoginDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "MultiScreen.h"
LastPage=0

ClassCount=5
Class1=CMultiScreenApp
Class3=CMainFrame
Class4=CAboutDlg

ResourceCount=5
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Class2=CMultiScreenView
Resource3=IDR_MAINFRAME (English (U.S.))
Resource4=IDD_ABOUTBOX (English (U.S.))
Class5=CLoginDlg
Resource5=IDD_LOGIN (English (U.S.))

[CLS:CMultiScreenApp]
Type=0
HeaderFile=MultiScreen.h
ImplementationFile=MultiScreen.cpp
Filter=N

[CLS:CMultiScreenView]
Type=0
HeaderFile=MultiScreenView.h
ImplementationFile=MultiScreenView.cpp
Filter=W
LastObject=CMultiScreenView
BaseClass=CWnd 
VirtualFilter=WC

[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=ID_VIEW_CSM_WHOLEFRAME




[CLS:CAboutDlg]
Type=0
HeaderFile=MultiScreen.cpp
ImplementationFile=MultiScreen.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Class=CAboutDlg

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_FILE_MRU_FILE1
Command6=ID_APP_EXIT
Command10=ID_EDIT_PASTE
Command11=ID_VIEW_TOOLBAR
Command12=ID_VIEW_STATUS_BAR
Command13=ID_APP_ABOUT
CommandCount=13
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command7=ID_EDIT_UNDO
Command8=ID_EDIT_CUT
Command9=ID_EDIT_COPY

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_NEXT_PANE
CommandCount=13
Command4=ID_EDIT_UNDO
Command13=ID_PREV_PANE


[TB:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_VIEW_43RATIO
Command2=ID_VIEW_MULTIPLEXING
Command3=ID_APP_ABOUT
CommandCount=3

[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=CMainFrame
Command1=ID_APP_EXIT
Command2=ID_VIEW_VF_NTSCM
Command3=ID_VIEW_VF_NTSCJ
Command4=ID_VIEW_VF_PALBDGHI
Command5=ID_VIEW_VF_PALM
Command6=ID_VIEW_VF_PALN
Command7=ID_VIEW_VF_SECAM
Command8=ID_VIEW_CF_RGB32
Command9=ID_VIEW_CF_RGB24
Command10=ID_VIEW_CF_RGB15
Command11=ID_VIEW_CF_YUV422_PACKED
Command12=ID_VIEW_CF_YUV411_PACKED
Command13=ID_VIEW_CF_Y8
Command14=ID_VIEW_CSM_WHOLEFRAME
Command15=ID_VIEW_CSM_WHOLEFIELD
Command16=ID_VIEW_CSM_CIF
Command17=ID_VIEW_CSM_FITTOWND
Command18=ID_VIEW_MULTIPLEXING
Command19=ID_VIEW_43RATIO
Command20=ID_VIEW_SHOWVIDEO
Command21=ID_VIEW_TOOLBAR
Command22=ID_VIEW_STATUS_BAR
Command23=ID_APP_ABOUT
CommandCount=23

[ACL:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_EDIT_COPY
Command2=ID_EDIT_PASTE
Command3=ID_EDIT_UNDO
Command4=ID_EDIT_CUT
Command5=ID_NEXT_PANE
Command6=ID_PREV_PANE
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_CUT
Command10=ID_EDIT_UNDO
CommandCount=10

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_LOGIN (English (U.S.))]
Type=1
Class=CLoginDlg
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

[CLS:CLoginDlg]
Type=0
HeaderFile=LoginDlg.h
ImplementationFile=LoginDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CLoginDlg

