/*-----------------------------------------------------------------------------
 * Class: Verbose.h
 * Author: Ricardo N. Guntur <ricardo.guntur@nyu.edu>
 * Course: Operating Systems -- Professor Allan Gottlieb
 -----------------------------------------------------------------------------*/
#ifndef LAB_3_TEST_VERBOSE_H
#define LAB_3_TEST_VERBOSE_H
#include <sstream>
#include <vector>
#include <iostream>
#include <string>

using namespace std;

//Debugging class
class verbose {

    static bool is_verbose;

public:

    static void setVerbosity(bool debug);

    void log(string message);

    void log(const char *message, int var);

    void log(const char *message, string str);

    bool getVerbosity();
};
#endif //LAB_3_TEST_VERBOSE_H
