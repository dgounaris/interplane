#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <list>
#include <string>
#include <cmath>
using namespace std;

static string inputFile = "input3.txt";
static string outputFile = "output.txt";

double planeSweepX;
//logging variables
static int totals=0;
static int totali=0;

class SegmentInfo {
    public:
        mutable int ref;
        mutable double a, b, c, t1, t2;
};

class EventInfo {
    public:
        char type;
        vector<SegmentInfo> involvedSegments;
        double x, y;
};

struct leastSegment {
    bool operator()(const SegmentInfo& s1, const SegmentInfo& s2) {
        //in the case the 2 elements are equals, there is no way we will find the correct since < function is used by find
        //the way to do this is by changing artificially the planesweepX just for this comparison when 2 equals are found
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
    double intX1;
    double intX2;
    int intC = 0;
    if (s1.a-s2.a == 0) {
        intX1 = (s2.c-s1.c)/(s1.b-s2.b);
        intX2 = intX1;
    }
    else {
        double D = (s1.b-s2.b)*(s1.b-s2.b) - 4*(s1.a-s2.a)*(s1.c-s2.c);
        if ( D < 0 ) {   
            return 0;
        }
        intX1 = (-(s1.b-s2.b) + sqrt(D))/(2*(s1.a-s2.a));
        intX2 = (-(s1.b-s2.b) - sqrt(D))/(2*(s1.a-s2.a));
    }
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

void printStatus(multiset<SegmentInfo,leastSegment> status) {
    printf("status: %d:", status.size());
    multiset<SegmentInfo,leastSegment>::iterator sIt = status.begin();
    for (sIt;sIt != status.end();sIt++) {
        printf(" %d", sIt->ref);
    }
    printf("\n");
}

char execNextEvent(set<EventInfo,leastEvent>& eventQ, multiset<SegmentInfo,leastSegment>& status, bool log=false) {
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
        multiset<SegmentInfo,leastSegment>::iterator currIt = status.find(infoToInsert);
        if (currIt != status.begin()) {
            intersectionsC += computeIntersections(eventQ, *std::prev(currIt), *currIt);
        }
        if (currIt != std::prev(status.end())) {
            intersectionsC += computeIntersections(eventQ, *currIt, *std::next(currIt));
        }
    }
    if (nextEvent.type == 'E') {
        //only 1 segment involved
        SegmentInfo toDelete = nextEvent.involvedSegments.front();
        multiset<SegmentInfo,leastSegment>::iterator sIt = status.find(toDelete);
        if (sIt != status.begin() && sIt != std::prev(status.end())) {
            intersectionsC += computeIntersections(eventQ, *std::prev(sIt), *std::next(sIt));
        }
        status.erase(toDelete);
    }
    if (nextEvent.type == 'I') {
        totali++;
        //supposing only 2 involved segments
        std::pair<multiset<SegmentInfo,leastSegment>::iterator,multiset<SegmentInfo,leastSegment>::iterator> getIt;
        getIt = status.equal_range(nextEvent.involvedSegments.at(0));
        int checkCounter = 0;
        for (multiset<SegmentInfo,leastSegment>::iterator checkIt = getIt.first; checkIt!=getIt.second; checkIt++) {
            checkCounter++;
        }
        if (checkCounter>2) {
            cout << "Unexpected number of intersecting segments: " << checkCounter  << " at point: " << planeSweepX << endl;
        }
        SegmentInfo tempswap;
        tempswap.a = getIt.first->a; tempswap.b = getIt.first->b; tempswap.c = getIt.first->c;
        tempswap.t1 = getIt.first->t1; tempswap.t2 = getIt.first->t2; tempswap.ref = getIt.first->ref;
        getIt.second--;
        //cout << "BEFORE " << getIt.first->a << getIt.second->a << endl;
        SegmentInfo new1;
        getIt.first->a = getIt.second->a; getIt.first->b = getIt.second->b; getIt.first->c = getIt.second->c;
        getIt.first->t1 = getIt.second->t1; getIt.first->t2 = getIt.second->t2; getIt.first->ref = getIt.second->ref;
        getIt.second->a = tempswap.a; getIt.second->b = tempswap.b; getIt.second->c = tempswap.c;
        getIt.second->t1 = tempswap.t1; getIt.second->t2 = tempswap.t2; getIt.second->ref = tempswap.ref;
        //suppose 2 segments in intersection
        multiset<SegmentInfo,leastSegment>::iterator si1 = getIt.first;
        multiset<SegmentInfo,leastSegment>::iterator si2 = getIt.second;
        //cout << "AFTER " << si1->a << endl;
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

void run(set<EventInfo,leastEvent>& eventQ, multiset<SegmentInfo,leastSegment>& status) {
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
    multiset<SegmentInfo,leastSegment> status;
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