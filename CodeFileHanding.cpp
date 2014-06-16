
#include "stdafx.h"
#include "CodeClean.h"
#include "CodeCleanDlg.h"
#include "CodeFileHanding.h"
#include "Global.h"


CodeFileHanding::CodeFileHanding(void)
{


	tmp_string_buf.resize(256);
	tmp_string_buf_type.resize(256);
}


CodeFileHanding::~CodeFileHanding(void)
{
}

void CodeFileHanding::SetFilePath(CString p_filepath)
{
	bracket_num = 0;
	pre_blank_num =0;
	next_pre_blank_num = 0;
	old_string = "";
	new_string = "";
	m_filepath = p_filepath;
	m_directory = m_filepath.Left( m_filepath.ReverseFind('\\')); 
	m_directory += L"\\code_clean_tmp";
	m_filename = m_filepath.Right(m_filepath.GetLength()- m_filepath.ReverseFind('\\'));
	m_filepathtmp = m_directory +m_filename;

	if(!PathIsDirectory(m_directory))
	{
		if(!CreateDirectory(m_directory,NULL))
		{
			MessageBox(_T("�����ļ���ʧ�ܣ�") + m_directory);
			return;
		}	
	}
	CopyFile(m_filepath,m_filepathtmp,FALSE);

}
int CodeFileHanding::DoFileClean()
{
	if(!code_file.Open(m_filepath,CStdioFile::modeCreate | CStdioFile::modeWrite))
	{
		MessageBox(_T("���ļ�ʧ�ܣ�")+m_filepath);
		return 1;
	}
	if(!old_file.Open(m_filepathtmp,CStdioFile::modeRead))
	{
		MessageBox(_T("���ļ�ʧ�ܣ�")+m_filepathtmp);
		return 1;
	}

	while(old_file.ReadString(old_string))
	{

		new_string = "";
		for(int i=0;i<256;i++)
		{
			tmp_string_buf[i] = "";
			tmp_string_buf_type[i] = START;
		}
		StringAnalyze();//�в��

		StringCombine();//�����
		
		if(bracket_num != 0)
		{
			for(int i =0 ;i<pre_blank_num;i++)
			{
				code_file.WriteString(L" ");
			}
		}
		for(int i =0 ;i<pre_blank_num;i++)
		{
			code_file.WriteString(L" ");
		}
		code_file.WriteString(new_string+"\n");
		pre_blank_num = next_pre_blank_num;
		old_string = "";
	}
	code_file.Close();
	old_file.Close();

	return 0;
}

void CodeFileHanding::StringAnalyze()//�в��,��һ��Դ���밴�ո�ͷ��Ų�ֳɵ��ʺͷ��š�
{
	int symbol_type = START;
	int last_symbol_type = START;

	buf_index = -1;

	int string_len = old_string.GetLength() ;
	for( int i = 0 ;i < string_len ;i++ )
	{
		symbol_type = SymbolTypeCheck(i);
		last_symbol_type = LineSplit( i , symbol_type , last_symbol_type );
	}

}

void CodeFileHanding::StringCombine()//����ϣ��������������
{
	//TODO����ÿһ��symbol��Ϊһ�����ţ������ʱ����������ж��Ƿ�Ϊ�����ļ���27-45��ʾ��˫�ַ��е�һ����
	int j = 0 ;
	int i = 0 ;
	int tmp = 0 ;
	//�ų�#include<a.h>�����a.h���߲����пո�
	if(tmp_string_buf[0] == "#" && tmp_string_buf[1] == "include" && tmp_string_buf[2] == "<")
	{
		tmp = (tmp_string_buf[4] == ".") ? 7 : 5;
		for(i=0 ; i<tmp ; i++)
		{
			new_string += tmp_string_buf[i];
			if(i == 1)
			{
				new_string += " ";
			}
		}
	}
	for(i=tmp; i <= buf_index  ;i++)
	{
		if(tmp_string_buf[i] == "(")
		{
			bracket_num ++;
		}
		else if(tmp_string_buf[i] == ")")
		{
			bracket_num --;
		}

		if((tmp_string_buf_type[i] == SYMBOL)||(tmp_string_buf_type[i] == BRACKETS))
		{
			for(j=0 ;j<MAX_SYMBOL_TYPE ;j++)
			{
				if(g_symbol_table[j] == tmp_string_buf[i])
				{
					break;
				}
			}
			if(j<MAX_SYMBOL_TYPE)
			{
				if(g_signconfig[j] & 0x01)
				{
					if((new_string.Right(1) !=" ")&&(!new_string.IsEmpty()))
					{
						new_string += " ";
					}					
				}
				else
				{
/*
					if(new_string.Right(1) ==" ")
					{
						new_string.Delete(new_string.GetLength()-1,1);
					}	
*/
				}
				new_string += tmp_string_buf[i];

				if(g_signconfig[j] & 0x02)
				{
					new_string += " ";
				}
			}
			else
			{
				if(new_string.Right(1) !=" ")
				{
					new_string += " ";
				}	
				new_string += tmp_string_buf[i];
				new_string += " ";
			}
		}
		else
		{
			if((i>0)&&(tmp_string_buf_type[i] == CHAR_OR_NUM)&&((tmp_string_buf_type[i-1] == CHAR_OR_NUM)||(tmp_string_buf_type[i-1] == QUOTATION)))
			{
				new_string += " ";
			}
			new_string += tmp_string_buf[i];
		}
	}
}

