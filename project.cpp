#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <string>
#include <cmath>
using namespace std;

double planeSweepX;

class EventInfo {
    public:
        char type;
        vector<int> involvedSegments;
        double x, y;
};

class SegmentInfo {
    public:
        int ref;
        double a, b, c, t1, t2;
};

struct leastSegment {
    bool operator()(const SegmentInfo& s1, const SegmentInfo& s2) {
        return (s1.a*planeSweepX*planeSweepX + s1.b*planeSweepX + s1.c) <
                (s2.a*planeSweepX*planeSweepX + s2.b*planeSweepX + s2.c);
    }
};

struct leastEvent {
    bool operator()(const EventInfo& first, const EventInfo& second) {
        return first.x < second.x;
    }
};

void readInput(set<EventInfo,leastEvent>& eventQ, map<int,SegmentInfo>* segments) {
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
            segment.ref = i;
            (*segments).insert( std::pair<int, SegmentInfo>(i, segment) ); //logn
            //start point
            EventInfo mySInfo;
            mySInfo.involvedSegments.push_back(i); mySInfo.type = 'S';
            mySInfo.x = t1; mySInfo.y = a*t1*t1 + b*t1 + c;
            eventQ.insert(mySInfo);
            //end point
            EventInfo myEInfo;
            myEInfo.involvedSegments.push_back(i); myEInfo.type = 'E';
            myEInfo.x = t2; myEInfo.y = a*t2*t2 + b*t2 + c;
            eventQ.insert(myEInfo);
        }
    }
    inFile.close();
    return;
}

void recreateStatus(map<SegmentInfo, int, leastSegment>& status) {
    map<SegmentInfo, int, leastSegment> newStatus;
    for(map<SegmentInfo, int, leastSegment>::iterator sIt = status.begin(); sIt != status.end(); sIt++) {
        newStatus.insert( pair<SegmentInfo,int>(sIt->first,sIt->second) );
    }
    status = newStatus;
}

void computeIntersections(set<EventInfo,leastEvent>& eventQ, SegmentInfo s1, SegmentInfo s2) {
    //using this: https://codereview.stackexchange.com/questions/51011/calculating-the-point-of-intersection-of-two-parabolas
    double intX1 = sqrt( ((s2.c-s1.c) + (s1.b-s2.b)*(s1.b-s2.b)/(s1.a-s2.a)) / (s1.a-s2.a) ) - (s1.b-s2.b)/(2*(s1.a-s2.a));
    double intX2 = -sqrt( ((s2.c-s1.c) + (s1.b-s2.b)*(s1.b-s2.b)/(s1.a-s2.a)) / (s1.a-s2.a) ) - (s1.b-s2.b)/(2*(s1.a-s2.a));
    //check if our intersection points are valid
    if (intX1>=s1.t1 && intX1>=s2.t1 && intX1<=s1.t2 && intX1<=s2.t2) {
        EventInfo e1;
        e1.involvedSegments.push_back(s1.ref); e1.involvedSegments.push_back(s2.ref);
        e1.type = 'I'; e1.x = intX1; e1.y = s1.a*intX1*intX1 + s1.b*intX1 + s1.c;
        eventQ.insert(e1);
        cout << "Insert intersection between " << s1.ref << " " << s2.ref << " at " << e1.x << endl;
    }
    if (intX2>=s1.t1 && intX2>=s2.t1 && intX2<=s1.t2 && intX2<=s2.t2) {
        EventInfo e2;
        e2.involvedSegments.push_back(s1.ref); e2.involvedSegments.push_back(s2.ref);
        e2.type = 'I'; e2.x = intX2; e2.y = s1.a*intX2*intX2 + s1.b*intX2 + s1.c;
        eventQ.insert(e2);
        cout << "Insert intersection between " << s1.ref << " " << s2.ref << " at " << e2.x << endl;
    }
    return;
}

void execNextEvent(set<EventInfo,leastEvent>& eventQ, map<SegmentInfo,int,leastSegment>& status, map<int,SegmentInfo> segments) {
    EventInfo nextEvent = *eventQ.begin();
    eventQ.erase(eventQ.begin());
    planeSweepX = nextEvent.x;
    if (nextEvent.type == 'S') {
        //only 1 involved segment
        int involvedSegment = nextEvent.involvedSegments.front();
        recreateStatus(status);
        SegmentInfo infoToInsert = segments.find(involvedSegment)->second;
        status.insert( std::pair<SegmentInfo,int>(infoToInsert, involvedSegment) );
        //find prev and next
        map<SegmentInfo,int,leastSegment>::iterator currIt = status.find(infoToInsert);
        if (currIt != status.begin()) {
            cout << "inhere" << endl;
            computeIntersections(eventQ, currIt->first, std::prev(currIt)->first);
        }
        if (currIt != std::prev(status.end())) {
            cout << "inthere" << endl;
            computeIntersections(eventQ, currIt->first, std::next(currIt)->first);
        }
    }
    if (nextEvent.type == 'E') {
        //only 1 segment involved
        recreateStatus(status);
        SegmentInfo toDelete = segments.find(nextEvent.involvedSegments.front())->second;
        status.erase(toDelete);
    }
    cout << "Post event at " << planeSweepX << ": ";
    for (map<SegmentInfo,int,leastSegment>::iterator it = status.begin(); it != status.end(); it++) {
        cout << it->second << "\n";
    }
}

void run(set<EventInfo,leastEvent>& eventQ, map<SegmentInfo,int,leastSegment>& status, map<int,SegmentInfo> segments) {
    while (eventQ.size()>0) {
        execNextEvent(eventQ, status, segments);
    }
}

int main() {
    map<int,SegmentInfo> segments;
    //initialize event queue and read segment equations
    set<EventInfo, leastEvent> eventQ;
    readInput(eventQ, &segments);
    //initialize status
    map<SegmentInfo,int,leastSegment> status;
    run(eventQ, status, segments);
}