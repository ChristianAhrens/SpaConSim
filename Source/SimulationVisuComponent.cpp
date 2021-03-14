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

	switch (m_currentVisibleType)
	{
	case VT_SoundObject:
		for (auto const& so : m_soundObjects)
		{
			grid.items.add(GridItem(so.second.get()));
		}
		break;
	case VT_MatrixInput:
		for (auto const& mi : m_matrixInputs)
		{
			grid.items.add(GridItem(mi.second.get()));
		}
		break;
	case VT_MatrixOutput:
		for (auto const& mo : m_matrixOutputs)
		{
			grid.items.add(GridItem(mo.second.get()));
		}
		break;
	case VT_None:
	default:
		break;
	}

    grid.performLayout(getLocalBounds());
}

void SimulationVisuComponent::SetSimulationChannelCount(int channelCount)
{
	m_layoutRows = channelCount / _layoutColumns;

	setSize(_width, _rowHeight * m_layoutRows);

	auto resizeReq = false;

	if (m_currentVisibleType == VT_SoundObject && m_soundObjects.size() != channelCount)
	{
		for (int i = 1; i <= channelCount; i++)
		{
			if (m_soundObjects.count(i) == 0)
			{
				m_soundObjects[i] = std::make_unique<SoundObjectComponent>(String(i));
				addAndMakeVisible(m_soundObjects.at(i).get());
			}
		}

		resizeReq = true;
	}

	if (m_currentVisibleType == VT_MatrixInput && m_matrixInputs.size() != channelCount)
	{
		for (int i = 1; i <= channelCount; i++)
		{
			if (m_matrixInputs.count(i) == 0)
			{
				m_matrixInputs[i] = std::make_unique<MatrixIOComponent>(String(i));
				addAndMakeVisible(m_matrixInputs.at(i).get());
			}
		}

		resizeReq = true;
	}

	if (m_currentVisibleType == VT_MatrixOutput && m_matrixOutputs.size() != channelCount)
	{
		for (int i = 1; i <= channelCount; i++)
		{
			if (m_matrixOutputs.count(i) == 0)
			{
				m_matrixOutputs[i] = std::make_unique<MatrixIOComponent>(String(i));
				addAndMakeVisible(m_matrixOutputs.at(i).get());
			}
		}

		resizeReq = true;
	}

	if (resizeReq)
		resized();
}

void SimulationVisuComponent::SetVisibleType(VisibleType type)
{
	m_currentVisibleType = type;

	resized();
}

void SimulationVisuComponent::Clear()
{
	for (auto const& so : m_soundObjects)
		removeChildComponent(so.second.get());
	m_soundObjects.clear();

	for (auto const& mi : m_matrixInputs)
		removeChildComponent(mi.second.get());
	m_matrixInputs.clear();

	for (auto const& mo : m_matrixOutputs)
		removeChildComponent(mo.second.get());
	m_matrixOutputs.clear();
}

