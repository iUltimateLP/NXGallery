/*
    NXGallery for Nintendo Switch
    Made with love by Jonathan Verbeek (jverbeek.de)

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
*/

#include "qrcode.hpp"
#include "qrcodegen.hpp"
using namespace nxgallery::ui;

// The XML content of the QrCode UI element
const std::string QrCodeXML = R"xml(
    <brls:Box
        width="auto"
        height="auto"
        axis="column"
        alignItems="center">

        <brls:Image
            id="image"
            width="auto"
            height="auto"
            focusable="false" />

    </brls:Box>
)xml";

QrCode::QrCode()
{
    // Create the element from the XML data
    this->inflateFromXMLString(QrCodeXML);

    // Forward a couple of attributes
    this->forwardXMLAttribute("imageWidth", this->image, "width");
    this->forwardXMLAttribute("imageHeight", this->image, "height");

    // Register pixelsPerModule as a custom XML attribute
    this->registerFloatXMLAttribute("pixelsPerModule", [=](float value) {
        this->pxPerModule = (int)value;
    });
}

brls::View* QrCode::create()
{
    // Called by the Borealis engine to create a new object of this kind
    return new QrCode();
}

void QrCode::setText(std::string _text)
{
    // Set the text
    this->text = _text;

    // Generate the QR code
    int imageSize;
    unsigned char* buffer = generateQRCode(&imageSize);

    // Apply it
    this->image->setImageFromRGBA(buffer, imageSize, imageSize);
}

unsigned char* QrCode::generateQRCode(int* outWidthHeight)
{
    // Generate the QR code using qrcodegen
    const qrcodegen::QrCode::Ecc errorCorrection = qrcodegen::QrCode::Ecc::LOW;
    const qrcodegen::QrCode qrCode = qrcodegen::QrCode::encodeText(this->text.c_str(), errorCorrection);

    // Get the colors from the theme in which we will paint this QR code
    NVGcolor foregroundColor = brls::Application::getTheme().getColor("nxgallery/qrcode/foreground");
    NVGcolor backgroundColor = brls::Application::getTheme().getColor("nxgallery/qrcode/background");

    // Allocate a buffer for the image
    int numPixelsPerLine = qrCode.getSize() * pxPerModule;
    int bufferSize = numPixelsPerLine * numPixelsPerLine * 4; // width * height * number of channels (RGBA = 4)
    unsigned char* buffer = (unsigned char*)calloc(sizeof(unsigned char), bufferSize);

    // Iterate through the modules of the QR code
    // This is O(n^4) but since this is only happening once - don't care :)
    for (int mY = 0; mY < qrCode.getSize(); mY++)
    {
        for (int mX = 0; mX < qrCode.getSize(); mX++)
        {
            // Get the value (either true (white) or false (black)) of this module
            bool moduleValue = qrCode.getModule(mX, mY);

            // Iterate through the pixels per module
            for (int pY = 0; pY < pxPerModule; pY++)
            {
                for (int pX = 0; pX < pxPerModule; pX++)
                {
                    // Calculate the position of the actual pixel
                    int x = (mX * pxPerModule) + pX;
                    int y = (mY * pxPerModule) + pY;

                    // Calculate the color of this pixel
                    NVGcolor color = moduleValue ? foregroundColor : backgroundColor;

                    // Set the memory for this pixel
                    // See: https://stackoverflow.com/questions/3902648/c-representing-a-3d-array-in-a-1d-array
                    memset(&buffer[(y * numPixelsPerLine + x) * 4 + 3], (unsigned char)(color.a * 255), sizeof(unsigned char)); // Alpha
                    memset(&buffer[(y * numPixelsPerLine + x) * 4 + 2], (unsigned char)(color.b * 255), sizeof(unsigned char)); // Blue
                    memset(&buffer[(y * numPixelsPerLine + x) * 4 + 1], (unsigned char)(color.g * 255), sizeof(unsigned char)); // Green
                    memset(&buffer[(y * numPixelsPerLine + x) * 4 + 0], (unsigned char)(color.r * 255), sizeof(unsigned char)); // Red
                }
            }
        }
    }

    // Return the width/height via the given pointer, if valid
    if (outWidthHeight != nullptr) *outWidthHeight = numPixelsPerLine;

    // Image generation done, return it
    return buffer;
}
