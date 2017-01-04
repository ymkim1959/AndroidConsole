
// MyDlg_MFCDlg.h : ��� ����
//

#pragma once
#include "RenderWnd.h"
#include "afxwin.h"

#define DIB_HEADER_MARKER ((WORD)('M'<<8) | 'B')
#define PALVERSION 0x300

// CMyDlg_MFCDlg ��ȭ ����
class CMyDlg_MFCDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CMyDlg_MFCDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.
	~CMyDlg_MFCDlg();
// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYDLG_MFC_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonShowBmp();
public:
	CRenderWnd * m_pRenderWnd; 	// �߰�
	CRect m_rcRenderWnd; 		//�߰�
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
