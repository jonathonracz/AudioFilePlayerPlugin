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
#include "AudioFileTimelineWaveform.h"
#include "AudioFileNavigationControl.h"

class AudioFileTimeline : public Component
{
public:
    AudioFileTimeline (AudioFormatManager& formatManagerToUse,
        AudioThumbnailCache& cacheToUse, Value sampleRate, Value windowLeft,
        Value windowRight, Value lengthSeconds, Value filename);

private:
    AudioFileTimelineWaveform waveform;
    AudioFileNavigationControl navigationControl;

    void resized() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioFileTimeline)
};
