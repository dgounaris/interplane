#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <list>
#include <string>
#include <cmath>
using namespace std;

static string inputFile = "input.txt";
static string outputFile = "output.txt";

double planeSweepX;
//logging variables
static int totals=0;
static int totali=0;

class SegmentInfo {
    public:
        int ref;
        double a, b, c, t1, t2;
};

class EventInfo {
    public:
        char type;
        vector<SegmentInfo> involvedSegments;
        double x, y;
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

void readInput(set<EventInfo,leastEvent>& eventQ) {
    ifstream inFile(inputFile);
    int segmentsNum = 0;
    if (inFile.is_open()) {
        inFile >> segmentsNum;
    }
    else {
        printf("Unable to open file\n");
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
            //start point
            EventInfo mySInfo;
            mySInfo.involvedSegments.push_back(segment); mySInfo.type = 'S';
            mySInfo.x = t1; mySInfo.y = a*t1*t1 + b*t1 + c;
            eventQ.insert(mySInfo);
            //end point
            EventInfo myEInfo;
            myEInfo.involvedSegments.push_back(segment); myEInfo.type = 'E';
            myEInfo.x = t2; myEInfo.y = a*t2*t2 + b*t2 + c;
            eventQ.insert(myEInfo);
        }
    }
    inFile.close();
    return;
}

int computeIntersections(set<EventInfo,leastEvent>& eventQ, SegmentInfo s1, SegmentInfo s2) {
    //using this: https://codereview.stackexchange.com/questions/51011/calculating-the-point-of-intersection-of-two-parabolas
    //if ( ((s2.c-s1.c) + (s1.b-s2.b)*(s1.b-s2.b)/(s1.a-s2.a)) / (s1.a-s2.a) < 0) {
    double D = (s1.b-s2.b)*(s1.b-s2.b) - 4*(s1.a-s2.a)*(s1.c-s2.c);
    if ( D < 0 ) {   
        return 0;
    }
    int intC = 0;
    double intX1 = (-(s1.b-s2.b) + sqrt(D))/(2*(s1.a-s2.a));
    double intX2 = (-(s1.b-s2.b) - sqrt(D))/(2*(s1.a-s2.a));
    //double intX1 = sqrt( ((s2.c-s1.c) + (s1.b-s2.b)*(s1.b-s2.b)/(s1.a-s2.a)) / (s1.a-s2.a) ) - (s1.b-s2.b)/(2*(s1.a-s2.a));
    //double intX2 = -sqrt( ((s2.c-s1.c) + (s1.b-s2.b)*(s1.b-s2.b)/(s1.a-s2.a)) / (s1.a-s2.a) ) - (s1.b-s2.b)/(2*(s1.a-s2.a));
    //check if our intersection points are valid
    if (intX1>=s1.t1 && intX1>=s2.t1 && intX1<=s1.t2 && intX1<=s2.t2 && intX1>planeSweepX) {
        EventInfo e1;
        e1.involvedSegments.push_back(s1); e1.involvedSegments.push_back(s2);
        e1.type = 'I'; e1.x = intX1; e1.y = s1.a*intX1*intX1 + s1.b*intX1 + s1.c;
        eventQ.insert(e1);
        intC++;
    }
    if (intX2>=s1.t1 && intX2>=s2.t1 && intX2<=s1.t2 && intX2<=s2.t2 && intX2>planeSweepX && intX1 != intX2) {
        EventInfo e2;
        e2.involvedSegments.push_back(s1); e2.involvedSegments.push_back(s2);
        e2.type = 'I'; e2.x = intX2; e2.y = s1.a*intX2*intX2 + s1.b*intX2 + s1.c;
        eventQ.insert(e2);
        intC++;
    }
    return intC;
    //TODO problem is that we check for out of bounds, it is not required, just dont insert
    //event in eventQ, but count it as computed!
}

void printStatus(set<SegmentInfo,leastSegment> status) {
    printf("status: %d:", status.size());
    set<SegmentInfo,leastSegment>::iterator sIt = status.begin();
    for (sIt;sIt != status.end();sIt++) {
        printf(" %d", sIt->ref);
    }
    printf("\n");
}

