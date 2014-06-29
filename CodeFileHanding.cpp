
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
	line_status = START;
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
			MessageBox(_T("建立文件夹失败：") + m_directory);
			return;
		}	
	}
	CopyFile(m_filepath,m_filepathtmp,FALSE);

}
/* /////////////////////////////////////
输 入：无
输 出：
功 能：对单个文件进行处理
*/ /////////////////////////////////////
int CodeFileHanding::DoFileClean()
{
	int blank_num = 0;
	int last_line_status =START;

	if(!code_file.Open(m_filepath,CStdioFile::modeCreate | CStdioFile::modeWrite))
	{
		MessageBox(_T("打开文件失败：")+m_filepath);
		return 1;
	}
	if(!old_file.Open(m_filepathtmp,CStdioFile::modeRead))
	{
		MessageBox(_T("打开文件失败：")+m_filepathtmp);
		return 1;
	}

	while(old_file.ReadString(old_string))
	{
		//变量初始化
		new_string = "";
		for(int i=0;i<256;i++)
		{
			tmp_string_buf[i] = "";
			tmp_string_buf_type[i] = START;
		}
		//行拆分
		StringAnalyze();

		//行组合
		StringCombine();

		//空格计算与行写入
		if(line_status == START && last_line_status == START )	///**/中保存原来面目，不做处理，两个状态用来处理*/符号
		{
			blank_num = BracketNumCheck();//空格计算

			for(int i =0 ;i<blank_num;i++)
			{
				code_file.WriteString(L" ");
			}
			code_file.WriteString(new_string+"\n");
		}
		else
		{
			code_file.WriteString(old_string+"\n");
		}

		//变量清理
		old_string = "";
		last_line_status = line_status ;
	}
	code_file.Close();
	old_file.Close();

	return 0;
}

void CodeFileHanding::StringAnalyze()//行拆分,将一行源代码按空格和符号拆分成单词和符号。
{
	int symbol_type = START;
	int last_symbol_type = line_status;
	if(last_symbol_type != START)
	{
		buf_index = 0;
	}
	else 
	{
		buf_index = -1;
	}

	int string_len = old_string.GetLength() ;
	for( int i = 0 ;i < string_len ;i++ )
	{
		symbol_type = SymbolTypeCheck(i);
		last_symbol_type = LineSplit(i , symbol_type , last_symbol_type );		
	}

}

void CodeFileHanding::StringCombine()//行组合，将单词组合起来
{
	//TODO：将每一个symbol作为一个符号，在组合时依次组合来判定是否为配置文件中27-45所示的双字符中的一个。
	int j = 0 ;
	int i = 0 ;
	int tmp = 0 ;
	//排除#include<a.h>情况，a.h两边不能有空格
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
	/*检测index所指向的在old_string中的字符，确定其类型，并返回类型
	  空格(' ')和制表符（'\t'）为BLANK
	  a-z，A-Z，0-9,及_为CHAR_OR_NUM
	  '\''和'\"'为QUOTATION
	  括号为BRACKETS
	  其他为SYMBOL
	*/
	int charactertype = START;
	if(old_string[index] == ' ' || old_string[index] == '\t')
	{//空白

		charactertype = BLANK ;
	}
	else if((old_string[index] >= 'a' && old_string[index] <= 'z') 
		|| (old_string[index] >= 'A' && old_string[index] <= 'Z')
		|| old_string[index] >= '0'&& old_string[index] <= '9'
		|| (old_string[index] == '_') || old_string[index] >=(unsigned char)0x80)
	{//字符

		charactertype = CHAR_OR_NUM ;
	}
	else if((old_string[index] == '\'' || old_string[index] == '\"') && !(((index-1)>=0 &&(old_string[index-1] == '\\')) &&( (index-2 <0)||(old_string[index-2] != '\\'))))
	{//引号

		charactertype = QUOTATION ;
	}
	else if(old_string[index] == '(' || old_string[index] == ')' || old_string[index] == '[' || old_string[index] == ']')
	{
		charactertype = BRACKETS ;
	}
	else if((old_string[index] == '/') && (old_string[index+1] == '/'))
	{
		charactertype = NOTE_SLASH ;
	}
	else if((old_string[index] == '/') && (old_string[index+1] == '*'))
	{
		charactertype = NOTE_STAR_START ;
	}
	else if((old_string[index] == '/') && (index -1>=0) &&(old_string[index-1] == '*'))
	{
		charactertype = NOTE_STAR_END ;
	}
	else
	{//其他，符号
		charactertype = SYMBOL ;
/*		if( old_string[index] == '{')
		{
			next_pre_blank_num += g_indentation;
		}
		else if(old_string[index] == '}')
		{
			pre_blank_num -= g_indentation;
			next_pre_blank_num -= g_indentation;
		}
		*/
	}
	return charactertype;
}

