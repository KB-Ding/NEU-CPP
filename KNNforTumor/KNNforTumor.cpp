// KNNTraining.cpp: 定义控制台应用程序的入口点。

#include "stdafx.h"
#include "sstream"
#include<iostream>
#include"fstream"
#include<map>  
#include<vector>  
#include<stdio.h>  
#include<cmath>  
#include<string>
#include<cstdlib>  
#include<algorithm>
#include"RBT.h"
#include<ctime>
using namespace std;

typedef string ClassType;
typedef string DataType;

map<int, map<string, map<string, double>>>dictionaryM;//第几个属性 的 哪个量 和 哪个类别  的 PMI

static const int ClassNum = 22;
static const int labelNum = 17;
static map<int, vector <string>>Allstrs;
static  int correct;
ifstream ftest;

int GetRandomValue(int min, int max)
{
	int randValue;
	srand(unsigned(time(NULL)));  //产生随机数种子
	randValue = min + rand() % (max - min + 1);  //生成随机数
	return randValue;
}


std::vector<std::string> split(std::string str, std::string pattern) {
	std::string::size_type pos;
	std::vector<std::string> rresult;
	str += pattern;
	int size = str.size();

	for (int i = 0; i < size; i++) {
		pos = str.find(pattern, i);
		if (pos < size) {
			std::string s = str.substr(i, pos - i);
			rresult.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	return rresult;
}

bool compareClass(pair<ClassType, int>& lhs, const pair<ClassType, int>& rhs)
{
	return lhs.second >rhs.second;
}

bool compare(pair<int, double>& lhs, pair<int, double>& rhs)
{
	return lhs.second < rhs.second;
}


class KNN {
private:
	int k;
	int SampleNum;
	DataType(*dataSet)[labelNum];

	ClassType *classes;
	ClassType *testclasses;
	//LabelType labels[labelNum];
	DataType* testData;
	map<int, DataType*> testDataeS;
	map<int, double> DistanceOrderByIndex;
	map<ClassType, int> RankClass;


public:
	KNN(int k, int sample, vector<int>&strs, vector<int>&testStrs);
	~KNN(void);
	void get_all_distance(int fh);
	double get_distance(DataType *d1, DataType *d2);
	void get_max_freq_label(int w);
	int GetSampleNum();
	void calculateKnn(void);



};

KNN::~KNN(void) {

	/*delete labels;
	delete classes;
	delete testclasses;
	delete testData;*/
}
KNN::KNN(int k, int sample, vector<int >&strs, vector<int>&testStrs) {


	this->k = k;
	this->SampleNum = 9 * sample;
	dataSet = new DataType[SampleNum][labelNum];

	classes = new ClassType[SampleNum];

	vector <string>result;
	for (int f = 0; f < strs.size(); ++f) {
		result = Allstrs[strs[f]];
		for (int h = 0; h < labelNum; h++) {
			dataSet[f][h] = result[h];

		}
		classes[f] = result[labelNum];
		result.clear();
	}

	//	cout << "Test:" << endl;

	testclasses = new ClassType[sample];

	vector <string>Testresult;
	for (int f = 0; f < testStrs.size(); ++f) {
		testDataeS[f] = new DataType[labelNum];
		Testresult = Allstrs[testStrs[f]];
		for (int h = 0; h < labelNum; h++) {
			testDataeS[f][h] = Testresult[h];

		}
		testclasses[f] = Testresult[labelNum];
		Testresult.clear();
	}

}

int KNN::GetSampleNum() {
	return SampleNum;
}

double KNN::get_distance(DataType *d1, DataType *d2) {
	double sum = 0;
	int res;
	for (int i = 0; i<labelNum; i++) {

		if (d1[i] == d2[i]) {
			res = 0;
		}
		else {
			res = 1;
		}
		sum += res;
	}
	return sqrt(sum);
}

void KNN::get_all_distance(int fh) {
	double distance;

	testData = testDataeS[fh];//每次抽出一个test
							  //cout << " 测试数据： ( ";
							  //for (int f = 0; f < labelNum; f++) {
							  //	cout << testData[f] << ",";
							  //}
							  //cout << "）" << endl;
	for (int i = 0; i<SampleNum; i++) {
		distance = get_distance(dataSet[i], testData);
		DistanceOrderByIndex[i] = distance;
	}


}

void KNN::get_max_freq_label(int w) {
	//map转换到vector：SortVector
	vector<pair<int, double>> SortVector;

	for (map<int, double>::iterator curr = DistanceOrderByIndex.begin(); curr != DistanceOrderByIndex.end(); curr++) {
		SortVector.push_back(make_pair(curr->first, curr->second));
	}

	//排序：SortVector,首次输出最小，升序排序
	sort(SortVector.begin(), SortVector.end(), compare);

	for (int i = 0; i<k; i++) {
		//	cout << "索引：" << SortVector[i].first << " 距离：" << SortVector[i].second << " 类别：" << classes[SortVector[i].first]
		//		<< " 标签值： ( ";
		for (int f = 0; f < labelNum; f++) {
			//		cout << dataSet[SortVector[i].first][f] << ",";
		}
		//	cout << "）" << endl;
		//取出排名第i个（当前）类别名称，记录出现次数到value（int），记录map：RankClass
		pair<int, double> indexWithDistance;
		indexWithDistance = SortVector[i];
		ClassType currentClass = classes[indexWithDistance.first];
		RankClass[currentClass]++;
	}
	SortVector.clear();

	//RankClass记录的map转换到vector：SortClassV
	vector<pair<ClassType, int>> SortClassV;

	for (map<ClassType, int>::iterator curr = RankClass.begin(); curr != RankClass.end(); curr++) {
		SortClassV.push_back(make_pair(curr->first, curr->second));
	}
	//排序，出现次数最多排在最首
	std::sort(SortClassV.begin(), SortClassV.end(), compareClass);
	pair<ClassType, int> choosePair = SortClassV[0];
	string chooseClass = choosePair.first;

	//	cout << "分类结果： " << chooseClass << " 类" << endl;
	//	cout << "实际结果： " << testclasses[w] << " 类" << endl;
	if (chooseClass == testclasses[w]) {
		correct++;//正确
	}
	RankClass.clear();
	DistanceOrderByIndex.clear();
	SortClassV.clear();
}

void KNN::calculateKnn(void) {
	for (int fh = 0; fh < SampleNum / 9; fh++) {
		KNN::get_all_distance(fh);
		KNN::get_max_freq_label(fh);
	}
}

int main(void) {
	int k;
	cout << "please input the k value : " << endl;
	cin >> k;
	string newline;
	vector<int>strs;
	map<int, vector<int>> storeMap;
	ifstream fin;
	fin.open("primary-tumor.txt");
	correct = 0;

	if (!fin) {
		cout << "can not open the file data.txt" << endl;
		exit(1);
	}

	/* input the dataSet */
	int sample = 0;

	while (getline(fin, newline)) {
		vector<string>v;
		v = split(newline, ",");
		Allstrs[sample] = v;
		++sample;
	}
	fin.close();


	/*换标签*/
	int count;
	string tempS;
	vector<string>findV;
//	double sep;
	int tempD;
	map<string, map<int, string>>fillM;//标识 列数 属性 存放最大频率属性
	map<string, int> moreFill;//类型名，出现次数
	map<string, int> lessFill;//类型名，出现次数
	map<string, int> middFill;//类型名，出现次数

	for (int z = 1; z < labelNum; z++) {


		for (int sam = 0; sam < sample; sam++) {
			if (Allstrs[sam][0] == "'>=60'") {
				if (Allstrs[sam][z] != "?") {
					moreFill[Allstrs[sam][z]]++;//计算出现次数
				}
				
			}
			else if (Allstrs[sam][0] == "'30-59'") {
				if (Allstrs[sam][z] != "?") {
					middFill[Allstrs[sam][z]]++;//计算出现次数
				}
				
			}
			else if (Allstrs[sam][0] == "'<30'") {
				if (Allstrs[sam][z] != "?") {
					lessFill[Allstrs[sam][z]]++;//计算出现次数
				}
				
			}
			
		}
		vector<pair<string, int>> menVec;

		for (map<string, int>::iterator curr = moreFill.begin(); curr != moreFill.end(); curr++) {
			menVec.push_back(make_pair(curr->first, curr->second));
		}
		std::sort(menVec.begin(), menVec.end(), compareClass);
		pair<string, int> choosePair = menVec[0];
		string mostMen = choosePair.first;
		fillM["'>=60'"][z] = mostMen;
		menVec.clear();

		for (map<string, int>::iterator curr = middFill.begin(); curr != middFill.end(); curr++) {
			menVec.push_back(make_pair(curr->first, curr->second));
		}
		std::sort(menVec.begin(), menVec.end(), compareClass);
		pair<string, int> choosePair1 = menVec[0];
		string mostMen1 = choosePair1.first;
		fillM["'30-59'"][z] = mostMen1;
		menVec.clear();

		for (map<string, int>::iterator curr = lessFill.begin(); curr != lessFill.end(); curr++) {
			menVec.push_back(make_pair(curr->first, curr->second));
		}
		std::sort(menVec.begin(), menVec.end(), compareClass);
		pair<string, int> choosePair2 = menVec[0];
		string mostMen2 = choosePair2.first;
		fillM["'<30'"][z] = mostMen;
		menVec.clear();
		lessFill.clear();
		middFill.clear();
		moreFill.clear();

	}

	for (int z = 1; z < labelNum; z++) {


		for (int sam = 0; sam < sample; sam++) {
			if (Allstrs[sam][z] == "?") {
				Allstrs[sam][z] = fillM[Allstrs[sam][0]][z];//Allstrs[sam][0]:首部标识符
			}
		}
	}
	
//	cout << Allstrs[0][0];
	

	//map<string, int> mention;//类型名，出现次数
	//map<int, map<string, double>>searchM;//第几个属性 的 哪个量 的 频率

	//for (int z = 0; z < labelNum + 1; z++) {

	//	for (int sam = 0; sam < sample; sam++) {
	//		mention[Allstrs[sam][z]]++;//计算出现次数
	//	}

	//	map<string, double>temp;
	//	for (map<string, int>::iterator curr = mention.begin(); curr != mention.end(); curr++) {
	//		temp[curr->first] = (double)curr->second / sample;//记录每个类别出现频率
	//	}

	//	searchM[z] = temp;//第z个属性里面的temp
	//	temp.clear();
	//	mention.clear();

	//}

	//for (int z = 0; z < labelNum + 1; z++) {
	//	double positive = 0;
	//	double negative = 0;

	//	for (int sam = 0; sam < sample; sam++) {
	//		mention[Allstrs[sam][z]]++;//类别统计
	//	}

	//	for (map<string, int>::iterator curr = mention.begin(); curr != mention.end(); curr++) {

	//		for (int sam = 0; sam < sample; sam++) {

	//			if (curr->first == Allstrs[sam][z] && Allstrs[sam][labelNum] == "positive") {
	//				positive++;
	//			}
	//			else if (Allstrs[sam][z] == curr->first&&Allstrs[sam][labelNum] == "negative") {
	//				negative++;
	//			}


	//		}
	//		cout << positive << endl;
	//		cout << searchM[z][curr->first] << endl;
	//		cout << searchM[labelNum]["positive"] << endl;
	//		cout << "****" << (double)(positive / sample) / (searchM[z][curr->first] * searchM[labelNum]["positive"]) << endl;

	//		dictionaryM[z][curr->first]["positive"] = max(0.0, log((double)(positive / sample) / (searchM[z][curr->first] * searchM[labelNum]["positive"])));//存表
	//		dictionaryM[z][curr->first]["negative"] = max(0.0, log((double)(negative / sample) / (searchM[z][curr->first] * searchM[labelNum]["negative"])));

	//	}

	//	mention.clear();

	//}


	cout << "生成随机序列" << endl;
	int F = floor(sample / 10);

	RBTree RBT = RBTree();
	srand(unsigned(time(NULL)));

	for (int m = 0; m < 10; m++) {

		for (int n = 0; n< F; n++) {

			int i = rand() % sample;  //生成随机数

			if (RBT.search(i)) {//红黑树检验
				n--;
				//	cout << "continuing";
				continue;
			}
			else {
				RBT.insert(i);
				cout << i << " ; ";
				strs.push_back(i);
			}

		}
		cout << endl;
		storeMap[m] = strs;
		strs.clear();

	}




	vector<int> tempV;
	for (int w = 0; w < 10; w++) {
		//	cout << "w等于：" << w<<endl;
		int h;
		if (w == 0) {
			h = 1;
		}
		else {
			h = 0;
		}

		tempV = storeMap[h];

		for (int f = 0; f < 10; f++) {
			if (f == w || f == h) { continue; }
			else {
				//	cout <<"f等于：" <<f<<endl;

				tempV.insert(tempV.end(), storeMap[f].begin(), storeMap[f].end());
			}
		}
		//	cout << "h等于：" << h << endl;

		KNN knn(k, F, tempV, storeMap[w]);
		knn.calculateKnn();
		tempV.clear();

	}
	cout << "***********************************" << endl;
	cout << "正确率：" << (double)correct / sample << endl;
	system("pause");

	return 0;
}