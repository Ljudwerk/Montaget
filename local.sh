# Utvecklingsbygge: fristående app (arm64, Debug), testfallen, stilprovet och panelbilden.
set -e
cmake -B build-dev -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_OSX_ARCHITECTURES=arm64
cmake --build build-dev --target Montaget_Standalone MOSnapshot MORender
./build-dev/MORender_artefacts/Debug/MORender --test
./build-dev/MOSnapshot_artefacts/Debug/MOSnapshot --stilprov docs/stilprov.png 2
./build-dev/MOSnapshot_artefacts/Debug/MOSnapshot docs/panelen.png 2
open build-dev/Montaget_artefacts/Debug/Standalone/Montaget.app
