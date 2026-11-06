# Release: VST3, AU och CLAP (universal), kopieras till ~/Library/Audio/Plug-Ins.
set -e
cmake -B build -G Xcode
cmake --build build --config Release --target Montaget_All
cp -R build/Montaget_artefacts/Release/VST3/Montaget.vst3 ~/Library/Audio/Plug-Ins/VST3/
cp -R build/Montaget_artefacts/Release/AU/Montaget.component ~/Library/Audio/Plug-Ins/Components/
cp -R build/Montaget_artefacts/Release/CLAP/Montaget.clap ~/Library/Audio/Plug-Ins/CLAP/
