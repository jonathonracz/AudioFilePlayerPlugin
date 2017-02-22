/*
    AudioThumbnailComp.cpp
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

#include "AudioThumbnailComp.h"

AudioThumbnailComp::AudioThumbnailComp(
    AudioFormatManager& formatManager,
    AudioTransportSource& transport,
    AudioThumbnailCache& thumbCache,
    const File& existingFile) :
        transportSource(transport),
        scrollbar(false),
        thumbnail(512, formatManager, thumbCache),
        isFollowingTransport(false)
{
    thumbnail.addChangeListener(this);

    addAndMakeVisible(scrollbar);
    scrollbar.setRangeLimits(visibleRange);
    scrollbar.setAutoHide(false);
    scrollbar.addListener(this);

    currentPositionMarker.setFill(Colours::white.withAlpha(0.85f));
    addAndMakeVisible(currentPositionMarker);

    setFile(existingFile);
}

AudioThumbnailComp::~AudioThumbnailComp()
{
    scrollbar.removeListener(this);
    thumbnail.removeChangeListener(this);
}

void AudioThumbnailComp::setFile(const File& file)
{
    if (file.existsAsFile())
    {
        thumbnail.setSource(new FileInputSource(file));
        const Range<double> newRange(0.0, thumbnail.getTotalLength());
        scrollbar.setRangeLimits(newRange);
        setRange(newRange);

        startTimerHz(40);
    }
}

File AudioThumbnailComp::getLastDroppedFile() const noexcept
{
    return lastFileDropped;
}

void AudioThumbnailComp::setZoomFactor(double amount)
{
    if (thumbnail.getTotalLength() > 0)
    {
        const double newScale = jmax(0.001, thumbnail.getTotalLength() * (1.0 - jlimit(0.0, 0.99, amount)));
        const double timeAtCentre = xToTime(getWidth() / 2.0f);
        setRange(Range<double>(timeAtCentre - newScale * 0.5, timeAtCentre + newScale * 0.5));
    }
}

void AudioThumbnailComp::setRange(Range<double> newRange)
{
    visibleRange = newRange;
    scrollbar.setCurrentRange(visibleRange);
    updateCursorPosition();
    repaint();
}

void AudioThumbnailComp::setFollowsTransport(bool shouldFollow)
{
    isFollowingTransport = shouldFollow;
}

void AudioThumbnailComp::paint(Graphics& g)
{
    g.fillAll(Colours::darkgrey);
    g.setColour(Colours::lightblue);

    if (thumbnail.getTotalLength() > 0.0)
    {
        Rectangle<int> thumbArea(getLocalBounds());
        thumbArea.removeFromBottom(scrollbar.getHeight() + 4);
        thumbnail.drawChannels(g, thumbArea.reduced (2),
            visibleRange.getStart(), visibleRange.getEnd(), 1.0f);
    }
    else
    {
        g.setFont(14.0f);
        g.drawFittedText("(No audio file selected)", getLocalBounds(), Justification::centred, 2);
    }
}

void AudioThumbnailComp::resized()
{
    scrollbar.setBounds(getLocalBounds().removeFromBottom(14).reduced(2));
}

void AudioThumbnailComp::changeListenerCallback(ChangeBroadcaster*)
{
    // this method is called by the thumbnail when it has changed, so we should repaint it..
    repaint();
}

void AudioThumbnailComp::filesDropped(const StringArray& files, int /*x*/, int /*y*/)
{
    lastFileDropped = File(files[0]);
    sendChangeMessage();
}

void AudioThumbnailComp::mouseDown(const MouseEvent& e)
{
    mouseDrag(e);
}

void AudioThumbnailComp::mouseDrag(const MouseEvent& e)
{
    if (canMoveTransport())
        transportSource.setPosition(jmax(0.0, xToTime((float)e.x)));
}

void AudioThumbnailComp::mouseUp(const MouseEvent&)
{
    transportSource.start();
}

void AudioThumbnailComp::mouseWheelMove(const MouseEvent&, const MouseWheelDetails& wheel)
{
    if (thumbnail.getTotalLength() > 0.0)
    {
        double newStart = visibleRange.getStart() - wheel.deltaX * (visibleRange.getLength()) / 10.0;
        newStart = jlimit(0.0, jmax(0.0, thumbnail.getTotalLength() - (visibleRange.getLength())), newStart);

        if (canMoveTransport())
            setRange(Range<double>(newStart, newStart + visibleRange.getLength()));

        repaint();
    }
}

float AudioThumbnailComp::timeToX(const double time) const
{
    return getWidth() * (float)((time - visibleRange.getStart()) / (visibleRange.getLength()));
}

double AudioThumbnailComp::xToTime(const float x) const
{
    return (x / getWidth()) * (visibleRange.getLength()) + visibleRange.getStart();
}

bool AudioThumbnailComp::canMoveTransport() const noexcept
{
    return !(isFollowingTransport && transportSource.isPlaying());
}

void AudioThumbnailComp::scrollBarMoved(ScrollBar* scrollBarThatHasMoved, double newRangeStart)
{
    if (scrollBarThatHasMoved == &scrollbar)
        if (!(isFollowingTransport && transportSource.isPlaying()))
            setRange(visibleRange.movedToStartAt(newRangeStart));
}

void AudioThumbnailComp::timerCallback()
{
    if (canMoveTransport())
        updateCursorPosition();
    else
        setRange(visibleRange.movedToStartAt(transportSource.getCurrentPosition() - (visibleRange.getLength() / 2.0)));
}

void AudioThumbnailComp::updateCursorPosition()
{
    currentPositionMarker.setVisible(transportSource.isPlaying() || isMouseButtonDown());

    currentPositionMarker.setRectangle(Rectangle<float>(timeToX(transportSource.getCurrentPosition()) - 0.75f, 0,
        1.5f, (float)(getHeight() - scrollbar.getHeight())));
}
