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

#include "mainframe.hpp"
#include <borealis/core/logger.hpp>
#include <borealis/core/util.hpp>
#include <borealis/views/applet_frame.hpp>
using namespace nxgallery::ui;

MainFrame::MainFrame()
{
    this->inflateFromXMLRes("xml/views/mainframe.xml");

    this->registerStringXMLAttribute("title", [this](std::string value) {
        this->setTitle(value);
    });

    this->registerFilePathXMLAttribute("icon", [this](std::string value) {
        this->setIconFromFile(value);
    });

    this->forwardXMLAttribute("iconInterpolation", this->icon, "interpolation");

    // Grab the version and set it
    this->version->setText("v" + std::string(VERSION_STRING));
}

void MainFrame::setIconFromRes(std::string name)
{
    this->icon->setVisibility(brls::Visibility::VISIBLE);
    this->icon->setImageFromRes(name);
}

void MainFrame::setIconFromFile(std::string path)
{
    this->icon->setVisibility(brls::Visibility::VISIBLE);
    this->icon->setImageFromFile(path);
}

void MainFrame::setTitle(std::string title)
{
    this->title->setText(title);
}

void MainFrame::setContentView(View* view)
{
    if (this->contentView)
    {
        // Remove the node
        this->removeView(this->contentView);
        this->contentView = nullptr;
    }

    if (!view)
        return;

    this->contentView = view;

    this->contentView->setDimensions(View::AUTO, View::AUTO);
    this->contentView->setGrow(1.0f);

    this->addView(this->contentView, 1);
}

void MainFrame::handleXMLElement(tinyxml2::XMLElement* element)
{
    if (this->contentView)
        brls::fatal("MainFrame can only have one child XML element");

    View* view = View::createFromXMLElement(element);
    this->setContentView(view);
}

brls::View* MainFrame::create()
{
    return new MainFrame();
}
