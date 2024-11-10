# smol-rack

AU/VST digital emulation plugins of the Micro Rack effect series from the 80s, from a famous Japanese manufacturer.

![The Micro Rack series](https://blogger.googleusercontent.com/img/b/R29vZ2xl/AVvXsEj5ijMQdRnU85cu9DdGSU1TaKpQSMjVFGgnNyqHLtWigSVUK7LscpbC2c08TNqrjSjcwYDu1otnxFwSNfIUJGI9YC5vao4X6ZuOWuZeYDc9zrQ5-HEb7GXZeSz6WaZEzxuau5ww/s640/1.jpg)

The following models are planned to be supported. The other models are analog-based, so they are much more complicated to emulate.

- ✅ **RRV-10 Digital Reverb**: using the HG61H20R36F chip emulator from the MUNT project, already working
- 🚧 **RCE-10 Chorus Ensemble**: based on the MB654119 chip (decap available, work in progress)
- ⛔️ **RDD-10 Digital Delay**, **RSD-10 Sampling Delay** and **RPD-10 Panning Delay**: based on the MB63H101 chip (no decap available yet)
- ⛔️ **RPS-10 Digital Pitch Shifter**: based on the M74007 chip (no decap available, likely difficult to reverse)
