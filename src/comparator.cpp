#include "comparator.h"

bool RowCmp::operator()(const vector<int> &row1, const vector<int> &row2) {
    for (int i = 0; i < columnIndices.size(); i++) {
        if ((row1[columnIndices[i]] == row2[columnIndices[i]]))  continue;
        if (sortingStrategy[i] == DESC) return row1[columnIndices[i]] > row2[columnIndices[i]];
        if (sortingStrategy[i] == ASC) return row1[columnIndices[i]] < row2[columnIndices[i]];
    }
    return false;
}