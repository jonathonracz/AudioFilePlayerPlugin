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

#pragma once

#include "JuceHeader.h"

class AudioFileNavigationControl    : public Component
{
public:
    /** windowLength: all manipulations are relative to the window length.
        totalLength: all manipulations are relative to the total length.
    */
    enum ControlFrameOfReference
    {
        windowLength,
        totalLength
    };

    AudioFileNavigationControl (Value sampleRate, Value windowLeft,
        Value windowRight, Value lengthSeconds,
        ControlFrameOfReference frameOfReference);

    ControlFrameOfReference frameOfReference;

private:
    Value sampleRate;
    Value windowLeft;
    Value windowRight;
    Value lengthSeconds;

    Point<int> lastMouseDragOffset;

    double getSecondsPerPixel() const;
    double getSecondsPerSample() const;
    double moveWindowLeftEdge (double numSeconds);
    double moveWindowRightEdge (double numSeconds);

    void mouseDown (const MouseEvent& event) override;
    void mouseUp (const MouseEvent& event) override;
    void mouseDrag (const MouseEvent& event) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioFileNavigationControl)
};
