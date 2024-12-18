include(GNUInstallDirs)

# Intercept orchestrator code.
add_library(rewrite_a OBJECT)
target_include_directories(rewrite_a PRIVATE source/ include/)

target_sources(rewrite_a PRIVATE source/rewrite.cpp)
target_sources(rewrite_a
        INTERFACE $<TARGET_OBJECTS:rewrite_a>)
target_link_libraries(rewrite_a PUBLIC
        domain_a
        main_a
        events_db_a
        exec_a
        flags_a
        rpc_a
        sys_a
        result_a
        spdlog::spdlog)

