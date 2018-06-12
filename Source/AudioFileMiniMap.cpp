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
    AudioThumbnailCache& cacheToUse, Value _sampleRate, Value _windowLeft,
    Value _windowRight, Value _lengthSeconds, Value _filename)
    : waveform (formatManagerToUse, cacheToUse, _filename),
      highlight (_windowLeft, _windowRight, _lengthSeconds),
      navigationControl (_sampleRate, _windowLeft, _windowRight, _lengthSeconds,
          AudioFileNavigationControl::ControlFrameOfReference::totalLength),
      windowLeft (_windowLeft), windowRight (_windowRight),
      lengthSeconds (_lengthSeconds)
{
    addAndMakeVisible (waveform);
    addAndMakeVisible (highlight);
    addAndMakeVisible (navigationControl);
}

void AudioFileMiniMap::resized()
{
    waveform.setBounds (getLocalBounds());
    highlight.setBounds (getLocalBounds());
    navigationControl.setBounds (getLocalBounds());
}
