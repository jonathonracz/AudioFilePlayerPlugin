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

#include "AudioFileDragTarget.h"

AudioFileDragTarget::AudioFileDragTarget (AudioFormatManager& _formatManager, Value _filenameValue)
    : formatManager (_formatManager),
      filenameValue (_filenameValue)
{
    setAlpha (0.0f); // To match initial "notDragging" state.
}

bool AudioFileDragTarget::willAccept (const StringArray &files)
{
    // Not sure if this can happen somehow, adding assert to CMA.
    jassert (files.size() > 0);

    // Reject adding more than one file.
    if (files.size() > 1)
        return false;

    // Attempt to create a reader for the incoming file.
    std::unique_ptr<AudioFormatReader> testReader (formatManager.createReaderFor (File (*files.begin())));
    return testReader != nullptr;
}

void AudioFileDragTarget::fadeToOppositeAlpha()
{
    float targetAlpha = 0.0f;
    if (getAlpha() == targetAlpha)
        targetAlpha = 1.0f;

    Desktop::getInstance().getAnimator().animateComponent (this, getBounds(), targetAlpha, 250, false, 1.0f, 1.0f);
}

void AudioFileDragTarget::paint (Graphics& g)
{
    // TODO: Make this look not terrible
    if (isAcceptingDrag)
    {
        g.fillAll (Colours::green);
    }
    else
    {
        g.fillAll (Colours::red);
    }
}

bool AudioFileDragTarget::isInterestedInFileDrag (const StringArray &files)
{
    // An undocumented behavior is that if this returns false, we don't get the
    // fileDragEnter callback.
    return true;
}

void AudioFileDragTarget::fileDragEnter (const StringArray &files, int x, int y)
{
    isAcceptingDrag = willAccept (files);
    repaint();
    fadeToOppositeAlpha();
}

void AudioFileDragTarget::fileDragExit (const StringArray &files)
{
    fadeToOppositeAlpha();
}

void AudioFileDragTarget::filesDropped (const StringArray &files, int x, int y)
{
    if (isAcceptingDrag)
        filenameValue = *files.begin();

    fadeToOppositeAlpha();
}

