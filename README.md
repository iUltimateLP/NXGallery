# NXGallery

<p align="center">
    <img height="150" src="https://i.imgur.com/PVXyrXG.png">
</p>

<p align="center">
    🎮 A Nintendo Switch Homebrew Application to transfer your screenshots to your phone in no-time 📱⚡
</p>

# What is it?
 + Simplifies the process of exchanging your console's gallery with your smartphone
 + Allows easy and quick sharing of screenshots with your friends
 + Needs Homebrew (CFW) installed on your Switch

# Screenshots
<table>
  <tr align="center">
      <td colspan="2"><img src="https://i.imgur.com/L9xFdht.png"></img></td>
  </tr>
  <tr align="center">
      <td><img src="https://i.imgur.com/BkNVZOd.png"></img></td>
      <td><img src="https://i.imgur.com/xuTYCvi.png"></img></td>
  </tr>
  <tr align="center">
    <td colspan="2"><img src="https://i.imgur.com/KfVBzmm.png"></img></td>
  </tr>
</table>

# Installing
Download the [latest release](https://github.com/iUltimateLP/NXGallery/releases) and drag the `NXGallery.nro` file to the `switch/` folder of your SD card. You can also drag the file to your Switch via [ftpd](https://github.com/mtheall/ftpd). NXGallery is also available in the [Homebrew App Store](https://apps.fortheusers.org/switch/NXGallery).

NXGallery was tested working on [Atmosphère](https://github.com/Atmosphere-NX/Atmosphere) 1.6.2 and firmware version 17.0.1, but should work above/below, if you know what you're doing.

# Building
Compiling this project requires a [Nintendo Switch Homebrew dev environment](https://switchbrew.org/wiki/Setting_up_Development_Environment) to be installed. Then, install the following dependencies (required for [borealis](https://github.com/natinusala/borealis)):
```
(sudo) (dkp-)pacman -S switch-glfw switch-mesa switch-glm
```
After that, clone this repo and run `make all` in the root of this repo. You will find all compiled files in the `out/` folder.

# Credits
I've used the following libraries, without this project wouldn't have been possible:
 + [libnx](https://github.com/switchbrew/libnx)
 + [Atmosphère](https://github.com/Atmosphere-NX/Atmosphere)
 + [nlohmann/json](https://github.com/nlohmann/json)
 + [React](https://reactjs.org/) + [React Material-UI](https://material-ui.com/)
 + [Borealis](https://github.com/natinusala/borealis)
 + [QR-Code-generator](https://github.com/nayuki/QR-Code-generator)

Also, special thanks to [HookedBehemoth's ShareNX](https://github.com/HookedBehemoth/ShareNX) project which has helped me out on some rough edges.

# License
```
MIT License

Copyright (c) 2020-2024 Jonathan Verbeek

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
