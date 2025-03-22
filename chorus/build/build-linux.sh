ROOT=$(cd "$(dirname "$0")/.."; pwd)

"$ROOT/build/bin/JUCE/Projucer" --resave "$ROOT/RCE10.jucer"

cd "$ROOT/Builds/LinuxMakefile"
make CONFIG=Release
