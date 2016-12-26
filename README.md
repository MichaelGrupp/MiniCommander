# MiniCommander
A simple, minimalistic but still powerful command line parser in less than 100 lines of C++11 code.

The library is header-only and only depends on the C++11 STL. This is not intended as a replacement for big libraries (e.g. boost::filesystem), rather as an option for quick tests and small projects.

***Features***

* *organize flags in groups*
* *mark groups as required, optional or "any of"*
* *add flags and their descriptions to the groups*
* *check if all flags are valid*
* *get single parameter from flags*
* *get multiple parameters from flags*

## Installation
Just drop the header file [MiniCommander.hpp](https://github.com/MichaelGrupp/MiniCommander/blob/master/MiniCommander.hpp) in your project and include it in your program.

## Example Usage
This code example [test.cpp](https://github.com/MichaelGrupp/MiniCommander/blob/master/test/test.cpp) shows how to use the command line interface functions offered by MiniCommander:

```c++
#include <iostream>
#include "MiniCommander.hpp"

using namespace std;  // just for example

int main(int argc, char *argv[])
{
    MiniCommander cmd(argc, argv);

    OptionGroup paths(Policy::required, "required paths");
    paths.addOption("-d", "path to data folder");
    paths.addOption("-f", "paths of one or multiple files (separated by space)");
    cmd.addOptionGroup(paths);

    OptionGroup formats(Policy::anyOf, "formats, choose one of them");
    formats.addOption("-x", "use x format");
    formats.addOption("-y", "use y format");
    formats.addOption("-z", "use z format");
    cmd.addOptionGroup(formats);

    OptionGroup optionals(Policy::optional, "optional parameters");
    optionals.addOption("-a", "activate something");
    optionals.addOption("--help", "show info and usage");
    cmd.addOptionGroup(optionals);

    if (!cmd.checkFlags() || cmd.optionExists("--help")) {
        cmd.printHelpMessage("MiniCommander Example\n\nUSAGE:");
        return EXIT_FAILURE;
    }

    string dataFolder = cmd.getParameter("-d");
    vector<string> filePaths = cmd.getMultiParameters("-f");
    if (dataFolder.empty() || filePaths.empty()) {
        cerr << "error: please specify required paths" << endl;
        cmd.printHelpMessage();
        return EXIT_FAILURE;
    }
    cout << "data folder: " << dataFolder << endl;
    cout << "file paths: ";
    for (auto& path : filePaths)
        cout << path << ", ";

    if (cmd.optionExists("-x"))
        cout << "\nusing x format!" << endl;
    else if (cmd.optionExists("-y"))
        cout << "\nusing y format!" << endl;
    else if (cmd.optionExists("-z"))
        cout << "\nusing z format!" << endl;

    if (cmd.optionExists("-a"))
        cout << "activating something optional!" << endl;

    return EXIT_SUCCESS;
}
```
If [test.cpp](https://github.com/MichaelGrupp/MiniCommander/blob/master/test/test.cpp) is compiled and called with the wrong parameters: 

`./test -d ~/Documents/data -f /Documents/xyz.txt /Documents/abc.psd /Documents/123.bin -a`,

(here, none of the options from the ```formats``` group is given and ```checkFlags()``` fails) we get the following output:
```
MiniCommander Example

USAGE:

[required paths]
-d	path to data folder
-f	paths of one or multiple files (separated by space)

[formats, choose one of them]
-x	use x format
-y	use y format
-z	use z format

[optional parameters]
--help	show info and usage
-a	activate something
```

With correct parameters, e.g.: 

`./test -x -d ~/Documents/data -f /Documents/xyz.txt /Documents/abc.psd /Documents/123.bin -a`, 

we get:
```
data folder: /Documents/data
file paths: /Documents/xyz.txt, /Documents/abc.psd, /Documents/123.bin, 
using x format!
activating something optional!
```
