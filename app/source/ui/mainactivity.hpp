/*
    NXGallery for Nintendo Switch
    Made with love by Jonathan Verbeek (jverbeek.de)
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

        // Binds the QRcode element to this->qrCode so it can be accessed from code
        BRLS_BIND(nxgallery::ui::QrCode, qrCode, "qrcode");
    };
}