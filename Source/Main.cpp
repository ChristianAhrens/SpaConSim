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

#include <JuceHeader.h>

#include "MainSoundscapeOSCSimComponent.h"

#include "../submodules/RemoteProtocolBridge/submodules/JUCE-AppBasics/Source/CustomLookAndFeel.h"

namespace SoundscapeOSCSim
{

//==============================================================================
class SoundscapeOSCSimApplication : public JUCEApplication
{
public:
    //==============================================================================
    SoundscapeOSCSimApplication() {}

    const String getApplicationName() override { return ProjectInfo::projectName; }
    const String getApplicationVersion() override { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override { return true; }

    //==============================================================================
    void initialise(const String& commandLine) override
    {
        ignoreUnused(commandLine);

        m_mainWindow.reset(new MainWindow(getApplicationName()));
    }

    void shutdown() override
    {
        m_mainWindow = nullptr; // (deletes our window)
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted(const String& commandLine) override
    {
        ignoreUnused(commandLine);
    }

    //==============================================================================
    /*
        This class implements the desktop window that contains an instance of
        our MainComponent class.
    */
    class MainWindow : public DocumentWindow
    {
    public:
        MainWindow(String name) : DocumentWindow(name,
            Desktop::getInstance().getDefaultLookAndFeel()
            .findColour(ResizableWindow::backgroundColourId),
            DocumentWindow::closeButton)
        {
            m_mainComponent = std::make_unique<MainSoundscapeOSCSimComponent>();
            m_customlookAndFeel = std::make_unique<JUCEAppBasics::CustomLookAndFeel>();

            setUsingNativeTitleBar(true);
            setContentOwned(m_mainComponent.get(), true);
            
            setLookAndFeel(m_customlookAndFeel.get());

#if JUCE_IOS || JUCE_ANDROID
            setFullScreen(true);
#else
            setResizable(false, false);
            centreWithSize(getWidth(), getHeight());
#endif

            setVisible(true);
        }

        void closeButtonPressed() override
        {
            // This is called when the user tries to close this window. Here, we'll just
            // ask the app to quit when this happens, but you can change this to do
            // whatever you need.
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

        /* Note: Be careful if you override any DocumentWindow methods - the base
            class uses a lot of them, so by overriding you might break its functionality.
            It's best to do all your work in your content component instead, but if
            you really have to override any DocumentWindow methods, make sure your
            subclass also calls the superclass's method.
        */

    private:
        std::unique_ptr<MainSoundscapeOSCSimComponent>      m_mainComponent;
        std::unique_ptr<JUCEAppBasics::CustomLookAndFeel>   m_customlookAndFeel;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };

private:
    std::unique_ptr<MainWindow> m_mainWindow;
};

}

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (SoundscapeOSCSim::SoundscapeOSCSimApplication)