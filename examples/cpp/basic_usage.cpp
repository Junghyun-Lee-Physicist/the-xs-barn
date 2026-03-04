/**
 * @file basic_usage.cpp
 * @brief Demonstrates how to use the CMS SampleDB.
 *
 * Build:
 *   make                                   (uses the provided Makefile)
 *   ./bin/basic_usage ../../data/samples_2017UL.json
 */

#include "cms/sample_loader.h"

#include <iomanip>
#include <iostream>
#include <string>

static void print_divider(int w = 80) { std::cout << std::string(w, '-') << "\n"; }

static void print_row(const cms::SampleInfo& s) {
    std::cout << std::left  << std::setw(52) << s.name
              << std::right << std::setw(14);
    if (s.cross_section_pb) std::cout << *s.cross_section_pb;
    else                    std::cout << "(pending)";
    std::cout << std::setw(14);
    if (s.nevents) std::cout << *s.nevents;
    else           std::cout << "(pending)";
    std::cout << "\n";
}

int main(int argc, char* argv[]) {
    std::string json_path = (argc > 1)
        ? argv[1]
        : "../../data/samples_2017UL.json";

    try {
        // ── 1. Load ─────────────────────────────────────────
        auto db = cms::SampleDB::from_json(json_path);
        std::cout << "\n=== The Barn — CMS Sample Database ===\n"
                  << "Loaded : " << db.size() << " samples ("
                  << db.count_filled() << " with cross section)\n\n";

        // ── 2. Single sample access ─────────────────────────
        if (auto* tt = db.find("TTTo2L2Nu_TuneCP5_powheg"); tt) {
            std::cout << "[Single sample]\n"
                      << "  Name    : " << tt->name << "\n"
                      << "  DAS     : " << tt->das_path << "\n"
                      << "  xsec    : " << tt->cross_section_pb.value_or(-1) << " pb\n"
                      << "  N_evt   : " << tt->nevents.value_or(0) << "\n"
                      << "  N_eff   : " << tt->effective_nevents().value_or(0) << "\n"
                      << "  accuracy: " << tt->accuracy.value_or("N/A") << "\n\n";
        }

        // ── 3. Filter by pattern ────────────────────────────
        auto dy = db.filter("DYJetsToLL");
        std::cout << "[Filter: DYJetsToLL] — " << dy.size() << " matches\n";
        print_divider();
        std::cout << std::left  << std::setw(52) << "Name"
                  << std::right << std::setw(14) << "xsec [pb]"
                  << std::setw(14) << "N events" << "\n";
        print_divider();
        for (const auto* s : dy) print_row(*s);
        std::cout << "\n";

        // ── 4. Iterate all top samples ──────────────────────
        std::cout << "[Top-quark samples (TT* + ST_*)]\n";
        print_divider();
        for (const auto& [name, info] : db) {
            if (name.find("TT") == 0 || name.find("ST_") == 0)
                print_row(info);
        }
        std::cout << "\n";

        // ── 5. Safe access (missing sample) ─────────────────
        if (!db.contains("SomeHypotheticalSample")) {
            std::cout << "[Safe access] 'SomeHypotheticalSample' not found — skipped.\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
