
// MyDlg_MFC.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CMyDlg_MFCApp:
// �� Ŭ������ ������ ���ؼ��� MyDlg_MFC.cpp�� �����Ͻʽÿ�.
//

class CMyDlg_MFCApp : public CWinApp
{
public:
	CMyDlg_MFCApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CMyDlg_MFCApp theApp;