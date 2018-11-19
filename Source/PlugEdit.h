/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.4.1

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2017 - ROLI Ltd.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class PlugEdit  : public AudioProcessorEditor,
                  public Slider::Listener,
                  public Button::Listener
{
public:
    //==============================================================================
    PlugEdit (PluginwithGuiAudioProcessor& p);
    ~PlugEdit();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;
    void buttonClicked (Button* buttonThatWasClicked) override;

    // Binary resources:
    static const char* logo_png;
    static const int logo_pngSize;
    static const char* param_png;
    static const int param_pngSize;


private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    PluginwithGuiAudioProcessor& processor;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<Slider> slider_x;
    std::unique_ptr<Slider> slider_y;
    std::unique_ptr<Slider> slider_z;
    std::unique_ptr<ImageButton> but_param;
    std::unique_ptr<Label> lab_x;
    std::unique_ptr<Label> lab_y;
    std::unique_ptr<Label> lab_z;
    std::unique_ptr<TextButton> but_start;
    std::unique_ptr<TextButton> but_stop;
    std::unique_ptr<TextButton> but_tempo;
    std::unique_ptr<Label> lab_tempo;
    Image cachedImage_logo_png_1;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlugEdit)
};

//[EndFile] You can add extra defines here...
//[/EndFile]
