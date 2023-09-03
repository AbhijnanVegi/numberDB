#include "global.h"

bool syntacticParseCHECKSYMMETRY() {
    logger.log("syntacticParseCHECKSYMMETRY");
    if (tokenizedQuery.size() != 2) {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = CHECKSYMMETRY;
    parsedQuery.checkSymmetryMatrixName = tokenizedQuery[1];
    return true;
}

bool semanticParseCHECKSYMMETRY() {
    logger.log("semanticParseCHECKSYMMETRY");
    if (!tableCatalogue.isMatrix(parsedQuery.checkSymmetryMatrixName)) {
        cout << "SEMANTIC ERROR: Matrix doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeCHECKSYMMETRY() {
    logger.log("executeCHECKSYMMETRY");
    Matrix* matrix = tableCatalogue.getMatrix(parsedQuery.checkSymmetryMatrixName);
    if (matrix->checkSymmetry()) cout << "TRUE" << endl;
    else cout << "FALSE" << endl;
    return;
}
