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

#pragma once

#include <borealis.hpp>

namespace nxgallery::ui
{
    // The same as the applet frame from borealis, but more customized towards NXGallery
    class MainFrame : public brls::Box
    {
    public:
        MainFrame();

        void handleXMLElement(tinyxml2::XMLElement* element) override;

        /**
         * Sets the content view for that AppletFrame.
         * Will be placed between header and footer and expanded with grow factor
         * and width / height to AUTO.
         */
        void setContentView(brls::View* view);

        void setTitle(std::string title);

        void setIconFromRes(std::string name);
        void setIconFromFile(std::string path);

        static brls::View* create();

    private:
        BRLS_BIND(brls::Label, title, "brls/applet_frame/title_label");
        BRLS_BIND(brls::Image, icon, "brls/applet_frame/title_icon");
        BRLS_BIND(brls::Label, version, "version");

    protected:
        brls::View* contentView = nullptr;
    };
}
