
// MyDlg_MFCDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "MyDlg_MFC.h"
#include "MyDlg_MFCDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
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


// CMyDlg_MFCDlg ��ȭ ����



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
	if (m_pDib) delete[] m_pDib;				// BMP������ ���, DIB �� Image buffer�� ���� ������.
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


// CMyDlg_MFCDlg �޽��� ó����

BOOL CMyDlg_MFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
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

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	ShowWindow(SW_MAXIMIZE);

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_pRenderWnd = new CRenderWnd;
	// left, top, right, bottom
	m_rcRenderWnd = CRect(500, 0, 1000, 350);
	m_pRenderWnd->Create(NULL, NULL, WS_VISIBLE | WS_CHILD, m_rcRenderWnd, this, 55601, NULL);

	m_strIP = GetLocalIP();;
	m_ctrlChatting.AddString(_T("My IP is ") + m_strIP);
	UpdateData(FALSE);
	GetDlgItem(IDC_BUTTON_SEND)->SetFocus();
	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
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
			//   printf("<bind ���� %d %d %s>\n", rp->ai_protocol, rp->ai_socktype, buf);
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
			TRACE(_T("[IPv4] <bind ���� %d %d %s>\n"), rp->ai_protocol, rp->ai_socktype, str4);
		}
		else if (rp->ai_family == AF_INET6)
		{
			sin6 = (sockaddr_in6 *)rp->ai_addr;
			inet_ntop(rp->ai_family, &sin6->sin6_addr, buf, sizeof(buf));
			//   printf("<bind ���� %d %d %s>\n", rp->ai_protocol, rp->ai_socktype, buf);
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
			TRACE(_T("[IPv6] <bind ���� %d %d %s>\n"), rp->ai_protocol, rp->ai_socktype, str6);

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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CMyDlg_MFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CMyDlg_MFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMyDlg_MFCDlg::OnBnClickedButtonShowBmp()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pRenderWnd->DrawBMP();
}


void CMyDlg_MFCDlg::OnBnClickedButtonClear()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
// LoadBmp : bmp�� head�� bitmap info�� ������ �ε��Ѵ�.
// image�� m_pDib�� �÷����´�. (pDib�� ���� ���� 4�� ����� �Ѵ�. ���� columns �� 191 �� color �̸� 573 byte �ε� �̸� 576 byte�� �Ѵ�.
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
	//	TRACE("[35-5]1: File '%s'�� �м�����\n", strFilename);
	// file open
	if (!file.Open(strFilename, CFile::modeRead | CFile::typeBinary))
	{
		return MX_FILE_OPEN_ERROR;
	}
	// file���� Ȯ�� 
	long nCount;
	try
	{
		nCount = (long)file.GetLength();
	}
	catch (...)
	{
		//		TRACE("[35-7] End of Fileã�µ� Error�߻�\n");
		file.Close();
		return MX_FILE_LENGTH_ERROR;
	}
	*pnLength = nCount;
	// Dicom file buffer����
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
		//		if(ddm) { TRACE("[35-9]Delete/CDicom::StartDicomFileInformation : new error������ �ٽ� ���� : delete[] m_buffer\n"); }
		e->Delete();
		return MX_MEMORY_BUFFER_ERROR;
	}
	ZeroMemory(m_buffer, nCount);
	// Dicom file �б�
	try
	{
		file.Read(m_buffer, nCount);
	}
	catch (CFileException *e)
	{
		//		TRACE("[35-10] File�� �о� ���µ� Error�߻�\n");
		file.Close();
		delete[]m_buffer;
		m_buffer = NULL;
		//		if(ddm) { TRACE("[35-11]Delete/file open error������ ���� : delete[] m_buffer\n"); }
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
	// step 1. File open -- �̹� �Ǿ� �ְ� �װ���� m_buffer�� ����.
	// step 2. Bitmap File Header (14 byte) = m_bmpHeader = �̴� Baseline.h�� �̹� 14 byte�� ����Ǿ� ����. (���� �ʿ� ����)
	// m_bmpHeader -- �̴� ���� �޸𸮸� �ξ� ó����.
	// m_pBitmapInfo -- �̴� m_buffer �� ������ ó���� (���� buffer�� ������ ����)
	// m_Dib -- Image ������ �̴� ���� �ξ� ó����.
	DWORD size;
	memcpy((LPSTR)&m_bmpHeader, p, sizeof(m_bmpHeader));

	//ȭ���� bmp �ϰ�쿡�� ȭ�� ó���� "BM"�̶�� ����������
	if (m_bmpHeader.bfType != DIB_HEADER_MARKER) {
		TRACE("This is not BMP file %X\n", m_bmpHeader.bfType);
		return FALSE;
	}
	//	size=m_bmpHeader.bfSize-sizeof(m_bmpHeader);	// ������ ��ü�� �� �о���..
	size = m_bmpHeader.bfOffBits - sizeof(m_bmpHeader);	// header �κи� �о���. (����κ��� m_buffer�� ����)

														//	if(tDib) delete [] tDib;
														//	tDib= new char[size];
														// Bitmap Info �б�
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
mode�� 1�̸� m_pBitmapInfo ����
�׷��� ������ ����
*/
// mode = LoadBmp���� True, Create ���� false(�̰��� ������� ����)
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

// BMP��
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

// pixel ������ �� (byte������ ���� �ƴ�)
int CMyDlg_MFCDlg::GetBmpWidth()
{
	//	TRACE("���� ������ �� %d, �޸𸮻��� �� %d\n", GetSize().cx, ((GetSize().cx + 3)/4)*4);
	return m_pBitmapInfo->bmiHeader.biWidth;

}

int CMyDlg_MFCDlg::GetBmpHeight()
{
	return m_pBitmapInfo->bmiHeader.biHeight;

}

void CMyDlg_MFCDlg::OnBnClickedButtonSend()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);
	m_ctrlChatting.AddString(m_strMessage);
}
