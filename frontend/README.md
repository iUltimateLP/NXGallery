# NXGallery

<p align="center">
    <img height="150" src="https://i.imgur.com/PVXyrXG.png">
</p>

<p align="center">
    🎮 A Nintendo Switch Homebrew Application to transfer your screenshots to your phone in no-time 📱⚡
</p>

# `frontend/`
This is the frontend code - it's the web page you'll see when opening NXGallery on your phone or on your computer. It utilizes [React](https://reactjs.org/) and [React Material-UI](https://material-ui.com/) ❤️

# Developing
To develop this frontend, you'll need to have the server active on the Switch - so just start NXGallery on your Switch and leave it open. Next, you need to tell the frontend where to find your Switch (and it's NXGallery server when developing). To do that, open up `index.js` and see the following lines:

```js
// Set the IP of your switch so it can connect in dev mode
const DEV_IP = "http://192.168.178.46:1234";
```

Replace the IP address here with the address of your Switch. Then, open up a command prompt and type:

```sh
$ cd frontend/
$ npm install
$ make dev
```

That will spawn the React development server and open up the page in your web browser. It should connect to the NXGallery server running on your Switch and you should be able to develop the frontend.

# Building
To build the frontend, simply type in a command prompt:

```sh
$ cd frontend/
$ make build
```

That will automatically build the frontend and copy it in the app's [romfs](https://github.com/iUltimateLP/NXGallery/tree/master/app/romfs/www) - so you can then just build the app and you're done.

# License
```
MIT License

Copyright (c) 2020-2021 Jonathan Verbeek

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
