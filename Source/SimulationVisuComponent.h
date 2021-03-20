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

namespace SpaConSim
{

//==============================================================================
/**
 * 
 */
class SimulationVisuComponent : public Component
{
public:	
    enum VisibleType
    {
        VT_None,
        VT_SoundObject,
        VT_MatrixInput,
        VT_MatrixOutput
    };

public:
    SimulationVisuComponent();
    ~SimulationVisuComponent() override;

    //==========================================================================
    void SetSimulationChannelCount(int channlCount);
    void SetVisibleType(VisibleType type);
    void Clear();

    //==========================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;

    //==============================================================================
    void onSimulationUpdated(const std::map<RemoteObjectAddressing, std::map<RemoteObjectIdentifier, std::vector<float>>>& simulationValues);

private:
    //==========================================================================
    class SoundObjectComponent : public Component
    {
    public:
        SoundObjectComponent(String displayName)
            : m_displayName(displayName)
        {
        }

        void updatePosValues(float xVal01, float yVal01)
        {
            m_xVal01 = xVal01;
            m_yVal01 = yVal01;

            repaint();
        }

        void updateMetaValues(float rvVal01, float spVal01, float dmVal01)
        {
            m_rvVal01 = rvVal01;
            m_spVal01 = spVal01;
            m_dmVal01 = dmVal01;

            repaint();
        }

        void paint(juce::Graphics& g) override
        {
            auto rect = getLocalBounds().reduced(2);

            auto xyPadAreaHeight = rect.getHeight() * 0.6f;
            auto fadersAreaHeight = rect.getHeight() * 0.4f;
            auto singleFaderHeight = fadersAreaHeight / 3;

            auto xyPadArea = rect.removeFromTop(xyPadAreaHeight);
            auto rvFaderArea = rect.removeFromTop(singleFaderHeight);
            auto spFaderArea = rect.removeFromTop(singleFaderHeight);
            auto dmFaderArea = rect.removeFromTop(singleFaderHeight);

            // draw a slightly darker text in background with the sound object number ('displayName')
            g.setColour(getLookAndFeel().findColour(Slider::backgroundColourId));
            g.drawText(m_displayName, xyPadArea.reduced(4), Justification::centred);

            // draw a slightly darker frame around the xy pad area
            g.setColour(getLookAndFeel().findColour(TextEditor::outlineColourId).darker());
            // xy pad rectangle
            g.drawRect(xyPadArea);

            // draw the xy cross and rv/sp/dm fader lines in outline colour
            g.setColour(getLookAndFeel().findColour(TextEditor::outlineColourId));
            //xy crosshair lines
            g.drawLine(xyPadArea.getX() + (xyPadArea.getWidth() * m_xVal01), xyPadArea.getY(), xyPadArea.getX() + (xyPadArea.getWidth() * m_xVal01), xyPadArea.getBottom());
            g.drawLine(xyPadArea.getX(), xyPadArea.getBottom() - (xyPadArea.getHeight() * m_yVal01), xyPadArea.getWidth(), xyPadArea.getBottom() - (xyPadArea.getHeight() * m_yVal01));
            // reverb fader line
            g.drawLine(rvFaderArea.getX(), rvFaderArea.getBottom(), rvFaderArea.getX() + (rvFaderArea.getWidth() * m_rvVal01), rvFaderArea.getBottom());
            // spread fader line
            g.drawLine(spFaderArea.getX(), spFaderArea.getBottom(), spFaderArea.getX() + (spFaderArea.getWidth() * m_spVal01), spFaderArea.getBottom());
            // delaymode toggle line
            auto widthOneThird = dmFaderArea.getWidth() / 3;
            auto dmX1 = widthOneThird * (m_dmVal01 * 2);
            auto dmX2 = widthOneThird * (m_dmVal01 * 2 + 1);
            g.drawLine(dmFaderArea.getX() + dmX1, dmFaderArea.getBottom(), dmFaderArea.getX() + dmX2, dmFaderArea.getBottom());
        }

        float m_xVal01{ 0.5f };
        float m_yVal01{ 0.5f };
        float m_rvVal01{ 0.3f };
        float m_spVal01{ 0.5f };
        float m_dmVal01{ 0.7f };

