/*

전공 : 컴퓨터전공
학번 : 2012004087
이름 : 이기준

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

Transaction DB를 tid순으로 내부 items를 순차적으로 스캔한다. (2중 for문)
Tid의 items를 하나씩 보면서 1-itemset candidates에 있으면 support값을 높여주고 없으면 새로 갱신해주는 방식으로 구현했다.
(사실, 1-itemset candidates는 map<int, int> (itemset, sup)으로 구현하는게 적절하지만 모든 n개의 itemset candidates를 모두 담는 변수가 있기 때문에 통일성을 위하여 map<set<int>, int>로 구현하였다.)

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

 이전 n-1 frequent itemset candidates 중 하나를 고정한 뒤 ,
차집합의 원소 개수가 1인 또다른 candidates들이 n개가 있으면 n개의 itemset을 하나로 합쳐서 
새로운 n itemset candidates를 구성한다.
GenerateCandidates에서는 고정된 n-1 frequent itemset candidates를 지정하며,
Extra_generateCandidates() 는 재귀형식으로 고정된 candidates와 차집합의 원소개수를 확인한다. 
n개를 충족하면 재귀 탈출조건이 만족되어 n-itemset candidates에 추가시킨다.
이 함수에서는 support값은 세지 않는다. 오직 n-itemset candidates만 구성한다.

*/
void GenerateCandidates(int n, map< set<int>, int> &tmpMap, map<set<int>, int>  & prevMap)
{
	set<int> tmpSet;
	for (map<set<int>, int >::iterator ms_iter = prevMap.begin(); ms_iter != prevMap.end(); ms_iter++)
		extra_GenerateCandidates(n - 1, ms_iter, next(ms_iter, 1), tmpSet, tmpMap, prevMap);
}



/**

Transaction DB의 tid를 하나씩 스캔하면서 만들어진 n-itemset candidates에 해당되면 support 값을 count한다.

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

Apriori algorithm의 원리에 따라서,
Support / Transaction id의 개수 * 100  < minimum support 일경우는 pruning의 대상이 된다.
지워지지 않은 itemset candidates는 frequent하다.

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

output단계에서 candidates를 itemset과 associate itemset으로 나누는 작업이다.
Candidates의 부분집합과 그 부분집합의 여집합도 같이 뽑아낸다.
공집합과 전체집합은 itemset이나 associate itemset이 없어지므로 생략한다.

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

