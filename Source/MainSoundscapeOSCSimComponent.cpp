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

    m_speedSliderLabel = std::make_unique<Label>("speedSliderLabe", "Simulation update interval (ms)");
    m_speedSliderLabel->attachToComponent(m_speedSlider.get(), true);
    addAndMakeVisible(m_speedSliderLabel.get());

    m_bridgingWrapper = std::make_unique<ProtocolBridgingWrapper>();
    m_bridgingWrapper->AddListener(this);

    setSize(320, 240);

    startTimer(500);
}

MainSoundscapeOSCSimComponent::~MainSoundscapeOSCSimComponent()
{

}

void MainSoundscapeOSCSimComponent::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(Colours::black);

    auto performanceRatio = jlimit(0.0f, 1.0f, static_cast<float>(m_bridgingPerformanceCounter / 500));

    auto bounds = getLocalBounds();
    g.setGradientFill(ColourGradient(
        Colours::blue, bounds.getCentreX(), bounds.getBottom(),
        Colours::transparentBlack, bounds.getCentreX(), bounds.getBottom() - (performanceRatio * bounds.getBottom()),
        true));
    g.fillAll();
}

void MainSoundscapeOSCSimComponent::resized()
{
    m_speedSlider->setBounds(getLocalBounds().removeFromTop(40).removeFromRight(getWidth() - 100).reduced(10, 0));
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

void MainSoundscapeOSCSimComponent::timerCallback()
{
    if (m_bridgingPerformanceCounter > 0)
        DBG(String(__FUNCTION__) + " " + String(m_bridgingPerformanceCounter) + " messages received in last 1000ms");
    else
        DBG(String(__FUNCTION__) + " NO messages received in last 1000ms");

    m_bridgingPerformanceCounter = 0;
}

}