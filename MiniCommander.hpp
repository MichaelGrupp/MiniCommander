// author: Michael Grupp
// https://github.com/MichaelGrupp/MiniCommander

#ifndef MINICMD
#define MINICMD

#include <set>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

enum class Policy {
    required,
    anyOf,
    optional
};

struct OptionGroup {
    Policy policy;
    std::string groupDescription;
    std::map<std::string, std::string> options;
    OptionGroup(Policy p, std::string description) : policy(p), groupDescription(description) {}
    void addOption(std::string flag, std::string desc = "") {
        options[flag] = desc;
    }
};

class MiniCommander {
public:
    MiniCommander(const int argc, char const*const* argv) {
        for (int i = 1; i < argc; ++i) {
            std::string str = std::string(argv[i]);
            size_t equal_pos = str.find_first_of('=');
            if (equal_pos == std::string::npos)
                tokens.push_back(str);
            else {  // split argument with '='
                tokens.push_back(str.substr(0, equal_pos));
                tokens.push_back(str.substr(equal_pos+1));
            }
        }
    }

    void addOptionGroup(OptionGroup group) {
        optionGroups.push_back(group);
    }

    bool checkFlags() const {
        bool valid = true;
        for (auto& group : optionGroups) {
            for (auto& o : group.options) {
                valid = optionExists(o.first);
                if (group.policy == Policy::required && !valid)
                    break;
                else if (group.policy == Policy::anyOf && valid)
                    break;
                else if (group.policy == Policy::optional)
                    valid = true;  // don't care
            }
            if (!valid)
                break;
        }
        return valid;
    }

    void printHelpMessage(std::string title = "\nUSAGE") const {
        std::cerr << title << std::endl;
        for (auto& group : optionGroups) {
            std::cerr << "\n[" + group.groupDescription + "]\n";
            for (auto& o : group.options)
                std::cerr << o.first << "\t" << o.second << std::endl;
        }
    }

    const std::string getParameter(const std::string& option) const {
        auto itr = std::find(tokens.begin(), tokens.end(), option);
        return (itr != tokens.end() && ++itr != tokens.end() && !isOption(*itr)) ? *itr : "";
    }

    const std::vector<std::string> getMultiParameters(const std::string& option) const {
        std::vector<std::string> params;
        auto itr = std::find(tokens.begin(), tokens.end(), option);
        while (itr != tokens.end() && ++itr != tokens.end() && !isOption(*itr)) {
            params.push_back(*itr);
        }
        return params;
    }

    bool optionExists(const std::string& option) const {
        return std::find(tokens.begin(), tokens.end(), option) != tokens.end();
    }

private:
    bool isOption(const std::string& str) const {
        for (auto& group : optionGroups) {
            if (group.options.find(str) != group.options.end())
                return true;
        }
        return false;
    }

    std::vector<std::string> tokens;
    std::vector<OptionGroup> optionGroups;
};

#endif  // MINICMD