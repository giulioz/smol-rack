# smol-rack

AU/VST digital emulation plugins of the Micro Rack effect series from the 80s, from a famous Japanese manufacturer.

![The Micro Rack series](https://blogger.googleusercontent.com/img/b/R29vZ2xl/AVvXsEj5ijMQdRnU85cu9DdGSU1TaKpQSMjVFGgnNyqHLtWigSVUK7LscpbC2c08TNqrjSjcwYDu1otnxFwSNfIUJGI9YC5vao4X6ZuOWuZeYDc9zrQ5-HEb7GXZeSz6WaZEzxuau5ww/s640/1.jpg)

## Downloads

- **RRV-10**
  - [MacOS AU](https://github.com/giulioz/smol-rack/releases/download/latest/RRV10.component.macOS.zip)
  - [MacOS VST](https://github.com/giulioz/smol-rack/releases/download/latest/RRV10.vst3.macOS.zip)
  - [Windows VST](https://github.com/giulioz/smol-rack/releases/download/latest/RRV10.vst3)

**NOTE (Windows)**: If you are having troubles with Windows 10, it's possible you need to install the [Visual C++ 2022 Redistributable](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170#latest-microsoft-visual-c-redistributable-version).

**NOTE (MacOs)**: If you are having troubles with MacOS, it's possible your operating system is blocking the plugin because it's coming from an unregister developer. You can allow this plugin by running this command on a terminal:

```sudo xattr -rd com.apple.quarantine /Users/<yourusername>/Library/Audio/Plug-Ins/Components/RRV10.component```

More info on this guide: https://www.osirisguitar.com/2020/04/01/how-to-make-unsigned-vsts-work-in-macos-catalina/

## Models

The following models are planned to be supported. The other models are analog-based, so they are much more complicated to emulate.

- ✅ **RRV-10 Digital Reverb**: using the HG61H20R36F chip emulator from the MUNT project, already working
- 🚧 **RCE-10 Chorus Ensemble**: based on the MB654119 chip (decap available, work in progress)
- ⛔️ **RDD-10 Digital Delay**, **RSD-10 Sampling Delay** and **RPD-10 Panning Delay**: based on the MB63H101 chip (no decap available yet)
- ⛔️ **RPS-10 Digital Pitch Shifter**: based on the M74007 chip (no decap available, likely difficult to reverse)
