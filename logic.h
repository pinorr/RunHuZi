/********************************************************************
created:	pinorr
file base:	logic.h
author:		pinorr	Q 505971450
purpose:	跑胡子基本算法
*********************************************************************/

#include "cmd_define.h"
#include <map>
#include <vector>
#include <algorithm>  
#include <cmath>

using namespace std;

map<__int64, BYTE>	g_mapKeyAll;

#define MAX_ANSWER_NUM		1024

struct stAnswer
{
	BYTE	num;
	__int64	llNode[7];
	BYTE	byNodeXi[7];
	BYTE	byIndex[7];

	stAnswer() { memset(this, 0, sizeof(*this)); }
	__int64	push(__int64 llVal, BYTE index)
	{
		map<__int64, BYTE>::iterator iter = g_mapKeyAll.find(llVal);
		if (iter != g_mapKeyAll.end() && num < 7)
		{
			byNodeXi[num] = iter->second;
			byIndex[num] = index;
			llNode[num++] = llVal;
			return llVal;
		}
		return 0;
	}
	__int64	pop()
	{
		if (num > 0)
			return llNode[--num];
		return 0;
	}
	BYTE	getHuXi()
	{
		BYTE byXi = 0;
		for (int i = 0; i < num; ++i)
			byXi += byNodeXi[i];
		return byXi;
	}
};

class CLogicPHZ
{
public:
	CLogicPHZ()
	{
		m_nAnswerNum = 0;
		m_nValNum = 0;
		m_nNullNum = 0;
		//TrainKey();
		readConfig("config.txt");
	}

	bool	readConfig(string strFile)
	{
		m_vctKeyVal.clear();
		m_vctKeyNull.clear();
		g_mapKeyAll.clear();
		char szPath[MAX_PATH] = { 0 };
		GetCurrentDirectoryA(MAX_PATH, szPath);
		char szFileName[MAX_PATH] = { 0 };
		sprintf_s(szFileName, "%s/%s", szPath, strFile.c_str());

		FILE *fp;
		errno_t err = fopen_s(&fp, szFileName, "r");
		if (fp != 0)
		{
			char szData[900] = { 0 };
			// 设置文件起始位置
			fseek(fp, 0, SEEK_SET);
			while (!feof(fp))
			{
				memset(szData, 0, sizeof(szData));
				vector<string> vctVal;
				// 读取一行
				fread(szData, sizeof(szData), 1, fp);
				for (int i = 0; i < 900; i += 9) {	
					__int64 lVal = *(__int64 *)&szData[i];
					if (lVal == 0) 
						break;					
					g_mapKeyAll[lVal] = *(BYTE *)&szData[i + 8];
				}
			}
			fclose(fp);	
			map<__int64, BYTE>::iterator iter = g_mapKeyAll.begin();
			for (; iter != g_mapKeyAll.end(); ++iter)
			{
				if (getNumByKey(iter->first) <= 2) continue;
				if (iter->second == 0) {
					m_mapKeyIndex[iter->first] = 158 + m_vctKeyNull.size();
					m_vctKeyNull.push_back(iter->first);
				}
				else {
					m_mapKeyIndex[iter->first] = m_vctKeyVal.size();
					m_vctKeyVal.push_back(iter->first);
				}
			}
			m_nValNum = m_vctKeyVal.size();
			m_nNullNum = m_vctKeyNull.size();
			return true;
		}
		return false;
	}

	int		getHuKeyInit(BYTE *cbCards, BYTE num, BYTE byThreshXi, bool bFindAll = false)
	{
		m_byThreshXi = byThreshXi;
		__int64 llVal = 0;
		for (int i = 0; i < num; ++i)
		{
			BYTE index = getIndexByVal(cbCards[i]);
			if (index >= MAX_TYPE) return false;
			llVal += (__int64)1 << (3 * index);
		}

		if (getCardsNum(llVal, 20) > 4)
			return 0;

		vector<BYTE>	vctIndex3;
		stAnswer		answer;
		for (int i = 0; i < 20; ++i)
		{
			BYTE num = getCardsNum(llVal, i);
			if (num == 4)
			{
				__int64 llNode = (__int64)4 << (i * 3);
				map<__int64, BYTE>::iterator iter = m_mapKeyIndex.find(llNode);
				if (iter == m_mapKeyIndex.end())
					continue;						// 不可能到此
				answer.push(llNode, iter->second);
				llVal -= llNode;
			}
			else if (num == 3)
			{
				vctIndex3.push_back(i);
				llVal -= __int64((__int64)3 << (i * 3));
			}
		}
		m_nAnswerNum = 0;
		BYTE nKing = getCardsNum(llVal, 20);
		BYTE nNum3 = vctIndex3.size();
		for (int flag = 0; flag < ((int)1 << nNum3); ++flag)
		{
			if (!bFindAll && m_nAnswerNum > 0)
				return m_nAnswerNum;

			BYTE nNum = 0;
			for (int i = 0; i < 8; ++i)
				nNum += (flag >> i) & 1;
			if (nNum > nKing) continue;

			__int64		llTemp = llVal;
			stAnswer	stTemp = answer;
			for (size_t i = 0; i < vctIndex3.size(); ++i)
			{
				__int64 llNode = (__int64)3 << (vctIndex3[i] * 3);
				if (flag & (1 << i)) {
					llNode += (__int64)1 << 60;
					llTemp -= (__int64)1 << 60;
				}
				map<__int64, BYTE>::iterator iter = m_mapKeyIndex.find(llNode);
				if (iter == m_mapKeyIndex.end())
					continue;						// 不可能到此
				stTemp.push(llNode, iter->second);
			}
			getHuKey(llTemp, byThreshXi, stTemp, 0, bFindAll);
		}
		return m_nAnswerNum;
	}

