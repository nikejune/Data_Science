/*

���� : ��ǻ������
�й� : 2012004087
�̸� : �̱���

*/

#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <fstream>
#include <algorithm> 
#include <sstream>
#include <set>
#include <map>
#include <iomanip>
#include <cmath>
using namespace std;

void ReadInputFile(vector<set<int> > & DB);
void WriteOutputFile(vector<set<int> > & DB, vector<map<set<int>, int> > & vecMap);

void FirstDBScan(vector<set<int> > & DB, map< set<int>, int> &tmpMap);
void GenerateCandidates(int n, map< set<int>, int> &tmpMap, map<set<int>, int> & prevMap);
void CountSup(vector<set<int> > &DB, map< set<int>, int> & tmpMap);
void LeaveFrequentCandidates(vector<set<int> > &DB, map<set<int>, int> &tmpMap, int min_sup);
void Powerset(set<int> &S, map<set<int>, set<int> > & retMap);

int main(int argc, char* argv[]) 
{
	if (argc == 1) {
		cout << "You should type MIN SUPPORT" << endl;
		return -1;
	}
	
	vector<set<int> > DB;
	vector<map<set<int>, int> > vecMap;
	map<set<int>, int> tempMap;
	
	int min_sup = atoi(argv[1]);


	//	input.txt -> DB
	ReadInputFile(DB);
	
	//DB 1st scan
	FirstDBScan(DB, tempMap);
	// pruning : remove not frequent pattern
	LeaveFrequentCandidates(DB, tempMap, min_sup);

	vecMap.push_back(tempMap);
	tempMap.clear();
	
	for (int i = 2; i <= vecMap.at(0).size(); i++) {
		//scan : generate candidates
		GenerateCandidates(i, tempMap, vecMap.at(i-2));
		// count the numbers of support 
		CountSup(DB, tempMap);
		// pruning : remove not frequent pattern
		LeaveFrequentCandidates(DB, tempMap, min_sup);
		if (tempMap.size() == 0)
			break;
		vecMap.push_back(tempMap);
		tempMap.clear();
	}

	// vecMap -> output.txt
	WriteOutputFile(DB, vecMap);
	return 0;
}
/**

input.txt -> DB

*/
void ReadInputFile(vector<set<int> > & DB)
{

	ifstream inputFile("input.txt");
	string tempLine;
	if (inputFile.is_open()) {
		while (getline(inputFile, tempLine, '\n')) {

			set<int> tempset;
			stringstream ss(tempLine);
			string temp;
			while (getline(ss, temp, '\t')) {
				tempset.insert(stoi(temp));
			}
			DB.push_back(tempset);
		}
	}
	else
		cout << "you need input.txt file" << endl;

}

/**

vecMap -> output.txt 

*/
void WriteOutputFile(vector<set<int> > & DB, vector<map<set<int>, int> > & vecMap)
{
	map<set<int>, set<int> > retMap;

	ofstream outputFile("output.txt");
	if (outputFile.is_open()) {
		outputFile << fixed << setprecision(2);

		for (vector<map<set<int>, int> >::iterator vms_iter = ++vecMap.begin(); vms_iter != vecMap.end(); vms_iter++)
			for (map<set<int>, int>::iterator ms_iter = vms_iter->begin(); ms_iter != vms_iter->end(); ms_iter++)
			{
				set<int> tmpS = ms_iter->first;
				Powerset(tmpS, retMap);
				for (map<set<int>, set<int> >::iterator mss_iter = retMap.begin(); mss_iter != retMap.end(); mss_iter++)
				{
					int S_cnt = 0;
					int S_A_cnt = 0;
					for (vector<set<int> >::iterator vs_iter = DB.begin(); vs_iter != DB.end(); vs_iter++)
					{
						set<int> tmpS2;

						tmpS2.insert(vs_iter->begin(), vs_iter->end());
						tmpS2.insert(mss_iter->first.begin(), mss_iter->first.end());
						if (tmpS2.size() == vs_iter->size()) {
							S_cnt++;
							tmpS2.insert(mss_iter->second.begin(), mss_iter->second.end());
							if (tmpS2.size() == vs_iter->size())
								S_A_cnt++;
						}
					}
					outputFile << "{";
					int idx = 0;
					for (set<int>::iterator s__iter = mss_iter->first.begin(); s__iter != mss_iter->first.end(); s__iter++, idx++)
					{
						outputFile << *s__iter;
						if (idx != mss_iter->first.size() - 1) {
							outputFile << ",";
						}
						else {
							outputFile << "}\t";
						}
					}

					outputFile << "{";
					idx = 0;
					for (set<int>::iterator s__iter = mss_iter->second.begin(); s__iter != mss_iter->second.end(); s__iter++, idx++)
					{
						outputFile << *s__iter;
						if (idx != mss_iter->second.size() - 1) {
							outputFile << ",";
						}
						else {
							outputFile << "}\t";
						}
					}
					outputFile << floor((double)ms_iter->second / DB.size() * 10000 + 0.5) / 100 << "\t";
					outputFile << floor((double)S_A_cnt / S_cnt * 10000 + 0.5) / 100 << endl;
				}

				retMap.clear();
			}
		outputFile.close();
	}

}
/**

Transaction DB�� tid������ ���� items�� ���������� ��ĵ�Ѵ�. (2�� for��)
Tid�� items�� �ϳ��� ���鼭 1-itemset candidates�� ������ support���� �����ְ� ������ ���� �������ִ� ������� �����ߴ�.
(���, 1-itemset candidates�� map<int, int> (itemset, sup)���� �����ϴ°� ���������� ��� n���� itemset candidates�� ��� ��� ������ �ֱ� ������ ���ϼ��� ���Ͽ� map<set<int>, int>�� �����Ͽ���.)

*/
void FirstDBScan(vector<set<int> > & DB, map< set<int>, int> &tmpMap)
{
	set<int> tmpSet;
	map<set<int>, int>::iterator ms_iter;
	for (vector<set<int> >::iterator vs_iter = DB.begin(); vs_iter != DB.end(); vs_iter++) {
		for (set<int>::iterator s_iter = vs_iter->begin(); s_iter != vs_iter->end(); s_iter++) {
			tmpSet.insert(*s_iter);
			ms_iter = tmpMap.find(tmpSet);
			if (ms_iter == tmpMap.end()) {
				tmpMap.insert(map<set<int>, int>::value_type(tmpSet, 1));
			}
			else
				ms_iter->second++;
			tmpSet.clear();
		}
	}
}

