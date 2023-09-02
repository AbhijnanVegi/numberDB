#include "global.h"

/**
 * @brief 
 * SYNTAX: EXPORT <relation_name> 
 */

bool syntacticParseEXPORT() {
    logger.log("syntacticParseEXPORT");
    if (tokenizedQuery.size() != 2) {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = EXPORT;
    parsedQuery.exportRelationName = tokenizedQuery[1];
    return true;
}

bool semanticParseEXPORT() {
    logger.log("semanticParseEXPORT");
    //Table should exist
    if (tableCatalogue.isTable(parsedQuery.exportRelationName))
        return true;
    cout << "SEMANTIC ERROR: No such relation exists" << endl;
    return false;
}

void executeEXPORT() {
    logger.log("executeEXPORT");
    Table *table = tableCatalogue.getTable(parsedQuery.exportRelationName);
    table->makePermanent();
    return;
}


/*Matrix Commands for export*/
bool syntacticParseEXPORT_MATRIX() {
    logger.log("syntacticParseEXPORT_MATRIX");
    if (tokenizedQuery.size() != 3 || tokenizedQuery[1] != "MATRIX") {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = EXPORT_MATRIX;
    parsedQuery.exportRelationName = tokenizedQuery[2];
    return true;
}

bool semanticParseEXPORT_MATRIX() {
    logger.log("semanticParseEXPORT_MATRIX");
    //Matrix should exist
    if (tableCatalogue.isMatrix(parsedQuery.exportRelationName))
        return true;
    cout << "SEMANTIC ERROR: Matrix doesn't exist" << endl;
    return false;
}

void executeEXPORT_MATRIX() {
    logger.log("executeEXPORT_MATRIX");
    Matrix *matrix = tableCatalogue.getMatrix(parsedQuery.exportRelationName);
    matrix->makePermanent();
    return;
}