	void	getHuKey(__int64 llVal, BYTE byThreshXi, stAnswer &answer, int nBegin, bool bFindAll)
	{
		if (m_nAnswerNum >= MAX_ANSWER_NUM)
			return;
		if (!bFindAll && m_nAnswerNum > 0)
			return;

		BYTE byLeft = getNumByKey(llVal);
		if (byLeft == 0){
			if (answer.getHuXi() >= byThreshXi)
				m_answer[m_nAnswerNum++] = answer;
			return;
		}
		else if (byLeft % 3 == 1){
			return;
		}
		else if (byLeft == 3 || byLeft == 2){
			map<__int64, BYTE>::iterator iter = g_mapKeyAll.find(llVal);
			if (iter != g_mapKeyAll.end())
			{
				iter = m_mapKeyIndex.find(llVal);
				BYTE index = iter != m_mapKeyIndex.end() ? iter->second : 0xFF;
				answer.push(llVal, index);
				if (answer.getHuXi() >= byThreshXi)
					m_answer[m_nAnswerNum++] = answer;
				answer.pop();
			}
			return;
		}
		else{
			int i = nBegin;
			//回溯有息牌组
			for (; i < m_nValNum; ++i)
			{
				if (isContainKey(llVal, m_vctKeyVal[i]))
				{
					llVal -= answer.push(m_vctKeyVal[i], i);
					BYTE byNum = getNumByKey(llVal);
					getHuKey(llVal, byThreshXi, answer, i, bFindAll);
					llVal += answer.pop();
				}
			}

			if (answer.getHuXi() <= byThreshXi)
				return;

			BYTE byNum = getNumByKey(llVal);
			if (byNum == 0)
				m_answer[m_nAnswerNum++] = answer;
			else if (byNum == 1)
				return;			
			else
			{
				//回溯无息牌组
				i -= m_nValNum;
				for (; i < m_nNullNum; ++i)
				{
					if (isContainKey(llVal, m_vctKeyNull[i]))
					{
						llVal -= answer.push(m_vctKeyNull[i], i + m_nValNum);
						BYTE byNum = getNumByKey(llVal);
						getHuKey(llVal, byThreshXi, answer, i + m_nValNum, bFindAll);
						llVal += answer.pop();
					}
				}
			}
		}
	}

	// 此处需过滤重复的答案
	void	getAnswer(vector<stAnswer> &vctOut)
	{
		if (m_nAnswerNum > 0 && m_nAnswerNum < MAX_ANSWER_NUM)
		{
			int			answer_num = 0;
			stAnswer	answer_temp[MAX_ANSWER_NUM];
			map<string, BYTE> mapFlag;
			char chrTemp[1024] = {};
			for (int i = 0; i < m_nAnswerNum; ++i) {
				stAnswer temp = m_answer[i];
				sort(temp.byIndex, temp.byIndex + 7);
				sprintf_s(chrTemp, "%02X%02X%02X%02X%02X%02X%02X", (int)temp.byIndex[0], (int)temp.byIndex[1], (int)temp.byIndex[2],
					(int)temp.byIndex[3], (int)temp.byIndex[4], (int)temp.byIndex[5], (int)temp.byIndex[6]);

				map<string, BYTE>::iterator iter = mapFlag.find(chrTemp);
				if (iter == mapFlag.end()) {
					mapFlag[chrTemp] = 1;
					answer_temp[answer_num++] = m_answer[i];
				}
				else
					continue;
			}			
			vctOut.resize(answer_num);
			memcpy(&vctOut[0], answer_temp, answer_num*sizeof(stAnswer));
		}
	}

private:
	inline void addMapVal(__int64 llKey, BYTE val)
	{
		BYTE num = getNumByKey(llKey);
		if (g_mapKeyAll[llKey] < val)
			g_mapKeyAll[llKey] = val;
	}

private:
	int						m_nAnswerNum;
	stAnswer				m_answer[MAX_ANSWER_NUM];
	vector<__int64>			m_vctKeyVal;					// 158
	vector<__int64>			m_vctKeyNull;					// 72
	map<__int64, BYTE>	m_mapKeyIndex;				// 72+158
	int						m_nValNum;
	int						m_nNullNum;
	BYTE					m_byThreshXi;
};
