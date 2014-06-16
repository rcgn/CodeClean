
// CodeCleanDlg.cpp : 实现文件
#include "stdafx.h"
#include "CodeClean.h"
#include "CodeCleanDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CCodeCleanDlg 对话框

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


// CCodeCleanDlg 消息处理程序

BOOL CCodeCleanDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_RidioButton = 0;
	m_FolderButton.EnableWindow(FALSE);
	m_FileButton.EnableWindow(FALSE);

	m_IniFilename = "rule.ini";
	CheckIniPath();
	GetIniFile();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCodeCleanDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CCodeCleanDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void CCodeCleanDlg::OnBnClickedFoldername()
{
	// 打开文件夹，设置待整理文件夹路径
	BROWSEINFO lpbi;
	wchar_t Buffer[MAX_PATH]={0};

	lpbi.lpszTitle = _T("选择文件夹") ;
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
	//浏览文件目录，选择待整理文件名
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
	// 执行文件整理动作
	UpdateData(TRUE);
	if(m_RidioButton == 1)
	{
		//如果整理文件
	    if(m_Filename != "")
	    {
			//实际整理函数入口
		    FileClean(m_Filename);
			MessageBox(_T("成功"));
	    }
		else
		{
			MessageBox(_T("文件路径为空！请选择文件路径。"));
		}
	}
	else if(m_RidioButton == 2)
	{
		//如果整理文件夹
	    if(m_Foldername != "")
	    {
			m_PathFile = m_Foldername + _T("//PathFile.txt");
			int res = pathfile.Open(m_PathFile,CStdioFile::modeCreate | CStdioFile::modeWrite , NULL);
			if(res)
			{
				//1.遍历文件夹，将需整理的文件全名写入PathFile.txt中。
				if(FileSearch(m_Foldername))
				{
					MessageBox(_T("打开PathFile文件失败"));
				}
				pathfile.Close();
			}

			res = pathfile.Open(m_PathFile,CStdioFile::modeRead , NULL);
			if(res)
			{
				CString    tmp_file_path; //临时存储从PathFile.txt读取出的待整理文件名的字符串

				while(pathfile.ReadString(tmp_file_path))
				{
					//每次读取一个文件名并执行文件整理。直到所有文件都整理完毕。
					tmp_file_path.Remove('\n');
					FileClean(tmp_file_path);
					//codefilehanding;
				}
				
			}
			pathfile.Close();
			MessageBox(_T("成功"));

	    }
		else
		{
			MessageBox(_T("文件夹路径为空！请选择文件夹路径。"));
		}	
	}
	else
	{
		MessageBox(_T("请选择需要操作的文件或文件夹！"));
	}

}


void CCodeCleanDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}


void CCodeCleanDlg::FileClean(CString pFilename)
{
	//文件整理函数
	codefilehanding.SetFilePath(pFilename); //设置待整理文件的全路径给codefilehanding对象
	if(codefilehanding.DoFileClean() != 0)  //直接调用codefilehanding的方法整理文件。
	{
		MessageBox(_T("整理文件失败:")+pFilename);
	}
}


void CCodeCleanDlg::OnBnClickedConfig()
{
	// 用notepad打开配置文件

	int res;
	res = (int)ShellExecute(NULL,L"open",m_IniPath,NULL,NULL,SW_SHOWNORMAL);
	if(res < 32)
	{	
		CString errorcode;
		errorcode.Format(L"%d",res);
		MessageBox(_T("打开配置文件失败")+errorcode);
	}
}


void CCodeCleanDlg::OnBnClickedFile()
{
	// 选择单个文件整理
	m_FolderButton.EnableWindow(FALSE);
	m_FileButton.EnableWindow(TRUE);
	m_RidioButton = 1;

}


void CCodeCleanDlg::OnBnClickedFolder()
{
	//选择文件夹整理
	m_FolderButton.EnableWindow(TRUE);
	m_FileButton.EnableWindow(FALSE);
	m_RidioButton = 2;
}


int CCodeCleanDlg::FileSearch(CString const p_Foldername)
{	
	//遍历文件夹，将待整理的文件的文件全名记录到p_Foldername存储的PathFile.txt文件中
	CString foldername = p_Foldername ;
	CString path    = NULL;

	if(p_Foldername.Right(1) != '\\')
	{
		foldername +=  "\\";
	}
	foldername += "*.*";

	CFileFind filefind;
	BOOL res = filefind.FindFile(foldername); //查找foldername目录下的所有文件

	while(res)
	{
		res = filefind.FindNextFile();
		path = filefind.GetFilePath();
		if(filefind.IsDots())
		{
			//上级目录
			continue;
		}
		else if(filefind.IsDirectory())
		{
			//如果是文件夹，递归处理
			FileSearch(path);
		}
		else if(path.Right(2) == ".c" || path.Right(4) == ".cpp" )
		{
			//只处理后缀名是.c和.cpp的文件
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
	//设置ini配置文件的文件全路径
	WCHAR  IniPathTmp[256] ;

	GetModuleFileName(NULL,IniPathTmp,256);
	m_IniPath = IniPathTmp;
	m_IniPath = m_IniPath.Left(m_IniPath.ReverseFind('\\'));
	m_IniPath = m_IniPath + "\\" + m_IniFilename;
}

int CCodeCleanDlg::GetIniFile()
{
	//读取配置文件
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