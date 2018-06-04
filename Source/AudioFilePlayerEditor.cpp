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

#include "AudioFilePlayerProcessor.h"
#include "AudioFilePlayerEditor.h"
#include "Identifiers.h"

AudioFilePlayerProcessorEditor::AudioFilePlayerProcessorEditor (AudioFilePlayerProcessor& p)
    : AudioProcessorEditor (&p),
      processor (p),
      dragTarget (processor.getFormatManager(), processor.getState().getPropertyAsValue (IDs::AudioFileName, nullptr)),
      miniMap (processor.getFormatManager(), processor.getThumbnailCache(),
          processor.getState().getPropertyAsValue (IDs::WaveformLeftViewSecond, nullptr),
          processor.getState().getPropertyAsValue (IDs::WaveformRightViewSecond, nullptr),
          processor.getState().getPropertyAsValue (IDs::AudioFileName, nullptr))
{
    setSize (400, 300);
    addAndMakeVisible (dragTarget);
    dragTarget.setAlwaysOnTop (true);

    addAndMakeVisible (miniMap);
}

AudioFilePlayerProcessorEditor::~AudioFilePlayerProcessorEditor()
{
}

void AudioFilePlayerProcessorEditor::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), Justification::centred, 1);
}

void AudioFilePlayerProcessorEditor::resized()
{
    dragTarget.setBounds (getLocalBounds().withTrimmedTop (getHeight() / 2));
    miniMap.setBounds (getLocalBounds().withHeight (getHeight() / 2));
}
