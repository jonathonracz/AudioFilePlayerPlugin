#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioFilePlayerProcessor::AudioFilePlayerProcessor() :
    readAheadThread("transport read ahead"),
    thumbnailCache(1),
    AudioProcessor(BusesProperties().withOutput("Output", AudioChannelSet::stereo()))
{
    formatManager.registerBasicFormats();
    readAheadThread.startThread(3);
}

AudioFilePlayerProcessor::~AudioFilePlayerProcessor()
{
    transportSource.setSource(nullptr);
}

//==============================================================================
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
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
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

//==============================================================================
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

//==============================================================================
bool AudioFilePlayerProcessor::hasEditor() const
{
    return true;
}

AudioProcessorEditor* AudioFilePlayerProcessor::createEditor()
{
    return new AudioFilePlayerEditor(*this);
}

//==============================================================================
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

//==============================================================================
// This creates new instances of the plugin..
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
