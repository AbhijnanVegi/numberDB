#include "global.h"

bool syntacticParseCOMPUTE() {
    logger.log("syntacticParseTranspose");
    if (tokenizedQuery.size() != 2) {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = COMPUTE_MATRIX;
    parsedQuery.computeMatrixName = tokenizedQuery[1];
    return true;
}

bool semanticParseCOMPUTE() {
    logger.log("semanticParseCompute");
    if (!tableCatalogue.isMatrix(parsedQuery.computeMatrixName)) {
        cout << "SEMANTIC ERROR: Matrix doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeCOMPUTE() {
    logger.log("executeCompute");
    Matrix *matrix = tableCatalogue.getMatrix(parsedQuery.computeMatrixName);

    matrix->compute();
    return;
}
