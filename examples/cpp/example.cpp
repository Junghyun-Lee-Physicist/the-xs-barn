/**
 * @file example.cpp
 * @brief Demonstrates how to use the CMS SampleDB in a standalone environment.
 *
 * Build:
 *   make            (uses the provided Makefile)
 *   or manually:
 *   g++ -std=c++17 -O2 -Iinclude src/example.cpp -o bin/example
 *
 * Run:
 *   ./bin/example ../data/samples_2017UL.json
 */

#include "cms/sample_loader.h"

#include <iomanip>
#include <iostream>
#include <string>

// ----------------------------------------------------------------
//  Pretty-print helpers
// ----------------------------------------------------------------
static void print_divider(int width = 80) {
    std::cout << std::string(width, '-') << "\n";
}

static void print_sample_row(const cms::SampleInfo& s) {
    std::cout << std::left  << std::setw(52) << s.name
              << std::right << std::setw(14);
    if (s.cross_section_pb)
        std::cout << *s.cross_section_pb;
    else
        std::cout << "(pending)";

    std::cout << std::setw(14);
    if (s.nevents)
        std::cout << *s.nevents;
    else
        std::cout << "(pending)";
    std::cout << "\n";
}

// ----------------------------------------------------------------
//  Main
// ----------------------------------------------------------------
int main(int argc, char* argv[]) {
    // Default path — can be overridden via CLI argument
    std::string json_path = (argc > 1)
        ? argv[1]
        : "../data/samples_2017UL.json";

    try {
        // ======================================
        //  1. Load the database
        // ======================================
        auto db = cms::SampleDB::from_json(json_path);

        std::cout << "\n=== CMS Sample Database ===\n"
                  << "Loaded : " << db.size() << " samples ("
                  << db.count_filled() << " with cross section)\n"
                  << "Source : " << json_path << "\n\n";

        // ======================================
        //  2. Access a single sample by name
        // ======================================
        if (auto* tt = db.find("TTTo2L2Nu_TuneCP5_powheg"); tt) {
            std::cout << "[Single sample access]\n";
            std::cout << "  Name    : " << tt->name << "\n"
                      << "  DAS     : " << tt->das_path << "\n"
                      << "  xsec    : " << tt->cross_section_pb.value_or(-1) << " pb\n"
                      << "  N_evt   : " << tt->nevents.value_or(0) << "\n"
                      << "  N_eff   : " << tt->effective_nevents().value_or(0) << "\n"
                      << "  accuracy: " << tt->accuracy.value_or("N/A") << "\n";
            if (tt->comment) {
                std::cout << "  comment : " << *tt->comment << "\n";
            }
            std::cout << "\n";
        }

        // ======================================
        //  3. Filter by substring pattern
        // ======================================
        {
            auto dy = db.filter("DYJetsToLL");
            std::cout << "[Filter: DYJetsToLL] — " << dy.size() << " matches\n";
            print_divider();
            std::cout << std::left  << std::setw(52) << "Name"
                      << std::right << std::setw(14) << "xsec [pb]"
                      << std::setw(14) << "N events" << "\n";
            print_divider();
            for (const auto* s : dy) print_sample_row(*s);
            std::cout << "\n";
        }

        // ======================================
        //  4. Loop over all samples — typical analysis usage
        // ======================================
        {
            std::cout << "[All tt̄ + single-top samples]\n";
            print_divider();
            for (const auto& [name, info] : db) {
                if (name.find("TT") == 0 || name.find("ST_") == 0) {
                    print_sample_row(info);
                }
            }
            std::cout << "\n";
        }

        // ======================================
        //  5. Safe access for possibly-missing sample
        // ======================================
        if (!db.contains("SomeHypotheticalSample")) {
            std::cout << "[Safe access] 'SomeHypotheticalSample' not in DB — skipped.\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
