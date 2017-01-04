
// MyDlg_MFCDlg.h : 헤더 파일
//

#pragma once
#include "RenderWnd.h"
#include "afxwin.h"

#define DIB_HEADER_MARKER ((WORD)('M'<<8) | 'B')
#define PALVERSION 0x300

// CMyDlg_MFCDlg 대화 상자
class CMyDlg_MFCDlg : public CDialogEx
{
// 생성입니다.
public:
	CMyDlg_MFCDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	~CMyDlg_MFCDlg();
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYDLG_MFC_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonShowBmp();
public:
	CRenderWnd * m_pRenderWnd; 	// 추가
	CRect m_rcRenderWnd; 		//추가
	afx_msg void OnBnClickedButtonClear();
	void OnRenderfileopen();
	afx_msg void OnBnClickedButtonFileOpen();
	//************** BMP
	unsigned char *	m_buffer;
	int StartFileInformation(CString strFilename, int *pnLength);
	BOOL LoadBmp(CString strPath);
	BOOL LoadBmp(BYTE *p, int nBufferLength);
	BOOL CreatePalette(BOOL mode);
	int GetBmpSamplesPerPixel();
	unsigned char * GetBmpImage();
	void FlipBmpY();
	int GetBmpWidth();
	int GetBmpHeight();
	BOOL SaveBmp(LPCSTR file);
	int m_nWidth, m_nHeight, m_nBitCount;
protected:
	BITMAPFILEHEADER m_bmpHeader;
	LPBITMAPINFO m_pBitmapInfo;
	LPBYTE m_pDib;
	int m_ndumy;
	int m_nColor;
	LPLOGPALETTE lpLogPalette;
	CPalette *cPalette;
	char m_strFileName[512];
public:
	CBitmap *m_pBitmap;
	CListBox m_ctrlChatting;
	CString m_strMessage;
	afx_msg void OnBnClickedButtonSend();
	CString m_strIP;
	CString GetLocalIP();
};
