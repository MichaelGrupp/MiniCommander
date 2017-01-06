// unit tests for MiniCommander.hpp using Google Test
// author: Michael Grupp

#include <array>
#include <chrono>
#include <regex>
#include <iostream>

#include <gtest/gtest.h>

#include "MiniCommander.hpp"

using namespace std;


class MiniCommanderTest : public ::testing::Test {
protected:
    void SetUp(int argc, char const* const* argv, bool unixFlags=false) {
        mc = new MiniCommander(argc, argv, unixFlags);
    }
    void TearDown() {
        delete mc;
    }
    MiniCommander* mc;
};

vector<OptionGroup> makeTestOptionGroups() {
    OptionGroup required(Policy::required, "required parameters");
    required.addOption("-d", "path to a dataset folder");
    required.addOption("-f", "path to one or multiple files");
    OptionGroup switches(Policy::anyOf, "switches, choose one");
    switches.addOption("--do_this");
    switches.addOption("--do_that");
    OptionGroup formats(Policy::anyOf, "formats, choose one or more");
    formats.addOption("-x", "use x format");
    formats.addOption("-y", "use y format");
    formats.addOption("-z", "use z format");
    OptionGroup optionals(Policy::optional, "optional parameters");
    optionals.addOption("-a", "activate something");
    optionals.addOption("--help", "show info and usage");
    std::vector<OptionGroup> optionGroups = {required, switches, formats, optionals};
    return optionGroups;
}

// quick hack for printing args
std::string print(int argc, char const*const* argv) {
    std::string str = "";
    for(int i=0; i<argc; i++) {
        str.append(argv[i]);
        str.append(" ");
    }
    return str;
}

TEST_F(MiniCommanderTest, testCheckFlags_correct) {
    // passing correct flags for flags from makeTestOptionGroups
    const int argc = 7;
    array<const char*, argc> argv_std = {"appname", "-d", "/data/dataset", "-f", "file1.txt", "-x", "--do_that"};
    char const* const* argv = (char const* const*)argv_std.data();
    SetUp(argc, argv);
    auto optionGroups = makeTestOptionGroups();
    for (auto& g : optionGroups)
        mc->addOptionGroup(g);
    EXPECT_EQ(mc->checkFlags(), true) << "failed with argv: " << print(argc, argv);
}

TEST_F(MiniCommanderTest, testCheckFlags_wrong) {
    // missing flags for flags from makeTestOptionGroups
    const int argc = 6;
    array<const char*, argc> argv_std = {"appname", "-d", "/data/dataset", "-f", "file1.txt", "-x"};
    char const* const* argv = (char const* const*)argv_std.data();
    SetUp(argc, argv);
    auto optionGroups = makeTestOptionGroups();
    for (auto& g : optionGroups)
        mc->addOptionGroup(g);
    EXPECT_EQ(mc->checkFlags(), false) << "failed with argv: " << print(argc, argv);
}

TEST_F(MiniCommanderTest, testParameter) {
    // test if a parameter can be passed after a flag
    const int argc = 7;
    array<const char*, argc> argv_std = {"appname", "-d", "/data/dataset", "-f", "file1.txt", "-y", "--do_this"};
    char const* const* argv = (char const* const*)argv_std.data();
    SetUp(argc, argv);
    auto optionGroups = makeTestOptionGroups();
    for (auto& g : optionGroups)
        mc->addOptionGroup(g);
    EXPECT_EQ(mc->checkFlags(), true) << "failed with argv: " << print(argc, argv);
    string param = mc->getParameter("-d").c_str();
    ASSERT_STREQ(param.c_str(), "/data/dataset");
}

TEST_F(MiniCommanderTest, testMissingParameter) {
    // test if an empty string is returned when a parameter is not given
    const int argc = 6;
    array<const char*, argc> argv_std = {"appname", "-d", "-f", "file1.txt", "-y", "--do_this"};
    char const* const* argv = (char const* const*)argv_std.data();
    SetUp(argc, argv);
    auto optionGroups = makeTestOptionGroups();
    for (auto& g : optionGroups)
        mc->addOptionGroup(g);
    EXPECT_EQ(mc->checkFlags(), true) << "failed with argv: " << print(argc, argv);
    string param = mc->getParameter("-d").c_str();
    ASSERT_STREQ(param.c_str(), "");
}

