// DrawMandelbrotDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DrawMandelbrot.h"
#include "DrawMandelbrotDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "Comm.h"
#include <omp.h>

// CDrawMandelbrotDlg dialog
CDrawMandelbrotDlg::CDrawMandelbrotDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDrawMandelbrotDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDrawMandelbrotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDrawMandelbrotDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOVE()
END_MESSAGE_MAP()


// CDrawMandelbrotDlg message handlers

BOOL CDrawMandelbrotDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	m_Left = -2.0;
	m_Right = 1.0;
	m_Top = 1.5;
	m_Bottom = -1.0;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDrawMandelbrotDlg::OnPaint()
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
		CDialog::OnPaint();
	}

	// Draw Mandelbrot.
	Draw();
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDrawMandelbrotDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


BOOL CDrawMandelbrotDlg::Draw()
{
	CRect rect;
	GetWindowRect(&rect);
	int iWidth = rect.Width();
	int iHeight = rect.Height();

	m_Top  = m_Bottom + (m_Right - m_Left) * iHeight / iWidth;

	CClientDC dc(this);
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, iWidth, iHeight);
	memDC.SelectObject(&bmp);

	BITMAPINFO bmpInfo; 
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);  
	bmpInfo.bmiHeader.biWidth = iWidth; 
	bmpInfo.bmiHeader.biHeight = -iHeight;
	bmpInfo.bmiHeader.biPlanes = 1;  
	bmpInfo.bmiHeader.biBitCount = 24;  
	bmpInfo.bmiHeader.biCompression = BI_RGB;  
	bmpInfo.bmiHeader.biSizeImage = 0;  
	bmpInfo.bmiHeader.biXPelsPerMeter = 3000;  
	bmpInfo.bmiHeader.biYPelsPerMeter = 3000;  
	bmpInfo.bmiHeader.biClrUsed = 0;  
	bmpInfo.bmiHeader.biClrImportant = 0;  

	long nLnBytes = (bmpInfo.bmiHeader.biWidth*3+3)/4*4;
	BYTE *pData = new BYTE[nLnBytes*abs(bmpInfo.bmiHeader.biHeight)];

	memset(pData, 0, nLnBytes*abs(bmpInfo.bmiHeader.biHeight));

	int iMaxLoop = 1500;

	double dWidthZ = m_Right - m_Left;
	double dHeightZ = m_Top - m_Bottom;

	int iTmp = -1;
	int index = -1;
	#pragma omp parallel for
	for (int i = 0; i < iWidth; ++i)
	{
		for (int j = 0; j < iHeight; ++j)
		{
			ComplexNum z;
			ComplexNum c;
			double dTmp;

			int iLoop = 0;
			z.real = 0;
			z.imag = 0;
			c.real = dWidthZ * (i + 1) / iWidth + m_Left;
			c.imag = dHeightZ * (j + 1) / iHeight + m_Bottom;
			
			int iTmp = 0;

			do
			{
				dTmp = z.real * z.real - z.imag * z.imag + c.real;
				z.imag = 2 * z.real * z.imag + c.imag;
				z.real = dTmp;
			} while((z.real * z.real + z.imag * z.imag) < 4 && (++iLoop < iMaxLoop) );

			if (iLoop != iMaxLoop)
			{

				iTmp = iLoop  % 255;
				index = j * nLnBytes + i * 3;
				pData[index++] = iTmp;
				pData[index++] = iTmp * 5;
				pData[index] = iTmp * 10;
			}
		}
	}

	SetDIBits(dc.m_hDC, bmp, 0, abs(bmpInfo.bmiHeader.biHeight), pData, &bmpInfo, DIB_RGB_COLORS);
	delete []pData;
	dc.BitBlt(0, 0, bmpInfo.bmiHeader.biWidth, abs(bmpInfo.bmiHeader.biHeight), &memDC, 0, 0, SRCCOPY);

	return true;
}


BOOL CDrawMandelbrotDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default

	CRect rect;
	GetWindowRect(&rect);
	int iWidth = rect.Width();
	int iHeight = rect.Height();

	double dWidthZ = m_Right - m_Left;
	double dHeightZ = m_Top - m_Bottom;

	ScreenToClient(&pt);

	CRect crect;
	GetClientRect(crect);

	if (zDelta > 0)
	{
		m_Left += dWidthZ * (pt.x - iWidth / 2) / iWidth;
		m_Right += dWidthZ * (pt.x - iWidth / 2) / iWidth;
		m_Top += dHeightZ * (pt.y - iHeight / 2) / iHeight;
		m_Bottom += dHeightZ * (pt.y - iHeight / 2) / iHeight;

		m_Left += dWidthZ / 4;
		m_Right -= dWidthZ / 4;
		m_Top -= dHeightZ / 4;
		m_Bottom += dHeightZ / 4;
	} else if (zDelta < 0)
	{
		m_Left -= dWidthZ / 4;
		m_Right += dWidthZ / 4;
		m_Top += dHeightZ / 4;
		m_Bottom -= dHeightZ / 4;
	}
	Draw();

	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void CDrawMandelbrotDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_MouseDownPoi = point;

	CDialog::OnLButtonDown(nFlags, point);
}

void CDrawMandelbrotDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	double dWidthZ = m_Right - m_Left;
	double dHeightZ = m_Top - m_Bottom;

	CRect rect;
	GetWindowRect(&rect);
	int iWidth = rect.Width();
	int iHeight = rect.Height();

	m_Left -= dWidthZ * (point.x - m_MouseDownPoi.x) / iWidth;
	m_Right -= dWidthZ * (point.x - m_MouseDownPoi.x) / iWidth;
	m_Top -= dHeightZ * (point.y - m_MouseDownPoi.y) / iHeight;
	m_Bottom -= dHeightZ * (point.y - m_MouseDownPoi.y) / iHeight;

	Draw();

	CDialog::OnLButtonUp(nFlags, point);
}

void CDrawMandelbrotDlg::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	// TODO: Add your message handler code here
}
