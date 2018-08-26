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

	m_left = -2.0;
	m_right = 1.0;
	m_top = 1.5;
	m_bottom = -1.0;

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
	int width = rect.Width();
	int height = rect.Height();

	m_top  = m_bottom + (m_right - m_left) * height / width;

	CClientDC dc(this);
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, width, height);
	memDC.SelectObject(&bmp);

	BITMAPINFO bmpInfo; 
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);  
	bmpInfo.bmiHeader.biWidth = width; 
	bmpInfo.bmiHeader.biHeight = -height;
	bmpInfo.bmiHeader.biPlanes = 1;  
	bmpInfo.bmiHeader.biBitCount = 24;  
	bmpInfo.bmiHeader.biCompression = BI_RGB;  
	bmpInfo.bmiHeader.biSizeImage = 0;  
	bmpInfo.bmiHeader.biXPelsPerMeter = 3000;  
	bmpInfo.bmiHeader.biYPelsPerMeter = 3000;  
	bmpInfo.bmiHeader.biClrUsed = 0;  
	bmpInfo.bmiHeader.biClrImportant = 0;  

	long bytesPerLine = (bmpInfo.bmiHeader.biWidth * 3 + 3) / 4 * 4;
	long bufferSize = bytesPerLine * height;

	BYTE *buffer = new BYTE[bufferSize];
	memset(buffer, 0, bufferSize);

	int maxLoop = 1500;

	double w = m_right - m_left;
	double h = m_top - m_bottom;

	int iTmp = -1;
	int index = -1;
	#pragma omp parallel for
	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height; ++j)
		{
			ComplexNum z;
			ComplexNum c;
			double dTmp;

			int loopCount = 0;
			c.real = w * (i + 1) / width + m_left;
			c.imag = h * (j + 1) / height + m_bottom;
			
			do
			{
				dTmp = z.real * z.real - z.imag * z.imag + c.real;
				z.imag = 2 * z.real * z.imag + c.imag;
				z.real = dTmp;
				++loopCount;
			} while((z.real * z.real + z.imag * z.imag) < 4 && loopCount < maxLoop);

			if (loopCount != maxLoop)
			{
				int iTmp = loopCount  % 255;
				index = j * bytesPerLine + i * 3;
				buffer[index] = iTmp;
				buffer[index + 1] = iTmp * 5;
				buffer[index + 2] = iTmp * 10;
			}
		}
	}

	SetDIBits(dc.m_hDC, bmp, 0, height, buffer, &bmpInfo, DIB_RGB_COLORS);
	delete []buffer;

	dc.BitBlt(0, 0, bmpInfo.bmiHeader.biWidth, height, &memDC, 0, 0, SRCCOPY);

	return true;
}


BOOL CDrawMandelbrotDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	CRect rect;
	GetWindowRect(&rect);
	int canvasW = rect.Width();
	int cnavasH = rect.Height();

	double w = m_right - m_left;
	double h = m_top - m_bottom;
	double deltaW = w / 4;
	double deltaH = h / 4;
	ScreenToClient(&pt);

	CRect crect;
	GetClientRect(crect);

	if (zDelta > 0)
	{
		m_left += w * (pt.x - canvasW / 2) / canvasW;
		m_right += w * (pt.x - canvasW / 2) / canvasW;
		m_top += h * (pt.y - cnavasH / 2) / cnavasH;
		m_bottom += h * (pt.y - cnavasH / 2) / cnavasH;

		m_left += deltaW;
		m_right -= deltaW;
		m_top -= deltaH;
		m_bottom += deltaH;
	}
	else if (zDelta < 0)
	{
		m_left -= deltaW;
		m_right += deltaW;
		m_top += deltaH;
		m_bottom -= deltaH;
	}
	Draw();

	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void CDrawMandelbrotDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_MouseDownPoint = point;
}

void CDrawMandelbrotDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	CRect rect;
	GetWindowRect(&rect);
	
	double deltaX = m_right - m_left * (point.x - m_MouseDownPoint.x) / rect.Width();
	double deltaY = m_top - m_bottom * (point.y - m_MouseDownPoint.y) / rect.Height();

	m_left -= deltaX;
	m_right -= deltaX;
	m_top -= deltaY;
	m_bottom -= deltaY;

	Draw();
}
