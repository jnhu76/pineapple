add_rules("mode.debug", "mode.release", "mode.valgrind")

-- 主程序
target("pineapple")
    set_kind("binary")
    add_files("src/main.cpp")
    set_languages("c99", "cxx20")
    set_optimize("fastest")
    set_toolchains("clang")
    set_warnings("all", "error")
    add_vectorexts("avx", "avx2")
    set_targetdir(".")

-- 测试
target("tests")
    set_kind("binary")
    add_files("src/tests.cpp")
    set_languages("cxx20")
    set_toolchains("clang")
    set_warnings("all", "error")
    add_vectorexts("avx", "avx2")