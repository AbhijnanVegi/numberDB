#include "global.h"

/**
 * @brief Construct a new Matrix::Matrix object
 *
 */
Matrix::Matrix() {
    logger.log("Matrix::Matrix");
}

/**
 * @brief Construct a new Matrix:: Matrix object used in the case where the data
 * file is available and LOAD command has been called. This command should be
 * followed by calling the load function;
 *
 * @param matrixName 
 */
Matrix::Matrix(string matrixName) {
    logger.log("Matrix::Matrix");
    this->sourceFileName = "../data/" + matrixName + ".csv";
    this->matrixName = matrixName;
}

/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates matrix
 * statistics.
 *
 * @return true if the matrix has been successfully loaded 
 * @return false if an error occurred 
 */
bool Matrix::load() {
    logger.log("Matrix::load");
    fstream fin(this->sourceFileName, ios::in);
    string line;
    if (getline(fin, line)) {
        fin.close();
        if (this->blockify())
            return true;
    }
    fin.close();
    return false;
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size. 
 *
 * @return true if successfully blockified
 * @return false otherwise
 */
bool Matrix::blockify() {
    logger.log("Matrix::blockify");
    ifstream fin(this->sourceFileName, ios::in);
    string line, word;
    int rowCounter = 0;
    unordered_set<int> dummy;
    dummy.clear();
    getline(fin, line);
    // count the number of columns
    this->columnCount = count(line.begin(), line.end(), ',') + 1;
    this->pagesPerRow = 1 + ((this->columnCount - 1) / this->maxRowsPerBlock);
    vector<vector<vector<int>>> pages(pagesPerRow, vector<vector<int>>());

    for (int pageCounter = 0; pageCounter < pagesPerRow - 1; pageCounter++) {
        pages[pageCounter].assign(this->maxRowsPerBlock, vector<int>(this->maxRowsPerBlock, 0));
    }

    pages[pagesPerRow - 1].assign(this->maxRowsPerBlock, vector<int>(this->columnCount % this->maxRowsPerBlock, 0));

    fin.seekg(0, ios::beg);
    while (getline(fin, line)) {
        stringstream s(line);
        for (auto &page: pages) {
            page.emplace_back(vector<int>());
        }
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++) {
            if (!getline(s, word, ','))
                return false;
            pages[columnCounter / this->maxRowsPerBlock][rowCounter][columnCounter % this->maxRowsPerBlock] = stoi(
                    word);
        }
        rowCounter++;
        if (rowCounter == this->maxRowsPerBlock) {
            for (auto &page: pages) {
                bufferManager.writePage(this->matrixName, this->blockCount, page, rowCounter);
                this->blockCount++;
                this->rowsPerBlockCount.emplace_back(rowCounter);
            }
            rowCounter = 0;
        }
    }
    if (rowCounter) {
        for (auto &page: pages) {
            bufferManager.writePage(this->matrixName, this->blockCount, page, rowCounter);
            this->blockCount++;
            this->rowsPerBlockCount.emplace_back(rowCounter);
        }
        rowCounter = 0;
    }

    if (this->blockCount == 0)
        return false;
    return true;
}

/**
 * @brief Given a row of values, this function will update the statistics it
 * stores i.e. it updates the number of rows that are present in the column and
 * the number of distinct values present in each column. These statistics are to
 * be used during optimisation.
 *
 * @param row 
 */
void Matrix::updateStatistics(vector<int> row) {
    this->rowCount++;
//    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++) {
//        if (!this->distinctValuesInColumns[columnCounter].count(row[columnCounter])) {
//            this->distinctValuesInColumns[columnCounter].insert(row[columnCounter]);
//            this->distinctValuesPerColumnCount[columnCounter]++;
//        }
//    }
}

/**
 * @brief Function prints the first few rows of the matrix. If the matrix contains
 * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
 * the rows are printed.
 *
 */
void Matrix::print() {
    logger.log("Matrix::print");
    uint count = min(PRINT_COUNT, this->columnCount);
    Cursor cursor(this->matrixName, 0);
    vector<int> row;

    vector<vector<int>> rows(count);

    for(int i = 0; i < pagesPerRow; i++)
    {
        cursor.nextPage(i);
        for(int j = 0; j < count; j++)
        {
            row = cursor.getNext();
            rows[j].insert(rows[j].end(), row.begin(), row.end());
            if ((j % maxRowsPerBlock) == (maxRowsPerBlock - 1)) {
                cursor.nextPage(i + pagesPerRow);
            }
        }
    }

    for (auto &row: rows) {
        writeRow(row, cout);
    }
}


/**
 * @brief This function returns one row of the matrix using the cursor object. It
 * returns an empty row is all rows have been read.
 *
 * @param cursor 
 * @return vector<int> 
 */
void Matrix::getNextPage(Cursor *cursor) {
    logger.log("Matrix::getNext");

    if (cursor->pageIndex < this->blockCount - 1) {
        cursor->nextPage(cursor->pageIndex + 1);
    }
}

void Matrix::getPage(Cursor *cursor, int pageIndex) {
    logger.log("Matrix::getPage");

    if (pageIndex < this->blockCount - 1) {
        cursor->nextPage(pageIndex);
    }
}


/**
 * @brief called when EXPORT command is invoked to move source file to "data"
 * folder.
 *
 */
void Matrix::makePermanent() {
    logger.log("Matrix::makePermanent");
    if (!this->isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->matrixName + ".csv";
    ofstream fout(newSourceFile, ios::out);
    Cursor cursor(this->matrixName, 0);
    vector<int> row;
    cout << "RowCount" << this->rowCount << "\n";
    row = cursor.getNext();
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++) {
        // take a subarray of size columnCount from row
        vector<int> currRow(row.begin() + rowCounter * this->columnCount,
                            row.begin() + (rowCounter + 1) * this->columnCount);
        this->writeRow(currRow, fout);
    }
    fout.close();
}

/**
 * @brief Function to check if matrix is already exported
 *
 * @return true if exported
 * @return false otherwise
 */
bool Matrix::isPermanent() {
    logger.log("Matrix::isPermanent");
    if (this->sourceFileName == "../data/" + this->matrixName + ".csv")
        return true;
    return false;
}

/**
 * @brief The unload function removes the matrix from the database by deleting
 * all temporary files created as part of this matrix
 *
 */
void Matrix::unload() {
    logger.log("Matrix::~unload");
    for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
        bufferManager.deleteFile(this->matrixName, pageCounter);
    if (!isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
}

/**
 * @brief Function that returns a cursor that reads rows from this matrix
 * 
 * @return Cursor 
 */
Cursor Matrix::getCursor() {
    logger.log("Matrix::getCursor");
    Cursor cursor(this->matrixName, 0);
    return cursor;
}