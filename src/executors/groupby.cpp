#include "comparator.h"
#include "global.h"

bool syntacticParseGROUPBY() {
    logger.log("syntacticParseGROUPBY");

    vector<pair<int, string>> syntax_check = {
        {3, "BY"}, {5, "FROM"}, {7, "HAVING"}, {11, "RETURN"}, };
    vector<string> fns = {"MAX", "MIN", "SUM", "AVG"};
    if (tokenizedQuery.size() != 13) return cout << "SYNTAX ERROR" << endl, false;
    for(const auto &[id, match] : syntax_check)
        if(tokenizedQuery[id] != match) return cout << "SYNTAX ERROR" << endl, false;

    auto valid_fn_syntax = [&](const string &s){
        if(s.size() <= 5) return false;
        if(s[3] != '(' or s.back() != ')') return false;
        for(auto &fn : fns) if(s.substr(0, 3) == fn) return true;
        return false;
    };

    if(!valid_fn_syntax(tokenizedQuery[8]) or !valid_fn_syntax(tokenizedQuery[12]))
        return cout << "SYNTAX ERROR" << endl, false;

    parsedQuery.queryType = GROUPBY;
    parsedQuery.groupByResultRelationName = tokenizedQuery[0];
    parsedQuery.groupByRelationName = tokenizedQuery[6];
    parsedQuery.groupByColumnName = tokenizedQuery[4];
    parsedQuery.groupByAggregationFunction = tokenizedQuery[8].substr(0, 3);
    parsedQuery.groupByAggregationAttribute = tokenizedQuery[8].substr(4);
    parsedQuery.groupByAggregationCompareVal = tokenizedQuery[10];
    parsedQuery.groupByReturnFunction = tokenizedQuery[12].substr(0, 3);
    parsedQuery.groupByReturnAttribute = tokenizedQuery[12].substr(4);
    parsedQuery.groupByAggregationAttribute.pop_back();    
    parsedQuery.groupByReturnAttribute.pop_back();    

    const auto bin_op = mapStringToBinOp(tokenizedQuery[9]);
    if(!bin_op.has_value()) return cout << "SYNTAX ERROR" << endl, false;
    parsedQuery.groupByBinaryOperator = bin_op.value();

    return true;
}

bool semanticParseGROUPBY() {
    logger.log("semanticParseGROUPBY");
    if (tableCatalogue.exists(parsedQuery.groupByResultRelationName))
        return cout << "SEMANTIC ERROR: Resultant relation already exists" << endl, false;
    if (!tableCatalogue.isTable(parsedQuery.groupByRelationName))
        return cout << "SEMANTIC ERROR: Relation doesn't exist" << endl, false;
    if (!tableCatalogue.isColumnFromTable(parsedQuery.groupByColumnName, parsedQuery.groupByRelationName))
        return cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl, false;
    if (!tableCatalogue.isColumnFromTable(parsedQuery.groupByAggregationAttribute, parsedQuery.groupByRelationName))
        return cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl, false;
    if (!tableCatalogue.isColumnFromTable(parsedQuery.groupByReturnAttribute, parsedQuery.groupByRelationName))
        return cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl, false;
    return true;
}

std::optional<int> apply(std::optional<int> a, std::optional<int> b, const string &fn){
    if(!a.has_value()) return b;
    if(!b.has_value()) return a;
    if(fn == "MAX") return max(a.value(), b.value());
    if(fn == "MIN") return min(a.value(), b.value());
    if(fn == "SUM" or fn == "AVG") return a.value() + b.value();
    return 0;
}

bool compare2(int first, int second, BinaryOperator binaryOperator) {
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
    return false;
}


void executeGROUPBY() {
    logger.log("executeGROUPBY");
    Table* table = tableCatalogue.getTable(parsedQuery.groupByRelationName);
    Table* sortedTable = new Table(parsedQuery.groupByRelationName + "_tmp");
    sortedTable->columnCount = table->columnCount;
    sortedTable->columns = table->columns;
    sortedTable->blockCount = table->blockCount;
    sortedTable->rowCount = table->rowCount;
    sortedTable->maxRowsPerBlock = table->maxRowsPerBlock;
    sortedTable->rowsPerBlockCount = table->rowsPerBlockCount;
    
    int groupByIndex = table->getColumnIndex(parsedQuery.groupByColumnName);
    int aggregateIndex = table->getColumnIndex(parsedQuery.groupByAggregationAttribute);
    int returnIndex = table->getColumnIndex(parsedQuery.groupByReturnAttribute);
    RowCmp cmp(vector<int>(1, groupByIndex), vector<SortingStrategy>(1, ASC));

    // Do a sorted copy
    for (int i = 0; i < table->blockCount; i++) {
        bufferManager.copyPage(table->tableName, i, sortedTable->tableName, i);
        bufferManager.sortPage(sortedTable->tableName, i, cmp);
    }
    mergeBlocks(sortedTable, cmp);
    tableCatalogue.insertTable(sortedTable);

    Cursor cursor = sortedTable->getCursor();
    std::optional<int> prv, aggregate, answer;
    int num_entries = 0;
    vector<string> resultColumns = {parsedQuery.groupByColumnName, parsedQuery.groupByReturnFunction + parsedQuery.groupByReturnAttribute};
    Table* resultTable = new Table(parsedQuery.groupByResultRelationName, resultColumns);
    int rowCount = 0;
    vector<vector<int>> rowBlock(resultTable->maxRowsPerBlock, vector<int>(resultTable->columnCount, 0));

    auto force_write_page = [&](){
        if(rowCount == 0) return;
        bufferManager.writePage(resultTable->tableName, resultTable->blockCount, rowBlock, rowCount);
        resultTable->blockCount++;
        resultTable->rowsPerBlockCount.push_back(rowCount);
        resultTable->rowCount += rowCount;
        rowCount = 0;
    };

    auto conditional_insert_row = [&](std::optional<int> aggregate, std::optional<int> answer){
        if(!aggregate.has_value() or !answer.has_value() or !prv.has_value()) return;
        int agg = aggregate.value(), ans = answer.value();
        if(parsedQuery.groupByAggregationFunction == "AVG") agg /= num_entries; // intentional
        if(parsedQuery.groupByReturnFunction == "AVG") ans /= num_entries; // intentional
        if(!compare2(agg, stoll(parsedQuery.groupByAggregationCompareVal), parsedQuery.groupByBinaryOperator))
            return;
        rowBlock[rowCount++] = {prv.value(), ans};
        if(rowCount == resultTable->maxRowsPerBlock) force_write_page();
    };
    
    for(auto row = cursor.getNext(); !row.empty(); row = cursor.getNext()){
        if(!prv.has_value() or prv != row[groupByIndex]){
            conditional_insert_row(aggregate, answer);
            num_entries = 1;
            prv = row[groupByIndex];
            aggregate = row[aggregateIndex];
            answer = row[returnIndex];
        }
        else {
            aggregate = apply(aggregate, row[aggregateIndex], parsedQuery.groupByAggregationFunction);
            answer = apply(answer, row[returnIndex], parsedQuery.groupByReturnFunction);
            num_entries++;
        }
    }
    conditional_insert_row(aggregate, answer);
    force_write_page();
    tableCatalogue.insertTable(resultTable);
    tableCatalogue.deleteTable(sortedTable->tableName);
    return;
}