void extra_GenerateCandidates(int n, map<set<int>, int >::iterator origin_iter, map<set<int>, int >::iterator cur_iter, set<int> &tmpSet, map< set<int>, int> &tmpMap, map<set<int>, int>  & prevMap) {


	if (n == 0)
	{
		set<int> uni;
		uni.insert(origin_iter->first.begin(), origin_iter->first.end());
		uni.insert(tmpSet.begin(), tmpSet.end());
		tmpMap.insert(map<set<int>, int>::value_type(uni, 0));
		return;
	}

	for (; cur_iter != prevMap.end(); cur_iter++)
	{

		set<int> diff;
		diff.insert(origin_iter->first.begin(), origin_iter->first.end());
		for (set<int>::iterator iter = cur_iter->first.begin(); iter != cur_iter->first.end(); iter++)
		{
			set<int>::iterator iter2 = diff.find(*iter);
			if (iter2 != diff.end())
				diff.erase(*iter);
		}
		if (diff.size() == 1) {
			if (n == 1)
				tmpSet = cur_iter->first;
			extra_GenerateCandidates(n - 1, origin_iter, next(cur_iter, 1), tmpSet, tmpMap, prevMap);

		}
	}
}

/**

 ���� n-1 frequent itemset candidates �� �ϳ��� ������ �� ,
�������� ���� ������ 1�� �Ǵٸ� candidates���� n���� ������ n���� itemset�� �ϳ��� ���ļ� 
���ο� n itemset candidates�� �����Ѵ�.
GenerateCandidates������ ������ n-1 frequent itemset candidates�� �����ϸ�,
Extra_generateCandidates() �� ����������� ������ candidates�� �������� ���Ұ����� Ȯ���Ѵ�. 
n���� �����ϸ� ��� Ż�������� �����Ǿ� n-itemset candidates�� �߰���Ų��.
�� �Լ������� support���� ���� �ʴ´�. ���� n-itemset candidates�� �����Ѵ�.

*/
void GenerateCandidates(int n, map< set<int>, int> &tmpMap, map<set<int>, int>  & prevMap)
{
	set<int> tmpSet;
	for (map<set<int>, int >::iterator ms_iter = prevMap.begin(); ms_iter != prevMap.end(); ms_iter++)
		extra_GenerateCandidates(n - 1, ms_iter, next(ms_iter, 1), tmpSet, tmpMap, prevMap);
}



/**

Transaction DB�� tid�� �ϳ��� ��ĵ�ϸ鼭 ������� n-itemset candidates�� �ش�Ǹ� support ���� count�Ѵ�.

*/
void CountSup(vector<set<int> > &DB, map<set<int>, int> &tmpMap)
{
	for (vector<set<int> >::iterator vs_iter = DB.begin(); vs_iter != DB.end(); vs_iter++)
		for (map<set<int>, int>::iterator ms_iter = tmpMap.begin(); ms_iter != tmpMap.end(); ms_iter++)
		{
			bool test = includes(vs_iter->begin(), vs_iter->end(), ms_iter->first.begin(), ms_iter->first.end());
			if (test)
				ms_iter->second++;
		}
}

/**

Apriori algorithm�� ������ ����,
Support / Transaction id�� ���� * 100  < minimum support �ϰ��� pruning�� ����� �ȴ�.
�������� ���� itemset candidates�� frequent�ϴ�.

*/
void LeaveFrequentCandidates(vector<set<int> > &DB, map<set<int>, int> &tmpMap, int min_sup)
{
	map<set<int>, int>::iterator ms_iter = tmpMap.begin();
	while (ms_iter != tmpMap.end())
	{
		if ((double)ms_iter->second / DB.size() * 100 < min_sup)
			tmpMap.erase(ms_iter++);
		else
			ms_iter++;
	}
}

/**

output�ܰ迡�� candidates�� itemset�� associate itemset���� ������ �۾��̴�.
Candidates�� �κ����հ� �� �κ������� �����յ� ���� �̾Ƴ���.
�����հ� ��ü������ itemset�̳� associate itemset�� �������Ƿ� �����Ѵ�.

*/
void Powerset(set<int> &S, map<set<int>, set<int> > & retMap)
{
	set<int>::iterator iter;
	set<int> tmpSet, tmpSet2;
		int i, j;
	int max = 1 << S.size();
	for (i = 1; i<max-1; i++) {
		for (iter= S.begin(),j = 0; j<S.size(); j++, iter++) {
			if (i & (1 << j))
				tmpSet.insert(*iter);
			else
				tmpSet2.insert(*iter);
		}
		retMap.insert(map<set<int>, set<int> >::value_type(tmpSet, tmpSet2));
		tmpSet.clear();
		tmpSet2.clear();
	}

}

