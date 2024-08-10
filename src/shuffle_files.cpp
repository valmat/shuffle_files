#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <random>
#include <iomanip>
#include <sstream>
#include <argagg/argagg.hpp>

namespace fs = std::filesystem;

void cpormv_show(const fs::path& from, const fs::path& to) noexcept
{
    std::cout << '[' << to << "] <- [" << from << ']' << std::endl;
}

void do_nothing(const fs::path&, const fs::path&) noexcept {}

void print_help(const char* prog_name, const argagg::parser& argparser)
{
    std::cerr << "Usage:\n" << prog_name << " [options]\n";
    std::cerr << argparser;
}

std::string generate_new_filename(size_t index, int num_digits, const std::string& extension, const std::string& postfix = "")
{
    std::ostringstream oss;
    oss << std::setw(num_digits) << std::setfill('0') << (index + 1) << extension << postfix;
    return oss.str();
}

std::string generate_postfix() noexcept
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, 35);
    std::string hash;
    hash.reserve(10);
    hash = ".";
    for (int i = 0; i < 5; ++i) {
        hash += "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"[dis(gen)];
    }
    return hash + ".tmp";
}
std::string remove_postfix(const std::string& file) noexcept
{
    return file.substr(0, file.length() - 10);
}

int main(int argc, char* argv[]) {
    argagg::parser argparser {{
        { "help", {"-h", "--help"}, "Shows this help message", 0 },
        { "in",   {"-i", "--in", "--dir"}, "Input directory", 1 },
        { "out",  {"-o", "--out"}, "Output directory (optional, defaults to input directory)", 1 },
        { "cp",   {"--cp"}, "Copy files (default behavior)", 0 },
        { "mv",   {"--mv"}, "Move files instead of copying", 0 },
        { "verb", {"-v", "--verb"}, "Verbose mode", 0 },
        { "ext",  {"-e", "--ext"}, "File extension to filter (e.g. --ext \".mp3\")", 1 }
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
        return 2;
    }

    fs::path input_dir     = args["in"].as<std::string>();
    fs::path output_dir    = args["out"] ? fs::path{args["out"].as<std::string>()} : input_dir;
    bool move_files        = args["mv"];
    bool verbose_mode      = args["verb"];
    std::string ext_filter = args["ext"] ? args["ext"].as<std::string>() : "";

    if (!fs::exists(input_dir)) {
        std::cerr << "Error: Input directory does not exist.\n";
        return 3;
    }

    if (std::error_code ec; !fs::exists(output_dir) && !fs::create_directory(output_dir, ec) && ec) {
        std::cerr << "Error creating directory [" << output_dir << "] : " << ec.message() << std::endl;
        return 4;
    }

    std::vector<fs::path> files;

    // Read files from input directory
    for (const auto& entry : fs::recursive_directory_iterator(input_dir)) {
        if (entry.is_regular_file()) {
            if (ext_filter.empty() || entry.path().extension() == ext_filter) {
                files.push_back(entry.path());
            }
        }
    }

    // Shuffle files
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(files.begin(), files.end(), g);

    // Determine the number of digits for file numbering
    size_t num_files = files.size();
    int num_digits = std::to_string(num_files).length();

    using fpp_t = void (*)(const fs::path&, const fs::path&);
    fpp_t mvorcp = move_files ? static_cast<fpp_t>(fs::rename) : static_cast<fpp_t>(fs::copy);
    fpp_t verbose_show = verbose_mode ? static_cast<fpp_t>(cpormv_show) : static_cast<fpp_t>(do_nothing);

    bool needs_temp_rename = (input_dir == output_dir) && move_files;
    std::string hash_postfix = needs_temp_rename ? generate_postfix() : "";

    // First pass: rename files with temporary postfix
    for (size_t i = 0; i < num_files; ++i) {
        fs::path new_filename = output_dir / generate_new_filename(i, num_digits, files[i].extension().string(), hash_postfix);
        
        verbose_show(files[i], new_filename);


        try {
            mvorcp(files[i], new_filename);
            files[i] = std::move(new_filename);
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Error:\n" << e.what() << std::endl;
            return 5;
        }
    }

    // Second pass: rename files to final names if nessesary
    if(needs_temp_rename) {
        for (size_t i = 0; i < num_files; ++i) {
            fs::path final_filename = remove_postfix(files[i]);
            
            verbose_show(files[i], final_filename);

            try {
                fs::rename(files[i], final_filename);
            } catch (const fs::filesystem_error& e) {
                std::cerr << "Error:\n" << e.what() << std::endl;
                return 6;
            }
        }
    }

    return 0;
}
