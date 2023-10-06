#include "comparator.h"

bool RowCmp::operator()(const vector<int> &row1, const vector<int> &row2) {
    for (int i = 0; i < columnIndices.size(); i++) {
        if (row1[columnIndices[i]] < row2[columnIndices[i]] and sortingStrategy[i] == ASC) return true;
        if (row1[columnIndices[i]] > row2[columnIndices[i]] and sortingStrategy[i] == DESC) return true;
    }
    return false;
}