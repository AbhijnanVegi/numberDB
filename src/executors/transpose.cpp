#include "global.h"

bool syntacticParseTRANSPOSE_MATRIX()
{
    logger.log("syntacticParseTranspose");
    if (tokenizedQuery.size() != 3 || tokenizedQuery[1] != "MATRIX")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = TRANSPOSE_MATRIX;
    parsedQuery.transposeMatrixName = tokenizedQuery[2];
    return true;
}

bool semanticParseTRANSPOSE_MATRIX()
{
    logger.log("semanticParseTranspose");
    if (!tableCatalogue.isMatrix(parsedQuery.transposeMatrixName))
    {
        cout << "SEMANTIC ERROR: Matrix doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeTRANSPOSE_MATRIX()
{
    logger.log("executeTranspose");
    Matrix* matrix = tableCatalogue.getMatrix(parsedQuery.transposeMatrixName);
    matrix->transpose();
    return;
}
