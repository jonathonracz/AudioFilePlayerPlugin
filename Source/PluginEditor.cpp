#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioFilePlayerEditor::AudioFilePlayerEditor(AudioFilePlayerProcessor& p) :
    AudioProcessorEditor(&p),
    processor(p)
{
    addAndMakeVisible(followTransportButton);
    followTransportButton.setButtonText("Follow Transport");
    followTransportButton.addListener(this);

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
    followTransportButton.removeListener(this);
}

//==============================================================================
void AudioFilePlayerEditor::paint(Graphics& g)
{
    g.fillAll(Colours::darkgrey);
}

void AudioFilePlayerEditor::resized()
{
    Rectangle<int> r(getLocalBounds().reduced(4));

    Rectangle<int> controls(r.removeFromBottom(64));

    followTransportButton.setBounds(controls.removeFromTop(25));
    startStopButton.setBounds(controls);

    r.removeFromBottom(6);
    thumbnail->setBounds(r.removeFromBottom(140));
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
    else if (buttonThatWasClicked == &followTransportButton)
    {
        thumbnail->setFollowsTransport(followTransportButton.getToggleState());
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
