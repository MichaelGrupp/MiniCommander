# MiniCommander 

A simple, minimalistic but still powerful command line parser in less than 100 lines of C++11 code.

The library is header-only and only depends on the C++11 STL. This is not intended as a replacement for big libraries (e.g. boost::filesystem), rather as an option for quick tests and small projects.

***Features***
* *check if a flag exists*
* *get single parameter from flags*
* *get multiple parameters from flags*
* *organize flags in groups and mark them as required, optional or "any of"*
* *add flags to the groups, and optionally their alternative flag and their description*
* *automatically check if all flags are valid*
* *automatically print help/usage messages*

## Platforms
You will need a C++ compiler that supports C++11 regex, e.g. Clang >3.6 or GCC >4.9.
Automatic [unit tests](https://github.com/MichaelGrupp/MiniCommander/blob/master/test/unit_test.cpp) written with [Google Test](https://github.com/google/googletest) are performed on Linux (compilers: GCC 4.9 & Clang 3.6), as well as on Windows (compiler: MSVC14). Click the build badges for more details:

[![Build Status](https://travis-ci.org/MichaelGrupp/MiniCommander.svg?branch=master)](https://travis-ci.org/MichaelGrupp/MiniCommander)
[![Build status](https://ci.appveyor.com/api/projects/status/8ubu1kv85rcmiohv/branch/master?svg=true&passingText=Windows%3A%20build%20passing&failingText=Windows%3A%20build%20failing&pendingText=Windows%3A%20build%20pending)](https://ci.appveyor.com/project/MichaelGrupp/minicommander)

## Installation
Just drop the header file [MiniCommander.hpp](https://github.com/MichaelGrupp/MiniCommander/blob/master/MiniCommander.hpp) in your project and include it in your program.

## Supported Formats for Arguments

By default, flags can be name arbitrarily. Single parameters appear after a flag (here: `-d`). Multiple space-separated parameters are also possible (here after `-f`).
```
-x -y -z -d param -f param1 param2 param3
-x -y -z -d=param -f=param1 param2 param3
```
By setting the optional `unixFlags`parameter of the class constructor to `true`, multiple options can be combined if they're defined in single dash format (`-*`):
```
-xyz -d param
-xyzd=param
-xyzd param
```
which behaves the same as `-x -y -z -d=param`.

* check existence of a single flag `-x` with `optionExists("-x")`
* a parameter string can be accessed with `getParameter("-d")` 
* `getMultiParameters("-f")` returns the parameter string vector of a multiple parameter flag

If parameters were not given, the parameter string returned by `getParameter` (or vector by `getMultiParameters`) is empty.

## Grouping Options with Policies
The `checkFlags()` function automatically checks if all required flags were given by the user. Furthermore, automatic help messages can be generated with `printHelpMessage`. 

To use these features, the command line options must be first grouped by their check policy: 
* ***required*** - all options of this group must be given
* ***optional*** - giving these options is not mandatory
* ***anyOf*** - at least one option of the group must be given

Example of a required option group:
```c++
 OptionGroup requiredGroup(Policy::required, "required options");
 requiredGroup.addOption("-d", "first required argument");
 requiredGroup.addOption("-f", "other required argument", "--flag");
```
Note that an **optional long alternative** `--flag` of the `-f` flag was added. This group can now be added to a *MiniCommander* instance via `addOptionGroup`.

## Example Usage
This code example [test.cpp](https://github.com/MichaelGrupp/MiniCommander/blob/master/test/test.cpp) shows how to use all of the command line interface functions offered by MiniCommander:

```c++
#include <iostream>
#include "MiniCommander.hpp"

using namespace std;  // just for example

int main(int argc, char *argv[])
{
    bool unixFlags = false;  // whether to split single char options, e.g. -xyz into -x -y -z
    MiniCommander cmd(argc, argv, true);

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
