// DrawMandelbrot.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CDrawMandelbrotApp:
// See DrawMandelbrot.cpp for the implementation of this class
//

class CDrawMandelbrotApp : public CWinApp
{
public:
	CDrawMandelbrotApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CDrawMandelbrotApp theApp;