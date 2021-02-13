/* Copyright (c) 2021, Christian Ahrens
 *
 * This file is part of SoundscapeOSCSim <https://github.com/ChristianAhrens/SoundscapeOSCSim>
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
/**
 * 
 */
class SimulationVisuComponent : public Component
{
public:
    SimulationVisuComponent();
    ~SimulationVisuComponent() override;

    //==========================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;

    //==========================================================================
    void SetSimulationChannelCount(int channlCount);

    //==============================================================================
    void onSimulationUpdated(const std::map<RemoteObjectAddressing, std::map<RemoteObjectIdentifier, std::vector<float>>>& simulationValues);

private:
    //==========================================================================
    class SoundObjectComponent : public Component
    {
    public:
        SoundObjectComponent()
        {
        }

        void updateValues(float xVal01, float yVal01, float rvVal01, float spVal01, float dmVal01)
        {
            m_xVal01 = xVal01;
            m_yVal01 = yVal01;
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
            g.drawLine(dmFaderArea.getX(), dmFaderArea.getBottom(), dmFaderArea.getX() + (dmFaderArea.getWidth() * m_dmVal01), dmFaderArea.getBottom());
        }

        float m_xVal01{ 0.5f };
        float m_yVal01{ 0.5f };
        float m_rvVal01{ 0.3f };
        float m_spVal01{ 0.5f };
        float m_dmVal01{ 0.7f };
    };
    std::map<int, std::unique_ptr<SimulationVisuComponent::SoundObjectComponent>>    m_soundObjects;

    const int _width{ 310 };
    const int _rowHeight{ 40 };
    const int _layoutColumns{ 8 };
    int m_layoutRows{ 4 };

};

};
