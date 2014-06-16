//CodeFileHanding.h
#pragma once
#include "Global.h"
#include <vector>

using namespace std;

class CodeFileHanding :public CDialogEx
{
public:
	CodeFileHanding(void);
	virtual ~CodeFileHanding(void);

	void SetFilePath(CString );
	int  DoFileClean(void);
	void StringAnalyze(void);
	void StringCombine(void);
	int  SymbolTypeCheck(int index);
	int	 LineSplit(int index ,int symbol, int last_symbol );

private:                      
	CString		m_filepath;  //�������ļ���·��
	CString		m_directory; //����old_file���ڵ��ļ���·��

	CString		m_filename;   //�������ļ����ļ�����������Ŀ¼����
	CString		m_filepathtmp;//�������ļ����ļ�ȫ��m_directory + m_filename

	CStdioFile	old_file;     //ԭ�ļ�
	CStdioFile	code_file;    //���������ļ�

	CString		old_string;					//ÿһ��ԭ�ļ�
	CString		new_string;                 //ÿһ�����ļ�
	int			pre_blank_num;              //ÿһ��ǰ��Ŀո���
	int			next_pre_blank_num;         //��һ��ǰ��Ŀո�������Ҫ��Ϊ�˼��ݡ�}��
	int			buf_index;					//tmp_string_buf����������
	int			bracket_num;				//����ƥ�䣬������+1��������-1��ȫ���ۼӣ���һ�н���ʱ����Ϊ0������һ�п�ͷ����һ��tab��


	vector< CString >	tmp_string_buf;   //����������ļ�ÿ�в�ֺ���ַ���
	vector< int >		tmp_string_buf_type;//��¼ÿһ���ַ��������ͣ�START-QUOTATION

//               ��ʼ    �հ�     ����    ����        �ַ�or����    ����     
	enum		{START , BLANK , SYMBOL ,BRACKETS , CHAR_OR_NUM , QUOTATION   }; //tmp_string_buf�洢���ַ��������͡�

};

