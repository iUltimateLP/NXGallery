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
|||
|:---:|:---:|
|![PC Light](https://i.imgur.com/VBm4jNg.png)|![PC Dark](https://i.imgur.com/Ie1EG9P.png)|
|![Phone Light](https://i.imgur.com/Uw6O5SI.png)|![Phone Dark](https://i.imgur.com/srVhPpm.png)|

# Installing
Download the [latest release](https://github.com/iUltimateLP/NXGallery/releases) and drag the `NXGallery.nro` file to the `switch/` folder of your SD card. You can also drag the file to your Switch via [ftpd](https://github.com/mtheall/ftpd). 

NXGallery was tested working on [Atmosphère](https://github.com/Atmosphere-NX/Atmosphere) 0.10.4 and firmware version 9.2.0, but should work above/below, if you know what you're doing.

# Building
Compiling this project requires a [Nintendo Switch Homebrew dev environment](https://switchbrew.org/wiki/Setting_up_Development_Environment) to be installed. After that, clone this repo and run `make all` in the root of this repo. You will find all compiled files in the `out/` folder.

## Note!
It is planned to offer a sysmodule version of this project too, but it's not done yet! Please **don't** try to install!

# Credits
I've used the following libraries, without this project wouldn't have been possible:
 + [libnx](https://github.com/switchbrew/libnx)
 + [Atmosphère](https://github.com/Atmosphere-NX/Atmosphere)
 + [nlohmann/json](https://github.com/nlohmann/json)
 + [React](https://reactjs.org/) + [React Material-UI](https://material-ui.com/)
