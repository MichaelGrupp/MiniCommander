# MiniCommander
A simple, minimalistic but still powerful command line parser written in C++11 for quick tests and small projects. 

The library is header-only and only depends on the C++11 STL.

***When you can't or don't want to use Boost for parsing a few strings, try MiniCommander!***

* *add flags and their descriptions*
* *mark flags as required, optional or "any of"*
* *check if all flags are valid*
* *get parameters from flags*
* *pass your own custom checks (e.g. regular expressions) of the tokens via lambda functions*

## Example Usage 

This code snippet shows how to use the command line interface functions offered by MiniCommander. As an example for the advanced functions, we pass a lambda function with a regular expression check into the MiniCommander object.

```c++

#include <iostream>
#include <regex>
#include "MiniCommander.hpp"

using namespace std;  // just for example

int main(int argc, char *argv[])
{
    MiniCommander cmd(argc, argv);
    cmd.addOption("-d", Policy::required, "path to data folder");
    cmd.addOption("-x", Policy::anyOf, "use x format");
    cmd.addOption("-y", Policy::anyOf, "use y format");
    cmd.addOption("-a", Policy::optional, "activate something");

    if (!cmd.checkFlags()) {
        cmd.printHelpMessage();
        return EXIT_FAILURE;
    }

    string dataFolder = cmd.getParameter("-d");
    if (dataFolder.empty()) {
        cerr << "error: please specify path to dataset" << endl;
        cmd.printHelpMessage();
        return EXIT_FAILURE;
    }

    if (cmd.optionExists("-x"))
        cout << "using x format!" << endl;
    else
        cout << "using y format!" << endl;
    if (cmd.optionExists("-a"))
        cout << "activating something optional!" << endl;

    // advanced stuff: inject custom token checks via a lambda function, e.g. with regex
    auto matchingRegex = [&](const string& token, const regex r) -> bool { return regex_match(token, r); };
    string re_str = ".*|.*\\/";
    regex re(re_str);

    if (cmd.anyToken<regex>(matchingRegex, re))
        cout << "some token matches regex: " << re_str << endl;
    vector<string> matches = cmd.whichTokens<regex>(matchingRegex, re);
    if (!matches.empty()) {
        cout << "the matching tokens are:" << endl;
        for (auto& m : matches)
            cout << m << endl;
    }
    if (cmd.allTokens<regex>(matchingRegex, re))
        cout << "all tokens match regex: " << re_str << endl;

    return EXIT_SUCCESS;
}
```

If we call this program (minicmd_test) without the required options: `./minicmd_test`, the following output is generated:
```
USAGE

[required options, provide all]
-d	path to data folder

[required options, provide any of them]
-x	use x format
-y	use y format

[optional]
-a	activate something
```

If it's called correctly: `./minicmd_test -d my/data/folder -x -a`, we get this output:
```
using x format!
activating something optional!
some token matches regex: .*|.*\/
the matching tokens are:
-d
my/data/folder
-x
-a
all tokens match regex: .*|.*\/
```
