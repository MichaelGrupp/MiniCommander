# MiniCommander
A simple, minimalistic command line parser written in C++11

***When you can't or don't want to use Boost for parsing a few strings, try MiniCommander!***

* *add flags and their descriptions*
* *mark flags as required, optional or "any of"*
* *check if all flags are valid*
* *get parameters from flags*
* *pass your own custom checks (e.g. regular expressions) of the tokens via lambda functions*

## Example Usage 

```c++

#include <iostream>
#include "MiniCommander.hpp"

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
    
    std::string dataFolder = cmd.getParameter("-d");
    if (dataFolder.empty()) {
        std::cout << errMsg("error: please specify path to dataset") << std::endl;
        cmd.printHelpMessage();
        return EXIT_FAILURE;
    }
    
    std::string format;
    if (cmd.optionExists("-x") 
      format = "x";
    else
      format = "y";
    
    bool activate = cmd.optionExists("-a");
    
    return EXIT_SUCCESS;
}
```
