
/********************************************************************
created:	pinorr
file base:	logic.h
author:		pinorr	Q 505971450
purpose:	跑胡子算法定义
*********************************************************************/

#include <windows.h>

#define		MAX_CARDS_NUM		84			// 
#define		MAX_TYPE			21			// 20张牌 + 赖子
#define		INVALID_VAL			0x00

//结点类型
enum enNode
{
	enNode_error,
	enNode_line,
	enNode_27A,
	enNode_DDd,
	enNode_num2,
	enNode_num3,
	enNode_num4,
	enNode_max,
};

//规则类型
enum enPlayType
{
	enPlayType_NORMAL = 0,		//一般
	enPlayType_XDD = 1,			//小大搭
	enPlayType_27A = 2,			//二七十
	enPlayType_123 = 3,			//一二三
	enPlayType_WANG = 4,		//王
	enPlayType_max,
};

#define TY_MY	(1+(1<<enPlayType_WANG)+(1<<enPlayType_123)+(1<<enPlayType_27A)+(1<<enPlayType_XDD))

static BYTE s_byIndex[MAX_TYPE] =
{
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,		// 一 ~ 十
	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A,		// 壹 ~ 拾
	0xFF,															// 赖子
};

static bool		isHavePlayType(enPlayType type)
{
	if (type >= enPlayType_max)
		return false;
	return (TY_MY & (1 << type)) != 0;
}

static BYTE		getIndexByVal(BYTE byCard)
{
	if (byCard == 0xFF) 
		return 20;
	BYTE byCor = byCard & 0xF0;
	BYTE byIndex = (byCor >> 4) * 10 + (byCard & 0x0F) - 1;
	if (byIndex >= MAX_TYPE)
		return 0xFF;
	return byIndex;
}

static void		getCardsByVal(__int64 llVal, BYTE byOut[4])
{
	BYTE byAll = 0;
	for (int i = 0; i < MAX_TYPE; ++i)
	{
		BYTE num = ((llVal >> (i * 3)) & 7);
		if (num > 0)
		{
			memset(&byOut[byAll], s_byIndex[i], min(4 - byAll, num));
			byAll += num;
		}
		if (byAll >= 4)
			break;
	}
}

static __int64	getKey(BYTE n1, BYTE n2, BYTE n3 = 0, BYTE n4 = 0)
{
	__int64 llKey = 0;
	BYTE index = getIndexByVal(n1);
	if (index < MAX_TYPE)
		llKey += (__int64)1 << (index * 3);
	index = getIndexByVal(n2);
	if (index < MAX_TYPE)
		llKey += (__int64)1 << (index * 3);
	index = getIndexByVal(n3);
	if (index < MAX_TYPE)
		llKey += (__int64)1 << (index * 3);
	index = getIndexByVal(n4);
	if (index < MAX_TYPE)
		llKey += (__int64)1 << (index * 3);
	return llKey;
}

inline int		getCardsNum(__int64 llVal, BYTE n)
{
	if (n < MAX_TYPE)
		return BYTE((llVal >> (n * 3)) & 7);

	return 0;
}

inline BYTE		getNumByKey(__int64 llVal)
{
	BYTE nNum = 0;
	for (int i = 0; i < MAX_TYPE; ++i)
		nNum += (llVal >> (3 * i)) & 7;
	return nNum;
}

inline bool		isContainKey(__int64 llVal, __int64 llKey)
{
	for (int i = 0; i < MAX_TYPE; ++i)
	{
		BYTE key = (llKey >> (3 * i)) & 7;
		if (key > 0 && key > ((llVal >> (3 * i)) & 7))
			return false;
	}
	return true;
}