set windows-shell := ["C:/Program Files/Git/bin/bash.exe", "-c"]

configure *args:
    npm install
    xmake config --mode=debug {{ args }}
    xmake require --build --force --yes

check: check-js check-cpp

check-js:
    npm run typecheck
    npm run lint

check-cpp:
    xmake build -r glint
    xmake check clang.tidy --quiet

build:
    xmake build glint

rebuild:
    xmake build -r glint

run game:
    xmake run glint {{ justfile_directory() / "examples" / game }}

install-local: build
    xmake install -o $HOME/.local/

format:
    npm run format
    xmake format

doc-js:
    npm run doc

prepare: format check
