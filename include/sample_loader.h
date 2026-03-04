/**
 * @file sample_loader.h
 * @brief Header-only CMS sample database loader for C++17.
 *
 * This is a LIBRARY header — #include it in your analysis code.
 * It wraps the raw JSON into a typed, queryable in-memory database.
 *
 * Dependency:
 *   nlohmann/json single-header (json.hpp)
 *   → place it at include/nlohmann/json.hpp
 *   → https://github.com/nlohmann/json/releases
 *
 * Usage:
 *   #include "cms/sample_loader.h"
 *   auto db = cms::SampleDB::from_json("data/samples_2017UL.json");
 *   double xsec = db["TTTo2L2Nu_TuneCP5_powheg"].cross_section_pb.value();
 */

#ifndef CMS_SAMPLE_LOADER_H
#define CMS_SAMPLE_LOADER_H

#include <nlohmann/json.hpp>

#include <algorithm>
#include <fstream>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace cms {

// ================================================================
//  SampleInfo — one MC sample record (immutable after construction)
// ================================================================
struct SampleInfo {
    std::string                name;
    std::string                das_path;
    std::optional<int64_t>     nevents;
    std::optional<int>         nfiles;
    std::optional<double>      cross_section_pb;
    std::optional<std::string> accuracy;
    std::optional<double>      frac_neg_weight;
    std::optional<std::string> xsec_ref;
    std::optional<std::string> comment;

    /// N_eff = N × (1 − 2f), proper normalization for NLO generators
    [[nodiscard]] std::optional<double> effective_nevents() const {
        if (!nevents || !frac_neg_weight) return std::nullopt;
        return static_cast<double>(*nevents) * (1.0 - 2.0 * (*frac_neg_weight));
    }
};

// ================================================================
//  SampleDB — queryable collection of SampleInfo
// ================================================================
class SampleDB {
public:
    // ---- factory ----

    /// Load from a local JSON file path
    static SampleDB from_json(const std::string& path) {
        std::ifstream ifs(path);
        if (!ifs.is_open()) {
            throw std::runtime_error("SampleDB: cannot open file '" + path + "'");
        }
        nlohmann::json j;
        ifs >> j;
        return build(j);
    }

    /// Load from a pre-parsed nlohmann::json object
    static SampleDB from_json_object(const nlohmann::json& j) {
        return build(j);
    }

    // ---- element access ----

    /// Access by name; throws std::out_of_range if not found
    const SampleInfo& operator[](const std::string& name) const {
        auto it = samples_.find(name);
        if (it == samples_.end()) {
            throw std::out_of_range("SampleDB: unknown sample '" + name + "'");
        }
        return it->second;
    }

    /// Access by name; returns nullptr if not found (no-throw)
    [[nodiscard]] const SampleInfo* find(const std::string& name) const noexcept {
        auto it = samples_.find(name);
        return (it != samples_.end()) ? &it->second : nullptr;
    }

    [[nodiscard]] bool   contains(const std::string& name) const { return samples_.count(name) > 0; }
    [[nodiscard]] size_t size()   const noexcept { return samples_.size(); }
    [[nodiscard]] bool   empty()  const noexcept { return samples_.empty(); }

    // ---- iteration (range-for compatible) ----

    auto begin()  const noexcept { return samples_.begin(); }
    auto end()    const noexcept { return samples_.end();   }
    auto cbegin() const noexcept { return samples_.cbegin(); }
    auto cend()   const noexcept { return samples_.cend();   }

    // ---- queries ----

    /// Return pointers to samples whose name contains `pattern`
    [[nodiscard]]
    std::vector<const SampleInfo*> filter(const std::string& pattern) const {
        std::vector<const SampleInfo*> out;
        for (const auto& [name, info] : samples_) {
            if (name.find(pattern) != std::string::npos) {
                out.push_back(&info);
            }
        }
        return out;
    }

    /// Return all sample short names
    [[nodiscard]]
    std::vector<std::string> names() const {
        std::vector<std::string> out;
        out.reserve(samples_.size());
        for (const auto& [name, _] : samples_) out.push_back(name);
        std::sort(out.begin(), out.end());
        return out;
    }

    /// Count how many samples have cross_section_pb filled
    [[nodiscard]]
    size_t count_filled() const noexcept {
        size_t n = 0;
        for (const auto& [_, info] : samples_) {
            if (info.cross_section_pb.has_value()) ++n;
        }
        return n;
    }

private:
    std::unordered_map<std::string, SampleInfo> samples_;

    // ---- internal builder ----
    static SampleDB build(const nlohmann::json& j) {
        SampleDB db;
        db.samples_.reserve(j.size());

        for (auto it = j.begin(); it != j.end(); ++it) {
            const auto& v = it.value();
            SampleInfo info;
            info.name     = it.key();
            info.das_path = v.at("das_path").get<std::string>();

            // Helper lambdas for null-safe extraction
            auto opt_i64 = [&](const char* k) -> std::optional<int64_t> {
                if (auto f = v.find(k); f != v.end() && !f->is_null()) return f->get<int64_t>();
                return std::nullopt;
            };
            auto opt_i32 = [&](const char* k) -> std::optional<int> {
                if (auto f = v.find(k); f != v.end() && !f->is_null()) return f->get<int>();
                return std::nullopt;
            };
            auto opt_dbl = [&](const char* k) -> std::optional<double> {
                if (auto f = v.find(k); f != v.end() && !f->is_null()) return f->get<double>();
                return std::nullopt;
            };
            auto opt_str = [&](const char* k) -> std::optional<std::string> {
                if (auto f = v.find(k); f != v.end() && !f->is_null()) return f->get<std::string>();
                return std::nullopt;
            };

            info.nevents          = opt_i64("nevents");
            info.nfiles           = opt_i32("nfiles");
            info.cross_section_pb = opt_dbl("cross_section_pb");
            info.accuracy         = opt_str("accuracy");
            info.frac_neg_weight  = opt_dbl("frac_neg_weight");
            info.xsec_ref         = opt_str("xsec_ref");
            info.comment          = opt_str("comment");

            db.samples_.emplace(info.name, std::move(info));
        }
        return db;
    }
};

} // namespace cms

#endif // CMS_SAMPLE_LOADER_H