char execNextEvent(set<EventInfo,leastEvent>& eventQ, set<SegmentInfo,leastSegment>& status, bool log=false) {
    if (eventQ.empty()) {
        printf("error: no more events\n");
        return 'R';
    }
    EventInfo nextEvent = *eventQ.begin();
    eventQ.erase(eventQ.begin());
    planeSweepX = nextEvent.x;
    int intersectionsC = 0;
    if (nextEvent.type == 'S') {
        totals++;
        //only 1 involved segment
        SegmentInfo infoToInsert = nextEvent.involvedSegments.front();
        status.insert( infoToInsert );
        //find prev and next
        set<SegmentInfo,leastSegment>::iterator currIt = status.find(infoToInsert);
        if (currIt != status.begin()) {
            intersectionsC += computeIntersections(eventQ, *currIt, *std::prev(currIt));
        }
        if (currIt != std::prev(status.end())) {
            intersectionsC += computeIntersections(eventQ, *currIt, *std::next(currIt));
        }
    }
    if (nextEvent.type == 'E') {
        //only 1 segment involved
        SegmentInfo toDelete = nextEvent.involvedSegments.front();
        set<SegmentInfo,leastSegment>::iterator sIt = status.find(toDelete);
        if (sIt != status.begin() && sIt != std::prev(status.end())) {
            intersectionsC += computeIntersections(eventQ, *std::prev(sIt), *std::next(sIt));
        }
        status.erase(toDelete);
    }
    if (nextEvent.type == 'I') {
        totali++;
        //intersecting with curves does not mean swapping!
        //it may be a tangent line on a curve
        //go a bit back to avoid unexpected bad clauses through the tree
        //then find and erase the 2 intersecting segments O(2logn)
        planeSweepX -= 0.0001;
        set<SegmentInfo,leastSegment>::iterator old1 = status.find(nextEvent.involvedSegments.at(0));
        status.erase(old1);
        set<SegmentInfo,leastSegment>::iterator old2 = status.find(nextEvent.involvedSegments.at(1));
        status.erase(old2);
        //go a bit forward than the intersection point, again to avoid unexpected behaviour
        planeSweepX += 0.0002;
        //reinserting in correct order is O(2logn)
        status.insert(nextEvent.involvedSegments.at(0));
        status.insert(nextEvent.involvedSegments.at(1));
        //suppose 2 segments in intersection
        set<SegmentInfo,leastSegment>::iterator si1 = status.find(nextEvent.involvedSegments.at(0));
        set<SegmentInfo,leastSegment>::iterator si2 = status.find(nextEvent.involvedSegments.at(1));
        //check intersections of si1 with new adjacents
        if (si1 != status.begin()) {
            intersectionsC += computeIntersections(eventQ, *si1, *std::prev(si1));
        }
        if (si1 != std::prev(status.end())) {
            intersectionsC += computeIntersections(eventQ, *si1, *std::next(si1));
        }
        //avoid duplicates
        if (si2 != status.begin() && std::prev(si2) != si1) {
            intersectionsC += computeIntersections(eventQ, *si2, *std::prev(si2));
        }
        if (si2 != std::prev(status.end()) && std::next(si2) != si1) {
            intersectionsC += computeIntersections(eventQ, *si2, *std::next(si2));
        }
    }
    if (log)
        printf("%1c %6.2f %1d\n", nextEvent.type, nextEvent.x, intersectionsC);
    return nextEvent.type;
}

void run(set<EventInfo,leastEvent>& eventQ, set<SegmentInfo,leastSegment>& status) {
    while (eventQ.size()>0) {
        execNextEvent(eventQ, status, false);
    }
    printf("summary: %d segments, %d intersections\n", totals, totali);
}

int main() {
    //initialize event queue and read segment equations
    set<EventInfo, leastEvent> eventQ;
    readInput(eventQ);
    //initialize status
    set<SegmentInfo,leastSegment> status;
    string inString;
    ifstream inFile(inputFile);
    if (!inFile.is_open()) {
        cout << "Could not load specified input file\n";
        return 1;
    }
    //this redirects stdout to the output file
    freopen(outputFile.c_str(),"w",stdout);
    do {
        inFile >> inString;
        if (inString.compare("step")==0) {
            execNextEvent(eventQ, status, false);
        }
        if (inString.compare("step-p")==0) {
            execNextEvent(eventQ, status, true);
        }
        if (inString.compare("run")==0) {
            run(eventQ, status);
            break;
        }
        if (inString.compare("status")==0) {
            printStatus(status);
        }
    } while(!inFile.eof());
    inFile.close();
    return 0;
}