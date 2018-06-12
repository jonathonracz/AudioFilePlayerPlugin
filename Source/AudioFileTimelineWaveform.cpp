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

#include "AudioFileTimelineWaveform.h"

AudioFileTimelineWaveform::AudioFileTimelineWaveform (
    AudioFormatManager& formatManagerToUse, AudioThumbnailCache& cacheToUse,
    Value _windowLeft, Value _windowRight, Value _filename)
    : thumbnail (128, formatManagerToUse, cacheToUse), windowLeft (_windowLeft),
      windowRight (_windowRight), filename (_filename)
{
    windowLeft.addListener (this);
    windowRight.addListener (this);
    filename.addListener (this);
}

void AudioFileTimelineWaveform::paint (Graphics& g)
{
    thumbnail.drawChannels (g, getLocalBounds(), double (windowLeft.getValue()), double (windowRight.getValue()), 1.0f);
}

void AudioFileTimelineWaveform::valueChanged (Value& value)
{
    if (value == filename)
        thumbnail.setSource (new FileInputSource (File (filename.toString())));
    else
        repaint();
}

void AudioFileTimelineWaveform::changeListenerCallback (ChangeBroadcaster* source)
{
    repaint();
}
