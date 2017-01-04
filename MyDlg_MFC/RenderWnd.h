#pragma once


// CRenderWnd

class CRenderWnd : public CWnd
{
	DECLARE_DYNAMIC(CRenderWnd)

public:
	CRenderWnd();
	virtual ~CRenderWnd();
	BOOL	DrawRenderWnd();	// 추가
	BOOL 	DrawBMP();
	void	EraseScreenOnly();
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);


	afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpCreateStruct);
// 영상처리
	// BMP File ---> Dlg.m_buffer --> m_pDib (24bit) --> m_pRGB --> m_Bitmap --> MemDC --> DC
	// Jpeg file --> m_Jpg.m_pBuf(jpg file) --> m_pData --> m_pRGB --> m_Bitmap --> MemDC --> DC
	// Raw file ---> Dlg.m_buffer --> 
	RGBQUAD * m_pRGB;
	CBitmap m_Bitmap;
	int m_nBitmapWidth, m_nBitmapHeight;
	void DrawColorImage(BYTE *pDib, int nWidth, int nHeight, int nBits, int modifiedWidth = -1);
	void EraseScreen();
};


