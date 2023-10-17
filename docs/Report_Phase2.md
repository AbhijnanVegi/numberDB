# Project Phase-1 Report

#### Team baseBase

- Rudransh Pratap Singh [2020111007]
- A Kishore Kumar [2020101126]
- Abhijnan Vegi [2020101004]

---

## Assumptions

- The table name does not end in `_sorted`

## The External Sorting Algorithm

The external sorting algorithm consists of two main phases: sorting and merging. In the sorting phase, we bring blocks
of data into the main memory and sort them before writing them back. In the merging phase, we merge sorted runs of data
to create longer sorted runs. Let's break down these phases further, considering a main-memory size of Nb blocks.

### Sorting Phase:

During the sorting phase, we go through the blocks of the table to be sorted., sort them in memory and write back the
sorted data into the same blocks, effectively achieving in-place sorting.

At the end of this phase, we have multiple sorted runs, with the number of runs being `ceil(B / (Nb - 1))`, where B is
the total block count.

### Merging Phase:

In the merging phase, we work with a certain number of sorted runs, which we'll represent as `Nr`. Initially, Nr is set
to
`Nr_0 = ceil(B / (Nb - 1))`, as explained earlier. In each merge phase stage, we merge the current Nr runs to
create `ceil(
Nr / (Nb - 1))` longer sorted runs, unless Nr is already 1, indicating that the file is already sorted.

Let's describe how a particular merge phase stage works. We start by fetching the first blocks from the next `Nb - 1`
runs (or fewer if there are fewer runs left). These entries are added to a priority queue using a comparator that
matches the input specifications. We then continuously extract the top element from the priority queue and place it in a
write buffer. Once this buffer is full, we perform a page write. If there are more rows in the same run after extracting
an element, they are added to the priority queue.

Similar to the sorting phase, we perform batched writes. When our write buffer is full (equivalent to one block), we
write it back to the disk with the appropriate file name. These stages continue until the number of runs reduces to 1,
indicating that the entire file is sorted.

## SORT Command

We use the external sorting algorithm developed in the first part of the assignment to sort the tables. Since the SORT
Command follows the syntax

```
SORT <table_name> BY <column_name1, column_name2,..., column_namek> IN
<ASC|DESC, ASC|DESC,..., ASC|DESC>
```

We then iterate through the tokens starting from the fourth token until we see a token with the value "IN", and append
it to a list. Similarly we also store the tokens corresponding to the sorting Strategies (ASC or DESC). We then finally
compare the lengths of the list of column names, and the list of sorting strategies and check whether they are equal.

For the Semantic checks, we check whether the table exists. If it does, we then check whether every column mentioned in
the query exists as an attribute of the relation.

We divide the implementation phase into 3 parts : Sorting of pages/Blocks, Merging of Blocks, and the final sorting
implementation

### Sorting of pages

We define a function called Page.sortPage() that sorts a page of a table in memory. The function takes the name of the
table, the index of the page to be sorted, and a RowCmp object as arguments. The RowCmp object is used to compare rows
of the page during the sort operation. The function first constructs the name of the page using the table name and page
index. It then checks if the page is already in the buffer pool. If the page is not in the buffer pool, the function
inserts it into the pool.

The function then iterates over the pages in the buffer pool and finds the page with the matching name. Once the page is
found, the sortPage() function of the Page class is called on the page object with the RowCmp object as an argument. The
sortPage() function sorts the rows of the page using the RowCmp object. The function then increments the write count of
the access logger to indicate that the page has been modified using a dirty bit and is written to disk when pushed out
of the buffer. This is then used in a function called sortBlockswhich takes in a table and a comparator as argument and
goes through every page of the table and sorts it using the mentioned comparator.

### Merging Blocks

This code defines a function called mergeBlocks() that merges blocks of a table in memory. The function takes a pointer
to a Table object and a RowCmp object as arguments. The RowCmp object is used to compare rows of the table during the
merge operation. The function first determines the number of blocks in the table and the number of memory buffers to
use. It then calculates the number of runs required to merge all the blocks.

The function then enters a loop that merges blocks until there is only one block left. In each iteration of the loop,
the function reads a fixed number of blocks into memory and merges them into a new block. The new block is then written
to disk. This process continues until all blocks have been merged into a single block.

