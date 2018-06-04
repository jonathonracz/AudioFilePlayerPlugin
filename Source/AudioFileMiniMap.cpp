/*
    Copyright (C) 2018 Jonathon Racz

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "AudioFileMiniMap.h"



AudioFileMiniMap::AudioFileMiniMap (AudioFormatManager& formatManagerToUse,
    AudioThumbnailCache& cacheToUse, Value _windowLeft, Value _windowRight,
    Value _filename)
    : windowLeft (_windowLeft), windowRight (_windowRight),
      filename (_filename), thumbnail (128, formatManagerToUse, cacheToUse)
{
    windowLeft.addListener (this);
    windowRight.addListener (this);
    filename.addListener (this);
    thumbnail.addChangeListener (this);
    setMouseCursor (MouseCursor (MouseCursor::StandardCursorType::DraggingHandCursor));
}

double AudioFileMiniMap::getSecondsPerPixel() const
{
    return thumbnail.getTotalLength() / getWidth();
}

int AudioFileMiniMap::getPixelOfSecond (double second) const
{
    return static_cast<int> ((getWidth() / thumbnail.getTotalLength()) * second);
}

double AudioFileMiniMap::moveWindowLeftEdge (double numSeconds)
{
    double secondsMoved = 0.0;
    if (numSeconds > 0.0) // Moving left edge right
        secondsMoved = std::min (numSeconds, double (windowRight.getValue()));
    else if (numSeconds < 0.0) // Moving left edge left
        secondsMoved = std::max (numSeconds, -double (windowLeft.getValue()));

    windowLeft = double (windowLeft.getValue()) + secondsMoved;
    return secondsMoved;
}

double AudioFileMiniMap::moveWindowRightEdge (double numSeconds)
{
    double secondsMoved = 0.0;
    if (numSeconds > 0.0) // Moving right edge right
        secondsMoved = std::min (numSeconds, thumbnail.getTotalLength() - double (windowRight.getValue()));
    else if (numSeconds < 0.0) // Moving right edge left
        secondsMoved = std::max (numSeconds, double (windowLeft.getValue()) - double (windowRight.getValue()));

    windowRight = double (windowRight.getValue()) + secondsMoved;
    return secondsMoved;
}

void AudioFileMiniMap::paint (Graphics& g)
{
    if (thumbnail.getTotalLength() > 0.0)
    {
        thumbnail.drawChannels (g, getLocalBounds(), 0.0, thumbnail.getTotalLength(), 1.0f);
        int windowLeftPosition = getPixelOfSecond (double (windowLeft.getValue()));
        int windowRightPosition = getPixelOfSecond (double (windowRight.getValue()));;

        g.setColour (Colours::black);
        Rectangle<int> miniMapHighlight (std::max (windowRightPosition - windowLeftPosition, 10), getHeight());
        miniMapHighlight = miniMapHighlight.withCentre (
            Point<int> (windowLeftPosition + ((windowRightPosition - windowLeftPosition) / 2), miniMapHighlight.getCentreY()));
        g.drawRect (miniMapHighlight);
    }
}

void AudioFileMiniMap::mouseDown (const MouseEvent& event)
{
    setMouseCursor (MouseCursor (MouseCursor::StandardCursorType::NoCursor));
    lastMouseDragOffset = event.getOffsetFromDragStart();
}

void AudioFileMiniMap::mouseUp (const MouseEvent& event)
{
    setMouseCursor (MouseCursor (MouseCursor::StandardCursorType::DraggingHandCursor));
}

void AudioFileMiniMap::mouseDrag (const MouseEvent& event)
{
    Point<int> dragDelta = event.getOffsetFromDragStart() - lastMouseDragOffset;

    if (dragDelta.getX() > 0)
    {
        moveWindowRightEdge (getSecondsPerPixel());
        moveWindowLeftEdge (getSecondsPerPixel());
    }
    else if (dragDelta.getX() < 0)
    {
        moveWindowLeftEdge (-getSecondsPerPixel());
        moveWindowRightEdge (-getSecondsPerPixel());
    }

    if (dragDelta.getY() > 0) // Zoom in
    {
        moveWindowLeftEdge (getSecondsPerPixel());
        moveWindowRightEdge (-getSecondsPerPixel());
    }
    else if (dragDelta.getY() < 0) // Zoom out
    {
        moveWindowLeftEdge (-getSecondsPerPixel());
        moveWindowRightEdge (getSecondsPerPixel());
    }

    lastMouseDragOffset = event.getOffsetFromDragStart();
}

void AudioFileMiniMap::valueChanged (Value& value)
{
    if (value == filename)
        thumbnail.setSource (new FileInputSource (File (filename.toString())));
    else
        repaint();
}

void AudioFileMiniMap::changeListenerCallback (ChangeBroadcaster* source)
{
    repaint();
}
