#include "comparator.h"
#include"global.h"

/**
 * @brief File contains method to process SORT commands.
 * 
 * syntax:
 * R <- SORT relation_name BY column_name IN sorting_order
 * 
 * sorting_order = ASC | DESC 
 */
bool syntacticParseSORT() {
    logger.log("syntacticParseSORT");
    if (tokenizedQuery[2] != "BY") {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = SORT;
    parsedQuery.sortRelationName = tokenizedQuery[1];
    auto queryIterator = tokenizedQuery.begin() + 3;
    for (; queryIterator <= tokenizedQuery.end() && *queryIterator != "IN"; queryIterator++) {
        parsedQuery.sortColumnNames.push_back(*queryIterator);
    }
    for (queryIterator++; queryIterator < tokenizedQuery.end(); queryIterator++) {
        if (*queryIterator == "ASC")
            parsedQuery.sortingStrategy.push_back(ASC);
        else if (*queryIterator == "DESC")
            parsedQuery.sortingStrategy.push_back(DESC);
        else {
            cout << "SYNTAX ERROR: " << *queryIterator << " not an order" << endl;
            return false;
        }
    }
    if (parsedQuery.sortColumnNames.empty() ||
        (parsedQuery.sortColumnNames.size() != parsedQuery.sortingStrategy.size())) {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    return true;
}

bool semanticParseSORT() {
    logger.log("semanticParseSORT");

    if (!tableCatalogue.isTable(parsedQuery.sortRelationName)) {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    for (const string &column: parsedQuery.sortColumnNames) {
        if (!tableCatalogue.isColumnFromTable(column, parsedQuery.sortRelationName)) {
            cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
            return false;
        }
    }

    return true;
}

void sortBlocks(Table *table, RowCmp cmp) {
    for (int i = 0; i < table->blockCount; i++)
        bufferManager.sortPage(table->tableName, i, cmp);
}

void mergeBlocks(Table *table, RowCmp cmp) {
    int bc = table->blockCount; //block count
    const int memoryBuffer = 10; // in memory buffers to use
    int nruns = 1; // runs
    int prevFileSize = 1;

    while (bc > 1) {
        int blocksToMerge = min(memoryBuffer - 1, bc);
        int filesToWrite = (bc + blocksToMerge - 1) / blocksToMerge;
        int newFileSize = blocksToMerge * prevFileSize;

        string prevTable = nruns == 1 ? table->tableName : table->tableName + "_Run" + to_string(nruns - 1);
        string newTable = table->tableName + "_Run" + to_string(nruns);

        int filesWritten = 0;
        int fileIndex = 0;
        while (filesWritten < filesToWrite) {
            // Read pages
            vector<Page> pages;
            vector<int> pos;
            for (; fileIndex < bc and pages.size() < blocksToMerge; fileIndex++) {
                pages.emplace_back(bufferManager.getPage(prevTable, fileIndex * prevFileSize, table->columnCount));
            }
            pos.assign(pages.size(), 0);

            vector<vector<int>> rows;
            int maxRows = pages[0].getDimensions().first;

            //Merge pages
            vector<int> minRow;
            int minRowIdx = 0;
            int blockNum = filesWritten * newFileSize;

            while (!pages.empty()) {
                // Get minimum row
                for (int i = 0; i < pages.size(); i++) {
                    vector<int> curPage = pages[i].getRow(pos[i]);
                    if (curPage.empty()) {
                        bufferManager.deleteFile(pages[i].pageName);
                        pages.erase(pages.begin() + i);
                        pos.erase(pos.begin() + i);
                        i--;
                        continue;
                    }
                    if (minRow.empty() or cmp(curPage, minRow)) {
                        minRow = curPage;
                        minRowIdx = i;
                    }
                }

                // Add minimum row to rows
                if (minRow.empty()) continue;
                rows.push_back(minRow);
                minRow.clear();
                if (rows.size() >= maxRows) {
                    bufferManager.writePage(newTable, blockNum, rows, rows.size());
                    blockNum++;
                    rows.clear();
                }

                //Update pages
                pos[minRowIdx]++;
                if (pos[minRowIdx] >= pages[minRowIdx].getDimensions().first) {
                    // Get next block of file
                    int pageIndex = pages[minRowIdx].getPageIndex();
                    if (++pageIndex % prevFileSize == 0) {
                        bufferManager.deleteFile(pages[minRowIdx].pageName);
                        pages.erase(pages.begin() + minRowIdx);
                        pos.erase(pos.begin() + minRowIdx);
                        continue;
                    }
                    bufferManager.deleteFile(pages[minRowIdx].pageName);
                    pages[minRowIdx] = bufferManager.getPage(prevTable, pageIndex, table->columnCount);
                    pos[minRowIdx] = 0;
                }
            }
            filesWritten++;
        }
        nruns++;
        prevFileSize = newFileSize;
        bc = filesToWrite;
    }

    // Rename file
    string oldName = "../data/temp/" + table->tableName + "_Run" + to_string(nruns - 1);
    string newName = "../data/temp/" + table->tableName;
    for (int i = 0; i < table->blockCount; i++) {
        bufferManager.renameFile(oldName + "_Page" + to_string(i), newName + "_Page" + to_string(i));
    }
}

void executeSORT() {
    logger.log("executeSORT");
    Table *table = tableCatalogue.getTable(parsedQuery.sortRelationName);
    vector<int> columnIndices;
    for (const string &col: parsedQuery.sortColumnNames) {
        columnIndices.push_back(table->getColumnIndex(col));
    }
    RowCmp cmp(columnIndices, parsedQuery.sortingStrategy);

    // sort blocks
    sortBlocks(table, cmp);

    // Merge blocks
    mergeBlocks(table, cmp);

    return;
}