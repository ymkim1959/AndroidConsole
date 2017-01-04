
// MyDlg_MFCDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "MyDlg_MFC.h"
#include "MyDlg_MFCDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMyDlg_MFCDlg 대화 상자



CMyDlg_MFCDlg::CMyDlg_MFCDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MYDLG_MFC_DIALOG, pParent)
	, m_strMessage(_T(""))
	, m_strIP(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pRenderWnd = nullptr;
	m_buffer = nullptr;
	m_pDib = nullptr;
	lpLogPalette = nullptr;
	cPalette = nullptr;
	m_pBitmapInfo = nullptr;
	m_pBitmap = nullptr;
	m_nWidth = m_nHeight = m_nBitCount = 0;
}

CMyDlg_MFCDlg::~CMyDlg_MFCDlg()
{
	if(m_pRenderWnd) delete m_pRenderWnd;
	if (m_buffer) delete [] m_buffer;
	if (m_pDib) delete[] m_pDib;				// BMP에서만 사용, DIB 용 Image buffer로 따로 생성함.
	if (lpLogPalette) delete[] lpLogPalette;
	if (cPalette) delete cPalette;
	if (m_pBitmap) delete m_pBitmap;
}

void CMyDlg_MFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ctrlChatting);
	DDX_Text(pDX, IDC_EDIT1, m_strMessage);
	DDX_Text(pDX, IDC_EDIT2, m_strIP);
}

BEGIN_MESSAGE_MAP(CMyDlg_MFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SHOW_BMP, &CMyDlg_MFCDlg::OnBnClickedButtonShowBmp)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CMyDlg_MFCDlg::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_FILE_OPEN, &CMyDlg_MFCDlg::OnBnClickedButtonFileOpen)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CMyDlg_MFCDlg::OnBnClickedButtonSend)
END_MESSAGE_MAP()


// CMyDlg_MFCDlg 메시지 처리기

BOOL CMyDlg_MFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	ShowWindow(SW_MAXIMIZE);

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_pRenderWnd = new CRenderWnd;
	// left, top, right, bottom
	m_rcRenderWnd = CRect(500, 0, 1000, 350);
	m_pRenderWnd->Create(NULL, NULL, WS_VISIBLE | WS_CHILD, m_rcRenderWnd, this, 55601, NULL);

	m_strIP = GetLocalIP();;
	m_ctrlChatting.AddString(_T("My IP is ") + m_strIP);
	UpdateData(FALSE);
	GetDlgItem(IDC_BUTTON_SEND)->SetFocus();
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

CString CMyDlg_MFCDlg::GetLocalIP()
{
	char ac[80];
	if (gethostname(ac, sizeof(ac)) == SOCKET_ERROR) {
		TRACE(_T("Error %d : when getting local host name.\n"), WSAGetLastError());
		return _T("ERROR 1");
	}
	TRACE(_T("Host name is %s.\n"), ac);
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	struct sockaddr_in *sin;
	struct sockaddr_in6 *sin6;
	int listen_fd_num = 0;
	char buf[80] = { 0x00, };
	int i = 0;
	memset(&hints, 0x00, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(ac, nullptr, &hints, &result) != 0) {
		TRACE(_T("Yow! Bad host lookup.\n"));
		return _T("Error 2");
	}
	for (rp = result; rp != NULL; rp = rp->ai_next)
	{
		listen_fd_num++;
	}
	// printf(_T("Num %d\n"), listen_fd_num);
	CString str4, str6;
	for (rp = result, i = 0; rp != NULL; rp = rp->ai_next, i++)
	{
		if (rp->ai_family == AF_INET)
		{
			sin = (sockaddr_in *)rp->ai_addr;
			inet_ntop(rp->ai_family, &sin->sin_addr, buf, sizeof(buf));
			//   printf("<bind 정보 %d %d %s>\n", rp->ai_protocol, rp->ai_socktype, buf);
#ifdef _UNICODE
			int nLen = MultiByteToWideChar(CP_ACP, 0, buf, sizeof(buf), NULL, NULL);
			BSTR bstr;
			bstr = SysAllocStringLen(NULL, nLen);
			MultiByteToWideChar(CP_ACP, 0, buf, sizeof(buf), bstr, nLen);
			str4.Format(_T("%s"), bstr);
			SysFreeString(bstr); 
#else
			str4.Format(_T("%s"), buf);
#endif
			TRACE(_T("[IPv4] <bind 정보 %d %d %s>\n"), rp->ai_protocol, rp->ai_socktype, str4);
		}
		else if (rp->ai_family == AF_INET6)
		{
			sin6 = (sockaddr_in6 *)rp->ai_addr;
			inet_ntop(rp->ai_family, &sin6->sin6_addr, buf, sizeof(buf));
			//   printf("<bind 정보 %d %d %s>\n", rp->ai_protocol, rp->ai_socktype, buf);
#ifdef _UNICOCDE
			int nLen = MultiByteToWideChar(CP_ACP, 0, buf, sizeof(buf), NULL, NULL);
			BSTR bstr;
			bstr = SysAllocStringLen(NULL, nLen);
			MultiByteToWideChar(CP_ACP, 0, buf, sizeof(buf), bstr, nLen);
			str6.Format(_T("%s"), bstr);
			SysFreeString(bstr); 
#else
			str6.Format(_T("%s"), buf);
#endif
			TRACE(_T("[IPv6] <bind 정보 %d %d %s>\n"), rp->ai_protocol, rp->ai_socktype, str6);

		}
	}
	freeaddrinfo(result);
	return(str4);
}

