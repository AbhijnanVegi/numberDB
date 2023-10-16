#include "global.h"

bool compare(int first, int second, BinaryOperator binaryOperator);

/**
 * @brief 
 * SYNTAX: R <- JOIN relation_name1, relation_name2 ON column_name1 bin_op column_name2
 */
bool syntacticParseJOIN() {
    logger.log("syntacticParseJOIN");
    if (tokenizedQuery.size() != 9 || tokenizedQuery[5] != "ON") {
        cout << "SYNTAC ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = JOIN;
    parsedQuery.joinResultRelationName = tokenizedQuery[0];
    parsedQuery.joinFirstRelationName = tokenizedQuery[3];
    parsedQuery.joinSecondRelationName = tokenizedQuery[4];
    parsedQuery.joinFirstColumnName = tokenizedQuery[6];
    parsedQuery.joinSecondColumnName = tokenizedQuery[8];


    string binaryOperator = tokenizedQuery[7];
    if (binaryOperator == "<")
        parsedQuery.joinBinaryOperator = LESS_THAN;
    else if (binaryOperator == ">")
        parsedQuery.joinBinaryOperator = GREATER_THAN;
    else if (binaryOperator == ">=" || binaryOperator == "=>")
        parsedQuery.joinBinaryOperator = GEQ;
    else if (binaryOperator == "<=" || binaryOperator == "=<")
        parsedQuery.joinBinaryOperator = LEQ;
    else if (binaryOperator == "==")
        parsedQuery.joinBinaryOperator = EQUAL;
    else if (binaryOperator == "!=")
        parsedQuery.joinBinaryOperator = NOT_EQUAL;
    else {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    return true;
}

bool semanticParseJOIN() {
    logger.log("semanticParseJOIN");

    if (tableCatalogue.isTable(parsedQuery.joinResultRelationName)) {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.joinFirstRelationName) ||
        !tableCatalogue.isTable(parsedQuery.joinSecondRelationName)) {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.joinFirstColumnName, parsedQuery.joinFirstRelationName) ||
        !tableCatalogue.isColumnFromTable(parsedQuery.joinSecondColumnName, parsedQuery.joinSecondRelationName)) {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    return true;
}

void executeJOIN() {
    logger.log("executeJOIN");
    Table *firstTable = tableCatalogue.getTable(parsedQuery.joinFirstRelationName);
    Table *secondTable = tableCatalogue.getTable(parsedQuery.joinSecondRelationName);

    Table *firstTableSorted = new Table(parsedQuery.joinFirstRelationName + "_sorted");
    Table *secondTableSorted = new Table(parsedQuery.joinSecondRelationName + "_sorted");

    // result table has columns of first and second table
    vector<string> firstTableColumns = firstTable->columns;
    vector<string> secondTableColumns = secondTable->columns;

    vector<string> resultTableColumns = firstTableColumns;
    resultTableColumns.insert(resultTableColumns.end(), secondTableColumns.begin(), secondTableColumns.end());
    Table *resultTable = new Table(parsedQuery.joinResultRelationName, resultTableColumns);
    resultTable->blockCount = 0;


    firstTableSorted->columnCount = firstTable->columnCount;
    firstTableSorted->columns = firstTable->columns;
    firstTableSorted->blockCount = firstTable->blockCount;
    firstTableSorted->rowCount = firstTable->rowCount;
    firstTableSorted->maxRowsPerBlock = firstTable->maxRowsPerBlock;
    firstTableSorted->rowsPerBlockCount = firstTable->rowsPerBlockCount;


    secondTableSorted->columnCount = secondTable->columnCount;
    secondTableSorted->columns = secondTable->columns;
    secondTableSorted->blockCount = secondTable->blockCount;
    secondTableSorted->rowCount = secondTable->rowCount;
    secondTableSorted->maxRowsPerBlock = secondTable->maxRowsPerBlock;
    secondTableSorted->rowsPerBlockCount = secondTable->rowsPerBlockCount;

    int firstTableColumnIndex = firstTable->getColumnIndex(parsedQuery.joinFirstColumnName);
    int secondTableColumnIndex = secondTable->getColumnIndex(parsedQuery.joinSecondColumnName);
    RowCmp cmp1(vector<int>(1, firstTableColumnIndex), vector<SortingStrategy>(1, ASC));
    RowCmp cmp2(vector<int>(1, secondTableColumnIndex), vector<SortingStrategy>(1, ASC));
    // Do a sorted copy
    for (int i = 0; i < firstTable->blockCount; i++) {
        bufferManager.copyPage(firstTable->tableName, i, firstTableSorted->tableName, i);
        bufferManager.sortPage(firstTableSorted->tableName, i, cmp1);
    }

    mergeBlocks(firstTableSorted, cmp1);
    tableCatalogue.insertTable(firstTableSorted);

    for (int i = 0; i < secondTable->blockCount; i++) {
        bufferManager.copyPage(secondTable->tableName, i, secondTableSorted->tableName, i);
        bufferManager.sortPage(secondTableSorted->tableName, i, cmp2);
    }

    mergeBlocks(secondTableSorted, cmp2);
    tableCatalogue.insertTable(secondTableSorted);


    Cursor firstCursor = firstTableSorted->getCursor();
    Cursor secondCursor = secondTableSorted->getCursor();
    vector<int> firstRow = firstCursor.getNext();
    vector<int> secondRow = secondCursor.getNext();
    vector<vector<int>> rowBlock(resultTable->maxRowsPerBlock, vector<int>(resultTable->columnCount, 0));
    int rowCount = 0;
    int pageCount = 0;
    while (!firstRow.empty()) {
        while (!secondRow.empty()) {
            if (compare(firstRow[firstTableColumnIndex], secondRow[secondTableColumnIndex],
                        parsedQuery.joinBinaryOperator)) {
                vector<int> resultRow = firstRow;
                resultRow.insert(resultRow.end(), secondRow.begin(), secondRow.end());
                rowBlock[rowCount] = resultRow;
                if (rowCount == resultTable->maxRowsPerBlock - 1) {
                    bufferManager.writePage(resultTable->tableName, resultTable->blockCount, rowBlock, rowCount);
                    resultTable->blockCount++;
                    resultTable->rowsPerBlockCount.push_back(rowCount);
                    resultTable->rowCount += rowCount;
                    rowCount = 0;
                } else {
                    rowCount++;
                }
            }
            secondRow = secondCursor.getNext();
        }
        firstRow = firstCursor.getNext();
        secondCursor = secondTableSorted->getCursor();
        secondRow = secondCursor.getNext();
    }
    if (rowCount != 0) {
        bufferManager.writePage(resultTable->tableName, resultTable->blockCount, rowBlock, rowCount);
        resultTable->rowsPerBlockCount.push_back(rowCount);
        resultTable->rowCount += rowCount;
        resultTable->blockCount++;
    }
    resultTable->blockify();
    tableCatalogue.insertTable(resultTable);
    tableCatalogue.deleteTable(firstTableSorted->tableName);
    tableCatalogue.deleteTable(secondTableSorted->tableName);
}


bool compare(int first, int second, BinaryOperator binaryOperator) {
    switch (binaryOperator) {
        case LESS_THAN:
            return first < second;
        case GREATER_THAN:
            return first > second;
        case LEQ:
            return first <= second;
        case GEQ:
            return first >= second;
        case EQUAL:
            return first == second;
        case NOT_EQUAL:
            return first != second;
        case NO_BINOP_CLAUSE:
            return false;
    }
}
