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

/*
 * @brief Construct a new Matrix:: copy constructor
 * */
Matrix::Matrix(Matrix *matrix) {
    logger.log("Matrix::Matrix");
    this->sourceFileName = matrix->sourceFileName;
    this->matrixName = matrix->matrixName;
    this->columnCount = matrix->columnCount;
    this->rowCount = matrix->rowCount;
    this->blockCount = matrix->blockCount;
    this->maxRowsPerBlock = matrix->maxRowsPerBlock;
    this->pagesPerRow = matrix->pagesPerRow;
    this->rowsPerBlockCount = matrix->rowsPerBlockCount;
    this->distinctValuesInColumns = matrix->distinctValuesInColumns;
    this->distinctValuesPerColumnCount = matrix->distinctValuesPerColumnCount;
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

    for (int i = 0; i < pagesPerRow; i++) {
        cursor.nextPage(i);
        for (int j = 0; j < count; j++) {
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
    cout << "RowCount " << this->columnCount << "\n";

    vector<int> row;
    vector<vector<int>> rows(maxRowsPerBlock, vector<int>(this->columnCount, 0));

    for (int k = 0; k < blockCount / pagesPerRow; k++) {
        for (int i = 0; i < pagesPerRow; i++) {
            cursor.nextPage(i + k * pagesPerRow);
            for (int j = 0; j < rowsPerBlockCount[i + k * pagesPerRow]; j++) {
                row = cursor.getNext();
                copy(row.begin(), row.end(), rows[j].begin() + i * maxRowsPerBlock);
            }
        }
        for (int i = 0; i < rowsPerBlockCount[k * pagesPerRow]; i++) {
            writeRow(rows[i], fout);
        }
    }
    fout.close();
}

void Matrix::rename(string newMatrixName) {
    logger.log("Matrix::rename");
    for (int i = 0; i < blockCount; i++) {
        string oldPageName = "../data/temp/" + this->matrixName + "_Page" + to_string(i);
        string newPageName = "../data/temp/" + newMatrixName + "_Page" + to_string(i);
        bufferManager.renameFile(oldPageName, newPageName);
    }
    this->matrixName = newMatrixName;
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

bool Matrix::checkSymmetry() {
    logger.log("Matrix::checkSymmetry");
    for (int i = 0; i < pagesPerRow; i++) {
        for (int j = 0; j <= i; j++) {
            Page page1, page2;
            page1 = bufferManager.getPage(this->matrixName, i * pagesPerRow + j);
            if (i == j)
                page2 = page1;
            else
                page2 = bufferManager.getPage(this->matrixName, j * pagesPerRow + i);

            // Check if page1 is transpose of page2
            for (int a = 0; a < rowsPerBlockCount[i * pagesPerRow + j]; a++) {
                for (int b = 0; b < rowsPerBlockCount[j * pagesPerRow + i]; b++) {
                    if (page1.getRow(a)[b] != page2.getRow(b)[a])
                        return false;
                }
            }
        }
    }
    return true;
}

void Matrix::transpose() {
    logger.log("Matrix::transpose");
    for (int i = 0; i < pagesPerRow; i++) {
        for (int j = 0; j <= i; j++) {
            Page page1, page2;
            page1 = bufferManager.getPage(this->matrixName, i * pagesPerRow + j);
            if (i == j)
                bufferManager.transposeMatrixPage(this->matrixName, i * pagesPerRow + j, page1);
            else {
                page2 = bufferManager.getPage(this->matrixName, j * pagesPerRow + i);
                bufferManager.transposeMatrixPage(this->matrixName, i * pagesPerRow + j, page2);
                bufferManager.transposeMatrixPage(this->matrixName, j * pagesPerRow + i, page1);
            }
        }
    }
}

void Matrix::compute() {
    // add new matrix with name "this->matrixName + "_result"" to tableCatalogue
    logger.log("Matrix::compute");
    string newMatrixName = this->matrixName + "_RESULT";
    Matrix *resultMatrix = new Matrix(newMatrixName);
    // add to tableCatalogue
    resultMatrix->columnCount = this->columnCount;
    resultMatrix->rowCount = this->rowCount;
    resultMatrix->blockCount = this->blockCount;
    resultMatrix->maxRowsPerBlock = this->maxRowsPerBlock;
    resultMatrix->pagesPerRow = this->pagesPerRow;
    resultMatrix->rowsPerBlockCount = this->rowsPerBlockCount;
    tableCatalogue.insertMatrix(resultMatrix);
    for (int i = 0; i < pagesPerRow; i++) {
        for (int j = 0; j <= i; j++) {
            Page page1, page2;
            page1 = bufferManager.getPage(this->matrixName, i * pagesPerRow + j);
            if (i == j) {
                auto [rowCount, columnCount] = page1.getDimensions();
                vector<vector<int>> rows(rowCount, vector<int>(columnCount, 0));
                for (int a = 0; a < rowCount; a++) {
                    for (int b = 0; b < columnCount; b++) {
                        rows[a][b] = page1.getRow(a)[b] - page1.getRow(b)[a];
                    }
                }
                bufferManager.writePage(newMatrixName, i * pagesPerRow + j, rows, rowCount);
            } else {
                page2 = bufferManager.getPage(this->matrixName, j * pagesPerRow + i);
                auto [rowCount1, columnCount1] = page1.getDimensions();
                auto [rowCount2, columnCount2] = page2.getDimensions();
                vector<vector<int>> rows1(rowCount1, vector<int>(columnCount1, 0));
                vector<vector<int>> rows2(rowCount2, vector<int>(columnCount2, 0));
                for (int a = 0; a < rowCount1; a++) {
                    for (int b = 0; b < columnCount1; b++) {
                        rows1[a][b] = page1.getRow(a)[b] - page2.getRow(b)[a];
                    }
                }
                for (int a = 0; a < rowCount2; a++) {
                    for (int b = 0; b < columnCount2; b++) {
                        rows2[a][b] = page2.getRow(a)[b] - page1.getRow(b)[a];
                    }
                }
                bufferManager.writePage(newMatrixName, i * pagesPerRow + j, rows1, rowCount1);
                bufferManager.writePage(newMatrixName, j * pagesPerRow + i, rows2, rowCount2);
            }
        }
    }


}
