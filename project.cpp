#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <string>
using namespace std;

class EventInfo {
    public:
        char type;
        vector<int> involvedSegments;
        double x, y;
};

bool compareEvents(const EventInfo first, const EventInfo second) {
    return first.x < second.x;
}

void readInput(list<EventInfo>* eventQ, map<int,vector<double>>* segments) {
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
            //start point
            EventInfo mySInfo;
            mySInfo.involvedSegments.push_back(i); mySInfo.type = 'S';
            mySInfo.x = t1; mySInfo.y = a*t1*t1 + b*t1 + c;
            (*eventQ).push_back(mySInfo);
            //end point
            EventInfo myEInfo;
            myEInfo.involvedSegments.push_back(i); myEInfo.type = 'E';
            myEInfo.x = t2; myEInfo.y = a*t2*t2 + b*t2 + c;
            (*eventQ).push_back(myEInfo);
        }
    }
    (*eventQ).sort(compareEvents);
    inFile.close();
    return;
}

void printStatus(set<int> status) {
    cout << "status: " << status.size() << ":";
    set<int>::iterator it = status.begin();
    for (it; it != status.end(); it++) {
        cout << " " << (*it);
    }
    cout << "\n";
    return;
}

int main() {
    double minX=0.0; //value from which we should start the sweep
    map<int,vector<double>> segments;
    //initialize event queue and read segment equations
    list<EventInfo> eventQ;
    readInput(&eventQ, &segments);
    //initialize status
    set<int> status;
    set<int>::iterator statusIt;
    string param;
    do {
        cin >> param;
        if (param.compare("status") == 0) {
            printStatus(status);
        }
    } while (param.compare("end") != 0);
}