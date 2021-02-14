/* Copyright (c) 2020-2021, Christian Ahrens
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

#include "MainSoundscapeOSCSimComponent.h"

#include <Image_utils.h>
#include <iOS_utils.h>

namespace SoundscapeOSCSim
{

//==============================================================================
MainSoundscapeOSCSimComponent::MainSoundscapeOSCSimComponent()
{
    m_speedSlider  = std::make_unique<Slider>(Slider::LinearHorizontal, Slider::TextBoxRight);
    m_speedSlider->setRange(0, 1000, 5);
    m_speedSlider->setSkewFactor(0.5f);
    m_speedSlider->setValue(m_bridgingPerformanceInterval);
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

    m_sectionLine1 = std::make_unique<WhiteLineComponent>(WhiteLineComponent::vAlign::Bottom);
    addAndMakeVisible(m_sectionLine1.get());
    
    m_appInstanceInfoLabel = std::make_unique<Label>("appInstanceInfo", JUCEApplication::getInstance()->getApplicationName() + String(" v") + String(JUCE_STRINGIFY(JUCE_APP_VERSION)));
    addAndMakeVisible(m_appInstanceInfoLabel.get());
    
    m_helpButton = std::make_unique<DrawableButton>("Help", DrawableButton::ButtonStyle::ImageFitted);
    m_helpButton->onClick = [this] {
        auto githubURL = String("https://www.github.com");
        auto companyName = String("ChristianAhrens");
        auto appName = JUCEApplication::getInstance()->getApplicationName();
        auto repoBasePath = String("blob/master");
        auto URLString = githubURL + "/" + companyName + "/" + appName + "/" + repoBasePath + "/";
        auto helpURLString = URLString + "README.md";
        URL(helpURLString).launchInDefaultBrowser();
    };
    addAndMakeVisible(m_helpButton.get());

    m_localSystemInterfacesInfoLabel = std::make_unique<Label>("localSystemInterfacesInfo", "- System's main IP is " + juce::IPAddress::getLocalAddress().toString());
    addAndMakeVisible(m_localSystemInterfacesInfoLabel.get());

    m_listeningPortAnnouncedInfoLabel = std::make_unique<Label>("listeningPortAnnouncedInfo", "- Listening on port " + String(RX_PORT_DS100_HOST));
    addAndMakeVisible(m_listeningPortAnnouncedInfoLabel.get());

    m_clientRemotePortInfoLabel = std::make_unique<Label>("clientRemotePortInfo", "- Sending data replies to remote client port " + String(RX_PORT_DS100_DEVICE));
    addAndMakeVisible(m_clientRemotePortInfoLabel.get());

    m_sectionLine2 = std::make_unique<WhiteLineComponent>(WhiteLineComponent::vAlign::Top);
    addAndMakeVisible(m_sectionLine2.get());

    m_simulationVisu = std::make_unique<SimulationVisuComponent>();
    m_simulationViewport = std::make_unique<Viewport>();
    m_simulationViewport->setViewedComponent(m_simulationVisu.get(), false);
    addAndMakeVisible(m_simulationViewport.get());  
    
    m_performanceMeter = std::make_unique<Slider>(Slider::LinearBar, Slider::TextBoxAbove);
    m_performanceMeter->setRange(0, 1, 1);
    m_performanceMeter->setValue(0);
    m_performanceMeter->setTextValueSuffix(" msg/s");
    m_performanceMeter->setEnabled(false);
    addAndMakeVisible(m_performanceMeter.get());

    m_bridgingWrapper = std::make_unique<ProtocolBridgingWrapper>();
    m_bridgingWrapper->onSimulationUpdated = [this](const std::map<RemoteObjectAddressing, std::map<RemoteObjectIdentifier, std::vector<float>>>& simulationValues) { this->m_simulationVisu->onSimulationUpdated(simulationValues); };
    m_bridgingWrapper->AddListener(this);
    
#if JUCE_IOS || JUCE_ANDROID
    setSize(_width, _width);
#else
    auto height = 12 * _rowHeight + 2 * _margin + m_simulationVisu->getHeight();
    setSize(_width, height);
#endif

    lookAndFeelChanged();

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
    auto safety = JUCEAppBasics::iOS_utils::getDeviceSafetyMargins();
    auto safeBounds = getLocalBounds();
    safeBounds.removeFromTop(safety._top);
    safeBounds.removeFromBottom(safety._bottom);
    safeBounds.removeFromLeft(safety._left);
    safeBounds.removeFromRight(safety._right);
    
    auto bounds = safeBounds.reduced(_margin, _margin);
    
    bounds.removeFromTop(_rowHeight);
    m_speedSlider->setBounds(bounds.removeFromTop(_rowHeight));
    
    bounds.removeFromTop(_rowHeight);
    auto editorBounds = bounds.removeFromTop(_rowHeight);
    m_channelSimSelect->setBounds(editorBounds.removeFromLeft((bounds.getWidth() / 2) - _margin));
    m_recordSimSelect->setBounds(editorBounds.removeFromRight((bounds.getWidth() / 2) - _margin));

    m_sectionLine1->setBounds(bounds.removeFromTop(_rowHeight));

    auto appInfoRowBounds = bounds.removeFromTop(_rowHeight);
    m_helpButton->setBounds(appInfoRowBounds.removeFromRight(_rowHeight));
    m_appInstanceInfoLabel->setBounds(appInfoRowBounds);
    
    m_localSystemInterfacesInfoLabel->setBounds(bounds.removeFromTop(_rowHeight));

    m_listeningPortAnnouncedInfoLabel->setBounds(bounds.removeFromTop(_rowHeight));

    m_clientRemotePortInfoLabel->setBounds(bounds.removeFromTop(_rowHeight));

    m_sectionLine2->setBounds(bounds.removeFromTop(_rowHeight));

    m_performanceMeter->setBounds(bounds.removeFromBottom(_rowHeight));
    bounds.removeFromBottom(_rowHeight);

    auto embeddedVisuBounds = Rectangle<int>(bounds.getWidth(), m_simulationVisu->getHeight());
    m_simulationVisu->setBounds(embeddedVisuBounds);
    m_simulationViewport->setBounds(bounds);

    // In case the area of the simulation does not fit into viewport,
    // the scrollbars are going to appear and to avoid the horizontal one,
    // we simply shrink the simulation width a bit (it adapts to forced width anyways)
    if (embeddedVisuBounds.getHeight() > bounds.getHeight())
        m_simulationVisu->setBounds(Rectangle<int>(
            embeddedVisuBounds.getWidth() - m_simulationViewport->getVerticalScrollBar().getWidth(), 
            embeddedVisuBounds.getHeight()));
}

void MainSoundscapeOSCSimComponent::lookAndFeelChanged()
{
    Component::lookAndFeelChanged();

    auto colourOn = getLookAndFeel().findColour(TextButton::ColourIds::textColourOnId);
    auto colourOff = getLookAndFeel().findColour(TextButton::ColourIds::textColourOffId);

    std::unique_ptr<juce::Drawable> NormalImage, OverImage, DownImage, DisabledImage, NormalOnImage, OverOnImage, DownOnImage, DisabledOnImage;
    JUCEAppBasics::Image_utils::getDrawableButtonImages(BinaryData::help24px_svg, NormalImage, OverImage, DownImage, DisabledImage, NormalOnImage, OverOnImage, DownOnImage, DisabledOnImage,
        colourOn, colourOff, colourOff, colourOff, colourOn, colourOn, colourOn, colourOn);

    m_helpButton->setImages(NormalImage.get(), OverImage.get(), DownImage.get(), DisabledImage.get(), NormalOnImage.get(), OverOnImage.get(), DownOnImage.get(), DisabledOnImage.get());
}

void MainSoundscapeOSCSimComponent::HandleMessageData(NodeId nodeId, ProtocolId senderProtocolId, RemoteObjectIdentifier Id, const RemoteObjectMessageData& msgData)
{
    ignoreUnused(nodeId);
    ignoreUnused(senderProtocolId);
    ignoreUnused(Id);
    ignoreUnused(msgData);

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
        else if (selectedId == 3)
            chCntValue = 128;
        
        if (m_bridgingWrapper)
            m_bridgingWrapper->SetSimulationChannelCount(chCntValue);

        if (m_simulationVisu)
            m_simulationVisu->SetSimulationChannelCount(chCntValue);

#if JUCE_IOS || JUCE_ANDROID
        resized();
#else
        auto height = 12 * _rowHeight + 2 * _margin + m_simulationVisu->getHeight();
        setSize(_width, height);
#endif
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
    if (m_bridgingPerformanceInterval > 0)
    {
        auto perfVal = static_cast<double>(m_bridgingPerformanceCounter) * (1000 / m_bridgingPerformanceInterval);
        if (m_bridgingPerformanceMaxCount < m_bridgingPerformanceCounter)
        {
            m_bridgingPerformanceMaxCount = m_bridgingPerformanceCounter;

            m_performanceMeter->setRange(0, perfVal, 1);
        }

        m_performanceMeter->setValue(perfVal);
    }

    m_bridgingPerformanceCounter = 0;
}

}
