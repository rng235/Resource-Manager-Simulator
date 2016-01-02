/*-----------------------------------------------------------------------------
 * Class: Verbose.cpp
 * Author: Ricardo N. Guntur <ricardo.guntur@nyu.edu>
 * Course: Operating Systems -- Professor Allan Gottlieb
 -----------------------------------------------------------------------------*/
#include "Verbose.h"

/*----------------------------------------------------------------------------------------------------------------------
 * Description: When verbose mode is activated, it will display all of the cycles involved
 * in a particular algorithm. Please use this only if you want to display the happenings between cycles.
 * This is especially useful when debugging the program.
 ---------------------------------------------------------------------------------------------------------------------*/
//Sets verbosity to true or false
void verbose::setVerbosity(bool userInput) {
    is_verbose = userInput;
}

//Gets verbosity
bool verbose::getVerbosity() {
    return is_verbose;
}

//Prints string message if verbosity is true
void verbose::log(string message) {
    if (is_verbose) {
        cout << "VERBOSE: " << message << endl;
    }
}

//Prints string message with int if verbosity is true
void verbose::log(const char *message, int var) {
    if (is_verbose) {
        cout << "VERBOSE: " << message << var << endl;
    }
}

//Prints string message with string if verbosity is true
void verbose::log(const char *message, string str) {
    if (is_verbose) {
        cout << "VERBOSE: " << message << endl;
        cout << "Current String: " << str << endl;
    }
}