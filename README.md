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
      <td><img src="https://i.imgur.com/VBm4jNg.png"></img></td>
      <td><img src="https://i.imgur.com/Ie1EG9P.png"></img></td>
  </tr>
  <tr align="center">
      <td><img src="https://i.imgur.com/Uw6O5SI.png"></img></td>
      <td><img src="https://i.imgur.com/srVhPpm.png"></img></td>
  </tr>
  <tr align="center">
    <td colspan="2"><img src="https://i.imgur.com/iVrEFRk.jpg"></img></td>
  </tr>
</table>

# Installing
Download the [latest release](https://github.com/iUltimateLP/NXGallery/releases) and drag the `NXGallery.nro` file to the `switch/` folder of your SD card. You can also drag the file to your Switch via [ftpd](https://github.com/mtheall/ftpd). 

NXGallery was tested working on [Atmosphère](https://github.com/Atmosphere-NX/Atmosphere) 0.12.0 and firmware version 10.0.2, but should work above/below, if you know what you're doing.

# Building
Compiling this project requires a [Nintendo Switch Homebrew dev environment](https://switchbrew.org/wiki/Setting_up_Development_Environment) to be installed. After that, clone this repo and run `make all` in the root of this repo. You will find all compiled files in the `out/` folder.

# Credits
I've used the following libraries, without this project wouldn't have been possible:
 + [libnx](https://github.com/switchbrew/libnx)
 + [Atmosphère](https://github.com/Atmosphere-NX/Atmosphere)
 + [nlohmann/json](https://github.com/nlohmann/json)
 + [React](https://reactjs.org/) + [React Material-UI](https://material-ui.com/)
