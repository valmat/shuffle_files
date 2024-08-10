#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <random>
#include <iomanip>
#include <fstream>
#include <argagg/argagg.hpp>

namespace fs = std::filesystem;

void print_help(const char* prog_name, const argagg::parser& argparser) {
    std::cerr << "Usage:\n" << prog_name << " [options]\n";
    std::cerr << argparser;
}

int main(int argc, char* argv[]) {
    argagg::parser argparser {{
        { "help", {"-h", "--help"}, "shows this help message", 0 },
        { "in",   {"-i", "--in", "--dir"}, "input directory", 1 },
        { "out",  {"-o", "--out"}, "output directory (optional, defaults to input directory)", 1 },
        { "cp",   {"--cp"}, "copy files (default behavior)", 0 },
        { "mv",   {"--mv"}, "move files instead of copying", 0 },
        { "ext",  {"-e", "--ext"}, "file extension to filter (e.g. --ext \".mp3\")", 1 }
    }};

    argagg::parser_results args;
    try {
        args = argparser.parse(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        print_help(argv[0], argparser);
        return 1;
    }

    if (args["help"]) {
        print_help(argv[0], argparser);
        return 0;
    }

    if (!args["in"]) {
        std::cerr << "Error: Input directory is required.\n";
        print_help(argv[0], argparser);
        return 1;
    }

    fs::path input_dir = args["in"].as<std::string>();
    fs::path output_dir = args["out"] ? fs::path{args["out"].as<std::string>()} : input_dir;
    bool move_files = args["mv"];
    std::string ext_filter = args["ext"] ? args["ext"].as<std::string>() : "";

    std::vector<fs::path> files;

    // Read files from input directory
    for (const auto& entry : fs::recursive_directory_iterator(input_dir)) {
        if (entry.is_regular_file()) {
            if (ext_filter.empty() || entry.path().extension() == ext_filter) {
                files.push_back(entry.path());
            }
        }
    }

    for(const auto& file: files) {
        std::cout << file << std::endl;
    }

    // Shuffle files
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(files.begin(), files.end(), g);

    // Determine the number of digits for file numbering
    size_t num_files = files.size();
    int num_digits = std::to_string(num_files).length();


    std::cout << "\n-----------------------\n" << std::endl;
    for(const auto& file: files) {
        std::cout << file << std::endl;
    }

    std::cout << "\n-----------------------\n" << std::endl;
    // Copy or move files to output directory with new names
    for (size_t i = 0; i < num_files; ++i) {
        fs::path new_filename = output_dir / (std::to_string(i + 1).insert(0, num_digits - std::to_string(i + 1).length(), '0') + files[i].extension().string());


        std::cout << new_filename << std::endl;

        if (move_files) {
            fs::rename(files[i], new_filename);
        } else {
            fs::copy(files[i], new_filename);
        }
    }

    return 0;
}