### Final Sorting implementation

We define a function which performs a sort operation on a table. The function first retrieves the table from a table
catalogue using its name. It then creates a vector of column indices that correspond to the columns that are being
sorted. The function iterates over the names of the columns being sorted and adds their indices to the vector. The
function then creates a RowCmp object that is used to compare rows of the table during the sort operation. The RowCmp
object is initialized with the vector of column indices and the sorting strategy specified in the parsed query.

The function then calls two helper functions to sort and merge the blocks of the table. The sortBlocks() function sorts
the blocks of the table using the RowCmp object. The mergeBlocks() function merges the sorted blocks of the table into a
single sorted block.

## JOIN Command

The JOIN Command was implemented by using the External Sorting Algorithm developed in the first part of the assignment.
Since the syntax of the command is as follows

```
<new_relation_name> <- JOIN <tablename1>, <tablename2> ON <column1>
<bin_op> <column2>
```

Therefore we ensure that there are exactly 9 tokens, and we perform the following breakdown of the tokens.

1) The first token corresponds to the name of the resulting table formed after JOIN
2) The fourth token corresponds to the name of the first table
3) The fifth token corresponds to the name of the second table
4) The seventh and ninth tokens correspond to the column names on which the two relations are to be sorted on
5) The eight token corresponds to the Binary operator being used for selecting the records

We then perform a semantic check on the table as well. We check whether any of the three tables : The resultant table,
or either of the two input tables are already loaded. If that is indeed the case, then we proceed to return an error
statement.

We define a function that performs a join operation on two tables. The function first retrieves the two tables from a
table catalogue using their names. It then creates two new tables, one for each of the original tables, and appends "_
sorted" to their names. The function then creates a new table that will hold the result of the join operation. The
columns of the result table are the union of the columns of the two original tables.

The function then sorts the two original tables based on the columns that are being joined. It does this by copying each
page of the original tables to the corresponding page of the new sorted tables and then sorting each page using a
comparison function. The sorted tables are then merged into a single sorted table.

The function then creates cursors for the two sorted tables and retrieves the first row from each table. It then
iterates over the rows of the two tables, comparing the values of the columns being joined using a binary operator. If
the comparisonis true, the function creates a new row that is the concatenation of the two rows being compared and adds
it to the result table. The function continues this process until all rows of the two tables have been compared.

Finally, the function writes the result table to disk, deletes the two sorted tables, and returns.

# GROUP BY Command

The provided code meticulously implements a GROUP BY query, encompassing syntactic parsing, semantic validation, and query execution. In the syntactic parsing phase, it diligently ensures the query adheres to the expected structure, meticulously validating the presence and sequence of keywords such as "BY," "FROM," "HAVING," and "RETURN." It also rigorously checks the syntax of aggregation functions, making certain they follow the prescribed format. Upon passing these syntactic checks, the code populates the parsedQuery data structure with critical information, including the result relation's name, grouping attribute, source relation name, aggregation functions, comparison values, and return functions.

The semantic parsing phase is equally comprehensive. It scrutinizes the query for semantic correctness, commencing by verifying the non-existence of the result relation in the table catalog, thus preventing overwriting of existing data. Subsequently, it examines the existence of the specified source relation and ensures that all columns referenced in the query indeed belong to this relation. Any discrepancies in these checks prompt the code to emit informative semantic error messages.

The query execution phase orchestrates the actual grouping and aggregation process. It first creates a sorted copy of the table, utilizing the designated grouping attribute and an ascending sorting strategy. It extracts the indices of the relevant columns, crucial for grouping, aggregation, and return operations. Employing a cursor, the code iterates through the sorted data, calculating aggregation results and satisfying the HAVING clause's conditions. To optimize memory usage, results are written in a block-wise manner. Subsequently, the result table is inserted into the table catalog, temporary tables are expeditiously deleted, and the function concludes its execution. This meticulous and detailed implementation ensures the robust handling of GROUP BY queries in a database context.

## Contributions

### Rudransh Pratap Singh

- Implemented `JOIN` Command
- Report

### A Kishore Kumar

- Implemented `GROUP BY`

### Abhijnan Vegi

- Implemented External Sorting, `SORT` Command


