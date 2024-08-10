#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <random>
#include <iomanip>
#include <fstream>
#include <argagg/argagg.hpp>

namespace fs = std::filesystem;

void print_help(const argagg::parser_results& args, const argagg::parser& parser) {
    std::cerr << "Usage: " << args[0] << " [options]\n";
    std::cerr << parser;
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
        print_help(args, argparser);
        return 1;
    }

    if (args["help"]) {
        print_help(args, argparser);
        return 0;
    }

    if (!args["in"]) {
        std::cerr << "Error: Input directory is required.\n";
        print_help(args, argparser);
        return 1;
    }


    return 0;
}
