#include "global.h"
/**
 * @brief Construct a new Page object. Never used as part of the code
 *
 */
Page::Page()
{
    this->pageName = "";
    this->tableName = "";
    this->pageIndex = -1;
    this->rowCount = 0;
    this->columnCount = 0;
    this->rows.clear();
    this->dirty = false;
}

/**
 * @brief Construct a new Page:: Page object given the table name and page
 * index. When tables are loaded they are broken up into blocks of BLOCK_SIZE
 * and each block is stored in a different file named
 * "<tablename>_Page<pageindex>". For example, If the Page being loaded is of
 * table "R" and the pageIndex is 2 then the file name is "R_Page2". The page
 * loads the rows (or tuples) into a vector of rows (where each row is a vector
 * of integers).
 *
 * @param tableName 
 * @param pageIndex 
 */
Page::Page(string tableName, int pageIndex)
{
    logger.log("Page::Page");
    this->tableName = tableName;
    this->pageIndex = pageIndex;
    this->pageName = "../data/temp/" + this->tableName + "_Page" + to_string(pageIndex);


    ifstream fin(pageName, ios::in);
    if (tableCatalogue.getType(tableName) == TABLE) {
        Table table = *tableCatalogue.getTable(tableName);
        this->columnCount = table.columnCount;
        uint maxRowCount = table.maxRowsPerBlock;
        vector<int> row(columnCount, 0);
        this->rows.assign(maxRowCount, row);

        this->rowCount = table.rowsPerBlockCount[pageIndex];
        int number;
        for (uint rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
        {
            for (int columnCounter = 0; columnCounter < columnCount; columnCounter++)
            {
                fin >> number;
                this->rows[rowCounter][columnCounter] = number;
            }
        }
    } else if (tableCatalogue.getType(tableName) == MATRIX) {
        Matrix matrix = *tableCatalogue.getMatrix(tableName);
        this->columnCount = (pageIndex % matrix.pagesPerRow) < matrix.pagesPerRow - 1 ? matrix.maxRowsPerBlock : matrix.columnCount % matrix.maxRowsPerBlock;
        this->rowCount = matrix.rowsPerBlockCount[pageIndex];
        vector<int> row(columnCount, 0);
        this->rows.assign(matrix.maxRowsPerBlock, row);

        int number;
        for (uint rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
        {
            for (int columnCounter = 0; columnCounter < columnCount; columnCounter++)
            {
                fin >> number;
                this->rows[rowCounter][columnCounter] = number;
            }
        }
    } else {
        logger.log("Page::Page: Invalid table type");
    }
    fin.close();
}

/**
 * @brief Get row from page indexed by rowIndex
 * 
 * @param rowIndex 
 * @return vector<int> 
 */
vector<int> Page::getRow(int rowIndex)
{
    logger.log("Page::getRow");
    vector<int> result;
    result.clear();
    if (rowIndex >= this->rowCount)
        return result;
    return this->rows[rowIndex];
}

Page::Page(string tableName, int pageIndex, vector<vector<int>> rows, int rowCount)
{
    logger.log("Page::Page");
    this->tableName = tableName;
    this->pageIndex = pageIndex;
    this->rows = rows;
    this->rowCount = rowCount;
    this->columnCount = rows[0].size();
    this->pageName = "../data/temp/"+this->tableName + "_Page" + to_string(pageIndex);
}

Page::Page(string tableName, int pageIndex, vector<int> row, int numCount)
{
    logger.log("Page::Page");
    this->tableName = tableName;
    this->pageIndex = pageIndex;
    this->rowCount = 1;
    this->columnCount = numCount;
    this->pageName = "../data/temp/"+this->tableName + "_Page" + to_string(pageIndex);
    this->rows.assign(1, row);
}

/**
 * @brief writes current page contents to file.
 * 
 */
void Page::writePage()
{
    logger.log("Page::writePage");
    ofstream fout(this->pageName, ios::trunc);
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (columnCounter != 0)
                fout << " ";
            fout << this->rows[rowCounter][columnCounter];
        }
        fout << endl;
    }
    fout.close();
}

bool Page::isDirty() const {
    return dirty;
}

void Page::updatePage(const Page &page) {
    logger.log("Page::updatePage");
    this->rows = page.rows;
    this->rowCount = page.rowCount;
    this->columnCount = page.columnCount;
    this->dirty = true;
}

void Page::tranposePage(const Page &page) {
    logger.log("Page::tranposePage");
    this->rows = page.rows;
    for (int i = 0; i < page.rowCount; i++) {
        for (int j = 0; j < page.columnCount; j++) {
            this->rows[j][i] = page.rows[i][j];
        }
    }
    this->rowCount = page.columnCount;
    this->columnCount = page.rowCount;
    this->dirty = true;
}
