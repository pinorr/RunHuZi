// test.cpp : 定义控制台应用程序的入口点。
//
#include "iostream"
#include "logic.h"
#include <string>

using namespace std;

void main()
{	
	BYTE cbCardData[] =
	{
		0x01, 0x01,
		0x02, 0x02, 0x02,
		0x03, 0x03, 0x03,
		0x14, 0x14, 0x04,
		0x15, 0x16, 0x05,
		0x16, 0x16, 0x06,
		0xFF, 0xFF, 0xFF,
		0xFF
	};

	//初始化
	CLogicPHZ m_logic;

	int num = 0;
	DWORD dwFlag = GetTickCount();
	for (int i = 0; i < 1; ++i)		//可执行很多次，方便计算耗时
		num = m_logic.getHuKeyInit(cbCardData, sizeof(cbCardData), 3, true);

	cout << GetTickCount() - dwFlag << " ms" << endl;

	// 获取结果
	vector<stAnswer> vctOut;
	m_logic.getAnswer(vctOut);
	// 打印结果
	for (int i = 0; i < min(50, (int)vctOut.size()); ++i)
	{
		stAnswer stTemp = vctOut[i];
		cout << "hu_xi = " << (int)stTemp.getHuXi() << endl;	// 总息数
		for (int n = 0; n < stTemp.num; ++n)					// 最多七组手牌
		{
			BYTE byOut[4] = {};
			BYTE byOutTry[4] = {};
			getCardsByVal(stTemp.llNode[n], byOut);				// 取出原始牌

			cout << "xi" << n << "=" << (int)stTemp.byNodeXi[n] << '\t';
			for (int m = 0; m < 4; ++m)	{
				cout << hex << "0x" << (int)byOut[m] << '\t';
			}
			cout << dec << endl;
		}
	}
	//*/
	cout << "num = " << num << '\t' << "RealNum = " << vctOut.size() << endl;
	Sleep(1000000);
}
