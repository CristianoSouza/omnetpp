//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#include "tester.h"

Define_Module(Tester);
Define_Module(Dummy);

void Tester::initialize()
{
    // None of the images are in the project directory, the images or the bitmaps subfolders, intentionally.
    // Adding the photos1 folder to the image path. This will make the images "lena" and "cgi/teapot" available.
    getEnvir()->appendToImagePath(resolveResourcePath("photos1").c_str());

    // Loading two images with default name, the image names will be "baboon" and "box".
    getEnvir()->loadImage(resolveResourcePath("photos2/baboon.png").c_str());
    getEnvir()->loadImage(resolveResourcePath("photos2/cgi/box.gif").c_str());

    // Loading an image with a custom name.
    getEnvir()->loadImage(resolveResourcePath("photos2/cgi/bunny.png").c_str(), "3d/hare");

    // This oval figure will always be placed where this module is in its parent.
    locator = new cOvalFigure();
    locator->setLineWidth(4);
    locator->setLineColor("red");
    locator->setZIndex(1.5);
    getParentModule()->getCanvas()->addFigure(locator);
}

void Tester::refreshDisplay() const
{
    cFigure::Point pos = getEnvir()->getSubmodulePosition(this);
    locator->setBounds(cFigure::Rectangle(pos.x - 25, pos.y - 25, 50, 50));
}
