configure:
    xmake config --mode=denug
    xmake project -k compile_commands

build:
    xmake build muen

run game:
    xmake run muen {{ justfile_dir() / "examples" / game }}

format:
    xmake format

