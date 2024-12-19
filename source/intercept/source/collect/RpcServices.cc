/*  Copyright (C) 2012-2024 by László Nagy
    This file is part of Bear.

    Bear is a tool to generate compilation database for clang tooling.

    Bear is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Bear is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "collect/RpcServices.h"
#include "Domain.h"
#include "collect/Reporter.h"
#include "collect/RewriteConfig.h"
#include "collect/Session.h"
#include <algorithm>
#include <array>
#include <fmt/ranges.h>
#include <iterator>
#include <spdlog/spdlog.h>

const static std::array<std::string, 5> EXTRA_ARGS { "-g", "-O0", "-fprofile-instr-generate", "-fcoverage-mapping", "-finstrument-functions-after-inlining" };

namespace ic {

    SupervisorImpl::SupervisorImpl(const Session& session, const RewriteConfig& rewrite_cfg)
            : rpc::Supervisor::Service()
            , session_(session)
            , rewrite_cfg_(rewrite_cfg)
    {
    }

    grpc::Status SupervisorImpl::Resolve(grpc::ServerContext*, const rpc::ResolveRequest* request, rpc::ResolveResponse* response)
    {
        return session_.resolve(from(request->execution()))
            .map<grpc::Status>([&response, this](Execution execution) {
                if (auto exec_name = execution.executable.filename(); exec_name == "clang++" or exec_name == "clang") {
                    if (auto src_file_path = execution.arguments.back(); rewrite_cfg_.contains(src_file_path)) {
                        auto& args = execution.arguments;
                        std::move(EXTRA_ARGS.begin(), EXTRA_ARGS.end(), std::back_inserter(args));
                        spdlog::info("hitted {}", fmt::join(args, " ,"));
                    }
                }
                // Need to copy the execution into the response.
                response->mutable_execution()->CopyFrom(into(execution));
                // Confirm it with an OK.
                return ::grpc::Status::OK;
            })
            .unwrap_or_else([](const auto& error) {
                return grpc::Status(::grpc::StatusCode::INVALID_ARGUMENT, error.what());
            });
    }

    InterceptorImpl::InterceptorImpl(Reporter& reporter)
            : rpc::Interceptor::Service()
            , reporter_(reporter)
    {
    }

    grpc::Status InterceptorImpl::Register(grpc::ServerContext*, const rpc::Event* request, google::protobuf::Empty*)
    {
        reporter_.report(*request);
        return ::grpc::Status::OK;
    }
}
