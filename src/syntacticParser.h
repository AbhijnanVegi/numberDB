#include "tableCatalogue.h"
#include "comparator.h"

using namespace std;

enum QueryType {
    CLEAR,
    CROSS,
    DISTINCT,
    EXPORT,
    GROUPBY,
    INDEX,
    JOIN,
    LIST,
    LOAD,
    LOAD_MATRIX,
    PRINT,
    PROJECTION,
    RENAME,
    SELECTION,
    SORT,
    ORDERBY,
    SOURCE,
    UNDETERMINED,
    EXPORT_MATRIX,
    PRINT_MATRIX,
    RENAME_MATRIX,
    CHECKSYMMETRY,
    TRANSPOSE_MATRIX,
    COMPUTE_MATRIX
};

enum BinaryOperator {
    LESS_THAN,
    GREATER_THAN,
    LEQ,
    GEQ,
    EQUAL,
    NOT_EQUAL,
    NO_BINOP_CLAUSE
};

enum SelectType {
    COLUMN,
    INT_LITERAL,
    NO_SELECT_CLAUSE
};

class ParsedQuery {

public:
    QueryType queryType = UNDETERMINED;

    string clearRelationName = "";

    string crossResultRelationName = "";
    string crossFirstRelationName = "";
    string crossSecondRelationName = "";

    string distinctResultRelationName = "";
    string distinctRelationName = "";

    string exportRelationName = "";

    IndexingStrategy indexingStrategy = NOTHING;
    string indexColumnName = "";
    string indexRelationName = "";

    BinaryOperator joinBinaryOperator = NO_BINOP_CLAUSE;
    string joinResultRelationName = "";
    string joinFirstRelationName = "";
    string joinSecondRelationName = "";
    string joinFirstColumnName = "";
    string joinSecondColumnName = "";

    string loadRelationName = "";

    string printRelationName = "";

    string projectionResultRelationName = "";
    vector<string> projectionColumnList;
    string projectionRelationName = "";

    string renameFromColumnName = "";
    string renameToColumnName = "";
    string renameRelationName = "";

    SelectType selectType = NO_SELECT_CLAUSE;
    BinaryOperator selectionBinaryOperator = NO_BINOP_CLAUSE;
    string selectionResultRelationName = "";
    string selectionRelationName = "";
    string selectionFirstColumnName = "";
    string selectionSecondColumnName = "";
    int selectionIntLiteral = 0;

    vector<SortingStrategy> sortingStrategy;
    vector<string> sortColumnNames;
    string sortRelationName = "";

    string orderByResultRelationName = "";
    string orderByRelationName = "";
    string orderByColumnName = "";
    SortingStrategy orderBySortingStrategy;

    string groupByResultRelationName = "";
    string groupByRelationName = "";
    string groupByColumnName = "";
    string groupByAggregationAttribute = "";
    string groupByAggregationFunction = "";
    string groupByReturnFunction = "";
    string groupByReturnAttribute = "";
    string groupByAggregationCompareVal = "";
    BinaryOperator groupByBinaryOperator = NO_BINOP_CLAUSE;
    SortingStrategy groupBySortingStrategy;

    string sourceFileName = "";

    string renameFromMatrixName = "";
    string renameToMatrixName = "";

    string checkSymmetryMatrixName = "";

    string transposeMatrixName = "";

    string computeMatrixName = "";

    ParsedQuery();

    void clear();
};

bool syntacticParse();

bool syntacticParseCLEAR();

bool syntacticParseCROSS();

bool syntacticParseDISTINCT();

bool syntacticParseEXPORT();

bool syntacticParseINDEX();

bool syntacticParseJOIN();

bool syntacticParseLIST();

bool syntacticParseLOAD();

bool syntacticParsePRINT();

bool syntacticParsePROJECTION();

bool syntacticParseRENAME();

bool syntacticParseSELECTION();

bool syntacticParseSORT();

bool syntacticParseORDERBY();

bool syntacticParseGROUPBY();

bool syntacticParseSOURCE();

bool syntacticParseLOAD_MATRIX();

bool syntacticParsePRINT_MATRIX();

bool syntacticParseEXPORT_MATRIX();

bool syntacticParseRENAME_MATRIX();

bool syntacticParseCHECKSYMMETRY();

bool syntacticParseCOMPUTE();

bool syntacticParseTRANSPOSE_MATRIX();

bool isFileExists(string tableName);

bool isQueryFile(string fileName);

std::optional<BinaryOperator> mapStringToBinOp(const string &s);
