#pragma once

#include "libresult/Result.h"
#include <fstream>
#include <list>
#include <nlohmann/json.hpp>
#include <string>

namespace ic {

    using json = nlohmann::json;

    struct FileConfig {
        std::string filename;
    };

    struct RewriteConfig {
        std::list<FileConfig> files;

        static rust::Result<RewriteConfig> from(const std::string_view& cfg_path)
        {
            std::ifstream f(cfg_path.data());
            if (!f.is_open()) {
                return rust::Err(std::runtime_error("Failed to open file"));
            }

            json data = json::parse(f);
            RewriteConfig cfg;

            for (auto& file : data["files"]) {
                cfg.files.push_back(FileConfig { file["filename"] });
            }

            f.close();

            return rust::Ok(cfg);
        }

        static rust::Result<RewriteConfig> empty()
        {
            return rust::Ok(RewriteConfig {});
        }

        bool contains(std::string_view fp) const noexcept
        {
            return std::any_of(files.begin(), files.end(), [&fp](auto fc) { return fc.filename == fp; });
        }
    };

}
