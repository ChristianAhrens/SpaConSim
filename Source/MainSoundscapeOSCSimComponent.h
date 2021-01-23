/* Copyright (c) 2021, Christian Ahrens
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

#pragma once

#include <JuceHeader.h>

#include "ProtocolBridgingWrapper.h"


namespace SoundscapeOSCSim
{

//==============================================================================
/*
 */
class MainSoundscapeOSCSimComponent :   public juce::Component,
                                        public ProtocolBridgingWrapper::Listener,
                                        public Slider::Listener,
                                        public Timer
{
public:
    MainSoundscapeOSCSimComponent();
    ~MainSoundscapeOSCSimComponent() override;

    //==========================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    //==========================================================================
    void HandleMessageData(NodeId nodeId, ProtocolId senderProtocolId, RemoteObjectIdentifier Id, const RemoteObjectMessageData& msgData) override;

    //==========================================================================
    void sliderValueChanged(Slider* slider) override;

    //==========================================================================
    void timerCallback() override;

private:
    std::unique_ptr<Label>      m_speedSliderLabel;
    std::unique_ptr<Slider>     m_speedSlider;

    std::unique_ptr<ProtocolBridgingWrapper>    m_bridgingWrapper;
    std::uint64_t                               m_bridgingPerformanceCounter{ 0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainSoundscapeOSCSimComponent)
};

};