#include "comparator.h"
#include "global.h"

bool syntacticParseORDERBY() {
    logger.log("syntacticParseORDERBY");
    if (tokenizedQuery.size() != 8 || tokenizedQuery[3] != "BY" || tokenizedQuery[6] != "ON")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = ORDERBY;
    parsedQuery.orderByResultRelationName = tokenizedQuery[0];
    parsedQuery.orderByRelationName = tokenizedQuery[7];
    parsedQuery.orderByColumnName = tokenizedQuery[4];
    if (tokenizedQuery[5] == "ASC")
        parsedQuery.orderBySortingStrategy = ASC;
    else if (tokenizedQuery[5] == "DESC")
        parsedQuery.orderBySortingStrategy = DESC;
    else {
        cout << "SYNTAX ERROR: " << tokenizedQuery[5] << " not an order" << endl;
        return false;
    }
    return true;
}

bool semanticParseORDERBY() {
    logger.log("semanticParseORDERBY");
    if (tableCatalogue.exists(parsedQuery.orderByResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.orderByRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    if (!tableCatalogue.isColumnFromTable(parsedQuery.orderByColumnName, parsedQuery.orderByRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    return true;
}

void executeORDERBY() {
    logger.log("executeORDERBY");
    Table* table = tableCatalogue.getTable(parsedQuery.orderByRelationName);
    Table* newTable = new Table(parsedQuery.orderByResultRelationName);
    newTable->columnCount = table->columnCount;
    newTable->columns = table->columns;
    newTable->blockCount = table->blockCount;
    newTable->rowCount = table->rowCount;
    newTable->maxRowsPerBlock = table->maxRowsPerBlock;
    newTable->rowsPerBlockCount = table->rowsPerBlockCount;
    int columnIndex = table->getColumnIndex(parsedQuery.orderByColumnName);
    RowCmp cmp(vector<int>(1, columnIndex), vector<SortingStrategy>(1, parsedQuery.orderBySortingStrategy));

    // Do a sorted copy
    for (int i = 0; i < table->blockCount; i++) {
        bufferManager.copyPage(table->tableName, i, newTable->tableName, i);
        bufferManager.sortPage(newTable->tableName, i, cmp);
    }

    mergeBlocks(newTable, cmp);
    tableCatalogue.insertTable(newTable);

    return;
}
