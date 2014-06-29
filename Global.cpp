#include "StdAfx.h"
#include "Global.h"

unsigned int  g_indentation;
unsigned char g_signconfig[MAX_SYMBOL_TYPE]={0};
CString g_symbol_table[MAX_SYMBOL_TYPE] =
{
	L"+" ,L"-" ,L"*" ,L"/" ,L"=" ,L"%" ,L"&" ,L"|" ,L">" ,L"<" ,

	L"^" ,L"?" ,L"(" ,L"[" ,L"{" ,L")" ,L"]" ,L"}" ,L"!" ,L"~" ,

	L"+" ,L"-" ,L"*" ,L"&" ,L";" ,L"," ,L"++" ,L"--" ,L"==" ,L"+=" ,

	L"-=" ,L"*=" ,L"/=" ,L"%=" ,L"&=" ,L"|=" ,L"^=" ,L"->" ,L"<<" ,L">>" ,

	L">=" ,L"<=" ,L"!=" ,L"&&" ,L"||" ,L"." ,L"#" ,L"//" ,L"/*" ,L"*/" ,

	L"<<=",L">>=",L"::" ,L"" ,L"" ,L"" ,L"" ,L"" ,L"" ,L"" ,
	L"" ,L"" ,L"" ,L"" ,
};