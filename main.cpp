#include"Network_Structure.h"
#include"O_D.h"
#include<ctime>
using namespace std;

int main() {
	/*
	//Sixous Falls Network
	//创建结果文件
	ofstream fout("F://网络建模与优化//交通流分配问题//Sixous Falls Network Result (MultiThread).txt", ios_base::out);
	if (!fout.is_open())
	{
		cerr << "打开文件失败!" << endl;
		return -1;
	}
	
	//导入数据
	vector<Arc>* ArcList = LoadArcList("F://网络建模与优化//交通流分配问题//Sixous Falls Network.csv");
	vector<vector<ODpair>>* ODpairs = LoadODpairs("F://网络建模与优化//交通流分配问题//Sixous Falls Network OD.csv");
	*/
	
	//Chicago sketch network
	//创建结果文件
	ofstream fout("F://网络建模与优化//交通流分配问题//Chicago Sketch Network Result (SingleThread).txt", ios_base::out);
	if (!fout.is_open())
	{
		cerr << "打开文件失败!" << endl;
		return -1;
	}
	//导入数据
	vector<Arc>* ArcList = LoadArcList("F://网络建模与优化//交通流分配问题//Chicago Sketch Network.csv");
	vector<vector<ODpair>>* ODpairs = LoadODpairs("F://网络建模与优化//交通流分配问题//Chicago Sketch Network OD.csv");
	
	
	NetworkStructure* Network = new NetworkStructure(*ArcList);
	/*
	for (int j = 0; j < (*ODpairs).back().size(); j++) {
		cout << (*ODpairs).back()[j].getOrigin() << "\t" << (*ODpairs).back()[j].getDestination() << "\t" << (*ODpairs).back()[j].getDemand() << endl;
	}
	*/
	/*
	//最短路算法测试
	
	vector<vector<Path>> All_ShortestPath = Network->MultiThread_Label_Correcting(*ODpairs);
	
	for (int OriNum = 0; OriNum < (*ODpairs).size(); OriNum++) {
		for (int DesNum = 0; DesNum < (*ODpairs)[OriNum].size(); DesNum++) {
			int Origin = (*ODpairs)[OriNum][DesNum].getOrigin();
			int Destination = (*ODpairs)[OriNum][DesNum].getDestination();
			double Demand = (*ODpairs)[OriNum][DesNum].getDemand();
			cout << "点" << Origin << "到点" << Destination << "的最短路：" << endl;
			cout << All_ShortestPath[OriNum][Destination - 1] << endl;
		}
	}
	*/
	
	clock_t StartTime = clock();
	Network->FW_Algorithm(*ODpairs, &NetworkStructure::SingleThread_Label_Correcting, fout);
	clock_t EndTime = clock();
	cout << "运行时间：" << ((double)EndTime - (double)StartTime) / CLOCKS_PER_SEC <<"秒"<< endl;
	fout << "运行时间：" << ((double)EndTime - (double)StartTime) / CLOCKS_PER_SEC << "秒" << endl;
	
	
	//释放内存
	delete Network;
	delete ODpairs;
	delete ArcList;
	//关闭文件
	fout.close();
	return 0;
}