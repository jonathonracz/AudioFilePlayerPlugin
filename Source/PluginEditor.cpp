/*
    PluginEditor.cpp
    Copyright (C) 2017 Jonathon Racz, ROLI Ltd.

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

#include "PluginProcessor.h"
#include "PluginEditor.h"

AudioFilePlayerEditor::AudioFilePlayerEditor(AudioFilePlayerProcessor& p) :
    AudioProcessorEditor(&p),
    processor(p)
{
    thumbnail = new AudioThumbnailComp(processor.formatManager, processor.transportSource, processor.thumbnailCache, processor.currentlyLoadedFile);
    addAndMakeVisible(thumbnail);
    thumbnail->addChangeListener(this);

    addAndMakeVisible(startStopButton);
    startStopButton.setButtonText("Play/Stop");
    startStopButton.addListener(this);
    startStopButton.setColour(TextButton::buttonColourId, Colour(0xff79ed7f));

    setOpaque(true);

    setSize(512, 220);
}

AudioFilePlayerEditor::~AudioFilePlayerEditor()
{
    thumbnail->removeChangeListener(this);
}

void AudioFilePlayerEditor::paint(Graphics& g)
{
    g.fillAll(Colours::grey);
}

void AudioFilePlayerEditor::resized()
{
    Rectangle<int> r(getLocalBounds().reduced(4));

    Rectangle<int> controls(r.removeFromBottom(32));

    startStopButton.setBounds(controls);

    r.removeFromBottom(6);
    thumbnail->setBounds(r.removeFromBottom(180));
    r.removeFromBottom(6);
}

void AudioFilePlayerEditor::buttonClicked (Button* buttonThatWasClicked) 
{
    if (buttonThatWasClicked == &startStopButton)
    {
        if (processor.transportSource.isPlaying())
        {
            processor.transportSource.stop();
        }
        else
        {
            processor.transportSource.setPosition(0);
            processor.transportSource.start();
        }
    }
}

void AudioFilePlayerEditor::changeListenerCallback(ChangeBroadcaster* source) 
{
    if (source == thumbnail)
    {
        processor.loadFileIntoTransport(thumbnail->getLastDroppedFile());
        thumbnail->setFile(thumbnail->getLastDroppedFile());
    }
}
