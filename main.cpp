/*-----------------------------------------------------------------------------
 * Class: main.cpp
 * Author: Ricardo N. Guntur <ricardo.guntur@nyu.edu>
 * Course: Operating Systems -- Professor Allan Gottlieb
 -----------------------------------------------------------------------------*/
#include "ResourceManager.h"
#include "Verbose.h"
#include <iostream>
#include <fstream>

using namespace std;

//Global Constants
const string VERBOSE = "--verbose";
bool verbose::is_verbose = false;

/*-----------------------------------------------------------------------------
 * There are 2 possible invocations
 * <program-name> <input-filename>
 * <program-name> --verbose <input-filename>
 -----------------------------------------------------------------------------*/
int main(int argc, char *argv[]) {
    ResourceManager *manager;
    verbose verbose_methods;
    ifstream input_stream;

    // IF one argument is passed in then there was no verbose flag
    if (argc == 2) {

        cout << "Input File: " << argv[1] << endl << endl;
        manager = new ResourceManager(argv[1]);
    }
    //With Verbose invocation
    else if (argc == 3) {
        if (argv[1] == VERBOSE) {

            //set verbosity to true
            verbose_methods.setVerbosity(true);
            cout << "VERBOSE MODE ON" << endl;
            cout << "Input File: " << argv[2] << endl << endl;
            manager = new ResourceManager(argv[2]);
        }
    }
    else {
        // Print correct format to run program
        cout << "Usage: <program-name> <input_file> or \n"
                "<program-name> --verbose <input_file>\n";
    }
}
