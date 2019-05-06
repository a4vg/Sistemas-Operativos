#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <unistd.h>

using namespace std;

float round_to(float number, int decimal){
    return round(number*100 *pow(10, decimal))/pow(10, decimal);
}

int main(){
    int befctxt=0, befproc=0;
    while (true){
        // Available memory (%)
        ifstream meminfo("/proc/meminfo");
        int memtotal, memava;
        meminfo.ignore(256, ' ');
        meminfo >> memtotal;
        meminfo.ignore(256, '\n');
        meminfo.ignore(256, '\n');
        meminfo.ignore(256, ' ');
        meminfo >> memava;
        cout << "\n\nAvailable memory: " <<  round_to((float) memava/memtotal,2) << "%";

        // Time spent in user mode (1), system mode (3) and idle (4) (%)
        // ex: cpu  711 0 606 136041 199 0 25 0 0 0
        ifstream statfile("/proc/stat");
        int timeuser, timesystem, timeidle, totaltime=0, temp;
        statfile.ignore(256, ' ');

        for (int i=0; i<7; ++i){
            switch(i){
                case 0: statfile >> timeuser; break;
                case 2: statfile >> timesystem; break;
                case 3: statfile >> timeidle; break;
                default: statfile >> temp;
            }
            totaltime+=temp;
        }
        totaltime+=timeuser + timesystem + timeidle;
        cout << "\nTime in user mode (%): " << round_to((float) timeuser/totaltime, 2) << "%";
        cout << "\nTime in system mode (%): " << round_to((float) timesystem/totaltime,2) << "%";
        cout << "\nTime in idle mode (%): " << round_to((float)timeidle/totaltime,2) << "%";

        // Context switches per second
        int ctxt, process;
        for (int i=0; i<5 ;++i) statfile.ignore(256, '\n');
        statfile.ignore(256, ' ');
        statfile >> ctxt;
        cout << "\nContext switches per second: " << ctxt-befctxt;
        befctxt = ctxt;

        // Process created per second
        statfile.ignore(256, '\n');
        statfile.ignore(256, '\n');
        statfile.ignore(256, ' ');
        statfile >> process;
        cout << "\nProcess created per second: \n" << process-befproc;
        befproc=process;

        

        sleep(1);
    }

    return EXIT_SUCCESS;
}