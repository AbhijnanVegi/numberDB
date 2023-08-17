#include "global.h"
/**
 * @brief 
 * SYNTAX: SOURCE filename
 */
bool syntacticParseSOURCE()
{
    logger.log("syntacticParseSOURCE");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = SOURCE;
    parsedQuery.sourceFileName = tokenizedQuery[1];
    return true;
}

bool semanticParseSOURCE()
{
    logger.log("semanticParseSOURCE");
    if (!isQueryFile(parsedQuery.sourceFileName))
    {
        cout << "SEMANTIC ERROR: File doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeSOURCE()
{
    logger.log("executeSOURCE");
    string file_path = "../data/" + parsedQuery.sourceFileName + ".ra";
    int file_descriptor = open(file_path.c_str(), O_RDONLY);
    if(file_descriptor == -1){
        std::cout << "EXECUTE SOURCE: Could not open file\n";
        perror("open");
        return;
    }
    struct stat file_info;
    if (fstat(file_descriptor, &file_info) == -1) {
        std::cout << "Failed to get file size.\n";
        close(file_descriptor);
        perror("fstat");
        return;
    }

    off_t file_size = file_info.st_size;
    char* file_data = (char*) mmap(NULL, file_size, PROT_READ, MAP_SHARED, file_descriptor, 0);
    if (file_data == MAP_FAILED) {
        std::cout << "Memory mapping failed." << std::endl;
        close(file_descriptor);
        perror("mmap");
        return;
    }

    regex delim("[^\\s,]+");
    for(int i=0; i < file_size; i++){
        int st = i;
        while(i < file_size and file_data[i] != '\n') i++;

        tokenizedQuery.clear();
        parsedQuery.clear();
        std::string command = std::string(file_data + st, i - st + 1);
        if(command.back() == '\0') command.back() = '\n';
        auto words_begin = std::sregex_iterator(command.begin(), command.end(), delim);
        auto words_end = std::sregex_iterator();
        for (std::sregex_iterator i = words_begin; i != words_end; ++i)
            tokenizedQuery.emplace_back((*i).str());
        if (tokenizedQuery.size() == 1 && tokenizedQuery.front() == "QUIT"){ break; }
        if (tokenizedQuery.empty()) { continue; }
        if (tokenizedQuery.size() == 1){
            std::cout << "SYNTAX ERROR" << std::endl;
            continue;
        }
        if (syntacticParse() && semanticParse())
        executeCommand();
    }

    munmap(file_data, file_size);
    close(file_descriptor);
    return;
}
