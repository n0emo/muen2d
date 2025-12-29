configure:
    npm install
    xmake config --mode=debug

check: check-js check-cpp

check-js:
    npm run typecheck:muen
    npm run typecheck:examples
    npm run lint

check-cpp:
    xmake build muen
    xmake check clang.tidy --quiet

build:
    xmake build muen

run game:
    xmake run muen {{ justfile_dir() / "examples" / game }}

format:
    npm run format
    xmake format

doc-js:
    npm run doc

prepare: format check
