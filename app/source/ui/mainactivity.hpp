/*
    NXGallery for Nintendo Switch
    Made with love by Jonathan Verbeek (jverbeek.de)

    MIT License

    Copyright (c) 2020-2022 Jonathan Verbeek

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
*/

#pragma once

#include <borealis.hpp>
#include "qrcode.hpp"

namespace nxgallery::ui
{
    // Some lines
    std::vector<std::string> niceLines = {
        "You look nice today!",
        "Brewster's comin!!",
        "Did you know you can calculate the times the DVD logo hits the corner?",
        "Much love to the whole Switch Homebrew team!",
        "I wish I was better at Kart",
        "You should try Minecraft!",
        "Nintendo... seriously?",
        "Did you know? The moon has \"moonquakes!\"",
        "You now breathe in manual mode!",
        "Rabbits are incapable of spewing.",
        "Bees sometimes sting other bees",
        "I should add more features instead of thinking of lines to put here.",
        "What a nice weather outside!",
        "If you don't like what's written here - make a PR on GitHub!"
    };

    // Main activity for Borealis
    class MainActivity : public brls::Activity
    {
    public:
        // Inflates the XML resource
        CONTENT_FROM_XML_RES("activity/main.xml");

        // Binds the elements to this so they can be accessed from code
        BRLS_BIND(nxgallery::ui::QrCode, qrCode, "qrcode");
        BRLS_BIND(brls::Label, address, "address");
    };
}