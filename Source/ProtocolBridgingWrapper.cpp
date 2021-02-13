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

#include "ProtocolBridgingWrapper.h"


namespace SoundscapeOSCSim
{

/**
 *
 */
ProtocolBridgingWrapper::ProtocolBridgingWrapper()
	:   m_servus("_osc._udp"),
        m_bridgingXml("SoundscapeOSCSim")
{
	SetupBridgingNode();
    
    m_processingNode.AddListener(this);
    
    m_servus.announce(RX_PORT_DS100_DEVICE, JUCEApplication::getInstance()->getApplicationName().toStdString());
}

/**
 *
 */
ProtocolBridgingWrapper::~ProtocolBridgingWrapper()
{
	auto oh = dynamic_cast<DS100_DeviceSimulation*>(m_processingNode.GetObjectDataHandling());
	if (oh)
		oh->removeListener(this);

	m_listeners.clear();
}

/**
 * Method to register a listener object to be called when the node has received the respective data via a node protocol.
 * @param listener	The listener object to add to the internal list of listeners
 */
void ProtocolBridgingWrapper::AddListener(ProtocolBridgingWrapper::Listener* listener)
{
	if (listener)
		m_listeners.push_back(listener);
}

/**
 * Send a Message out via the active bridging node.
 * @param Id	The id of the remote object to be sent.
 * @param msgData	The message data to be sent.
 * @return True on success, false on failure
 */
bool ProtocolBridgingWrapper::SendMessage(RemoteObjectIdentifier Id, RemoteObjectMessageData& msgData)
{
	return m_processingNode.SendMessageTo(DS100_1_PROCESSINGPROTOCOL_ID, Id, msgData);
}

/**
 * Called when the OSCReceiver receives a new OSC message, since Controller inherits from OSCReceiver::Listener.
 * It forwards the message to all registered Processor objects.
 * @param callbackMessage	The node data to handle encapsulated in a calbackmsg struct..
 */
void ProtocolBridgingWrapper::HandleNodeData(const ProcessingEngineNode::NodeCallbackMessage* callbackMessage)
{
    if (!callbackMessage)
        return;
    
	for (auto l : m_listeners)
        l->HandleMessageData(callbackMessage->_protocolMessage._nodeId, callbackMessage->_protocolMessage._senderProtocolId, callbackMessage->_protocolMessage._Id, callbackMessage->_protocolMessage._msgData);
}

/**
 * Disconnect the active bridging nodes' protocols.
 */
void ProtocolBridgingWrapper::Disconnect()
{
	auto oh = dynamic_cast<DS100_DeviceSimulation*>(m_processingNode.GetObjectDataHandling());
	if (oh)
		oh->removeListener(this);

	m_processingNode.Stop();
}

/**
 * Disconnect and re-connect the OSCSender to a host specified by the current ip settings.
 */
void ProtocolBridgingWrapper::Reconnect()
{
	Disconnect();

	auto nodeXmlElement = m_bridgingXml.getChildByName(ProcessingEngineConfig::getTagName(ProcessingEngineConfig::TagID::NODE));
	if (nodeXmlElement)
		m_processingNode.setStateXml(nodeXmlElement);

	auto oh = dynamic_cast<DS100_DeviceSimulation*>(m_processingNode.GetObjectDataHandling());
	if (oh)
		oh->addListener(this);

	m_processingNode.Start();
}

/**
 * Sets the current simulation interval value to internal xml config and inserts it into processing node.
 * @param intervalValue		The new interval value to set active
 * @return True on success, false on failure
 */
bool ProtocolBridgingWrapper::SetSimulationInterval(int intervalValue)
{
	auto nodeXmlElement = m_bridgingXml.getChildByName(ProcessingEngineConfig::getTagName(ProcessingEngineConfig::TagID::NODE));
	if (nodeXmlElement)
	{
		auto objectHandlingXmlElement = nodeXmlElement->getChildByName(ProcessingEngineConfig::getTagName(ProcessingEngineConfig::TagID::OBJECTHANDLING));
		if (objectHandlingXmlElement)
		{
			auto simRefreshIntervalXmlElement = objectHandlingXmlElement->getChildByName(ProcessingEngineConfig::getTagName(ProcessingEngineConfig::TagID::REFRESHINTERVAL));
			if (simRefreshIntervalXmlElement)
			{
				simRefreshIntervalXmlElement->setAttribute(ProcessingEngineConfig::getAttributeName(ProcessingEngineConfig::AttributeID::INTERVAL), intervalValue);

				Reconnect();

				return true;
			}
			else
				return false;
		}
		else
			return false;
	}
	else
		return false;
}


/**
 * Sets the current simulation channel count value to internal xml config and inserts it into processing node.
 * @param channlCountValue        The new interval value to set active
 * @return True on success, false on failure
 */
bool ProtocolBridgingWrapper::SetSimulationChannelCount(int channlCountValue)
{
    auto nodeXmlElement = m_bridgingXml.getChildByName(ProcessingEngineConfig::getTagName(ProcessingEngineConfig::TagID::NODE));
    if (nodeXmlElement)
    {
        auto objectHandlingXmlElement = nodeXmlElement->getChildByName(ProcessingEngineConfig::getTagName(ProcessingEngineConfig::TagID::OBJECTHANDLING));
        if (objectHandlingXmlElement)
        {
            auto simChCntXmlElement = objectHandlingXmlElement->getChildByName(ProcessingEngineConfig::getTagName(ProcessingEngineConfig::TagID::SIMCHCNT));
            if (simChCntXmlElement)
            {
                simChCntXmlElement->setAttribute(ProcessingEngineConfig::getAttributeName(ProcessingEngineConfig::AttributeID::COUNT), channlCountValue);

                Reconnect();

                return true;
            }
            else
                return false;
        }
        else
            return false;
    }
    else
        return false;
}

/**
 * Sets the current simulation record count value to internal xml config and inserts it into processing node.
 * @param recordCountValue        The new interval value to set active
 * @return True on success, false on failure
 */
bool ProtocolBridgingWrapper::SetSimulationRecordCount(int recordCountValue)
{
    auto nodeXmlElement = m_bridgingXml.getChildByName(ProcessingEngineConfig::getTagName(ProcessingEngineConfig::TagID::NODE));
    if (nodeXmlElement)
    {
        auto objectHandlingXmlElement = nodeXmlElement->getChildByName(ProcessingEngineConfig::getTagName(ProcessingEngineConfig::TagID::OBJECTHANDLING));
        if (objectHandlingXmlElement)
        {
            auto simMapCntXmlElement = objectHandlingXmlElement->getChildByName(ProcessingEngineConfig::getTagName(ProcessingEngineConfig::TagID::SIMMAPCNT));
            if (simMapCntXmlElement)
            {
                simMapCntXmlElement->setAttribute(ProcessingEngineConfig::getAttributeName(ProcessingEngineConfig::AttributeID::COUNT), recordCountValue);

                Reconnect();

                return true;
            }
            else
                return false;
        }
        else
            return false;
    }
    else
        return false;
}

/**
 * Reimplemented from DS100_DeviceSimulation_Listener to be notified of simulation value updates
 * 
 * @param	simulationValues	The updated map of object addressing and values
 */
void ProtocolBridgingWrapper::simulationUpdated(const std::map<RemoteObjectAddressing, std::map<RemoteObjectIdentifier, std::vector<float>>>& simulationValues)
{
	if (onSimulationUpdated)
		onSimulationUpdated(simulationValues);
}

/**
 * Method to create a basic configuration to use to setup the single supported
 * bridging node.
 * @param bridgingProtocolsToActivate	Bitmask definition of what bridging protocols to activate in the bridging node about to be created.
 */
void ProtocolBridgingWrapper::SetupBridgingNode()
{
	auto nodeXmlElement = std::make_unique<XmlElement>(ProcessingEngineConfig::getTagName(ProcessingEngineConfig::TagID::NODE));

	nodeXmlElement->setAttribute(ProcessingEngineConfig::getAttributeName(ProcessingEngineConfig::AttributeID::ID), DEFAULT_PROCNODE_ID);

	auto objectHandlingXmlElement = nodeXmlElement->createNewChildElement(ProcessingEngineConfig::getTagName(ProcessingEngineConfig::TagID::OBJECTHANDLING));
	if (objectHandlingXmlElement)
	{
		objectHandlingXmlElement->setAttribute(ProcessingEngineConfig::getAttributeName(ProcessingEngineConfig::AttributeID::MODE), ProcessingEngineConfig::ObjectHandlingModeToString(OHM_DS100_DeviceSimulation));
        
		auto simChCountXmlElement = objectHandlingXmlElement->createNewChildElement(ProcessingEngineConfig::getTagName(ProcessingEngineConfig::TagID::SIMCHCNT));
		if (simChCountXmlElement)
			simChCountXmlElement->setAttribute(ProcessingEngineConfig::getAttributeName(ProcessingEngineConfig::AttributeID::COUNT), 64);

		auto simMappingsCountXmlElement = objectHandlingXmlElement->createNewChildElement(ProcessingEngineConfig::getTagName(ProcessingEngineConfig::TagID::SIMMAPCNT));
		if (simMappingsCountXmlElement)
			simMappingsCountXmlElement->setAttribute(ProcessingEngineConfig::getAttributeName(ProcessingEngineConfig::AttributeID::COUNT), 4);

		auto simRefreshIntervalXmlElement = objectHandlingXmlElement->createNewChildElement(ProcessingEngineConfig::getTagName(ProcessingEngineConfig::TagID::REFRESHINTERVAL));
		if (simRefreshIntervalXmlElement)
			simRefreshIntervalXmlElement->setAttribute(ProcessingEngineConfig::getAttributeName(ProcessingEngineConfig::AttributeID::INTERVAL), 200);
	}

	// DS100 protocol - RoleA
	auto protocolAXmlElement = nodeXmlElement->createNewChildElement(ProcessingEngineConfig::getTagName(ProcessingEngineConfig::TagID::PROTOCOLA));
	if (protocolAXmlElement)
	{
		protocolAXmlElement->setAttribute(ProcessingEngineConfig::getAttributeName(ProcessingEngineConfig::AttributeID::ID), DS100_1_PROCESSINGPROTOCOL_ID);

		protocolAXmlElement->setAttribute(ProcessingEngineConfig::getAttributeName(ProcessingEngineConfig::AttributeID::TYPE), ProcessingEngineConfig::ProtocolTypeToString(PT_OSCProtocol));
		protocolAXmlElement->setAttribute(ProcessingEngineConfig::getAttributeName(ProcessingEngineConfig::AttributeID::USESACTIVEOBJ), 0);
        
        auto ipAdressXmlElement = protocolAXmlElement->createNewChildElement(ProcessingEngineConfig::getTagName(ProcessingEngineConfig::TagID::IPADDRESS));
        if (ipAdressXmlElement)
            ipAdressXmlElement->setAttribute(ProcessingEngineConfig::getAttributeName(ProcessingEngineConfig::AttributeID::ADRESS), "");

		auto clientPortXmlElement = protocolAXmlElement->createNewChildElement(ProcessingEngineConfig::getTagName(ProcessingEngineConfig::TagID::CLIENTPORT));
		if (clientPortXmlElement)
			clientPortXmlElement->setAttribute(ProcessingEngineConfig::getAttributeName(ProcessingEngineConfig::AttributeID::PORT), RX_PORT_DS100_HOST);

		auto hostPortXmlElement = protocolAXmlElement->createNewChildElement(ProcessingEngineConfig::getTagName(ProcessingEngineConfig::TagID::HOSTPORT));
		if (hostPortXmlElement)
			hostPortXmlElement->setAttribute(ProcessingEngineConfig::getAttributeName(ProcessingEngineConfig::AttributeID::PORT), RX_PORT_DS100_DEVICE);
	}

	m_processingNode.setStateXml(nodeXmlElement.get());

	auto oh = dynamic_cast<DS100_DeviceSimulation*>(m_processingNode.GetObjectDataHandling());
	if (oh)
		oh->addListener(this);

	m_bridgingXml.addChildElement(nodeXmlElement.release());
}

}
