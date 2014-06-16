
// CodeCleanDlg.h : ͷ�ļ�
//

#pragma once

#include "CodeFileHanding.h"
#include "afxwin.h"
#include "Global.h"


// CCodeCleanDlg �Ի���
class CCodeCleanDlg : public CDialogEx
{
// ����
public:
	CCodeCleanDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CODECLEAN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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

	CString m_PathFile;       //�洢PathFile.txt��ȫ·��
	CStdioFile pathfile;      //pathfile�ļ�
	CodeFileHanding codefilehanding;//�ļ��������һ��CodeFileHanding �Ķ���

	CString m_IniFilename;    //�����ļ�����rule.ini��
	CString m_IniPath;        //�����ļ�ȫ·��

public:
	void CheckIniPath(void);
	int CCodeCleanDlg::GetIniFile(void);
};
