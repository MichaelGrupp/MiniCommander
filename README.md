# MiniCommander
A simple, minimalistic but still powerful command line parser in less than 100 lines of C++11 code.

The library is header-only and only depends on the C++11 STL. This is not intended as a replacement for big libraries (e.g. <boost/filesystem>), rather as an option for quick tests and small projects.

***Features**

* *organize flags in groups*
* *mark groups as required, optional or "any of"*
* *add flags and their descriptions to the groups*
* *check if all flags are valid*
* *get parameters from flags*

## Example Usage 

This code snippet shows how to use the command line interface functions offered by MiniCommander.

```c++

#include <iostream>
#include "MiniCommander.hpp"

using namespace std;  // just for example

int main(int argc, char *argv[])
{
    MiniCommander cmd(argc, argv);

    OptionGroup paths(Policy::required, "required paths");
    paths.addOption("-d", "path to data folder");
    paths.addOption("-t", "path to test folder");
    cmd.addOptionGroup(paths);

    OptionGroup formats(Policy::anyOf, "formats, choose any of them");
    formats.addOption("-x", "use x format");
    formats.addOption("-y", "use y format");
    formats.addOption("-z", "use z format");
    cmd.addOptionGroup(formats);

    OptionGroup optionals(Policy::optional, "optional parameters");
    optionals.addOption("-a", "activate something");
    optionals.addOption("--help", "show info and usage");
    cmd.addOptionGroup(optionals);

    if (!cmd.checkFlags() || cmd.optionExists("--help")) {
        cmd.printHelpMessage();
        return EXIT_FAILURE;
    }

    string dataFolder = cmd.getParameter("-d");
    string testFolder = cmd.getParameter("-t");
    if (dataFolder.empty() || testFolder.empty()) {
        cerr << "error: please specify required paths" << endl;
        cmd.printHelpMessage();
        return EXIT_FAILURE;
    }

    if (cmd.optionExists("-x"))
        cout << "using x format!" << endl;
    else if (cmd.optionExists("-y"))
        cout << "using y format!" << endl;
    else if (cmd.optionExists("-z"))
        cout << "using z format!" << endl;

    if (cmd.optionExists("-a"))
        cout << "activating something optional!" << endl;

    return EXIT_SUCCESS;
}
```
