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

AudioFilePlayerProcessor::AudioFilePlayerProcessor()
    : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
      state (IDs::StateRoot),
      audioStreamThread ("Audio Disk Streaming"),
      thumbnailCache (std::numeric_limits<int>::max()),
      audioFileName (state, IDs::AudioFileName, nullptr, {}),
      audioFileLoaded (state, IDs::AudioFileLoaded, nullptr, false),
      audioFileSampleRate (state, IDs::AudioFileSampleRate, nullptr, 0.0),
      audioFileLengthSeconds (state, IDs::AudioFileLengthSeconds, nullptr, 0.0),
      audioFileLeftViewSecond (state, IDs::AudioFileLeftViewSecond, nullptr, 0.0),
      audioFileRightViewSecond (state, IDs::AudioFileRightViewSecond, nullptr, 0.0)
{
    formatManager.registerBasicFormats();
    state.addListener (this);
    audioStreamThread.startThread();
}

AudioFilePlayerProcessor::~AudioFilePlayerProcessor()
{
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

bool AudioFilePlayerProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

void AudioFilePlayerProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    audioTransport.prepareToPlay (samplesPerBlock, sampleRate);
}

void AudioFilePlayerProcessor::releaseResources()
{
    audioTransport.releaseResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AudioFilePlayerProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void AudioFilePlayerProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;

    int numReaderChannels = (audioSource) ? audioSource->getAudioFormatReader()->numChannels : 0;
    for (int i = numReaderChannels; i < buffer.getNumChannels(); ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    audioTransport.getNextAudioBlock ({ &buffer, 0, buffer.getNumSamples() });
}

AudioProcessorEditor* AudioFilePlayerProcessor::createEditor()
{
    return new AudioFilePlayerProcessorEditor (*this);
}

void AudioFilePlayerProcessor::getStateInformation (MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> stateXml (state.createXml());
    if (stateXml)
        AudioProcessor::copyXmlToBinary (*stateXml, destData);
}

void AudioFilePlayerProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> stateXml (AudioProcessor::getXmlFromBinary (data, sizeInBytes));
    if (stateXml)
        state = ValueTree::fromXml (*stateXml);
}

void AudioFilePlayerProcessor::valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property)
{
    if (property == IDs::AudioFileName)
    {
        suspendProcessing (true);

        audioFileName.forceUpdateOfCachedValue();
        audioFileLoaded = false;
        audioFileSampleRate = 0.0;
        audioFileLengthSeconds = 0.0;
        audioFileLeftViewSecond = 0.0;
        audioFileRightViewSecond = 0.0;

        audioTransport.setSource (nullptr);
        audioSource.reset();
        thumbnailCache.clear();

        AudioFormatReader* rawReader = formatManager.createReaderFor (File (audioFileName));
        if (rawReader)
        {
            audioSource = std::make_unique<AudioFormatReaderSource> (rawReader, true);
            audioTransport.setSource (audioSource.get(), 32768, &audioStreamThread, rawReader->sampleRate, rawReader->numChannels);
            audioFileSampleRate = rawReader->sampleRate;
            audioFileLoaded = true;
            audioFileLengthSeconds = audioTransport.getLengthInSeconds();
            audioFileLeftViewSecond = 0.0;
            audioFileRightViewSecond = audioFileLengthSeconds.get();
        }

        suspendProcessing (false);
    }
}

AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioFilePlayerProcessor();
}
