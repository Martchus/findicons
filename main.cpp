#include "resources/config.h"

#include <c++utilities/application/argumentparser.h>
#include <c++utilities/io/ansiescapecodes.h>
#include <c++utilities/io/catchiofailure.h>
#include <c++utilities/io/misc.h>

#include <filesystem>
#include <iostream>
#include <regex>
#include <set>
#include <string>
#include <vector>

using namespace std;
using namespace ApplicationUtilities;
using namespace EscapeCodes;
using namespace IoUtilities;

void addStringsFromFiles(bool enableDebugOutput, set<string> &to, const vector<filesystem::path> &files, const char *searchPattern, initializer_list<size_t> relevantCaptures) {
    const regex searchRegex(searchPattern);
    thread_local smatch match;
    for (const auto &file : files) {
        if (enableDebugOutput) {
            cerr << Phrases::Info << "Processing " << file << Phrases::End;
        }
        try {
            const auto fileContents = readFile(file.c_str());
            for(auto searchPosition = fileContents.cbegin();
                regex_search(searchPosition, fileContents.cend(), match, searchRegex);
                searchPosition = match.suffix().first) {
                for (const auto captureIndex : relevantCaptures) {
                    to.insert(match[captureIndex]);
                }
                if (enableDebugOutput) {
                    for (size_t i = 0, end = match.size(); i != end; ++i) {
                        cerr << i << ": " << match[i] << '\n';
                    }
                }
            }
        } catch (...) {
            const auto *ioError = catchIoFailure();
            cerr << Phrases::Warning << "Unable to read " << file << ": " << ioError << Phrases::End;
        }
    }
}

int main(int argc, char *argv[])
{
    // setup argument parser
    SET_APPLICATION_INFO;
    ArgumentParser parser;
    Argument projectDirsArg("project-dirs", 'd', "specifies the project directories to search for icons");
    projectDirsArg.setValueNames({ "path" });
    projectDirsArg.setRequiredValueCount(Argument::varValueCount);
    projectDirsArg.setConstraints(1, 1);
    projectDirsArg.setImplicit(true);
    ConfigValueArgument debugArg("debug", '\0', "enables debug output");
    HelpArgument helpArg(parser);
    parser.setMainArguments({ &projectDirsArg, &debugArg, &helpArg });

    // parse specified arguments
    parser.parseArgsOrExit(argc, argv);

    // find relevant files
    vector<filesystem::path> uiFiles, cppFiles, qmlFiles;
    for (const auto *projectDir : projectDirsArg.values()) {
        for(const auto &item: filesystem::recursive_directory_iterator(projectDir)) {
            if (!item.is_regular_file() && !item.is_symlink()) {
                continue;
            }

            auto path(item.path());
            const auto extension(path.extension());
            if (extension == ".ui") {
                uiFiles.emplace_back(path);
            } else if (extension == ".cpp" || extension == ".cxx") {
                cppFiles.emplace_back(path);
            } else if (extension == ".qml") {
                qmlFiles.emplace_back(path);
            }
        }
    }

    // search files for patterns
    set<string> iconNames;
    addStringsFromFiles(debugArg.isPresent(), iconNames, uiFiles,
                        "<iconset.*theme=\"([^\"]+)\".*>", { 1 });
    addStringsFromFiles(debugArg.isPresent(), iconNames, cppFiles,
                        "QIcon::fromTheme\\(QStringLiteral\\(\"([^\"]+)\"\\)\\)", { 1 });
    addStringsFromFiles(debugArg.isPresent(), iconNames, qmlFiles,
                        "(icon|icon\\.name|iconName|source)\\: (\"([^\"]+)\"|[^\\?]* \\? \"([^\"]+)\" : \"([^\"]+)\"|[^\\?]* \\? \\([^\\?]* \\? \"([^\"]+)\" : \"([^\"]+)\"\\) : \\([^\\?]* \\? \"([^\"]+)\" : \"([^\"]+)\"\\))", { 3, 4, 5, 6, 7, 8, 9 });

    // print results
    iconNames.erase(string());
    for (const auto &iconName : iconNames) {
        cout << iconName << '\n';
    }
}
