
#ifndef INDEXING_STRATEGY
#include "cursor.h"
#define INDEXING_STRATEGY
enum IndexingStrategy
{
    BTREE,
    HASH,
    NOTHING
};
#endif

extern const float BLOCK_SIZE;

/**
 * @brief Matrix class holds all information related to a loaded matrix.
 *
 */
class Matrix {
    vector<unordered_set<int>> distinctValuesInColumns;

public:
    string sourceFileName = ""; // name of the source file
    string matrixName = ""; // name of the table
    uint columnCount = 0;
    long long int rowCount = 0;
    vector<uint> distinctValuesPerColumnCount;
    uint blockCount = 0;
    uint maxRowsPerBlock = sqrt((BLOCK_SIZE * 1000) / (sizeof(int)));
    uint pagesPerRow = 0;
    vector<uint> rowsPerBlockCount;
    bool indexed = false;
    string indexedColumn = "";
    IndexingStrategy indexingStrategy = NOTHING;

    bool blockify();

    void updateStatistics(vector<int> row);

    Matrix();

    Matrix(string matrixName);

    Matrix(Matrix *matrix);

    bool load();

    void print();

    void makePermanent();

    void rename(string newMatrixName);

    bool checkSymmetry();

    void transpose();

    bool isPermanent();

    void getNextPage(Cursor *cursor);

    void getPage(Cursor *cursor, int pageIndex);

    Cursor getCursor();

    int getColumnIndex(string columnName);

    void unload();

    /**
 * @brief Static function that takes a vector of valued and prints them out in a
 * comma seperated format.
 *
 * @tparam T current usages include int and string
 * @param row 
 */
    template<typename T>
    void writeRow(vector<T> row, ostream &fout) {
        logger.log("Matrix::writeRow");
        for (int columnCounter = 0; columnCounter < row.size(); columnCounter++) {
            if (columnCounter != 0)
                fout << ", ";
            fout << row[columnCounter];
        }
        fout << endl;
    }

/**
 * @brief Static function that takes a vector of valued and prints them out in a
 * comma seperated format.
 *
 * @tparam T current usages include int and string
 * @param row 
 */
    template<typename T>
    void writeRow(vector<T> row) {
        logger.log("Matrix::writeRow");
        ofstream fout(this->sourceFileName, ios::app);
        this->writeRow(row, fout);
        fout.close();
    }

    void compute();
};