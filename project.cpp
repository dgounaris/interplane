#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <list>
using namespace std;

bool comparePoints(const pair<int,double> first, const pair<int,double> second) {
    return first.second < second.second;
}

void readInput(list<pair<int,double>>* points, map<int,vector<double>>* segments) {
    ifstream inFile("input.txt");
    int segmentsNum = 0;
    if (inFile.is_open()) {
        inFile >> segmentsNum;
    }
    else {
        cout << "Unable to open file\n";
    }
    if (segmentsNum > 0) {
        int i;
        double a, b, c, t1, t2;
        for (i=0;i<segmentsNum;i++) {
            vector<double> segment;
            inFile >> a >> b >> c >> t1 >> t2;
            vector<double>::iterator it = segment.begin();
            it = segment.insert(it,a);
            it = segment.insert(++it,b);
            it = segment.insert(++it,c);
            it = segment.insert(++it,t1);
            segment.insert(++it,t2);
            (*segments).insert( std::pair<int, vector<double>>(i, segment) ); //logn
            (*points).push_back( pair<int,double>(i, t1) ); //const
            (*points).push_back( pair<int,double>(i, t2) );
        }
    }
    (*points).sort(comparePoints);
    inFile.close();
    return;
}

int main() {
    double minX=0.0; //value from which we should start the sweep
    map<int,vector<double>> segments;
    list<pair<int,double>> points;
    readInput(&points, &segments);
    //printing the read input for debug purposes
    for (map<int,vector<double>>::iterator it = segments.begin(); it != segments.end(); it++) {
        int i;
        cout << it->first << " : ";
        for (i=0;i<(it->second).size();i++) {
            cout << (it->second).at(i) << " ";
        }
        cout << "\n";
    }
    for (list<pair<int,double>>::iterator listIt = points.begin(); listIt != points.end(); listIt++) {
        cout << (*listIt).first << " => " << (*listIt).second << endl;
    }
    //initialize status
    set<int> status;
    set<int>::iterator statusIt;
}