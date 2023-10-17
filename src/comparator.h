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
    RowCmp() = delete;
    RowCmp(const vector<int> &columnIndices, const vector<SortingStrategy> &sortingStrategy) : columnIndices(columnIndices),
                                                                                       sortingStrategy(sortingStrategy) {};
    RowCmp(const RowCmp &copy) = default;
    RowCmp(RowCmp &&copy) noexcept = default;
    RowCmp& operator=(RowCmp other){
        other.swap(*this);
        return *this;
    }
    void swap(RowCmp &rhs) noexcept {
        using std::swap;
        swap(rhs.columnIndices, columnIndices);
        swap(rhs.sortingStrategy, sortingStrategy);
    }
    friend void swap(RowCmp &a, RowCmp &b) noexcept { a.swap(b); }
    bool operator() (const vector<int>& row1, const vector<int>& row2);
};


#endif //DS_COMPARATOR_H
