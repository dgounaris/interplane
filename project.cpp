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

class SegmentInfo {
    public:
        double a, b, c, t1, t2;
};

bool compareEvents(const EventInfo first, const EventInfo second) {
    return first.x < second.x;
}

void readInput(list<EventInfo>* eventQ, map<int,SegmentInfo>* segments) {
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
            SegmentInfo segment;
            inFile >> a >> b >> c >> t1 >> t2;
            segment.a = a;
            segment.b = b;
            segment.c = c;
            segment.t1 = t1;
            segment.t2 = t2;
            (*segments).insert( std::pair<int, SegmentInfo>(i, segment) ); //logn
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

void run(list<EventInfo>& eventQ, set<int>& status, map<int,SegmentInfo> segments) {
    while (eventQ.size()>0) {

    }
}

int main() {
    map<int,SegmentInfo> segments;
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