void SimulationVisuComponent::onSimulationUpdated(const std::map<RemoteObjectAddressing, std::map<RemoteObjectIdentifier, std::vector<float>>>& simulationValues)
{
	float xVal = 0.0f;
	float yVal = 0.0f;
	float rvVal = 0.0f;
	float spVal = 0.0f;
	float dmVal = 0.0f;
	float levelVal = 0.0f;
	float gainVal = 0.0f;
	float muteVal = 0.0f;

	for (auto const& simValue : simulationValues)
	{
		auto const& channel				= simValue.first._first;
		auto const& record				= simValue.first._second;
		auto const& remoteObjectsMap	= simValue.second;

		if (m_soundObjects.count(channel) > 0)
		{
			xVal = 0.5f;
			yVal = 0.5f;
			rvVal = 0.0f;
			spVal = 0.5f;
			dmVal = 0.0f;

			if (record == 1)
			{
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

				m_soundObjects.at(channel)->updatePosValues(xVal, yVal);
			}

			if (record == INVALID_ADDRESS_VALUE)
			{
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

				m_soundObjects.at(channel)->updateMetaValues(rvVal, spVal, dmVal);
			}
		}

		if (m_matrixInputs.count(channel) > 0)
		{
			levelVal = 0.4f;
			gainVal = 0.4f;
			muteVal = 1.0f;

			if (record == INVALID_ADDRESS_VALUE)
			{
				if (remoteObjectsMap.count(ROI_MatrixInput_LevelMeterPreMute) > 0 && !remoteObjectsMap.at(ROI_MatrixInput_LevelMeterPreMute).empty())
				{
					levelVal = jmap(remoteObjectsMap.at(ROI_MatrixInput_LevelMeterPreMute).front(),
						ProcessingEngineConfig::GetRemoteObjectRange(ROI_MatrixInput_LevelMeterPreMute).getStart(),
						ProcessingEngineConfig::GetRemoteObjectRange(ROI_MatrixInput_LevelMeterPreMute).getEnd(), 0.0f, 1.0f);
				}
				if (remoteObjectsMap.count(ROI_MatrixInput_Gain) > 0 && !remoteObjectsMap.at(ROI_MatrixInput_Gain).empty())
				{
					gainVal = jmap(remoteObjectsMap.at(ROI_MatrixInput_Gain).front(),
						ProcessingEngineConfig::GetRemoteObjectRange(ROI_MatrixInput_Gain).getStart(),
						ProcessingEngineConfig::GetRemoteObjectRange(ROI_MatrixInput_Gain).getEnd(), 0.0f, 1.0f);
				}
				if (remoteObjectsMap.count(ROI_MatrixInput_Mute) > 0 && !remoteObjectsMap.at(ROI_MatrixInput_Mute).empty())
				{
					muteVal = jmap(remoteObjectsMap.at(ROI_MatrixInput_Mute).front(),
						ProcessingEngineConfig::GetRemoteObjectRange(ROI_MatrixInput_Mute).getStart(),
						ProcessingEngineConfig::GetRemoteObjectRange(ROI_MatrixInput_Mute).getEnd(), 0.0f, 1.0f);
				}

				m_matrixInputs.at(channel)->updateValues(levelVal, gainVal, muteVal);
			}
		}

		if (m_matrixOutputs.count(channel) > 0)
		{
			levelVal = 0.6f;
			gainVal = 0.6f;
			muteVal = 0.0f;

			if (record == INVALID_ADDRESS_VALUE)
			{
				if (remoteObjectsMap.count(ROI_MatrixOutput_LevelMeterPostMute) > 0 && !remoteObjectsMap.at(ROI_MatrixOutput_LevelMeterPostMute).empty())
				{
					levelVal = jmap(remoteObjectsMap.at(ROI_MatrixOutput_LevelMeterPostMute).front(),
						ProcessingEngineConfig::GetRemoteObjectRange(ROI_MatrixOutput_LevelMeterPostMute).getStart(),
						ProcessingEngineConfig::GetRemoteObjectRange(ROI_MatrixOutput_LevelMeterPostMute).getEnd(), 0.0f, 1.0f);
				}
				if (remoteObjectsMap.count(ROI_MatrixOutput_Gain) > 0 && !remoteObjectsMap.at(ROI_MatrixOutput_Gain).empty())
				{
					gainVal = jmap(remoteObjectsMap.at(ROI_MatrixOutput_Gain).front(),
						ProcessingEngineConfig::GetRemoteObjectRange(ROI_MatrixOutput_Gain).getStart(),
						ProcessingEngineConfig::GetRemoteObjectRange(ROI_MatrixOutput_Gain).getEnd(), 0.0f, 1.0f);
				}
				if (remoteObjectsMap.count(ROI_MatrixInput_Mute) > 0 && !remoteObjectsMap.at(ROI_MatrixInput_Mute).empty())
				{
					muteVal = jmap(remoteObjectsMap.at(ROI_MatrixOutput_Mute).front(),
						ProcessingEngineConfig::GetRemoteObjectRange(ROI_MatrixOutput_Mute).getStart(),
						ProcessingEngineConfig::GetRemoteObjectRange(ROI_MatrixOutput_Mute).getEnd(), 0.0f, 1.0f);
				}

				m_matrixOutputs.at(channel)->updateValues(levelVal, gainVal, muteVal);
			}
		}
	}
}

}
