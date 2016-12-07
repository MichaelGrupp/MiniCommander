// author: Michael Grupp
// https://github.com/MichaelGrupp/MiniCommander

#ifndef MINICMD
#define MINICMD

#include <set>
#include <map>
#include <vector>
#include <functional>
#include <iostream>


enum class Policy {
    required,
    anyOf,
    optional
};


class MiniCommander {
public:
    MiniCommander(const int &argc, char **argv) {
        for (int i = 1; i < argc; ++i)
            this->tokens.push_back(std::string(argv[i]));
    }

    void addOption(std::string flag, Policy policy, std::string desc = "") {
        this->options[policy].insert(std::make_pair(flag, desc));
    }

    bool checkFlags() {
        bool valid = false;
        for (auto& o : options[Policy::anyOf]) {
            valid = optionExists(o.first.c_str());
            if (valid)
	        break;
	    }
        if (!valid)
            return false;
        for (auto& o : options[Policy::required]) {
            valid = optionExists(o.first.c_str());
            if (!valid)
                break;
        }
        return valid;
    }

    void printHelpMessage(std::string title = "USAGE") {
        std::cerr << title << std::endl;
        std::cerr << "\n[required options, provide all]\n";
        for (auto& o : options[Policy::required])
            std::cerr << o.first << "\t" << o.second << std::endl;
        std::cerr << "\n[required options, provide any of them]\n";
        for (auto& o : options[Policy::anyOf])
            std::cerr << o.first << "\t" << o.second << std::endl;
        std::cerr << "\n[optional]\n";
        for (auto& o : options[Policy::optional])
            std::cerr << o.first << "\t" << o.second << std::endl;
        std::cerr << std::endl;
    }

    const std::string getParameter(const std::string& option) const {
        std::vector<std::string>::const_iterator itr;
        itr = std::find(this->tokens.begin(), this->tokens.end(), option);
        if (itr != this->tokens.end() && ++itr != this->tokens.end()) {
            // return parameter if it is not another option
            if (!isOption(*itr))
                return *itr;
        }
        return "";
    }

    bool optionExists(const std::string& option) const {
        return std::find(this->tokens.begin(), this->tokens.end(), option) != this->tokens.end();
    }

    bool isOption(const std::string& str) const {
        for (auto& policySet : options) {
            for (auto& desc : policySet.second){
                if (desc.first == str)
                    return true;
            }
        }
        return false;
    }

    // check whether any token satisfies the custom boolean functor or lambda 'f' which evaluates against 't'
    // of type T, e.g. a regex: f = [&](std::string token, std::regex re) { return ... }
    template<typename T>
    bool anyToken(const std::function<bool(std::string, T)>& f, T& t) const {
        for (auto& token: tokens) {
            if (f(token, t))
                return true;
        }
        return false;
    }
    template<typename T> // for rvalues
    bool anyToken(const std::function<bool(std::string, T)>& f, T&& t) const { return anyToken(f, t); };

    // check whether all tokens satisfy the custom boolean functor or lambda 'f'
    template<typename T>
    bool allTokens(const std::function<bool(std::string, T)>& f, T& t) const {
        for (auto& token: tokens) {
            if (!f(token, t))
                return false;
        }
        return true;
    }
    template<typename T> // for rvalues
    bool allTokens(const std::function<bool(std::string, T)>& f, T&& t) const { return allTokens(f, t); };

    // return the tokens which satisfy the boolean functor or lambda 'f'
    template<typename T>
    std::vector<std::string> whichTokens(const std::function<bool(std::string, T)>& f, T& t) const {
        std::vector<std::string> matches;
        for (auto& token: tokens) {
            if (f(token, t))
                matches.push_back(token);
        }
        return matches;
    }
    template<typename T> // for rvalues
    std::vector<std::string> whichTokens(const std::function<bool(std::string, T)>& f, T&& t) const {
        return whichTokens(f, t); };

private:
    std::vector<std::string> tokens;
    std::map<Policy, std::set<std::pair<std::string, std::string>>> options;
};

#endif  // MINICMD
