/*
    NXGallery for Nintendo Switch
    Made with love by Jonathan Verbeek (jverbeek.de)
*/

#pragma once

#include <borealis.hpp>
#include <switch.h>

namespace nxgallery::ui
{
    // This is a Borealis UI element which renders a QR code encoding the given text
    class QrCode : public brls::Box
    {
    public:
        // Constructors
        QrCode();

        // Borealis view creator
        static brls::View* create();

        // Sets the text to encode and forces the image to generate a new QR code
        void setText(std::string _text);

    private:
        // Generates a QR code with the given size and returns the pointer to the image data
        unsigned char* generateQRCode(int* outWidthHeight);

    private:
        // Image component defined in the XML
        BRLS_BIND(brls::Image, image, "image");

        // Worker thread for generating the QR code
        Thread worker;

        // The text to encode in the QR code
        std::string text = "";

        // The size (in pixels) per module in the QR code
        int pxPerModule = 10;
    };
}
