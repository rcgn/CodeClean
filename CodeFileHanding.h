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
	CString		m_filepath;  //待整理文件的路径
	CString		m_directory; //保存old_file所在的文件夹路径

	CString		m_filename;   //待整理文件的文件名（不包括目录名）
	CString		m_filepathtmp;//待整理文件的文件全名m_directory + m_filename

	CStdioFile	old_file;     //原文件
	CStdioFile	code_file;    //整理后的新文件

	CString		old_string;					//每一行原文件
	CString		new_string;                 //每一行新文件
	int			pre_blank_num;              //每一行前面的空格数
	int			next_pre_blank_num;         //下一行前面的空格数，主要是为了兼容“}”
	int			buf_index;					//tmp_string_buf向量的索引
	int			bracket_num;				//括号匹配，左括号+1，右括号-1，全文累加，若一行结束时，不为0，则下一行开头缩进一个tab。


	vector< CString >	tmp_string_buf;   //保存待整理文件每行拆分后的字符串
	vector< int >		tmp_string_buf_type;//记录每一个字符串的类型：START-QUOTATION

//               开始    空白     符号    括号        字符or数字    引号     
	enum		{START , BLANK , SYMBOL ,BRACKETS , CHAR_OR_NUM , QUOTATION   }; //tmp_string_buf存储的字符串的类型。

};