void CMyDlg_MFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMyDlg_MFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMyDlg_MFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMyDlg_MFCDlg::OnBnClickedButtonShowBmp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pRenderWnd->DrawBMP();
}


void CMyDlg_MFCDlg::OnBnClickedButtonClear()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pRenderWnd->EraseScreenOnly();
}

void CMyDlg_MFCDlg::OnRenderfileopen()
{
	CString szFilter = _T("BMP Files (*.bmp)|*.bmp|Jpg files (*.jpg)|*.jpg|");
	CFileDialog Dlg(TRUE, nullptr, nullptr, OFN_HIDEREADONLY, szFilter, nullptr);
	if (IDOK == Dlg.DoModal())
	{
		CWaitCursor	wait;
		CString strPath = Dlg.GetPathName();
		CString strExt = Dlg.GetFileExt();
		int nFileExt = Dlg.m_ofn.nFilterIndex;

		switch (nFileExt) {
		case 1: {
			BOOL bRT = LoadBmp(strPath);
			if (bRT == TRUE) m_pRenderWnd->DrawColorImage(m_pDib, m_nWidth, m_nHeight, m_nBitCount);
			break;
		}
		case 2: {
			//				LoadJPG(strPath);
			break;
		}
		default: {
		}
		}
//		Invalidate(TRUE);
	}
}

void CMyDlg_MFCDlg::OnBnClickedButtonFileOpen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnRenderfileopen();
}

#define MX_GOOD 0
#define MX_FILE_OPEN_ERROR 1
#define MX_FILE_LENGTH_ERROR 2
#define MX_MEMORY_BUFFER_ERROR 3
#define MX_FILE_READ_ERROR 4
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//			BMP 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LoadBmp : bmp를 head와 bitmap info로 정보를 로드한다.
// image는 m_pDib에 올려놓는다. (pDib의 가로 경계는 4의 배수로 한다. 예로 columns 이 191 인 color 이면 573 byte 인데 이를 576 byte로 한다.
BOOL CMyDlg_MFCDlg::LoadBmp(CString strPath)
{
	CWaitCursor wait;
	int nLength;
	int nRT = StartFileInformation(strPath, &nLength);
	BOOL bRT = LoadBmp(m_buffer, nLength);
	FlipBmpY();
	m_nWidth = m_pBitmapInfo->bmiHeader.biWidth;
	m_nHeight = m_pBitmapInfo->bmiHeader.biHeight;
	m_nBitCount = m_pBitmapInfo->bmiHeader.biBitCount;
	CString str;
	if (nRT == MX_GOOD) {
		m_ctrlChatting.AddString(_T("BMP File : ") + strPath);
		str.Format(_T("File size is %d."), m_pBitmapInfo->bmiHeader.biSizeImage);
		m_ctrlChatting.AddString(str);
		str.Format(_T("Height %d * Width %d * BitCount %d"), m_pBitmapInfo->bmiHeader.biHeight, m_pBitmapInfo->bmiHeader.biWidth, m_pBitmapInfo->bmiHeader.biBitCount);
		m_ctrlChatting.AddString(str);
		return TRUE;
	}
	else {
		str.Format(_T("BMP file OPEN ERROR = %d"), nRT);
		m_ctrlChatting.AddString(str);
		return FALSE;
	}
}


