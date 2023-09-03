#include "accessLogger.h"

AccessLogger::AccessLogger() {
    this->reads = 0;
    this->writes = 0;
}

void AccessLogger::clear() {
    this->reads = 0;
    this->writes = 0;
}