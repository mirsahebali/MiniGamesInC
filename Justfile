
dev:
  mkdir -p build
  cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

build:
  cmake --build build

run GAME: build
  cd src/ && ../build/{{GAME}}/{{GAME}}

release:
  mkdir -p release
  cmake -S . -B release
  cmake --build release

clean:
  rm -rf build tests-out

dbg GAME: build
  cd build/{{GAME}} && gf2 ./{{GAME}}


lsp:
  ln -s ./build/compile_commands.json .