int CMyDlg_MFCDlg::StartFileInformation(CString strFilename, int *pnLength)
{
	CFile file;
	//	TRACE("[35-5]1: File '%s'를 분석하자\n", strFilename);
	// file open
	if (!file.Open(strFilename, CFile::modeRead | CFile::typeBinary))
	{
		return MX_FILE_OPEN_ERROR;
	}
	// file길이 확인 
	long nCount;
	try
	{
		nCount = (long)file.GetLength();
	}
	catch (...)
	{
		//		TRACE("[35-7] End of File찾는데 Error발생\n");
		file.Close();
		return MX_FILE_LENGTH_ERROR;
	}
	*pnLength = nCount;
	// Dicom file buffer생성
	try
	{
		if (m_buffer) delete[] m_buffer;
		m_buffer = new unsigned char[nCount];
	}
	catch (CMemoryException *e)
	{
		CString Err;
		Err.Format(_T("[352] Memory Allocation Error for Reading DICOM files"));
		AfxMessageBox(Err);	// 352
		file.Close();
		delete[] m_buffer;
		m_buffer = NULL;
		//		if(ddm) { TRACE("[35-9]Delete/CDicom::StartDicomFileInformation : new error에의해 다시 지움 : delete[] m_buffer\n"); }
		e->Delete();
		return MX_MEMORY_BUFFER_ERROR;
	}
	ZeroMemory(m_buffer, nCount);
	// Dicom file 읽기
	try
	{
		file.Read(m_buffer, nCount);
	}
	catch (CFileException *e)
	{
		//		TRACE("[35-10] File을 읽어 오는데 Error발생\n");
		file.Close();
		delete[]m_buffer;
		m_buffer = NULL;
		//		if(ddm) { TRACE("[35-11]Delete/file open error에의해 지움 : delete[] m_buffer\n"); }
		e->Delete();
		return MX_FILE_READ_ERROR;
	}
	// Dicom file close
	file.Close();
	return MX_GOOD;
}

// File ---Load---> Dlg.m_buffer --> m_pDib (24bit) --> m_pRGB --> m_Bitmap --> MemDC --> DC
BOOL CMyDlg_MFCDlg::LoadBmp(BYTE *p, int nBufferLength)
{
	// step 1. File open -- 이미 되어 있고 그결과가 m_buffer에 있음.
	// step 2. Bitmap File Header (14 byte) = m_bmpHeader = 이는 Baseline.h에 이미 14 byte로 선언되어 있음. (지울 필요 없음)
	// m_bmpHeader -- 이는 따로 메모리를 두어 처리함.
	// m_pBitmapInfo -- 이는 m_buffer 를 참조로 처리함 (따로 buffer를 만들지 않음)
	// m_Dib -- Image 정보로 이는 따로 두어 처리함.
	DWORD size;
	memcpy((LPSTR)&m_bmpHeader, p, sizeof(m_bmpHeader));

	//화일이 bmp 일경우에는 화일 처음에 "BM"이라고 설정해있음
	if (m_bmpHeader.bfType != DIB_HEADER_MARKER) {
		TRACE("This is not BMP file %X\n", m_bmpHeader.bfType);
		return FALSE;
	}
	//	size=m_bmpHeader.bfSize-sizeof(m_bmpHeader);	// 나머지 전체를 다 읽었네..
	size = m_bmpHeader.bfOffBits - sizeof(m_bmpHeader);	// header 부분만 읽었다. (영상부분은 m_buffer에 있음)

														//	if(tDib) delete [] tDib;
														//	tDib= new char[size];
														// Bitmap Info 읽기
														//	memcpy(tDib,p+sizeof(m_bmpHeader), size);

	m_pBitmapInfo = (LPBITMAPINFO)(p + sizeof(m_bmpHeader));
	switch (m_pBitmapInfo->bmiHeader.biBitCount)
	{
	case 32:
	case 24:
		m_nColor = 0;
		break;
	case 8:
		m_nColor = 256;
		break;
	case 4:
		m_nColor = 16;
		break;
	case 1:
		m_nColor = 2;
		break;
	default:
		ASSERT(0);
		break;
	}
	if (m_pDib) delete[] m_pDib;
	try {
		m_pDib = new unsigned char[m_pBitmapInfo->bmiHeader.biSizeImage];
		memcpy(m_pDib, p + m_bmpHeader.bfOffBits, m_pBitmapInfo->bmiHeader.biSizeImage);
	}
	catch (CMemoryException *e)
	{
		CString str;
		str.Format(_T("[BMP00] Not enough memory : Wanted memory %d KB"), m_pBitmapInfo->bmiHeader.biSizeImage / 1024);
		AfxMessageBox(str);	// BMP00
		e->Delete();
		delete[] m_pDib;
		m_pDib = NULL;
		return FALSE;
	}

	int nReal = m_pBitmapInfo->bmiHeader.biWidth * m_pBitmapInfo->bmiHeader.biHeight * ((m_pBitmapInfo->bmiHeader.biBitCount + 7) / 8);
	CString str;

	TRACE("BMP file size (%dKB) >= %d = X %d * Y %d * %d Byte\n", m_pBitmapInfo->bmiHeader.biSizeImage / 1024, nReal / 1024, m_pBitmapInfo->bmiHeader.biWidth, m_pBitmapInfo->bmiHeader.biHeight, (m_pBitmapInfo->bmiHeader.biBitCount + 7) / 8);
	if ((m_pBitmapInfo->bmiHeader.biBitCount + 7) / 8 < 3) {

		str.Format(_T("[BMP01] This is not 32bit color BMP file. [%d]"), m_pBitmapInfo->bmiHeader.biBitCount);
		AfxMessageBox(str);	// BMP01
		return FALSE;
	}
	if ((int)m_pBitmapInfo->bmiHeader.biSizeImage < nReal)
	{
		TRACE("m_pBitmapInfo->bmiHeader.biSizeImage %d is less then nReal %d\n", m_pBitmapInfo->bmiHeader.biSizeImage, nReal);
		str.Format(_T("[BMP02] File size information(biSizeImage=%d) is less then the real size of BMP file (%d)."), m_pBitmapInfo->bmiHeader.biSizeImage, nReal);
		AfxMessageBox(str);	// BMP02
							//		m_pBitmapInfo->bmiHeader.biSizeImage = nReal;
		return FALSE;
	}
	//	ASSERT(m_pBitmapInfo->bmiHeader.biSizeImage >= nReal);

	if (m_nColor)
		if (!CreatePalette(TRUE))
			return FALSE;
	return TRUE;
}

