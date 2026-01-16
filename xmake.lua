add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "build"})

add_requires("quickjs-ng v0.11.0", {alias = "quickjs", configs = {debug = true}} )
add_requires("raylib 5.5")
add_requires("spdlog 1.16.0", {
    configs = {
        header_only = false,
        std_format = true,
    },
})

set_languages({"c++23", "c11"})
set_warnings("all", "extra")

if is_plat("windows") then
    add_defines("_CRT_SECURE_NO_WARNINGS")
end


target("muen")
    set_kind("binary")
    add_files(
        "src/engine.cpp",
        "src/error.cpp",
        "src/jsutil.cpp",
        "src/main.cpp",
        "src/plugins/*.cpp"
    )
    add_files("src/**.js")
    add_includedirs("src", {public = true})
    add_headerfiles("src/(**.hpp)")
    add_headerfiles("src/(**.h)")
    add_packages({"quickjs", "spdlog", "raylib"})
    add_defines("SPDLOG_COMPILED_LIB")
    add_defines("SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE")
    add_rules("utils.bin2c", {extensions = ".js"})

--
-- If you want to known more usage about xmake, please see https://xmake.io
--
-- ## FAQ
--
-- You can enter the project directory firstly before building project.
--
--   $ cd projectdir
--
-- 1. How to build project?
--
--   $ xmake
--
-- 2. How to configure project?
--
--   $ xmake f -p [macosx|linux|iphoneos ..] -a [x86_64|i386|arm64 ..] -m [debug|release]
--
-- 3. Where is the build output directory?
--
--   The default output directory is `./build` and you can configure the output directory.
--
--   $ xmake f -o outputdir
--   $ xmake
--
-- 4. How to run and debug target after building project?
--
--   $ xmake run [targetname]
--   $ xmake run -d [targetname]
--
-- 5. How to install target to the system directory or other output directory?
--
--   $ xmake install
--   $ xmake install -o installdir
--
-- 6. Add some frequently-used compilation flags in xmake.lua
--
-- @code
--    -- add debug and release modes
--    add_rules("mode.debug", "mode.release")
--
--    -- add macro definition
--    add_defines("NDEBUG", "_GNU_SOURCE=1")
--
--    -- set warning all as error
--    set_warnings("all", "error")
--
--    -- set language: c99, c++11
--    set_languages("c99", "c++11")
--
--    -- set optimization: none, faster, fastest, smallest
--    set_optimize("fastest")
--
--    -- add include search directories
--    add_includedirs("/usr/include", "/usr/local/include")
--
--    -- add link libraries and search directories
--    add_links("tbox")
--    add_linkdirs("/usr/local/lib", "/usr/lib")
--
--    -- add system link libraries
--    add_syslinks("z", "pthread")
--
--    -- add compilation and link flags
--    add_cxflags("-stdnolib", "-fno-strict-aliasing")
--    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})
--
-- @endcode
--
