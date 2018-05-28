Before running the program, open the project.cpp file and set inputFile and outputFile to the i/o files you want.

To compile and run program:
g++ -o project project.cpp
./project

General approach:
As explained in the slides, we use an events and a status balanced binary tree (using c++ set from the STL, which is implemented as such a tree).
By overriding the sorting functions in the tree we can sort by y coordinate (using a left-to-right plane sweep).
Then, we just implement the algorithm presented in class, using the class execNextEvent to handle the next event each time.

Time complexity (during my analysis I focus on the non-constant operations):

readInput:
The function for reading input from file iterates through n+1 lines (n = number of segments) and adds each segment in a set.
To create the EventInfo structure, we first need to insert all the involved event segments in a vector.
Since vector has an insert function that runs linearly on the number of elements inserted, and we only insert 1 element, the vector insert time is constant.
Thus, we are interested only in the time it will take to insert the node in the set eventQ. This is O(logn) for each insertion (since set is implemented as a balanced binary tree in C++ STL) and we have n insertions, yielding a total O(nlogn).

printStatus:
This function iterates through all the elements of a status and prints their ref value. At most, the status set contains n elements, as many as the total segments.
Iterating through a set using an iterator, as defined in C++ standard, takes a constant time.
Thus, the total complexity of this function is O(n).

computeIntersections:
This function checks for intersections between 2 given segments (can be done in constant type by using well-known equations for that problem).
If it finds intersection(s), it adds them in the eventQ set. Inserting in the set is done in O(logn).

execNextEvent:
This function first gets the first event from the eventQ set (in constant type).
Then, it erases that value from the eventQ, which (since we use an iterator as parameter) is done in constant time.
In case the event is the start of a segment:
    We get the first element of the saved event's involvedSegments vector in constant time.
    We insert that segment in the status set (in O(logn)).
    We get an iterator for the newly inserted node (in O(logn)) and we compute intersections with previous and next element.
    ComputeIntersections has a complexity O(logn) as shown previously and we run it 2 times, giving still an O(logn) complexity.
    Totally, a start segment event takes c + O(logn) + O(logn) + O(logn) + O(logn) = O(logn)
In case the event is the end of a segment:
    We get the first element of the saved event's involvedSegments vector in constant time.
    We find that segment in the set (in O(logn)).
    We compute intersections between its previous and next elements (as shown previously, in O(logn)).
    Finally, we delete the ending segment (O(logn)).
    Totally, an end segment event takes c + O(logn) + O(logn) + O(logn) = O(logn)
In case the event is an intersection event (between only 2 segments):
    We retrieve the 2 intersecting segments from the involvedSegments vector (constant time, we know there are only 2).
    We erase these segments from the status set (O(logn)).
    We add again these segments at a point just past the intersection (O(logn)).
    We compute intersections between these segments and their new adjacent ones (O(4logn)).
    Totally, for an intersection the time taken is c + O(logn) + O(logn) + O(4logn) = O(logn)
Thus, in any case the function takes an O(logn) time.

run:
This function runs execNextEvent (which has a complexity of O(logn)) for every event created. The total number of events is 2n (2 for each segment) and I (for the intersections created).
Thus, totally we get O((n+I)logn).

The complete program runs readInput and run, yielding O(nlogn) + O((n+I)logn) = O((n+I)logn)

Space complexity:

The only structures in our program are eventQ, status and the elements they contain (EventInfo and SegmentInfo).
EventInfo and SegmentInfo have a known constant size, so they are irrelevant.
Status set contains at most n elements (at most n lines will intersect the plane sweep at any time), yielding O(n).
EventQ set contains at most all the segment ending events and all the intersections, meaning O(n+I).
Thus, the total space complexity is O(n+I).