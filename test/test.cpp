#include <iostream>
#include "MiniCommander.hpp"

using namespace std;  // just for example

int main(int argc, char *argv[])
{
    bool unixFlags = false;  // whether to split -xyz into -x -y -z
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
    if (cmd.optionExists("-y"))
        cout << "\nusing y format!" << endl;
    if (cmd.optionExists("-z"))
        cout << "\nusing z format!" << endl;

    if (cmd.optionExists("-a"))
        cout << "activating something optional!" << endl;

    return EXIT_SUCCESS;
}