int CodeFileHanding::LineSplit(int index ,int symbol, int last_symbol )
{
	//TODO:注释排除/*  */
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
				line_status =  QUOTATION;
			}
			else if(symbol == NOTE_SLASH)
			{
				linestatus = NOTE_SLASH;
				tmp_string_buf_type[++buf_index] = NOTE_SLASH;
			}
			else if(symbol == NOTE_STAR_START)
			{
				linestatus = NOTE_STAR_START;
				tmp_string_buf_type[++buf_index] = NOTE_STAR_START;
				line_status =  NOTE_STAR_START;
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
				line_status =  QUOTATION;
			}
			else if(symbol == NOTE_SLASH)
			{
				linestatus = NOTE_SLASH ;
				tmp_string_buf[++buf_index ] = "" ;
				tmp_string_buf[buf_index] += old_string[index];
				tmp_string_buf_type[buf_index] = NOTE_SLASH ;
			}
			else if(symbol == NOTE_STAR_START)
			{
				linestatus = NOTE_STAR_START ;
				tmp_string_buf[++buf_index ] = "" ;
				tmp_string_buf[buf_index] += old_string[index];
				tmp_string_buf_type[buf_index] = NOTE_STAR_START ;
				line_status =  NOTE_STAR_START;
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
				line_status =  QUOTATION;
			}
			else if(symbol == NOTE_SLASH)
			{
				linestatus = NOTE_SLASH ;
				tmp_string_buf[++buf_index ] = "" ;
				tmp_string_buf[buf_index] += old_string[index];
				tmp_string_buf_type[buf_index] = NOTE_SLASH ;
			}
			else if(symbol == NOTE_STAR_START)
			{
				linestatus = NOTE_STAR_START ;
				tmp_string_buf[++buf_index ] = "" ;
				tmp_string_buf[buf_index] += old_string[index];
				tmp_string_buf_type[buf_index] = NOTE_STAR_START ;
				line_status =  NOTE_STAR_START;
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
				line_status =  START;
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
				line_status =  QUOTATION;
			}
			else if(symbol == NOTE_SLASH)
			{
				linestatus = NOTE_SLASH ;
				tmp_string_buf[++buf_index ] = "" ;
				tmp_string_buf[buf_index] += old_string[index];
				tmp_string_buf_type[buf_index] = NOTE_SLASH ;
			}
			else if(symbol == NOTE_STAR_START)
			{
				linestatus = NOTE_STAR_START ;
				tmp_string_buf[++buf_index ] = "" ;
				tmp_string_buf[buf_index] += old_string[index];
				tmp_string_buf_type[buf_index] = NOTE_STAR_START ;
				line_status =  NOTE_STAR_START;
			}
			break;
		}
	case NOTE_SLASH:
		{
			linestatus = NOTE_SLASH ;
			tmp_string_buf[buf_index] += old_string[index];
			tmp_string_buf_type[buf_index] = NOTE_SLASH;

			break ;
		}
	case NOTE_STAR_START:
		{
			linestatus = (symbol == NOTE_STAR_END) ? START : NOTE_STAR_START;
			tmp_string_buf[buf_index] += old_string[index];
			tmp_string_buf_type[buf_index] = NOTE_STAR_START;
			if(linestatus != NOTE_STAR_START)
			{
				tmp_string_buf[++buf_index ] = "" ;
				line_status =  START;
			}
			break;
		}
	}
	return linestatus;
}

int CodeFileHanding::BracketNumCheck(void)
{
	int i=0;
	int blank_num = 0;

	last_bracket_num = bracket_num;
	pre_blank_num = next_pre_blank_num;

	for(i=0 ;i <= buf_index ;i++)
	{
		if(tmp_string_buf[i] == "(" && tmp_string_buf_type[i]  == BRACKETS)
		{
			bracket_num ++;
		}
		else if(tmp_string_buf[i] == ")" && tmp_string_buf_type[i]  == BRACKETS)
		{
			bracket_num --;
		}

		if(tmp_string_buf[i] == "{" && tmp_string_buf_type[i]  == SYMBOL)
		{
			next_pre_blank_num += g_indentation;
		}
		else if(tmp_string_buf[i] == "}" && tmp_string_buf_type[i]  == SYMBOL)
		{
			next_pre_blank_num -= g_indentation;
		}
	}
	if(tmp_string_buf[0] == "}" && tmp_string_buf_type[0]  == SYMBOL)
	{
		blank_num = pre_blank_num - g_indentation;
		bracket_num = 0;		//小括号容错，如果在大括号的时候，小括号数目不为0 ，则出错，将括号数目清零。
	}
	else
	{
		blank_num = pre_blank_num ;
	}
	//小括号处理
	if(last_bracket_num > 0)
	{
		blank_num += g_indentation;
	}

	return blank_num;
}