#include"semanticParser.h"

void executeCommand();

void executeCLEAR();

void executeCROSS();

void executeDISTINCT();

void executeEXPORT();

void executeINDEX();

void executeJOIN();

void executeLIST();

void executeLOAD();

void executePRINT();

void executePROJECTION();

void executeRENAME();

void executeSELECTION();

void executeSORT();

void executeORDERBY();

void executeGROUPBY();

void executeSOURCE();

void executeLOAD_MATRIX();

void executePRINT_MATRIX();

void executeEXPORT_MATRIX();

void executeRENAME_MATRIX();

void executeCHECKSYMMETRY();

void executeTRANSPOSE_MATRIX();

void executeCOMPUTE();

bool evaluateBinOp(int value1, int value2, BinaryOperator binaryOperator);

void printRowCount(int rowCount);

void sortBlocks(Table* table, RowCmp cmp);

void mergeBlocks(Table *table, RowCmp cmp);
