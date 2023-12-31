#include"global.h"

bool semanticParse() {
    logger.log("semanticParse");
    switch (parsedQuery.queryType) {
        case CLEAR:
            return semanticParseCLEAR();
        case CROSS:
            return semanticParseCROSS();
        case DISTINCT:
            return semanticParseDISTINCT();
        case EXPORT:
            return semanticParseEXPORT();
        case INDEX:
            return semanticParseINDEX();
        case JOIN:
            return semanticParseJOIN();
        case LIST:
            return semanticParseLIST();
        case LOAD:
            return semanticParseLOAD();
        case PRINT:
            return semanticParsePRINT();
        case PROJECTION:
            return semanticParsePROJECTION();
        case RENAME:
            return semanticParseRENAME();
        case SELECTION:
            return semanticParseSELECTION();
        case SORT:
            return semanticParseSORT();
        case ORDERBY:
            return semanticParseORDERBY();
        case GROUPBY:
            return semanticParseGROUPBY();
        case SOURCE:
            return semanticParseSOURCE();
        case LOAD_MATRIX:
            return semanticParseLOAD_MATRIX();
        case PRINT_MATRIX:
            return semanticParsePRINT_MATRIX();
        case EXPORT_MATRIX:
            return semanticParseEXPORT_MATRIX();
        case RENAME_MATRIX:
            return semanticParseRENAME_MATRIX();
        case CHECKSYMMETRY:
            return semanticParseCHECKSYMMETRY();
        case TRANSPOSE_MATRIX:
            return semanticParseTRANSPOSE_MATRIX();
        case COMPUTE_MATRIX:
            return semanticParseCOMPUTE();
        default:
            cout << "SEMANTIC ERROR" << endl;
    }

    return false;
}