TEST_F(MiniCommanderTest, testEqualParameter) {
    // test if a parameter can be passed directly after a flag with an equal sign
    const int argc = 6;
    array<const char*, argc> argv_std = {"appname", "-d=/data/dataset", "-f", "file1.txt", "-z", "--do_this"};
    char const* const* argv = (char const* const*)argv_std.data();
    SetUp(argc, argv);
    auto optionGroups = makeTestOptionGroups();
    for (auto& g : optionGroups)
        mc->addOptionGroup(g);
    EXPECT_EQ(mc->checkFlags(), true) << "failed with argv: " << print(argc, argv);
    string param = mc->getParameter("-d");
    ASSERT_STREQ(param.c_str(), "/data/dataset");
}

TEST_F(MiniCommanderTest, testMissingEqualParameter) {
    // test if an empty string is returned when a parameter is not given
    const int argc = 6;
    array<const char*, argc> argv_std = {"appname", "-d=", "-f", "file1.txt", "-y", "--do_this"};
    char const* const* argv = (char const* const*)argv_std.data();
    SetUp(argc, argv);
    auto optionGroups = makeTestOptionGroups();
    for (auto& g : optionGroups)
        mc->addOptionGroup(g);
    EXPECT_EQ(mc->checkFlags(), true) << "failed with argv: " << print(argc, argv);
    string param = mc->getParameter("-d");
    ASSERT_STREQ(param.c_str(), "");
}

TEST_F(MiniCommanderTest, testMultiParameters) {
    // test if multiple parameters are handled correctly
    const int argc = 8;
    array<const char*, argc> argv_std = {"appname", "-d=/data/dataset",
                                         "-f", "first.txt", "second.txt", "third.txt",
                                         "-y", "--do_this"};
    char const* const* argv = (char const* const*)argv_std.data();
    SetUp(argc, argv);
    auto optionGroups = makeTestOptionGroups();
    for (auto& g : optionGroups)
        mc->addOptionGroup(g);
    EXPECT_EQ(mc->checkFlags(), true) << "failed with argv: " << print(argc, argv);
    string param = mc->getParameter("-d");
    ASSERT_STREQ(param.c_str(), "/data/dataset");
    vector<string> params = mc->getMultiParameters("-f");
    ASSERT_TRUE(params.size() == 3) << "actual size is: " << param.size() << "\nfailed with argv: " << print(argc, argv);
    ASSERT_STREQ(params[0].c_str(), "first.txt") << "failed with argv: " << print(argc, argv);
    ASSERT_STREQ(params[1].c_str(), "second.txt") << "failed with argv: " << print(argc, argv);
    ASSERT_STREQ(params[2].c_str(), "third.txt") << "failed with argv: " << print(argc, argv);
}

TEST_F(MiniCommanderTest, testMissingMultiParameters) {
    // test if missing multiple parameters result in empty vector
    const int argc = 5;
    array<const char*, argc> argv_std = {"appname", "-d=/data/dataset", "-f", "-y", "--do_this"};
    char const* const* argv = (char const* const*)argv_std.data();
    SetUp(argc, argv);
    auto optionGroups = makeTestOptionGroups();
    for (auto& g : optionGroups)
        mc->addOptionGroup(g);
    EXPECT_EQ(mc->checkFlags(), true) << "failed with argv: " << print(argc, argv);
    string param = mc->getParameter("-d");
    ASSERT_STREQ(param.c_str(), "/data/dataset");
    vector<string> params = mc->getMultiParameters("-f");
    ASSERT_TRUE(params.empty()) << "actual size is: " << param.size() << "\nfailed with argv: " << print(argc, argv);
}

