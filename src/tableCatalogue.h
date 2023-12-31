#include "table.h"
#include "matrix.h"

/**
 * @brief The TableCatalogue acts like an index of tables existing in the
 * system. Everytime a table is added(removed) to(from) the system, it needs to
 * be added(removed) to(from) the tableCatalogue. 
 *
 */
enum Types {
    TABLE,
    MATRIX,
    NONE
};

class TableCatalogue
{

    unordered_map<string, Table*> tables;
    unordered_map<string, Matrix*> matrices;

public:
    TableCatalogue() {}
    void insertTable(Table* table);
    void deleteTable(string tableName);
    Table* getTable(string tableName);
    bool isTable(string tableName);
    bool isColumnFromTable(string columnName, string tableName);
    void print();

    // Matrix functions
    void insertMatrix(Matrix* matrix);
    void deleteMatrix(string matrixName);
    void renameMatrix(string matrixName, string newMatrixName);
    Matrix* getMatrix(string matrixName);
    bool isMatrix(string matrixName);
    void printMatrix();

    bool exists(string name);
    Types getType(string name);

    ~TableCatalogue();
};