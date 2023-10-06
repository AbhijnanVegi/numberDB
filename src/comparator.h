#ifndef DS_COMPARATOR_H
#define DS_COMPARATOR_H
#include <vector>

using namespace std;

enum SortingStrategy {
    ASC,
    DESC,
    NO_SORT_CLAUSE
};

class RowCmp {
private:
    vector<int> columnIndices;
    vector<SortingStrategy> sortingStrategy;

public:
    RowCmp(const vector<int> columnIndices, vector<SortingStrategy> sortingStrategy) : columnIndices(columnIndices),
                                                                                       sortingStrategy(sortingStrategy) {};
    bool operator() (const vector<int>& row1, const vector<int>& row2);
};


#endif //DS_COMPARATOR_H
