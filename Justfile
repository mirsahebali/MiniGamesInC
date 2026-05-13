
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

dbg:
  gf2 ./build/InfinityCastle/InfinityCastle


lsp:
  ln -s ./build/compile_commands.json .
