// RenderWnd.cpp : 구현 파일입니다.
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



// CRenderWnd 메시지 처리기입니다.




int CRenderWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	return 0;
}


void CRenderWnd::OnDestroy()
{
	CWnd::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CRenderWnd::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CWnd::OnDropFiles(hDropInfo);
}


void CRenderWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CWnd::OnPaint()을(를) 호출하지 마십시오.
	DrawRenderWnd();
}

// 다음 전체를 입력
// 입력 = m_Bitmap
// 출력 : DC (화면에 그려줌)
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
	double dScreenRatio = rcClient.Width() / (double)rcClient.Height(); // 화면의 높이 대비 가로 비율 (예 1:1.9)
	double dImageRatio = m_nBitmapWidth / (double)m_nBitmapHeight;	  // 영상의 높이 대비 가로 비율 (예 1:1.9)
	if (dScreenRatio >= dImageRatio) {
		// 영상이 화면 대비 아래로 갈다. 옆은 줄여야 한다.
		nCopyHeight = rcClient.Height();
		nCopyWidth = (int)(rcClient.Width() * dImageRatio / dScreenRatio);
	}
	else {
		// 영상이 화면 대비 옆으로 길다. 높이를 줄여야 한다.
		nCopyWidth = rcClient.Width();
		nCopyHeight = (int)(rcClient.Height() * dScreenRatio / dImageRatio);
	}

	BOOL bRC = DC.StretchBlt(0, 0, nCopyWidth, nCopyHeight, &MemDC, 0, 0, m_nBitmapWidth, m_nBitmapHeight, SRCCOPY);	// [8], [11]
																																			//	BOOL bRC = DC.StretchBlt(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), &MemDC, 0, 0, m_nBitmapWidth, m_nBitmapHeight, SRCCOPY);	// [8], [11]
																																			//	DC.BitBlt(0, 0, m_nBitmapWidth, m_nBitmapHeight, &MemDC, 0, 0, SRCCOPY);   // 1:1 그림 그리기

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
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

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

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	return TRUE;
}

// color영상을 화면에 보여주어라
// BMP file --> m_buffer(BMP file) --> pBib ----> m_pRGB --> m_Bitmap --> MemDC --> DC
// Jpeg file --> m_pBuf(jpg file) --> m_pData --> m_pRGB --> m_Bitmap --> MemDC --> DC
// 입력 pDib 체계 (nBits = 24bit or 32 bit)
// 출력 32bit display 장치 화면에 그림.
// modifiedWidth 가 -1 이면 1:1 그대로 그리고, 어떤 양의 값이면 그 폭으로 축소하라는 의미
// LoadBMP, LoadJpg 에서 호출
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
	int nByte = nBits / 8;				// 24bit 체계이면 3 Byte
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
	// 디스플레이 시스템이 32bit 인 경우 CreateBitmap 으로 직접 만들어 주면 된다.
	// 만약 임의의 bit에 대응하려면 CBitmap::CreateCompatibleBitmap 에 의해 만든후, SetBitmapBits 로 넣어준다.
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

// 이전의 영상을 지운다.
// 원래 들어 있는 m_Bitmap의 영상정보도 지운다. : m_Bitmap에 white 영상이 들어가 있다.
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
	// 디스플레이 시스템이 32bit 인 경우 CreateBitmap 으로 직접 만들어 주면 된다.
	// 만약 임의의 bit에 대응하려면 CBitmap::CreateCompatibleBitmap 에 의해 만든후, SetBitmapBits 로 넣어준다.
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
