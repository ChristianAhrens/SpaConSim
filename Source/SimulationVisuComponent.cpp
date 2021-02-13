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

#include "SimulationVisuComponent.h"


namespace SoundscapeOSCSim
{

//==============================================================================
SimulationVisuComponent::SimulationVisuComponent()
{
	setSize(_width, _rowHeight * m_layoutRows);
}

SimulationVisuComponent::~SimulationVisuComponent()
{

}

void SimulationVisuComponent::paint(juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

void SimulationVisuComponent::resized()
{
    Grid grid;
    grid.alignItems = Grid::AlignItems::center;
    grid.alignContent = Grid::AlignContent::center;

	for (int i = 0; i < _layoutColumns; i++)
		grid.templateColumns.add(Grid::TrackInfo(1_fr));
	for (int i = 0; i < m_layoutRows; i++)
		grid.templateRows.add(Grid::TrackInfo(1_fr));

    for (auto const& so : m_soundObjects)
    {
        grid.items.add(GridItem(so.second.get()));
    }

    grid.performLayout(getLocalBounds());
}

void SimulationVisuComponent::SetSimulationChannelCount(int channelCount)
{
	m_layoutRows = channelCount / _layoutColumns;

	setSize(_width, _rowHeight * m_layoutRows);

	if (m_soundObjects.size() != channelCount)
	{
		for (int i = 1; i <= channelCount; i++)
		{
			if (m_soundObjects.count(i) == 0)
			{
				m_soundObjects[i] = std::make_unique<SoundObjectComponent>();
				addAndMakeVisible(m_soundObjects.at(i).get());
			}
		}

		resized();
	}
}

void SimulationVisuComponent::onSimulationUpdated(const std::map<RemoteObjectAddressing, std::map<RemoteObjectIdentifier, std::vector<float>>>& simulationValues)
{
	float xVal = 0.0f;
	float yVal = 0.0f;
	float rvVal = 0.0f;
	float spVal = 0.0f;
	float dmVal = 0.0f;

	for (auto const& simValue : simulationValues)
	{
		auto const& channel				= simValue.first._first;
		auto const& remoteObjectsMap	= simValue.second;

		if (m_soundObjects.count(channel) > 0)
		{
			xVal = 0.0f;
			yVal = 0.0f;
			rvVal = 0.0f;
			spVal = 0.0f;
			dmVal = 0.0f;

			if (remoteObjectsMap.count(ROI_CoordinateMapping_SourcePosition_X) > 0 && !remoteObjectsMap.at(ROI_CoordinateMapping_SourcePosition_X).empty())
			{
				xVal = jmap(remoteObjectsMap.at(ROI_CoordinateMapping_SourcePosition_X).front(),
					ProcessingEngineConfig::GetRemoteObjectRange(ROI_CoordinateMapping_SourcePosition_X).getStart(),
					ProcessingEngineConfig::GetRemoteObjectRange(ROI_CoordinateMapping_SourcePosition_X).getEnd(), 0.0f, 1.0f);
			}
			if (remoteObjectsMap.count(ROI_CoordinateMapping_SourcePosition_Y) > 0 && !remoteObjectsMap.at(ROI_CoordinateMapping_SourcePosition_Y).empty())
			{
				yVal = jmap(remoteObjectsMap.at(ROI_CoordinateMapping_SourcePosition_Y).front(),
					ProcessingEngineConfig::GetRemoteObjectRange(ROI_CoordinateMapping_SourcePosition_Y).getStart(),
					ProcessingEngineConfig::GetRemoteObjectRange(ROI_CoordinateMapping_SourcePosition_Y).getEnd(), 0.0f, 1.0f);
			}
			if (remoteObjectsMap.count(ROI_MatrixInput_ReverbSendGain) > 0 && !remoteObjectsMap.at(ROI_MatrixInput_ReverbSendGain).empty())
			{
				rvVal = jmap(remoteObjectsMap.at(ROI_MatrixInput_ReverbSendGain).front(),
					ProcessingEngineConfig::GetRemoteObjectRange(ROI_MatrixInput_ReverbSendGain).getStart(),
					ProcessingEngineConfig::GetRemoteObjectRange(ROI_MatrixInput_ReverbSendGain).getEnd(), 0.0f, 1.0f);
			}
			if (remoteObjectsMap.count(ROI_Positioning_SourceSpread) > 0 && !remoteObjectsMap.at(ROI_Positioning_SourceSpread).empty())
			{
				spVal = jmap(remoteObjectsMap.at(ROI_Positioning_SourceSpread).front(),
					ProcessingEngineConfig::GetRemoteObjectRange(ROI_Positioning_SourceSpread).getStart(),
					ProcessingEngineConfig::GetRemoteObjectRange(ROI_Positioning_SourceSpread).getEnd(), 0.0f, 1.0f);
			}
			if (remoteObjectsMap.count(ROI_Positioning_SourceDelayMode) > 0 && !remoteObjectsMap.at(ROI_Positioning_SourceDelayMode).empty())
			{
				dmVal = jmap(remoteObjectsMap.at(ROI_Positioning_SourceDelayMode).front(),
					ProcessingEngineConfig::GetRemoteObjectRange(ROI_Positioning_SourceDelayMode).getStart(),
					ProcessingEngineConfig::GetRemoteObjectRange(ROI_Positioning_SourceDelayMode).getEnd(), 0.0f, 1.0f);
			}

			m_soundObjects.at(channel)->updateValues(xVal, yVal, rvVal, spVal, dmVal);
		}
	}
}

}
