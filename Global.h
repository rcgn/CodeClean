//Global.h
#pragma once

#define MAX_SYMBOL_TYPE  64

enum g_signname
{
	//	 0+    1-     2*     3/      4=     5%     6&      7|      8>      9<
	plus ,minus ,multi ,divide ,equal ,model ,bitand ,bitor ,greater ,less ,
	//	10^    11?      12(    13[    14{    15)     16]     17}    18!    19~
	xor ,question ,left1 ,left2 ,left3 ,right1 ,right2 ,right3 ,exct ,bitnot ,
	//	  20+       21-      22*     23&        24;       25,     26++         27--        28==         29+=      
	positive ,negative ,point ,reference ,semicolon ,comma ,doubleplus ,doubleminus ,doubleequal ,plusequal ,
	//	  30-=         31*=       32/=         33%=        34&=      35|=     36^=      37->     38<<     39>>     
	minusequal ,multiequal ,divideequal ,modelequal ,andequal ,orequal ,xorequal ,pointto ,turnleft ,turnright ,
	//	  40>=       41<=       42!=   43&& 44|| 45.   46#       47//         48/*         49*/
	greaterequal ,lessequal ,notequal ,and ,or , dot , sharp , doubleslash , slashstar , starslash , 
	//    50<<=        51>>=
	leftmoveequal , rightmoveequal ,
};

extern unsigned char g_signconfig[MAX_SYMBOL_TYPE];

extern CString g_symbol_table[MAX_SYMBOL_TYPE];

extern unsigned int  g_indentation;