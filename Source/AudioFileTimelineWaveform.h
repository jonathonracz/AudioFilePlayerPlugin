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

class AudioFileTimelineWaveform : public Component,
                                  public Value::Listener,
                                  public ChangeListener
{
public:
    AudioFileTimelineWaveform (AudioFormatManager& formatManagerToUse,
        AudioThumbnailCache& cacheToUse, Value windowLeft, Value windowRight,
        Value filename);

private:
    AudioThumbnail thumbnail;
    Value windowLeft;
    Value windowRight;
    Value filename;

    void paint (Graphics& g) override;
    void valueChanged (Value& value) override;
    void changeListenerCallback (ChangeBroadcaster* source) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioFileTimelineWaveform)
};
