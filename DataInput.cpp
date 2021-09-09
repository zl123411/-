#include"Network_Structure.h"
using namespace std;


bool tailsort(Arc& a1, Arc& a2) {
	return a1.getInitNode() < a2.getInitNode();
}

vector<Arc>* LoadArcList(string data_path) {
	vector<Arc>* ArcList = new vector<Arc>;
	ArcInput reader(data_path);
	reader.readline();	//第一行为标签
	while (!reader.readline()) {
		ArcList->push_back(Arc(reader.data));
	}
	//按起点从小到大排序
	sort(ArcList->begin(), ArcList->end(), tailsort);
	return ArcList;
}

vector<vector<ODpair>>* LoadODpairs(string data_path) {
	ODInput reader(data_path);
	vector<vector<ODpair>>* ODpairs = new vector<vector<ODpair>>;
	vector<ODpair> All_ODpairs;
	int Origin = 1;
	double TotalFlow = 0;
	reader.readline();
	while (!reader.readline()) {
		string* str_point = reader.data;
		if (str_point[1].empty()) {
			Origin = atoi(str_point[0].c_str());
			continue;
		}
		for (int i = 0; i < 10; i += 2) {
			if (!str_point[i].empty()) {
				double Demand = atof(str_point[i + 1].c_str());
				All_ODpairs.push_back(ODpair(Origin, atoi(str_point[i].c_str()), Demand));
				TotalFlow += Demand;
			}
			else break;
		}
	}
	//二维数组转换
	vector<ODpair> OriginOD;
	int PreOrigin = All_ODpairs[0].getOrigin();
	for (int od = 0; od < All_ODpairs.size(); od++) {
		int Origin = All_ODpairs[od].getOrigin();
		if (Origin == PreOrigin) {
			OriginOD.push_back(All_ODpairs[od]);
		}
		else {
			ODpairs->push_back(OriginOD);
			OriginOD.clear();
			OriginOD.push_back(All_ODpairs[od]);
			PreOrigin = Origin;
		}
	}
	ODpairs->push_back(OriginOD);
	
	cout << "ODpairs导入成功" << endl;
	cout << "Total OD Flow：" << TotalFlow << endl;
	cout << "--------------------" << endl;
	return ODpairs;
}