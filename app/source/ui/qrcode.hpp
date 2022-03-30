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
