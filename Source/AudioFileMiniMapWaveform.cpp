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

#include "AudioFileMiniMapWaveform.h"

AudioFileMiniMapWaveform::AudioFileMiniMapWaveform (
    AudioFormatManager& formatManagerToUse, AudioThumbnailCache& cacheToUse,
    Value _filename)
    : filename (_filename), thumbnail (128, formatManagerToUse, cacheToUse)
{
    setInterceptsMouseClicks (false, false);
    filename.addListener (this);
    thumbnail.addChangeListener (this);
}

void AudioFileMiniMapWaveform::paint (Graphics& g)
{
    thumbnail.drawChannels (g, getLocalBounds(), 0.0, thumbnail.getTotalLength(), 1.0f);
}

void AudioFileMiniMapWaveform::valueChanged (Value& value)
{
    thumbnail.setSource (new FileInputSource (File (filename.toString())));
}

void AudioFileMiniMapWaveform::changeListenerCallback (ChangeBroadcaster* source)
{
    repaint();
}
