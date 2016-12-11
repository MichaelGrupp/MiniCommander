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