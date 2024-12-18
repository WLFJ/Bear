#include "rewrite/rewrite-forward.h"

namespace rw {

    Rewrite::Rewrite(const ps::ApplicationLogConfig& log_config) noexcept
            : ps::SubcommandFromArgs("rewrite", log_config)
    {
    }

    rust::Result<ps::CommandPtr> Rewrite::command(const flags::Arguments& args, const char** envp) const
    {
        return rust::Result<ps::CommandPtr>(rust::Err(std::runtime_error("Not implemented")));
    }

}
