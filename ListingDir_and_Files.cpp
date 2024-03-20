//
// Created by Łukasz on 27.02.2024.
//

#include <iostream>
#include <vector>
#include <future>
#include <filesystem>
#include <stdexcept>
#include <algorithm>

typedef std::vector<std::string> StringVector;

typedef std::pair<std::string, std::filesystem::file_status> StringStatusPair;
typedef std::vector<StringStatusPair> StringStatusVector;

StringStatusVector getDirectoryContentsWithStatus(const std::string& directory) {
    StringStatusVector contents;

    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        contents.emplace_back(entry.path().generic_string(), entry.status());
    }

    return contents;
}

StringVector listDirectoryContents(std::string &&directory) {
    StringVector filenames;

    try {
        StringStatusVector contentsWithStatus = getDirectoryContentsWithStatus(directory);

        // Sort the directory contents based on file name
        std::sort(contentsWithStatus.begin(), contentsWithStatus.end(), [](const auto& a, const auto& b) {
            return a.first < b.first;
        });

        for (const auto& [filename, status] : contentsWithStatus) {
            if (std::filesystem::is_directory(status)) {
                auto ftr = std::async(std::launch::async, &listDirectoryContents, filename);
                StringVector subdirectoryContents = ftr.get();
                filenames.insert(filenames.end(), subdirectoryContents.begin(), subdirectoryContents.end());
            } else {
                filenames.push_back(filename);
            }
        }
    }
    catch (const std::exception& ex) {
        throw std::runtime_error("Error while listing directory contents: " + std::string(ex.what()));
    }
    catch (...) {
        throw std::runtime_error("Unknown error occurred while listing directory contents.");
    }

    return filenames;
}

#ifdef _WIN32 // Windows specific code
#include <Windows.h>

void showPopupMessage(const std::string& message) {
    MessageBox(NULL, message.c_str(), "Information", MB_OK | MB_ICONINFORMATION);
}
#else // Unix based systems
#include <ncurses.h>

void showPopupMessage(const std::string& message) {
    initscr(); // Initialize the ncurses screen
    printw("%s\n", message.c_str()); // Print the message
    refresh(); // Refresh the screen
    getch(); // Wait for user input
    endwin(); // End the ncurses session
}
#endif

int main() {
    std::string rootDirectory = "D:\\IdeaProjects";  //change directory !!!! !!!! !!!!
    showPopupMessage("Please ensure you have set the correct root directory before running the program.");
    auto futureContents = std::async(std::launch::async, &listDirectoryContents, rootDirectory);

    try {
        StringVector directoryContents = futureContents.get();
        std::for_each(directoryContents.begin(), directoryContents.end(), [](const std::string& name) {
            std::cout << name;
        });
    }
    catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown exception occurred." << std::endl;
    }

    return 0;
}