/*
CreatePalette
mode가 1이면 m_pBitmapInfo 에서
그렇지 않으면 만듬
*/
// mode = LoadBmp에서 True, Create 에서 false(이것은 사용하지 않음)
BOOL CMyDlg_MFCDlg::CreatePalette(BOOL mode)
{
	int r, g, b;
	if (cPalette)
		delete cPalette;
	if (lpLogPalette) delete[] lpLogPalette;
	cPalette = NULL;
	lpLogPalette = NULL;

	lpLogPalette = (LPLOGPALETTE) new char[sizeof(LOGPALETTE) + sizeof(PALETTEENTRY)*m_nColor];
	cPalette = new CPalette;
	if (lpLogPalette == NULL || cPalette == NULL)
		return FALSE;
	lpLogPalette->palVersion = PALVERSION;
	lpLogPalette->palNumEntries = m_nColor;

	if (mode)
	{
		for (int i = 0; i<m_nColor; i++)
		{
			lpLogPalette->palPalEntry[i].peRed = m_pBitmapInfo->bmiColors[i].rgbRed;
			lpLogPalette->palPalEntry[i].peGreen = m_pBitmapInfo->bmiColors[i].rgbGreen;
			lpLogPalette->palPalEntry[i].peBlue = m_pBitmapInfo->bmiColors[i].rgbBlue;
		}
	}
	else
	{
		int i = 0;
		for (r = 0; r<256; r += 51)
			for (g = 0; g<256; g += 51)
				for (b = 0; b<256; b += 51)
				{
					lpLogPalette->palPalEntry[i].peRed = r;
					m_pBitmapInfo->bmiColors[i].rgbRed = r;
					lpLogPalette->palPalEntry[i].peGreen = g;
					m_pBitmapInfo->bmiColors[i].rgbGreen = g;
					lpLogPalette->palPalEntry[i].peBlue = b;
					m_pBitmapInfo->bmiColors[i].rgbBlue = b;
					i++;
					if (i>m_nColor)
						return TRUE;
				}
	}
	cPalette->CreatePalette(lpLogPalette);
	return TRUE;
}


int CMyDlg_MFCDlg::GetBmpSamplesPerPixel()
{
	return (m_pBitmapInfo->bmiHeader.biBitCount + 7) / 8;
}

// BMP용
void CMyDlg_MFCDlg::FlipBmpY()
{
	int N = GetBmpSamplesPerPixel();
	int RealWidth = ((GetBmpWidth() * N + 3) / 4) * 4;
	int Height = GetBmpHeight();

	int i;
	BYTE *pLine = new BYTE[RealWidth * Height];
	for (i = 0; i<Height / 2; i++)
	{
		memcpy(pLine, &m_pDib[i * RealWidth], RealWidth);

		memcpy(&m_pDib[i * RealWidth], &m_pDib[(Height - 1 - i) * RealWidth], RealWidth);

		memcpy(&m_pDib[(Height - 1 - i) * RealWidth], pLine, RealWidth);
	}
	delete[] pLine;
}

// pixel 단위의 폭 (byte단위의 폭이 아님)
int CMyDlg_MFCDlg::GetBmpWidth()
{
	//	TRACE("실제 영상의 폭 %d, 메모리상의 폭 %d\n", GetSize().cx, ((GetSize().cx + 3)/4)*4);
	return m_pBitmapInfo->bmiHeader.biWidth;

}

int CMyDlg_MFCDlg::GetBmpHeight()
{
	return m_pBitmapInfo->bmiHeader.biHeight;

}

void CMyDlg_MFCDlg::OnBnClickedButtonSend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_ctrlChatting.AddString(m_strMessage);
}
