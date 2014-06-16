
// CodeCleanDlg.h : 头文件
//

#pragma once

#include "CodeFileHanding.h"
#include "afxwin.h"
#include "Global.h"


// CCodeCleanDlg 对话框
class CCodeCleanDlg : public CDialogEx
{
// 构造
public:
	CCodeCleanDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CODECLEAN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedFoldername();
	afx_msg void OnBnClickedFilename();
	CString m_Filename;
	CString m_Foldername;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedConfig();
	afx_msg void OnBnClickedFile();
	afx_msg void OnBnClickedFolder();
	CButton m_FileButton;
	CButton m_FolderButton;
private:
	int m_RidioButton;
	int FileSearch(CString const p_Foldername );
	void FileClean(CString pFilename);

	CString m_PathFile;       //存储PathFile.txt的全路径
	CStdioFile pathfile;      //pathfile文件
	CodeFileHanding codefilehanding;//文件整理对象，一个CodeFileHanding 的对象。

	CString m_IniFilename;    //配置文件名“rule.ini”
	CString m_IniPath;        //配置文件全路径

public:
	void CheckIniPath(void);
	int CCodeCleanDlg::GetIniFile(void);
};
