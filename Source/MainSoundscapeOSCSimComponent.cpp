/* Copyright (c) 2020-2021, Christian Ahrens
 *
 * This file is part of SoundscapeBridgeApp <https://github.com/ChristianAhrens/SoundscapeBridgeApp>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "MainSoundscapeOSCSimComponent.h"

#include <JuceHeader.h>

namespace SoundscapeOSCSim
{

//==============================================================================
MainSoundscapeOSCSimComponent::MainSoundscapeOSCSimComponent()
{
    m_speedSlider  = std::make_unique<Slider>(Slider::LinearHorizontal, Slider::TextBoxRight);
    m_speedSlider->setRange(50, 1000, 50);
    m_speedSlider->setValue(200);
    m_speedSlider->addListener(this);
    addAndMakeVisible(m_speedSlider.get());
    m_speedSliderLabel = std::make_unique<Label>("speedSliderLabel", "Simulation update interval (ms)");
    m_speedSliderLabel->attachToComponent(m_speedSlider.get(), false);
    addAndMakeVisible(m_speedSliderLabel.get());
    
    m_channelSimEdit  = std::make_unique<TextEditor>();
    m_channelSimEdit->setInputFilter(this, false);
    m_channelSimEdit->setText("64");
    m_channelSimEdit->addListener(this);
    addAndMakeVisible(m_channelSimEdit.get());
    m_channelSimEditLabel = std::make_unique<Label>("channelSimSliderLabel", "Simulated channel count");
    m_channelSimEditLabel->attachToComponent(m_channelSimEdit.get(), false);
    addAndMakeVisible(m_channelSimEditLabel.get());
    
    m_recordSimEdit  = std::make_unique<TextEditor>();
    m_recordSimEdit->setInputFilter(this, false);
    m_recordSimEdit->setText("1");
    m_recordSimEdit->addListener(this);
    addAndMakeVisible(m_recordSimEdit.get());
    m_recordSimEditLabel = std::make_unique<Label>("recordSimSliderLabel", "Simulated record count");
    m_recordSimEditLabel->attachToComponent(m_recordSimEdit.get(), false);
    addAndMakeVisible(m_recordSimEditLabel.get());
    
    m_performanceMeter = std::make_unique<Slider>(Slider::LinearBar, Slider::TextBoxAbove);
    m_performanceMeter->setRange(0, 1, 1);
    m_performanceMeter->setValue(0);
    m_performanceMeter->setTextValueSuffix(" msg/s");
    m_performanceMeter->setEnabled(false);
    addAndMakeVisible(m_performanceMeter.get());

    m_bridgingWrapper = std::make_unique<ProtocolBridgingWrapper>();
    m_bridgingWrapper->AddListener(this);

    setSize(320, 145);

    startTimer(m_bridgingPerformanceInterval);
}

MainSoundscapeOSCSimComponent::~MainSoundscapeOSCSimComponent()
{

}

void MainSoundscapeOSCSimComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

void MainSoundscapeOSCSimComponent::resized()
{
    auto rowHeight = 25;
    auto bounds = getLocalBounds().reduced(5, 5);
    
    bounds.removeFromTop(rowHeight);
    m_speedSlider->setBounds(bounds.removeFromTop(rowHeight));
    
    bounds.removeFromTop(rowHeight);
    auto editorBounds = bounds.removeFromTop(rowHeight);
    m_channelSimEdit->setBounds(editorBounds.removeFromLeft(0.5f * bounds.getWidth() - 5));
    m_recordSimEdit->setBounds(editorBounds.removeFromRight(0.5f * bounds.getWidth() - 5));
    
    m_performanceMeter->setBounds(bounds.removeFromBottom(rowHeight));
}

void MainSoundscapeOSCSimComponent::HandleMessageData(NodeId nodeId, ProtocolId senderProtocolId, RemoteObjectIdentifier Id, const RemoteObjectMessageData& msgData)
{
    m_bridgingPerformanceCounter++;
}

void MainSoundscapeOSCSimComponent::sliderValueChanged(Slider* slider)
{
    if (m_speedSlider.get() == slider)
    {
        auto intervalValue = static_cast<int>(m_speedSlider->getValue());

        if (m_bridgingWrapper)
            m_bridgingWrapper->SetSimulationInterval(intervalValue);
    }
}

void MainSoundscapeOSCSimComponent::textEditorTextChanged (TextEditor& editor)
{
    if (m_channelSimEdit.get() == &editor)
    {
        auto chCntValue = static_cast<int>(m_channelSimEdit->getText().getIntValue());

        if (m_bridgingWrapper)
            m_bridgingWrapper->SetSimulationChannelCount(chCntValue);
    }
    else if (m_recordSimEdit.get() == &editor)
    {
        auto recCntValue = static_cast<int>(m_recordSimEdit->getText().getIntValue());

        if (m_bridgingWrapper)
            m_bridgingWrapper->SetSimulationRecordCount(recCntValue);
    }
}

String MainSoundscapeOSCSimComponent::filterNewText (TextEditor& editor, const String& newInput)
{
    auto newValue = 0;
    
    if (m_channelSimEdit.get() == &editor)
        newValue = jlimit(1, 128, newInput.getIntValue());
    else if (m_recordSimEdit.get() == &editor)
        newValue = jlimit(1, 4, newInput.getIntValue());
    
    return String(newValue);
}

void MainSoundscapeOSCSimComponent::timerCallback()
{
    if (m_bridgingPerformanceMaxCount < m_bridgingPerformanceCounter)
    {
        m_bridgingPerformanceMaxCount = m_bridgingPerformanceCounter;
        
        m_performanceMeter->setRange(0, m_bridgingPerformanceMaxCount * (1000/m_bridgingPerformanceInterval), 1);
    }
    
    m_performanceMeter->setValue(m_bridgingPerformanceCounter * (1000/m_bridgingPerformanceInterval));

    m_bridgingPerformanceCounter = 0;
}

}
