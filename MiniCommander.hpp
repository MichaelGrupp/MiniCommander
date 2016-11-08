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

    bool checkFlags() const {
        bool valid = false;
        for (auto &o : options[Policy::anyOf])
            valid = optionExists(o.first.c_str());
        if (!valid)
            return valid;
        for (auto &o : options[Policy::required]) {
            valid = optionExists(o.first.c_str());
            if (!valid)
                break;
        }
        return valid;
    }

    void printHelpMessage() const {
        std::cout << errMsg("\nUSAGE") << std::endl;
        std::cout << "\n[required options, provide all]\n";
        for (auto &o : options[Policy::required])
            std::cout << o.first << "\t" << o.second << std::endl;
        std::cout << "\n[required options, provide any of them]\n";
        for (auto &o : options[Policy::anyOf])
            std::cout << o.first << "\t" << o.second << std::endl;
        std::cout << "\n[optional]\n";
        for (auto &o : options[Policy::optional])
            std::cout << o.first << "\t" << o.second << std::endl;
        std::cout << std::endl;
    }

    const std::string getParameter(const std::string &option) const {
        std::vector<std::string>::const_iterator itr;
        itr = std::find(this->tokens.begin(), this->tokens.end(), option);
        if (itr != this->tokens.end() && ++itr != this->tokens.end()) {
            return *itr;
        }
        return "";
    }

    bool optionExists(const std::string &option) const {
        return std::find(this->tokens.begin(), this->tokens.end(), option) != this->tokens.end();
    }

    // check whether any token satisfies the custom boolean functor or lambda 'f' which evaluates against 't'
    // of type T, e.g. a regex: f = [&](std::string token, std::regex re) { return ... }
    template<typename T>
    bool anyToken(const std::function<bool(std::string, T)> f, T&& t) const {
        bool satisfied = false;
        for (auto &token: tokens) {
            satisfied = f(token, std::forward<T>(t));
            if (satisfied)
                break;
        }
        return satisfied;
    }

private:
    std::vector<std::string> tokens;
    std::map<Policy, std::set<std::pair<std::string, std::string>>> options;
};
