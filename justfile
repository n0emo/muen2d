set windows-shell := ["C:/Program Files/Git/bin/bash.exe", "-c"]

configure *args:
    npm install
    xmake config --mode=debug {{ args }}

check: check-js check-cpp

check-js:
    npm run typecheck
    npm run lint

check-cpp:
    rm -rf build
    xmake build muen
    xmake check clang.tidy --quiet

build:
    xmake build muen

run game:
    xmake run muen {{ justfile_dir() / "examples" / game }}

install-local: build
    xmake install -o $HOME/.local/

format:
    npm run format
    xmake format

doc-js:
    npm run doc

prepare: format check
