# Project Phase-1 Report

#### Team baseBase

- Rudransh Pratap Singh [2020111007]
- A Kishore Kumar [2020101126]
- Abhijnan Vegi [2020101004]

---

## Overview of the changes made

Added/Modified Files:

```
src
├── executors
    ├── print.cpp (Modified to support Matrix print)
    ├── export.cpp (Modified to support Matrix export)
    ├── rename.cpp (Modified to support Matrix rename)
    ├── transpose.cpp (Added to support Matrix transpose)
    ├── checksymmetry.cpp (Added to support Matrix checksymmetry)
    ├── compute.cpp (Added to support Matrix compute)
    ├── load.cpp (Modified to support Matrix load)
├── matrix.cpp (Added)
├── matrix.h (Added)
├── tableCatalogue.cpp (Modified)
├── tableCatalogue.h (Modified)
├── syntacticParser.cpp (Modified)
├── syntacticParser.h (Modified)
├── semanticParser.cpp (Modified)
├── semanticParser.h (Modified)
├── executor.cpp (Modified)
├── executor.h (Modified)
├── global.h (Modified)
```

## Page Design

The Page Design works as follows. The Matrix is broken into blocks of size 15x15. Each block is stored in a separate
file.
The blocks are stored in row major order. The number of blocks is given as ceil(n/15)*ceil(n/15).

This ensures that the transpose, checksymmetry and compute operations are efficient, since all we need to do in order to
perform a transpose is as follows :

- For the diagonal blocks, just compute the transpose directly
- For the Off Diagonal blocks, swap the blocks and then compute the transpose for each of them

<img src="./design.png" height="146" width="128">

---

### LOAD Command

We made changes to the `tableCatalogue` so that it supports both tables and matrices. We then added the insertMatrix
function, which adds matrices to the tableCatalogue.

```cpp
void executeLOAD_MATRIX()
{
    logger.log("executeLOAD");
    Matrix *matrix = new Matrix(parsedQuery.loadRelationName);
    if (matrix->load())
    {
        tableCatalogue.insertMatrix(matrix);
        cout << "Loaded Matrix. Dimensions: " << matrix->columnCount << endl;
    }
    return;
}
```

We then modified the `load.cpp` file to support the matrix load operations, and added the declarations for the functions
in the `semanticParser.h/semanticParser.cpp`, `syntacticParser.h/syntacticParser.cpp`, and `executor.h/executor.cpp`
files.

Also since the syntax for the MATRIX commands follows the format `COMMAND MATRIX <MATRIX_NAME>`. We added support for 3
arguments in the `syntacticParser.cpp` file.

### PRINT Command

We made similar changes to the PRINT Command as well. We first added the `Matrix::print()` function in `matrix.cpp`
which prints the matrix in the required format.
We then modified the `print.cpp` file to support the matrix print operations, and added the declarations for the
functions
in the `semanticParser.h/semanticParser.cpp`, `syntacticParser.h/syntacticParser.cpp`, and `executor.h/executor.cpp`
files.

### RENAME Command

Implementing rename requires us to not only change the name of the matrix in the Table Catalogue, but we also need to
rename the pages corresponding to the matrix, since the pages are named as `<matrixname>_Page_<pageindex>`. Therefore we
first added support for the command by adding the `TableCatalogue::renameMatrix()` function in `tableCatalogue.cpp`.
This function changes the name of the matrix from `oldMatrixName` to `newMatrixName` in the Table Catalogue. It also
calls the `Matrix::rename` function which renames the pages corresponding to the matrix. It does this by calling the
`BufferManager::renameFile` and `BufferManager::deleteFile` functions.

### EXPORT Command

The Export Commands works similar to the PRINT command, except that it writes the output to a file by the name
of `MATRIX_NAME.csv`. We implemented the `Matrix::makePermanent` function which writes the matrix to a file with the
appropriate name.

### CHECKSYMMETRY Command

### TRANSPOSE Command

### COMPUTE Command

---

## Learnings

## Assumptions

---

## Contributions

### Rudransh Pratap Singh

- Implemented Matrix and MatrixCatalogue classes
- Implemented `EXPORT` command
- Report

### A Kishore Kumar

- Implemented `TRANSPOSE` , `CHECKSYMMETRY` and `COMPUTE` commands

### Abhijnan Vegi

- Implemented `LOAD` , `PRINT` and `RENAME` commands
- Adjusted Page Design and matrix representation

