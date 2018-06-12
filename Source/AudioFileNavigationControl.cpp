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

#include "AudioFileNavigationControl.h"

AudioFileNavigationControl::AudioFileNavigationControl (Value _sampleRate,
    Value _windowLeft, Value _windowRight, Value _lengthSeconds,
    ControlFrameOfReference _frameOfReference)
    : frameOfReference (_frameOfReference), sampleRate (_sampleRate),
      windowLeft (_windowLeft), windowRight (_windowRight),
      lengthSeconds (_lengthSeconds)
{
    setMouseCursor (MouseCursor (MouseCursor::StandardCursorType::DraggingHandCursor));
}

double AudioFileNavigationControl::getSecondsPerPixel() const
{
    double length = 0.0;
    switch (frameOfReference)
    {
        case ControlFrameOfReference::windowLength:
        {
            length = double (windowRight.getValue()) - double (windowLeft.getValue());
            break;
        }
        case ControlFrameOfReference::totalLength:
        {
            length = double (lengthSeconds.getValue());
            break;
        }
        default: jassertfalse;
    }

    return length / getWidth();
}

double AudioFileNavigationControl::getSecondsPerSample() const
{
    return 1.0 / double (sampleRate.getValue());
}

double AudioFileNavigationControl::moveWindowLeftEdge (double numSeconds)
{
    double secondsMoved = 0.0;
    if (numSeconds > 0.0) // Moving left edge right
        secondsMoved = std::min (numSeconds, double (windowRight.getValue()) - getSecondsPerSample());
    else if (numSeconds < 0.0) // Moving left edge left
        secondsMoved = std::max (numSeconds, -double (windowLeft.getValue()));

    windowLeft = double (windowLeft.getValue()) + secondsMoved;
    return secondsMoved;
}

double AudioFileNavigationControl::moveWindowRightEdge (double numSeconds)
{
    double secondsMoved = 0.0;
    if (numSeconds > 0.0) // Moving right edge right
        secondsMoved = std::min (numSeconds, double (lengthSeconds.getValue()) - double (windowRight.getValue()));
    else if (numSeconds < 0.0) // Moving right edge left
        secondsMoved = std::max (numSeconds, double (windowLeft.getValue()) - double (windowRight.getValue()) + getSecondsPerSample());

    windowRight = double (windowRight.getValue()) + secondsMoved;
    return secondsMoved;
}

void AudioFileNavigationControl::mouseDown (const MouseEvent& event)
{
    setMouseCursor (MouseCursor (MouseCursor::StandardCursorType::NoCursor));
    lastMouseDragOffset = event.getOffsetFromDragStart();
}

void AudioFileNavigationControl::mouseUp (const MouseEvent& event)
{
    setMouseCursor (MouseCursor (MouseCursor::StandardCursorType::DraggingHandCursor));
}

void AudioFileNavigationControl::mouseDrag (const MouseEvent& event)
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
