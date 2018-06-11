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

#include "AudioFileMiniMapHighlight.h"

AudioFileMiniMapHighlight::AudioFileMiniMapHighlight (Value _windowLeft,
    Value _windowRight, Value _lengthSeconds)
    : windowLeft (_windowLeft), windowRight (_windowRight),
      lengthSeconds (_lengthSeconds)
{
    setInterceptsMouseClicks (false, false);
    windowLeft.addListener (this);
    windowRight.addListener (this);
    lengthSeconds.addListener (this);
}

int AudioFileMiniMapHighlight::getPixelOfSecond (double second) const
{
    return static_cast<int> ((getWidth() / double (lengthSeconds.getValue())) * second);
}

void AudioFileMiniMapHighlight::paint (Graphics& g)
{
    if (double (lengthSeconds.getValue()) <= 0.0)
        return;

    int windowLeftPosition = getPixelOfSecond (double (windowLeft.getValue()));
    int windowRightPosition = getPixelOfSecond (double (windowRight.getValue()));;

    Rectangle<int> miniMapHighlight (std::max (windowRightPosition - windowLeftPosition, 10), getHeight());
    miniMapHighlight = miniMapHighlight.withCentre (
        Point<int> (windowLeftPosition + ((windowRightPosition - windowLeftPosition) / 2), miniMapHighlight.getCentreY()));
    g.setColour (Colours::white.withAlpha (0.5f));
    g.fillRect (miniMapHighlight);
}

void AudioFileMiniMapHighlight::valueChanged (Value &value)
{
    repaint();
}
