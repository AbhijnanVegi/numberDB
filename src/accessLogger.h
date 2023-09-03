#ifndef DS_ACCESSLOGGER_H
#define DS_ACCESSLOGGER_H

#endif //DS_ACCESSLOGGER_H

using namespace std;

class AccessLogger
{
public:
    int reads = 0, writes = 0;

    AccessLogger();
    void clear();
};