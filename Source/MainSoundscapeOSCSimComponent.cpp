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
    
    m_channelSimSelect  = std::make_unique<ComboBox>();
    m_channelSimSelect->addItemList(StringArray{"32", "64", "128"}, 1);
    m_channelSimSelect->addListener(this);
    m_channelSimSelect->setSelectedId(1);
    addAndMakeVisible(m_channelSimSelect.get());
    m_channelSimSelectLabel = std::make_unique<Label>("channelSimSelectLabel", "Simulated channel count");
    m_channelSimSelectLabel->attachToComponent(m_channelSimSelect.get(), false);
    addAndMakeVisible(m_channelSimSelectLabel.get());
    
    m_recordSimSelect  = std::make_unique<ComboBox>();
    m_recordSimSelect->addItemList(StringArray{"1", "4"}, 1);
    m_recordSimSelect->addListener(this);
    m_recordSimSelect->setSelectedId(1);
    addAndMakeVisible(m_recordSimSelect.get());
    m_recordSimSelectLabel = std::make_unique<Label>("recordSimSelectLabel", "Simulated record count");
    m_recordSimSelectLabel->attachToComponent(m_recordSimSelect.get(), false);
    addAndMakeVisible(m_recordSimSelectLabel.get());
    
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
    m_channelSimSelect->setBounds(editorBounds.removeFromLeft(0.5f * bounds.getWidth() - 5));
    m_recordSimSelect->setBounds(editorBounds.removeFromRight(0.5f * bounds.getWidth() - 5));
    
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

void MainSoundscapeOSCSimComponent::comboBoxChanged (ComboBox* comboBox)
{
    if (m_channelSimSelect.get() == comboBox)
    {
        auto chCntValue = 0;
        auto selectedId = m_channelSimSelect->getSelectedId();
        if (selectedId == 1)
            chCntValue = 32;
        else if (selectedId ==2)
            chCntValue = 64;
        else if (selectedId == 2)
            chCntValue = 128;
        
        if (m_bridgingWrapper)
            m_bridgingWrapper->SetSimulationChannelCount(chCntValue);
    }
    else if (m_recordSimSelect.get() == comboBox)
    {
        auto recCntValue = 0;
        auto selectedId = m_recordSimSelect->getSelectedId();
        if (selectedId == 1)
            recCntValue = 1;
        else if (selectedId)
            recCntValue = 4;

        if (m_bridgingWrapper)
            m_bridgingWrapper->SetSimulationRecordCount(recCntValue);
    }
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