int  CodeFileHanding::SymbolTypeCheck(int index)
{
	/*���index��ָ�����old_string�е��ַ���ȷ�������ͣ�����������
	  �ո�(' ')���Ʊ����'\t'��ΪBLANK
	  a-z��A-Z��0-9,��_ΪCHAR_OR_NUM
	  '\''��'\"'ΪQUOTATION
	  ����ΪBRACKETS
	  ����ΪSYMBOL
	*/
	int charactertype = START;
	if(old_string[index] == ' ' || old_string[index] == '\t')
	{//�հ�

		charactertype = BLANK ;
	}
	else if((old_string[index] >= 'a' && old_string[index] <= 'z') 
		|| (old_string[index] >= 'A' && old_string[index] <= 'Z')
		|| old_string[index] >= '0'&& old_string[index] <= '9'
		|| (old_string[index] == '_') || old_string[index] >=(unsigned char)0x80)
	{//�ַ�

		charactertype = CHAR_OR_NUM ;
	}
	else if((old_string[index] == '\'' || old_string[index] == '\"') && old_string[index-1] != '\\')
	{//����

		charactertype = QUOTATION ;
	}
	else if(old_string[index] == '(' || old_string[index] == ')' || old_string[index] == '[' || old_string[index] == ']')
	{
		charactertype = BRACKETS ;
	}
	else
	{//����������
		charactertype = SYMBOL ;
		if( old_string[index] == '{')
		{
			next_pre_blank_num += g_indentation;
		}
		else if(old_string[index] == '}')
		{
			pre_blank_num -= g_indentation;
			next_pre_blank_num -= g_indentation;
		}
	}
	return charactertype;
}
/*
int CodeFileHanding::LineSplit(int index ,int symbol, int last_symbol )
{
	int linestatus = START;
	switch(last_symbol)
	{
		case START:
		{
			if(symbol == START)
			{
				linestatus = START;
				break;
			}
			else if (symbol == BLANK)
			{
				linestatus = START;
				break;
			}
			else if (symbol == SYMBOL)
			{
				linestatus = SYMBOL;
				tmp_string_buf_type[++buf_index] = SYMBOL;
			}
			else if(symbol == BRACKETS)
			{
				linestatus = BRACKETS;
				tmp_string_buf_type[++buf_index] = BRACKETS;
			}
			else if (symbol == CHAR_OR_NUM)
			{
				linestatus =CHAR_OR_NUM;
				tmp_string_buf_type[++buf_index] = CHAR_OR_NUM;
			}
			else if (symbol == QUOTATION)
			{
				linestatus = QUOTATION;
				tmp_string_buf_type[++buf_index] = QUOTATION;
			}
			tmp_string_buf[buf_index] = "";
			tmp_string_buf[buf_index] += old_string[index];
			
			break;
		}
		case SYMBOL:
		{
			if(symbol == START)
			{
				linestatus = START ;

			}
			else if (symbol == BLANK)
			{
				linestatus = START ;

			}
			else if (symbol == SYMBOL)
			{
				linestatus = SYMBOL;
//				tmp_string_buf[++buf_index ] = "" ;
				tmp_string_buf[buf_index] += old_string[index];
				tmp_string_buf_type[buf_index] = SYMBOL;
			}
			else if (symbol == BRACKETS)
			{
				linestatus = BRACKETS;
				tmp_string_buf[++buf_index ] = "" ;
				tmp_string_buf[buf_index] += old_string[index];
				tmp_string_buf_type[buf_index] = BRACKETS;
			}
			else if (symbol == CHAR_OR_NUM)
			{
				linestatus = CHAR_OR_NUM;
				tmp_string_buf[++buf_index ] = "" ;
				tmp_string_buf[buf_index] += old_string[index];
				tmp_string_buf_type[buf_index] = CHAR_OR_NUM;
			}
			else if (symbol == QUOTATION)
			{
				linestatus = QUOTATION;
				tmp_string_buf[++buf_index ] = "" ;
				tmp_string_buf[buf_index] += old_string[index];
				tmp_string_buf_type[buf_index] = QUOTATION;
			}
			break;
		}
		case CHAR_OR_NUM:
		{
			if(symbol == START)
			{
				linestatus = START;

			}
			else if (symbol == BLANK)
			{
				linestatus = START;

			}
			else if (symbol == SYMBOL)
			{
				linestatus = SYMBOL;
				tmp_string_buf[++buf_index ] = "" ;
				tmp_string_buf[buf_index] += old_string[index];
				tmp_string_buf_type[buf_index] = SYMBOL;
			}
			else if(symbol == BRACKETS)
			{
				linestatus = BRACKETS;
				tmp_string_buf[++buf_index ] = "" ;
				tmp_string_buf[buf_index] += old_string[index];
				tmp_string_buf_type[buf_index] = BRACKETS;
			}
			else if (symbol == CHAR_OR_NUM)
			{
				linestatus = CHAR_OR_NUM;
				tmp_string_buf[buf_index] += old_string[index];
				tmp_string_buf_type[buf_index] = CHAR_OR_NUM;
			}
			else if (symbol == QUOTATION)
			{
				linestatus = QUOTATION;
				tmp_string_buf[++buf_index ] = "" ;
				tmp_string_buf[buf_index] += old_string[index];
				tmp_string_buf_type[buf_index] = QUOTATION;
			}
			break;
		}
		case QUOTATION:
		{
			linestatus = (symbol == QUOTATION) ? START : QUOTATION;
			tmp_string_buf[buf_index] += old_string[index];
			tmp_string_buf_type[buf_index] = QUOTATION;
			if(linestatus != QUOTATION)
			{
				tmp_string_buf[++buf_index ] = "" ;
			}
			break;
		}
		case BRACKETS:
		{
			if(symbol == START)
			{
				linestatus = START ;

			}
			else if (symbol == BLANK)
			{
				linestatus = START ;

			}
			else if (symbol == SYMBOL)
			{
				linestatus = SYMBOL;
				tmp_string_buf[++buf_index ] = "" ;
				tmp_string_buf[buf_index] += old_string[index];
				tmp_string_buf_type[buf_index] = SYMBOL;
			}
			else if (symbol == BRACKETS)
			{
				linestatus = BRACKETS;
				tmp_string_buf[++buf_index ] = "" ;
				tmp_string_buf[buf_index] += old_string[index];
				tmp_string_buf_type[buf_index] = BRACKETS;
			}
			else if (symbol == CHAR_OR_NUM)
			{
				linestatus = CHAR_OR_NUM;
				tmp_string_buf[++buf_index ] = "" ;
				tmp_string_buf[buf_index] += old_string[index];
				tmp_string_buf_type[buf_index] = CHAR_OR_NUM;
			}
			else if (symbol == QUOTATION)
			{
				linestatus = QUOTATION;
				tmp_string_buf[++buf_index ] = "" ;
				tmp_string_buf[buf_index] += old_string[index];
				tmp_string_buf_type[buf_index] = QUOTATION;
			}
			break;
		}
	}

	return linestatus;
}
*/
int CodeFileHanding::LineSplit(int index ,int symbol, int last_symbol )
{
	//TODO:ע���ų�/*  */
	int linestatus = START;
	CString Symbol_tmp = L"";
	switch(last_symbol)
	{
	case START:
		{
			if(symbol == START)
			{
				linestatus = START;
				break;
			}
			else if (symbol == BLANK)
			{
				linestatus = START;
				break;
			}
			else if (symbol == SYMBOL)
			{
				linestatus = SYMBOL;
				tmp_string_buf_type[++buf_index] = SYMBOL;
			}
			else if(symbol == BRACKETS)
			{
				linestatus = BRACKETS;
				tmp_string_buf_type[++buf_index] = BRACKETS;
			}
			else if (symbol == CHAR_OR_NUM)
			{
				linestatus =CHAR_OR_NUM;
				tmp_string_buf_type[++buf_index] = CHAR_OR_NUM;
			}
			else if (symbol == QUOTATION)
			{
				linestatus = QUOTATION;
				tmp_string_buf_type[++buf_index] = QUOTATION;
			}
			tmp_string_buf[buf_index] = "";
			tmp_string_buf[buf_index] += old_string[index];

			break;
		}
	case SYMBOL:
		{
			if(symbol == START)
			{
				linestatus = START ;

			}
			else if (symbol == BLANK)
			{
				linestatus = START ;

			}
			else if (symbol == SYMBOL)
			{
				linestatus = SYMBOL;
				Symbol_tmp = tmp_string_buf[buf_index] +old_string[index];
				for(int i =0;i<MAX_SYMBOL_TYPE;i++)
				{
					if(Symbol_tmp == g_symbol_table[i])
					{
						tmp_string_buf[buf_index] = Symbol_tmp;
						return linestatus;
					}
				}
				tmp_string_buf[++buf_index ] = "" ;
				tmp_string_buf[buf_index] += old_string[index];
				tmp_string_buf_type[buf_index] = SYMBOL;
			}
			else if (symbol == BRACKETS)
			{
				linestatus = BRACKETS;
				tmp_string_buf[++buf_index ] = "" ;
				tmp_string_buf[buf_index] += old_string[index];
				tmp_string_buf_type[buf_index] = BRACKETS;
			}
			else if (symbol == CHAR_OR_NUM)
			{
				linestatus = CHAR_OR_NUM;
				tmp_string_buf[++buf_index ] = "" ;
				tmp_string_buf[buf_index] += old_string[index];
				tmp_string_buf_type[buf_index] = CHAR_OR_NUM;
			}
			else if (symbol == QUOTATION)
			{
				linestatus = QUOTATION;
				tmp_string_buf[++buf_index ] = "" ;
				tmp_string_buf[buf_index] += old_string[index];
				tmp_string_buf_type[buf_index] = QUOTATION;
			}
			break;
		}
	case CHAR_OR_NUM:
		{
			if(symbol == START)
			{
				linestatus = START;

			}
			else if (symbol == BLANK)
			{
				linestatus = START;

			}
			else if (symbol == SYMBOL)
			{
				linestatus = SYMBOL;
				tmp_string_buf[++buf_index ] = "" ;
				tmp_string_buf[buf_index] += old_string[index];
				tmp_string_buf_type[buf_index] = SYMBOL;
			}
			else if(symbol == BRACKETS)
			{
				linestatus = BRACKETS;
				tmp_string_buf[++buf_index ] = "" ;
				tmp_string_buf[buf_index] += old_string[index];
				tmp_string_buf_type[buf_index] = BRACKETS;
			}
			else if (symbol == CHAR_OR_NUM)
			{
				linestatus = CHAR_OR_NUM;
				tmp_string_buf[buf_index] += old_string[index];
				tmp_string_buf_type[buf_index] = CHAR_OR_NUM;
			}
			else if (symbol == QUOTATION)
			{
				linestatus = QUOTATION;
				tmp_string_buf[++buf_index ] = "" ;
				tmp_string_buf[buf_index] += old_string[index];
				tmp_string_buf_type[buf_index] = QUOTATION;
			}
			break;
		}
	case QUOTATION:
		{
			linestatus = (symbol == QUOTATION) ? START : QUOTATION;
			tmp_string_buf[buf_index] += old_string[index];
			tmp_string_buf_type[buf_index] = QUOTATION;
			if(linestatus != QUOTATION)
			{
				tmp_string_buf[++buf_index ] = "" ;
			}
			break;
		}
	case BRACKETS:
		{
			if(symbol == START)
			{
				linestatus = START ;

			}
			else if (symbol == BLANK)
			{
				linestatus = START ;

			}
			else if (symbol == SYMBOL)
			{
				linestatus = SYMBOL;
				tmp_string_buf[++buf_index ] = "" ;
				tmp_string_buf[buf_index] += old_string[index];
				tmp_string_buf_type[buf_index] = SYMBOL;
			}
			else if (symbol == BRACKETS)
			{
				linestatus = BRACKETS;
				tmp_string_buf[++buf_index ] = "" ;
				tmp_string_buf[buf_index] += old_string[index];
				tmp_string_buf_type[buf_index] = BRACKETS;
			}
			else if (symbol == CHAR_OR_NUM)
			{
				linestatus = CHAR_OR_NUM;
				tmp_string_buf[++buf_index ] = "" ;
				tmp_string_buf[buf_index] += old_string[index];
				tmp_string_buf_type[buf_index] = CHAR_OR_NUM;
			}
			else if (symbol == QUOTATION)
			{
				linestatus = QUOTATION;
				tmp_string_buf[++buf_index ] = "" ;
				tmp_string_buf[buf_index] += old_string[index];
				tmp_string_buf_type[buf_index] = QUOTATION;
			}
			break;
		}
	}
	return linestatus;
}