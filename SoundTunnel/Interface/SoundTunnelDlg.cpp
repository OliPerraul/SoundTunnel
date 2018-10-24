
// SoundTunnelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SoundTunnel.h"
#include "SoundTunnelDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSoundTunnelDlg dialog



CSoundTunnelDlg::CSoundTunnelDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SOUNDTUNNEL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSoundTunnelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSoundTunnelDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CSoundTunnelDlg message handlers

BOOL CSoundTunnelDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	Start();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSoundTunnelDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSoundTunnelDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CSoundTunnelDlg::Start() {

	audioDuplicator->RunAsync();
	//audioThread.

	//std::thread myThread([&audioDuplicator] {
	//	audioDuplicator->Run();
	//
	//});
	//
	//MessageBox(NULL, L"Press ok to stop", 0, MB_ICONASTERISK);
	//std::async([this] {
	//	audioDuplicator->Run();
	//});
	//audioThread = thread{ [this] {
	//	audioDuplicator->Run();
	//}};
	//myThread.join();
}


void CSoundTunnelDlg::Stop() {
	audioDuplicator->Stop();
	audioDuplicator->WaitForDestroy();
}

