#include"logger.h"
#include"comparator.h"
/**
 * @brief The Page object is the main memory representation of a physical page
 * (equivalent to a block). The page class and the page.h header file are at the
 * bottom of the dependency tree when compiling files. 
 *<p>
 * Do NOT modify the Page class. If you find that modifications
 * are necessary, you may do so by posting the change you want to make on Moodle
 * or Teams with justification and gaining approval from the TAs. 
 *</p>
 */

class Page {


    string tableName;
    int pageIndex;
    int columnCount;
    int rowCount;
    vector<vector<int>> rows;
    bool dirty = false;

public:

    string pageName = "";

    Page();

    pair<int, int> getDimensions() {
        return make_pair(this->rowCount, this->columnCount);
    }

    Page(string tableName, int pageIndex);
    Page(string tableName, int pageIndex, int columnCount);

    int getPageIndex() const;

    Page(string tableName, int pageIndex, vector<vector<int>> rows, int rowCount);

    Page(string tableName, int pageIndex, vector<int> row, int numCount);

    bool isDirty() const;

    void updatePage(const Page &page);
    void updatePage(const vector<vector<int>> &rows);

    vector<int> getRow(int rowIndex);

    void writePage();

    void tranposePage(const Page &page);

    void sortPage(RowCmp cmp);
};