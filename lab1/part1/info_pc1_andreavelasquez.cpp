#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main(){
    // Get cpu name
    ifstream cpuinfo("/proc/cpuinfo");
    string line;
    while (getline(cpuinfo, line, '\n')){
       if (line.find("model name")!=string::npos){
            cout << "a. " << line << endl;
       }
    }

    // Get installed memory
    ifstream meminfo("/proc/meminfo");
    getline(meminfo, line, '\n');
    cout << "b." << line << endl;

    // Get kernel version
    ifstream versionfile("/proc/version");
    getline(versionfile, line, '\n');
    cout << "c. " << line.substr(0, line.find(" (")) << endl;

    // Get last time system initiated
    ifstream uptime("/proc/uptime");
    getline(uptime, line, '\n');
    cout << "d. " << line << endl;


    return EXIT_SUCCESS;
}