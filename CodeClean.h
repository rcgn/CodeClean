
// CodeClean.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCodeCleanApp:
// �йش����ʵ�֣������ CodeClean.cpp
//

class CCodeCleanApp : public CWinApp
{
public:
	CCodeCleanApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCodeCleanApp theApp;