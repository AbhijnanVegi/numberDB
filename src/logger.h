#include<iostream>
#include<bits/stdc++.h>
#include<sys/stat.h> 
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include<fstream>

using namespace std;

class Logger{

    string logFile = "log";
    ofstream fout;
    
    public:

    Logger();
    void log(string logString);
};

extern Logger logger;