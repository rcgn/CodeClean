
// CodeCleanDlg.cpp : ʵ���ļ�
#include "stdafx.h"
#include "CodeClean.h"
#include "CodeCleanDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CCodeCleanDlg �Ի���

CCodeCleanDlg::CCodeCleanDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCodeCleanDlg::IDD, pParent)
	, m_RidioButton(0)
	, m_PathFile(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_Filename = _T("");
	m_Foldername = _T("");
}

void CCodeCleanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FILENAME, m_Filename);
	DDV_MaxChars(pDX, m_Filename, 255);
	DDX_Text(pDX, IDC_FOLDERNAME, m_Foldername);
	DDV_MaxChars(pDX, m_Foldername, 255);
	DDX_Control(pDX, IDC_FILE_BUTTON, m_FileButton);
	DDX_Control(pDX, IDC_FLODER_BUTTON, m_FolderButton);
}

BEGIN_MESSAGE_MAP(CCodeCleanDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_FLODER_BUTTON, &CCodeCleanDlg::OnBnClickedFoldername)
	ON_BN_CLICKED(IDC_FILE_BUTTON, &CCodeCleanDlg::OnBnClickedFilename)
	ON_BN_CLICKED(IDOK, &CCodeCleanDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CCodeCleanDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CONFIG_BUTTON, &CCodeCleanDlg::OnBnClickedConfig)
	ON_BN_CLICKED(IDC_FILE, &CCodeCleanDlg::OnBnClickedFile)
	ON_BN_CLICKED(IDC_FOLDER, &CCodeCleanDlg::OnBnClickedFolder)
END_MESSAGE_MAP()


// CCodeCleanDlg ��Ϣ�������

BOOL CCodeCleanDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_RidioButton = 0;
	m_FolderButton.EnableWindow(FALSE);
	m_FileButton.EnableWindow(FALSE);

	m_IniFilename = "rule.ini";
	CheckIniPath();
	GetIniFile();
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CCodeCleanDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CCodeCleanDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void CCodeCleanDlg::OnBnClickedFoldername()
{
	// ���ļ��У����ô������ļ���·��
	BROWSEINFO lpbi;
	wchar_t Buffer[MAX_PATH]={0};

	lpbi.lpszTitle = _T("ѡ���ļ���") ;
	lpbi.hwndOwner = m_hWnd;
	lpbi.iImage = NULL;
	lpbi.lParam = NULL;
	lpbi.lpfn = NULL;
	lpbi.pidlRoot =NULL;
	lpbi.pszDisplayName = Buffer; 
	lpbi.ulFlags = 0;// BIF_EDITBOX;// | BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;

	LPITEMIDLIST lp = SHBrowseForFolder(&lpbi);

	if(lp)
	{
		SHGetPathFromIDList(lp,Buffer);
		m_Foldername = Buffer;
	}
	UpdateData(FALSE);
}


void CCodeCleanDlg::OnBnClickedFilename()
{
	//����ļ�Ŀ¼��ѡ��������ļ���
//	CString szFilter;
	CFileDialog filedlg(TRUE,NULL,NULL,NULL,NULL);//szFilter);
	if(filedlg.DoModal() == IDOK)
	{
		m_Filename = filedlg.GetPathName();
		UpdateData(FALSE);
	}
}


void CCodeCleanDlg::OnBnClickedOk()
{
	// ִ���ļ�������
	UpdateData(TRUE);
	if(m_RidioButton == 1)
	{
		//��������ļ�
	    if(m_Filename != "")
	    {
			//ʵ�����������
		    FileClean(m_Filename);
			MessageBox(_T("�ɹ�"));
	    }
		else
		{
			MessageBox(_T("�ļ�·��Ϊ�գ���ѡ���ļ�·����"));
		}
	}
	else if(m_RidioButton == 2)
	{
		//��������ļ���
	    if(m_Foldername != "")
	    {
			m_PathFile = m_Foldername + _T("//PathFile.txt");
			int res = pathfile.Open(m_PathFile,CStdioFile::modeCreate | CStdioFile::modeWrite , NULL);
			if(res)
			{
				//1.�����ļ��У�����������ļ�ȫ��д��PathFile.txt�С�
				if(FileSearch(m_Foldername))
				{
					MessageBox(_T("��PathFile�ļ�ʧ��"));
				}
				pathfile.Close();
			}

			res = pathfile.Open(m_PathFile,CStdioFile::modeRead , NULL);
			if(res)
			{
				CString    tmp_file_path; //��ʱ�洢��PathFile.txt��ȡ���Ĵ������ļ������ַ���

				while(pathfile.ReadString(tmp_file_path))
				{
					//ÿ�ζ�ȡһ���ļ�����ִ���ļ�����ֱ�������ļ���������ϡ�
					tmp_file_path.Remove('\n');
					FileClean(tmp_file_path);
					//codefilehanding;
				}
				
			}
			pathfile.Close();
			MessageBox(_T("�ɹ�"));

	    }
		else
		{
			MessageBox(_T("�ļ���·��Ϊ�գ���ѡ���ļ���·����"));
		}	
	}
	else
	{
		MessageBox(_T("��ѡ����Ҫ�������ļ����ļ��У�"));
	}

}


void CCodeCleanDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnCancel();
}


void CCodeCleanDlg::FileClean(CString pFilename)
{
	//�ļ�������
	codefilehanding.SetFilePath(pFilename); //���ô������ļ���ȫ·����codefilehanding����
	if(codefilehanding.DoFileClean() != 0)  //ֱ�ӵ���codefilehanding�ķ��������ļ���
	{
		MessageBox(_T("�����ļ�ʧ��:")+pFilename);
	}
}


void CCodeCleanDlg::OnBnClickedConfig()
{
	// ��notepad�������ļ�

	int res;
	res = (int)ShellExecute(NULL,L"open",m_IniPath,NULL,NULL,SW_SHOWNORMAL);
	if(res < 32)
	{	
		CString errorcode;
		errorcode.Format(L"%d",res);
		MessageBox(_T("�������ļ�ʧ��")+errorcode);
	}
}


void CCodeCleanDlg::OnBnClickedFile()
{
	// ѡ�񵥸��ļ�����
	m_FolderButton.EnableWindow(FALSE);
	m_FileButton.EnableWindow(TRUE);
	m_RidioButton = 1;

}


void CCodeCleanDlg::OnBnClickedFolder()
{
	//ѡ���ļ�������
	m_FolderButton.EnableWindow(TRUE);
	m_FileButton.EnableWindow(FALSE);
	m_RidioButton = 2;
}


int CCodeCleanDlg::FileSearch(CString const p_Foldername)
{	
	//�����ļ��У�����������ļ����ļ�ȫ����¼��p_Foldername�洢��PathFile.txt�ļ���
	CString foldername = p_Foldername ;
	CString path    = NULL;

	if(p_Foldername.Right(1) != '\\')
	{
		foldername +=  "\\";
	}
	foldername += "*.*";

	CFileFind filefind;
	BOOL res = filefind.FindFile(foldername); //����foldernameĿ¼�µ������ļ�

	while(res)
	{
		res = filefind.FindNextFile();
		path = filefind.GetFilePath();
		if(filefind.IsDots())
		{
			//�ϼ�Ŀ¼
			continue;
		}
		else if(filefind.IsDirectory())
		{
			//������ļ��У��ݹ鴦��
			FileSearch(path);
		}
		else if(path.Right(2) == ".c" || path.Right(4) == ".cpp" )
		{
			//ֻ�����׺����.c��.cpp���ļ�
			char tmp_path[256] = {0};
			path +="\n";
			WideCharToMultiByte(CP_OEMCP,NULL,path,-1,tmp_path,path.GetLength(),NULL,FALSE);
			pathfile.Write(tmp_path,path.GetLength());
		}
	}
		return 0;

}


void CCodeCleanDlg::CheckIniPath(void)
{
	//����ini�����ļ����ļ�ȫ·��
	WCHAR  IniPathTmp[256] ;

	GetModuleFileName(NULL,IniPathTmp,256);
	m_IniPath = IniPathTmp;
	m_IniPath = m_IniPath.Left(m_IniPath.ReverseFind('\\'));
	m_IniPath = m_IniPath + "\\" + m_IniFilename;
}

int CCodeCleanDlg::GetIniFile()
{
	//��ȡ�����ļ�
	CString KeyName;
	CString tmp;

	g_indentation =  GetPrivateProfileInt(L"Type",(LPCTSTR)_T("Indentation"),0,m_IniPath);
	for(int i=0;i<MAX_SYMBOL_TYPE;i++)
	{
		KeyName = L"Symbol_";
		tmp.Format(L"%d",i+1);
		KeyName = KeyName + tmp;
		g_signconfig[i] = GetPrivateProfileInt(L"Symbol",(LPCTSTR)KeyName,0,m_IniPath);
	}
	return 0;
}