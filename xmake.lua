add_rules("mode.debug", "mode.release")

target("pineapple")
    set_kind("binary")
    add_files("src/*.cpp")
    set_languages("c99", "cxx20")
    set_optimize("fastest")
    set_toolchains("clang")
    set_warnings("all", "error")
