ROOT=$(cd "$(dirname "$0")/.."; pwd)

"$ROOT/build/bin/JUCE/Projucer" --resave "$ROOT/RRV10.jucer"

cd "$ROOT/Builds/LinuxMakefile"
make CONFIG=Release