TEST_F(MiniCommanderTest, testOptionExists) {
    // test if optionExists runs properly
    const int argc = 7;
    array<const char*, argc> argv_std = {"appname", "-d=/data/dataset", "-f", "file1.txt", "-y", "--do_this", "--help"};
    char const* const* argv = (char const* const*)argv_std.data();
    SetUp(argc, argv);
    auto optionGroups = makeTestOptionGroups();
    for (auto& g : optionGroups)
        mc->addOptionGroup(g);
    EXPECT_EQ(mc->checkFlags(), true) << "failed with argv: " << print(argc, argv);
    EXPECT_TRUE(mc->optionExists("--help")) << "failed with argv: " << print(argc, argv);
    EXPECT_FALSE(mc->optionExists("-a")) << "failed with argv: " << print(argc, argv);
}

TEST_F(MiniCommanderTest, testNoArgs) {
    // test what happens when no option groups are added
    const int argc = 1;
    array<const char*, argc> argv_std = {"appname"};
    char const* const* argv = (char const* const*)argv_std.data();
    SetUp(argc, argv);
    EXPECT_EQ(mc->checkFlags(), true) << "failed with argv: " << print(argc, argv);
    string param = mc->getParameter("-d");
    ASSERT_STREQ(param.c_str(), "");
    vector<string> params = mc->getMultiParameters("-f");
    ASSERT_TRUE(params.empty()) << "actual size is: " << param.size() << "\nfailed with argv: " << print(argc, argv);
    EXPECT_FALSE(mc->optionExists("--help")) << "failed with argv: " << print(argc, argv);
    EXPECT_FALSE(mc->optionExists("-a")) << "failed with argv: " << print(argc, argv);
}

TEST_F(MiniCommanderTest, testUnixOptionsCorrect) {
    // test Unix option behaviour, i.e. -xyz should be parsed like -x -y -z
    const int argc = 6;
    array<const char*, argc> argv_std = {"appname", "-d=/data/dataset", "-f", "file1.txt", "-xyz", "--do_this"};
    char const* const* argv = (char const* const*)argv_std.data();
    bool unixFlags = true;
    SetUp(argc, argv, unixFlags);
    auto optionGroups = makeTestOptionGroups();
    for (auto& g : optionGroups)
        mc->addOptionGroup(g);
    EXPECT_EQ(mc->checkFlags(), true) << "Unix option check failed with argv: " << print(argc, argv);
    string param = mc->getParameter("-d");
    ASSERT_STREQ(param.c_str(), "/data/dataset");
    vector<string> params = mc->getMultiParameters("-f");
    ASSERT_STREQ(params[0].c_str(), "file1.txt");
    ASSERT_TRUE(mc->optionExists("-x"));
    ASSERT_TRUE(mc->optionExists("-y"));
    ASSERT_TRUE(mc->optionExists("-z"));
}

TEST_F(MiniCommanderTest, testUnixOptionsWrong) {
    // test wrong Unix option behaviour, i.e. --xyz should NOT be parsed like -x -y -z
    const int argc = 6;
    array<const char*, argc> argv_std = {"appname", "-d=/data/dataset", "-f", "file1.txt", "--xyz", "--do_thisdo_that"};
    char const* const* argv = (char const* const*)argv_std.data();
    bool unixFlags = true;
    SetUp(argc, argv, unixFlags);
    auto optionGroups = makeTestOptionGroups();
    for (auto& g : optionGroups)
        mc->addOptionGroup(g);
    EXPECT_EQ(mc->checkFlags(), false) << "Unix option check failed with argv: " << print(argc, argv);
    string param = mc->getParameter("-d");
    ASSERT_STREQ(param.c_str(), "/data/dataset");
    vector<string> params = mc->getMultiParameters("-f");
    ASSERT_STREQ(params[0].c_str(), "file1.txt");
    ASSERT_FALSE(mc->optionExists("-x"));
    ASSERT_FALSE(mc->optionExists("-y"));
    ASSERT_FALSE(mc->optionExists("-z"));
    ASSERT_FALSE(mc->optionExists("--do_this"));
    ASSERT_FALSE(mc->optionExists("--do_that"));
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}