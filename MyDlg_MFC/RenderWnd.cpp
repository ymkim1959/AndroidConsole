// RenderWnd.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MyDlg_MFC.h"
#include "RenderWnd.h"


// CRenderWnd

IMPLEMENT_DYNAMIC(CRenderWnd, CWnd)

CRenderWnd::CRenderWnd()
{
	m_pRGB = nullptr;
}

CRenderWnd::~CRenderWnd()
{
	if (m_pRGB) delete[] m_pRGB;
}


BEGIN_MESSAGE_MAP(CRenderWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_DROPFILES()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_NCCREATE()
END_MESSAGE_MAP()



// CRenderWnd �޽��� ó�����Դϴ�.




int CRenderWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.
	return 0;
}


void CRenderWnd::OnDestroy()
{
	CWnd::OnDestroy();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}


void CRenderWnd::OnDropFiles(HDROP hDropInfo)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CWnd::OnDropFiles(hDropInfo);
}


void CRenderWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
					   // �׸��� �޽����� ���ؼ��� CWnd::OnPaint()��(��) ȣ������ ���ʽÿ�.
	DrawRenderWnd();
}

// ���� ��ü�� �Է�
// �Է� = m_Bitmap
// ��� : DC (ȭ�鿡 �׷���)
BOOL CRenderWnd::DrawRenderWnd()
{
	CClientDC DC(this);
	CDC MemDC;
	MemDC.CreateCompatibleDC(&DC);

	CBitmap *pOldBitmap = (CBitmap *)MemDC.SelectObject(&m_Bitmap);

	CRect rcClient;
	GetClientRect(&rcClient);
	DC.SetStretchBltMode(HALFTONE);
	// 
	int nCopyWidth, nCopyHeight;
	double dScreenRatio = rcClient.Width() / (double)rcClient.Height(); // ȭ���� ���� ��� ���� ���� (�� 1:1.9)
	double dImageRatio = m_nBitmapWidth / (double)m_nBitmapHeight;	  // ������ ���� ��� ���� ���� (�� 1:1.9)
	if (dScreenRatio >= dImageRatio) {
		// ������ ȭ�� ��� �Ʒ��� ����. ���� �ٿ��� �Ѵ�.
		nCopyHeight = rcClient.Height();
		nCopyWidth = (int)(rcClient.Width() * dImageRatio / dScreenRatio);
	}
	else {
		// ������ ȭ�� ��� ������ ���. ���̸� �ٿ��� �Ѵ�.
		nCopyWidth = rcClient.Width();
		nCopyHeight = (int)(rcClient.Height() * dScreenRatio / dImageRatio);
	}

	BOOL bRC = DC.StretchBlt(0, 0, nCopyWidth, nCopyHeight, &MemDC, 0, 0, m_nBitmapWidth, m_nBitmapHeight, SRCCOPY);	// [8], [11]
																																			//	BOOL bRC = DC.StretchBlt(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), &MemDC, 0, 0, m_nBitmapWidth, m_nBitmapHeight, SRCCOPY);	// [8], [11]
																																			//	DC.BitBlt(0, 0, m_nBitmapWidth, m_nBitmapHeight, &MemDC, 0, 0, SRCCOPY);   // 1:1 �׸� �׸���

	MemDC.SelectObject(pOldBitmap);


	return TRUE;

}

void CRenderWnd::EraseScreenOnly()
{
	CRect rcWnd;
	GetClientRect(&rcWnd);
	CClientDC DC(this);
	TRACE("*** %d, %d, %d, %d\n", rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom);
	DC.FillSolidRect(rcWnd, RGB(0xff,0xff,0xff));
}

void CRenderWnd::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CWnd::OnTimer(nIDEvent);
}

BOOL CRenderWnd::DrawBMP()
{
	CClientDC WinDC(this);
	CDC MemDC;
	MemDC.CreateCompatibleDC(&WinDC);

	CBitmap bitmap;
	bitmap.LoadBitmap(IDB_BITMAP1);

	CBitmap *pOldBitmap = (CBitmap *)MemDC.SelectObject(&bitmap);

	CRect rcClient;
	GetWindowRect(rcClient);

	WinDC.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &MemDC, 0, 0, SRCCOPY);
	//	WinDC.BitBlt(rcClient.left+550, rcClient.top, rcClient.Width(), rcClient.Height(), &MemDC, 0, 0, SRCCOPY);

	MemDC.SelectObject(pOldBitmap);
	UpdateData(FALSE);
	return TRUE;
}


BOOL CRenderWnd::OnNcCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (!CWnd::OnNcCreate(lpCreateStruct))
		return FALSE;

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.
	return TRUE;
}

