// DrawMandelbrotDlg.h : header file
//

#pragma once


// CDrawMandelbrotDlg dialog
class CDrawMandelbrotDlg : public CDialog
{
// Construction
public:
	CDrawMandelbrotDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_DRAWMANDELBROT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	double m_Left, m_Right, m_Top, m_Bottom;
	CPoint m_MouseDownPoi;
	BOOL Draw();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMove(int x, int y);
};
