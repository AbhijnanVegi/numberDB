#include "global.h"

BufferManager::BufferManager() {
    logger.log("BufferManager::BufferManager");
}

/**
 * @brief Function called to read a page from the buffer manager. If the page is
 * not present in the pool, the page is read and then inserted into the pool.
 *
 * @param tableName 
 * @param pageIndex 
 * @return Page 
 */
Page BufferManager::getPage(string tableName, int pageIndex) {
    logger.log("BufferManager::getPage");
    string pageName = "../data/temp/" + tableName + "_Page" + to_string(pageIndex);
    accessLogger.reads++;
    if (this->inPool(pageName))
        return this->getFromPool(pageName);
    else
        return this->insertIntoPool(tableName, pageIndex);
}


Page BufferManager::getPage(string tableName, int pageIndex, int columnCount) {
    logger.log("BufferManager::getPage");
    string pageName = "../data/temp/" + tableName + "_Page" + to_string(pageIndex);
    accessLogger.reads++;
    if (this->inPool(pageName))
        return this->getFromPool(pageName);
    else
        return this->insertIntoPool(tableName, pageIndex, columnCount);
}

/**
 * @brief Checks to see if a page exists in the pool
 *
 * @param pageName 
 * @return true 
 * @return false 
 */
bool BufferManager::inPool(string pageName) {
    logger.log("BufferManager::inPool");
    for (auto page: this->pages) {
        if (pageName == page.pageName)
            return true;
    }
    return false;
}

/**
 * @brief If the page is present in the pool, then this function returns the
 * page. Note that this function will fail if the page is not present in the
 * pool.
 *
 * @param pageName 
 * @return Page 
 */
Page BufferManager::getFromPool(string pageName) {
    logger.log("BufferManager::getFromPool");
    for (auto page: this->pages)
        if (pageName == page.pageName)
            return page;
}

/**
 * @brief Inserts page indicated by tableName and pageIndex into pool. If the
 * pool is full, the pool ejects the oldest inserted page from the pool and adds
 * the current page at the end. It naturally follows a queue data structure. 
 *
 * @param tableName 
 * @param pageIndex 
 * @return Page 
 */
Page BufferManager::insertIntoPool(string tableName, int pageIndex) {
    logger.log("BufferManager::insertIntoPool");
    Page page(tableName, pageIndex);
    if (this->pages.size() >= BLOCK_COUNT) {
        if (this->pages.front().isDirty())
            this->pages.front().writePage();
        pages.pop_front();
    }
    pages.push_back(page);
    return page;
}

Page BufferManager::insertIntoPool(string tableName, int pageIndex, int columnCount) {
    logger.log("BufferManager::insertIntoPool");
    Page page(tableName, pageIndex, columnCount);
    if (this->pages.size() >= BLOCK_COUNT) {
        if (this->pages.front().isDirty())
            this->pages.front().writePage();
        pages.pop_front();
    }
    pages.push_back(page);
    return page;
}

Page BufferManager::insertIntoPool(Page &page) {
    logger.log("BufferManager::insertIntoPool");
    if (this->pages.size() >= BLOCK_COUNT) {
        if (this->pages.front().isDirty())
            this->pages.front().writePage();
        pages.pop_front();
    }
    pages.push_back(page);
    return page;
}

/**
 * @brief The buffer manager is also responsible for writing pages. This is
 * called when new tables are created using assignment statements.
 *
 * @param tableName 
 * @param pageIndex 
 * @param rows 
 * @param rowCount 
 */
void BufferManager::writePage(string tableName, int pageIndex, vector<vector<int>> rows, int rowCount) {
    logger.log("BufferManager::writePage");
    string pageName = "../data/temp/" + tableName + "_Page" + to_string(pageIndex);
    if (inPool(pageName)) {
        for (auto &page: pages) {
            if (page.pageName == pageName) {
                page.updatePage(rows);
            }
        }
    } else {
        Page page(tableName, pageIndex, rows, rowCount);
        page.writePage();
    }
    accessLogger.writes++;
}

/**
 * @brief Deletes file names fileName
 *
 * @param fileName 
 */
void BufferManager::deleteFile(string fileName) {

    if (remove(fileName.c_str()))
        logger.log("BufferManager::deleteFile: Err");
    else logger.log("BufferManager::deleteFile: Success");
}

/**
 * @brief Overloaded function that calls deleteFile(fileName) by constructing
 * the fileName from the tableName and pageIndex.
 *
 * @param tableName 
 * @param pageIndex 
 */
void BufferManager::deleteFile(string tableName, int pageIndex) {
    logger.log("BufferManager::deleteFile");
    string fileName = "../data/temp/" + tableName + "_Page" + to_string(pageIndex);
    this->deleteFile(fileName);
}

void BufferManager::renameFile(string oldName, string newName) {
    logger.log("BufferManager::renameFile");
    if (rename(oldName.c_str(), newName.c_str()))
        logger.log("BufferManager::renameFile: Err");
    else logger.log("BufferManager::renameFile: Success");

    if (this->inPool(oldName)) {
        for (auto page: this->pages) {
            if (oldName == page.pageName) {
                page.pageName = newName;
                break;
            }
        }
    }
}

void BufferManager::transposeMatrixPage(string tableName, int pageIndex, const Page page) {
    logger.log("BufferManager::transposeMatrixPage");
    string pageName = "../data/temp/" + tableName + "_Page" + to_string(pageIndex);
    if (!this->inPool(pageName)) {
        this->insertIntoPool(tableName, pageIndex);
    }
    for (auto &p: this->pages) {
        if (p.pageName == pageName) {
            p.tranposePage(page);
            accessLogger.writes++;
            break;
        }
    }
}

void
BufferManager::copyPage(std::string tableName, int pageIndex, std::string newTableName, int newPageIndex, bool write) {
    logger.log("BufferManager::copyPage");
    string pageName = "../data/temp/" + tableName + "_Page" + to_string(pageIndex);
    Page p = this->getPage(tableName, pageIndex);
    p.pageName = "../data/temp/" + newTableName + "_Page" + to_string(newPageIndex);
    if (write)
        p.writePage();
    insertIntoPool(p);
}

void BufferManager::sortPage(string tableName, int pageIndex, RowCmp cmp) {
    logger.log("BufferManager::sortPage");
    string pageName = "../data/temp/" + tableName + "_Page" + to_string(pageIndex);
    if (!this->inPool(pageName)) {
        this->insertIntoPool(tableName, pageIndex);
    }
    for (auto &p: this->pages) {
        if (p.pageName == pageName) {
            p.sortPage(cmp);
            accessLogger.writes++;
            break;
        }
    }
}