// color������ ȭ�鿡 �����־��
// BMP file --> m_buffer(BMP file) --> pBib ----> m_pRGB --> m_Bitmap --> MemDC --> DC
// Jpeg file --> m_pBuf(jpg file) --> m_pData --> m_pRGB --> m_Bitmap --> MemDC --> DC
// �Է� pDib ü�� (nBits = 24bit or 32 bit)
// ��� 32bit display ��ġ ȭ�鿡 �׸�.
// modifiedWidth �� -1 �̸� 1:1 �״�� �׸���, � ���� ���̸� �� ������ ����϶�� �ǹ�
// LoadBMP, LoadJpg ���� ȣ��
void CRenderWnd::DrawColorImage(BYTE *pDib, int nWidth, int nHeight, int nBits, int modifiedWidth)
{
//	EraseScreen();
	int nLength = nHeight * nWidth;
	if (m_pRGB) delete[] m_pRGB;
	try {
		m_pRGB = new RGBQUAD[nLength];
	}
	catch (CMemoryException *e)
	{
		CString Err;
		Err.Format(_T("[352-1] Memory Allocation Error for Reading files : %d*%d=%d"), nWidth, nHeight, nLength);
		AfxMessageBox(Err);	// 352
		e->Delete();
		return;
	}
	int nByte = nBits / 8;				// 24bit ü���̸� 3 Byte
	for (int i = 0; i<nLength; i++) {
		m_pRGB[i].rgbBlue = pDib[i*nByte];
		m_pRGB[i].rgbGreen = pDib[i*nByte + 1];
		m_pRGB[i].rgbRed = pDib[i*nByte + 2];
	}
	CClientDC DC(this);
	// m_Bitmap --> MemDC --> DC
	int nColor_DC = DC.GetDeviceCaps(BITSPIXEL);
	CRect rcClient;
	GetClientRect(&rcClient);

	m_Bitmap.Detach();
	ASSERT(nColor_DC == 32);
	// ���÷��� �ý����� 32bit �� ��� CreateBitmap ���� ���� ����� �ָ� �ȴ�.
	// ���� ������ bit�� �����Ϸ��� CBitmap::CreateCompatibleBitmap �� ���� ������, SetBitmapBits �� �־��ش�.
	if (nColor_DC == 32) {
		m_Bitmap.CreateBitmap(nWidth, nHeight, 1, 32, m_pRGB);
	}
	else {
		m_Bitmap.CreateCompatibleBitmap(&DC, nWidth, nHeight);
		m_Bitmap.SetBitmapBits(nWidth * nHeight * nColor_DC / 8, m_pRGB);
	}
	m_nBitmapWidth = nWidth;
	m_nBitmapHeight = nHeight;
	DrawRenderWnd(); 
}

// ������ ������ �����.
// ���� ��� �ִ� m_Bitmap�� ���������� �����. : m_Bitmap�� white ������ �� �ִ�.
// 1) BOOL Cchatting_kymDlg::OnInitDialog()
// 2) BOOL CRenderWnd::DrawTestBMP()
// 3) void CRenderWnd::DrawColorImage(BYTE *pDib, int nWidth, int nHeight, int nBits, int modifiedWidth)
void CRenderWnd::EraseScreen()
{
	CRect rcWnd;
	GetClientRect(&rcWnd);
	int nLength = rcWnd.Width()*rcWnd.Height();
	RGBQUAD * pRGB = new RGBQUAD[nLength];
	for (int i = 0; i<nLength; i++) {
		pRGB[i].rgbBlue = 0xff;
		pRGB[i].rgbGreen = 0xff;
		pRGB[i].rgbRed = 0xff;
	}
	CClientDC DC(this);
	// m_Bitmap --> MemDC --> DC
	int nColor_DC = DC.GetDeviceCaps(BITSPIXEL);

	m_Bitmap.Detach();
	ASSERT(nColor_DC == 32);
	// ���÷��� �ý����� 32bit �� ��� CreateBitmap ���� ���� ����� �ָ� �ȴ�.
	// ���� ������ bit�� �����Ϸ��� CBitmap::CreateCompatibleBitmap �� ���� ������, SetBitmapBits �� �־��ش�.
	if (nColor_DC == 32) {
		m_Bitmap.CreateBitmap(rcWnd.Width(), rcWnd.Height(), 1, 32, pRGB);
	}
	else {
		m_Bitmap.CreateCompatibleBitmap(&DC, rcWnd.Width(), rcWnd.Height());
		m_Bitmap.SetBitmapBits(nLength * nColor_DC / 8, pRGB);
	}
	m_nBitmapWidth = rcWnd.Width();
	m_nBitmapHeight = rcWnd.Height();
	DrawRenderWnd();
	delete[] pRGB;
}
