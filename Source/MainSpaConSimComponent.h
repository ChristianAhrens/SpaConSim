/* Copyright (c) 2021, Christian Ahrens
 *
 * This file is part of SpaConSim <https://github.com/ChristianAhrens/SpaConSim>
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
#include "SimulationVisuComponent.h"

#include "../submodules/JUCE-AppBasics/Source/SplitButtonComponent.h"


namespace SpaConSim
{


//==============================================================================
/*
 */
class MainSpaConSimComponent :   public juce::Component,
                                        public ProtocolBridgingWrapper::Listener,
                                        public Slider::Listener,
                                        public ComboBox::Listener,
                                        public JUCEAppBasics::SplitButtonComponent::Listener,
                                        public Timer
{
public:
    MainSpaConSimComponent();
    ~MainSpaConSimComponent() override;

    //==========================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    //==============================================================================
    void lookAndFeelChanged() override;

    //==========================================================================
    void HandleMessageData(NodeId nodeId, ProtocolId senderProtocolId, RemoteObjectIdentifier Id, const RemoteObjectMessageData& msgData) override;

    //==========================================================================
    void sliderValueChanged(Slider* slider) override;
    
    //==========================================================================
    void comboBoxChanged (ComboBox* comboBox) override;

    //==========================================================================
    void buttonClicked(JUCEAppBasics::SplitButtonComponent* button, std::uint64_t buttonId) override;

    //==========================================================================
    void timerCallback() override;

private:
    //==========================================================================
    class WhiteLineComponent : public Component
    {
    public:
        enum vAlign
        {
            Center,
            Top,
            Bottom
        };

        WhiteLineComponent(vAlign a = Center)
        {
            m_alignment = a;
        }

        //==========================================================================
        void paint(juce::Graphics& g) override
        {
            auto yPos = 0.0f;
            switch (m_alignment)
            {
            case Top:
                yPos = 0.0f;
                break;
            case Bottom:
                yPos = static_cast<float>(getHeight());
                break;
            case Center:
            default:
                yPos = 0.5f * static_cast<float>(getHeight());
                break;
            }

            g.setColour(getLookAndFeel().findColour(TextEditor::outlineColourId));
            g.drawLine(0.0f, yPos, static_cast<float>(getWidth()), yPos, 1.0f);
        }

    private:
        vAlign m_alignment;
    };

    //==========================================================================
    int getSummedUpHeight();

    //==========================================================================
    std::unique_ptr<Label>                      m_speedSliderLabel;
    std::unique_ptr<Slider>                     m_speedSlider;
    
    std::unique_ptr<Label>                      m_channelSimSelectLabel;
    std::unique_ptr<ComboBox>                   m_channelSimSelect;
    
    std::unique_ptr<Label>                      m_recordSimSelectLabel;
    std::unique_ptr<ComboBox>                   m_recordSimSelect;

    std::unique_ptr<WhiteLineComponent>         m_sectionLine1;

    std::unique_ptr<Label>                      m_appInstanceInfoLabel;
    std::unique_ptr<DrawableButton>	            m_helpButton;
    std::unique_ptr<Label>                      m_localSystemInterfacesInfoLabel;
    std::unique_ptr<Label>                      m_listeningPortAnnouncedInfoLabel;
    std::unique_ptr<Label>                      m_clientRemotePortInfoLabel;

    std::unique_ptr<WhiteLineComponent>         m_sectionLine2;

    std::unique_ptr<JUCEAppBasics::SplitButtonComponent>            m_simulationVisuTypeButton;
    std::map<std::uint64_t, SimulationVisuComponent::VisibleType>   m_simulationVisuTypeButtonIds;
    std::unique_ptr<SimulationVisuComponent>                        m_simulationVisu;
    std::unique_ptr<Viewport>					                    m_simulationViewport;
    
    std::unique_ptr<Slider>                     m_performanceMeter;

    std::unique_ptr<ProtocolBridgingWrapper>    m_bridgingWrapper;
    std::uint32_t                               m_bridgingPerformanceInterval{ 400 };
    std::uint64_t                               m_bridgingPerformanceCounter{ 0 };
    std::uint64_t                               m_bridgingPerformanceMaxCount{ 0 };

    const int _rowHeight{ 25 };
    const int _margin{ 5 };
    const int _width{ 320 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainSpaConSimComponent)
};

};
