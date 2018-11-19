/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PlugEdit.h"

//==============================================================================
PluginwithGuiAudioProcessor::PluginwithGuiAudioProcessor()
/*#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif*/
{
	//adding parameters to display in Reaper

	addParameter(val_x=new AudioParameterFloat("val_x", // parameter ID
												"val_x", // parameter name
												0.000f,   // minimum value
												1.000f,   // maximum value
												0.500f)); // default value)
	addParameter(val_y = new AudioParameterFloat("val_y", 
												"val_y", 
												0.000f,   
												1.000f,   
												0.500f)); 
	addParameter(val_z = new AudioParameterFloat("val_z", 
												"val_z",
												0.000f,   
												1.000f,   
												0.500f));

}

PluginwithGuiAudioProcessor::~PluginwithGuiAudioProcessor()
{
}

//==============================================================================
const String PluginwithGuiAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginwithGuiAudioProcessor::acceptsMidi() const
{
	return false;
   /*#if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif*/
}

bool PluginwithGuiAudioProcessor::producesMidi() const
{
	return false;
   /*#if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif*/
}

bool PluginwithGuiAudioProcessor::isMidiEffect() const
{
	return false;
  /* #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif*/
}

double PluginwithGuiAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginwithGuiAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PluginwithGuiAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PluginwithGuiAudioProcessor::setCurrentProgram (int index)
{
}

const String PluginwithGuiAudioProcessor::getProgramName (int index)
{
    return {};
}

void PluginwithGuiAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void PluginwithGuiAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void PluginwithGuiAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PluginwithGuiAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
	return true;
 /*#if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif*/
}
#endif

void PluginwithGuiAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	playHead = this->getPlayHead();
	
	if (playHead != nullptr && playHead->getCurrentPosition(currentPositionInfo)){
		tempo = currentPositionInfo.bpm;
	};
   /* ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }*/
}

//==============================================================================
bool PluginwithGuiAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* PluginwithGuiAudioProcessor::createEditor()
{
    return new PlugEdit (*this);
}

//==============================================================================
void PluginwithGuiAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void PluginwithGuiAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginwithGuiAudioProcessor();
}