        String m_displayName{ "n" };
    };
    std::map<int, std::unique_ptr<SimulationVisuComponent::SoundObjectComponent>>    m_soundObjects;

    //==========================================================================
    class MatrixIOComponent : public Component
    {
    public:
        MatrixIOComponent(String displayName)
            : m_displayName(displayName)
        {
        }

        void updateValues(float levelVal01, float gainVal01, float muteVal01)
        {
            m_lVal01 = levelVal01;
            m_gVal01 = gainVal01;
            m_mVal01 = muteVal01;

            repaint();
        }

        void paint(juce::Graphics& g) override
        {
            auto rect = getLocalBounds().reduced(2);

            auto channelstripAreaWidth = rect.getHeight() * 0.6f;
            auto lefRightMargin = static_cast<int>(0.5f * (rect.getWidth() - channelstripAreaWidth));

            rect.removeFromLeft(lefRightMargin);
            rect.removeFromRight(lefRightMargin);
            auto channelstripArea = rect;

            // draw a slightly darker frame around the channelstriop area
            g.setColour(getLookAndFeel().findColour(TextEditor::outlineColourId).darker());
            // surronding channelstrip rectangle
            g.drawRect(channelstripArea);

            channelstripArea.reduce(2, 2);
            auto muteButtonRect = channelstripArea.removeFromBottom(channelstripArea.getWidth() / 2).reduced(2);
            auto backgroundNumberRect = channelstripArea;
            auto levelMeterRect = channelstripArea.removeFromLeft(channelstripArea.getWidth() / 2).reduced(2);
            auto gainFaderRect = channelstripArea.reduced(2);

            // draw a slightly darker text in background with the sound object number ('displayName')
            g.setColour(getLookAndFeel().findColour(Slider::backgroundColourId));
            g.drawText(m_displayName, backgroundNumberRect, Justification::centred);

            // draw the levelmeter and mute button areas in a slightly darker outline colour
            g.setColour(getLookAndFeel().findColour(TextEditor::outlineColourId).darker());
            //levelmeter and mute button rects
            g.drawRect(levelMeterRect);
            g.drawRect(muteButtonRect);
            
            // fill the levelmeter, mutebutton and draw the gain fader in outline colour
            g.setColour(getLookAndFeel().findColour(TextEditor::outlineColourId));
            
            // level meter fill
            auto levelMeterFillRect = levelMeterRect.reduced(1);
            auto levelMeterValuePos = static_cast<float>(levelMeterFillRect.getHeight()) * m_lVal01;
            g.fillRect(levelMeterFillRect.removeFromBottom(static_cast<int>(levelMeterValuePos)));

            // mute button fill
            auto muteButtonFillRect = muteButtonRect.reduced(1);
            if (m_mVal01 > 0.5f)
                g.fillRect(muteButtonFillRect);

            // gain fader
            auto gainFaderValuePos = static_cast<float>(gainFaderRect.getHeight()) * m_gVal01;
            auto valueYPosition = static_cast<float>(gainFaderRect.getBottom()) - gainFaderValuePos;
            g.drawLine(gainFaderRect.getCentreX(), gainFaderRect.getBottom(), gainFaderRect.getCentreX(), valueYPosition);
            g.drawLine(gainFaderRect.getX(), valueYPosition,      gainFaderRect.getRight(), valueYPosition);
            g.drawLine(gainFaderRect.getX(), valueYPosition + 1,  gainFaderRect.getRight(), valueYPosition + 1);
            g.drawLine(gainFaderRect.getX(), valueYPosition - 1,  gainFaderRect.getRight(), valueYPosition - 1);
        }

        float m_lVal01{ 0.5f };
        float m_gVal01{ 0.5f };
        float m_mVal01{ 1.0f };

        String m_displayName{ "n" };
    };
    std::map<int, std::unique_ptr<SimulationVisuComponent::MatrixIOComponent>>    m_matrixInputs;
    std::map<int, std::unique_ptr<SimulationVisuComponent::MatrixIOComponent>>    m_matrixOutputs;

    const int _width{ 310 };
    const int _rowHeight{ 40 };
    const int _layoutColumns{ 8 };
    int m_layoutRows{ 4 };
    String  m_displayName{ "None" };
    VisibleType m_currentVisibleType{ VT_None };
    

};

};
