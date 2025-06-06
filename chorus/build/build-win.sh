ROOT=$(cd "$(dirname "$0")/.."; pwd)

# Resave jucer files
"$ROOT/build/bin/JUCE/Projucer.exe" --resave "$ROOT/RCE10.jucer"

VS_WHERE="C:/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe"
  
MSBUILD_EXE=$("$VS_WHERE" -latest -requires Microsoft.Component.MSBuild -find "MSBuild\**\Bin\MSBuild.exe")
echo $MSBUILD_EXE

cd "$ROOT/Builds/VisualStudio2022"
"$MSBUILD_EXE" "RCE10.sln" "//p:VisualStudioVersion=19.0" "//m" "//t:Build" "//p:Configuration=Release" "//p:PlatformTarget=x64" "//p:PreferredToolArchitecture=x64"
