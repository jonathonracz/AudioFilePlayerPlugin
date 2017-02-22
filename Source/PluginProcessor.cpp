/*
    PluginProcessor.cpp
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

AudioFilePlayerProcessor::AudioFilePlayerProcessor() :
    AudioProcessor(BusesProperties().withOutput("Output", AudioChannelSet::stereo())),
    thumbnailCache(1),
    readAheadThread("transport read ahead")
{
    formatManager.registerBasicFormats();
    readAheadThread.startThread(3);
}

AudioFilePlayerProcessor::~AudioFilePlayerProcessor()
{
    transportSource.setSource(nullptr);
}

const String AudioFilePlayerProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioFilePlayerProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioFilePlayerProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

double AudioFilePlayerProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioFilePlayerProcessor::getNumPrograms()
{
    return 1;
}

int AudioFilePlayerProcessor::getCurrentProgram()
{
    return 0;
}

void AudioFilePlayerProcessor::setCurrentProgram(int index)
{
}

const String AudioFilePlayerProcessor::getProgramName(int index)
{
    return String();
}

void AudioFilePlayerProcessor::changeProgramName(int index, const String& newName)
{
}

void AudioFilePlayerProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    transportSource.prepareToPlay(samplesPerBlock, sampleRate);
}

void AudioFilePlayerProcessor::releaseResources()
{
    transportSource.releaseResources();
}

void AudioFilePlayerProcessor::processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    for(int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    transportSource.getNextAudioBlock(AudioSourceChannelInfo(buffer));
}

bool AudioFilePlayerProcessor::hasEditor() const
{
    return true;
}

AudioProcessorEditor* AudioFilePlayerProcessor::createEditor()
{
    return new AudioFilePlayerEditor(*this);
}

void AudioFilePlayerProcessor::getStateInformation(MemoryBlock& destData)
{
    XmlElement xml("plugin-settings");

    xml.setAttribute("audiofile", currentlyLoadedFile.getFullPathName());

    copyXmlToBinary(xml, destData);
}

void AudioFilePlayerProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    ScopedPointer<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr)
    {
        if (xmlState->hasTagName("plugin-settings"))
        {
            currentlyLoadedFile = File::createFileWithoutCheckingPath(xmlState->getStringAttribute("audiofile"));
            if (currentlyLoadedFile.existsAsFile())
            {
                loadFileIntoTransport(currentlyLoadedFile);
            }
        }
    }
}

AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioFilePlayerProcessor();
}

void AudioFilePlayerProcessor::loadFileIntoTransport(const File& audioFile)
{
    // unload the previous file source and delete it..
    transportSource.stop();
    transportSource.setSource(nullptr);
    currentAudioFileSource = nullptr;

    AudioFormatReader* reader = formatManager.createReaderFor(audioFile);
    currentlyLoadedFile = audioFile;

    if (reader != nullptr)
    {
        currentAudioFileSource = new AudioFormatReaderSource(reader, true);

        // ..and plug it into our transport source
        transportSource.setSource(
            currentAudioFileSource,
            32768,                   // tells it to buffer this many samples ahead
            &readAheadThread,        // this is the background thread to use for reading-ahead
            reader->sampleRate);     // allows for sample rate correction
    }
}
