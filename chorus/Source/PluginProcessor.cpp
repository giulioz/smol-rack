/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ChorusAudioProcessor::ChorusAudioProcessor()
    : AudioProcessor(
          BusesProperties()
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)) {

  addParameter(enabled = new juce::AudioParameterBool(
                   juce::ParameterID{"enabled", 1}, // parameterID
                   "Enabled",                 // parameter name
                   true));                    // default value

  enabled->addListener(this);
}

ChorusAudioProcessor::~ChorusAudioProcessor() {}

//==============================================================================
void ChorusAudioProcessor::parameterValueChanged(int parameterIndex,
                                                 float newValue) {
  sendChangeMessage();
}

void ChorusAudioProcessor::parameterGestureChanged(int parameterIndex,
                                                   bool gestureIsStarting) {}

//==============================================================================
const juce::String ChorusAudioProcessor::getName() const {
  return JucePlugin_Name;
}

bool ChorusAudioProcessor::acceptsMidi() const { return false; }

bool ChorusAudioProcessor::producesMidi() const { return false; }

bool ChorusAudioProcessor::isMidiEffect() const { return false; }

double ChorusAudioProcessor::getTailLengthSeconds() const {
  // TODO
  return 0.0;
}

int ChorusAudioProcessor::getNumPrograms() {
  return 1; // NB: some hosts don't cope very well if you tell them there are 0
            // programs, so this should be at least 1, even if you're not really
            // implementing programs.
}

int ChorusAudioProcessor::getCurrentProgram() { return 0; }

void ChorusAudioProcessor::setCurrentProgram(int index) {}

const juce::String ChorusAudioProcessor::getProgramName(int index) {
  return {};
}

void ChorusAudioProcessor::changeProgramName(int index,
                                             const juce::String &newName) {}

//==============================================================================
void ChorusAudioProcessor::prepareToPlay(double sampleRate,
                                         int samplesPerBlock) {
}

void ChorusAudioProcessor::releaseResources() {
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

bool ChorusAudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  // Some plugin hosts, such as certain GarageBand versions, will only
  // load plugins that support stereo bus layouts.
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

  // This checks if the input layout matches the output layout
  if (layouts.getMainInputChannelSet() != layouts.getMainInputChannelSet())
    return false;

  return true;
}

void ChorusAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                        juce::MidiBuffer &midiMessages) {
  juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  // In case we have more outputs than inputs, this code clears any output
  // channels that didn't contain input data, (because these aren't
  // guaranteed to be empty - they may contain garbage).
  // This is here to avoid people getting screaming feedback
  // when they first compile a plugin, but obviously you don't need to keep
  // this code if your algorithm always overwrites all the output channels.
  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  auto *channelDataL = buffer.getWritePointer(0);
  auto *channelDataR = buffer.getWritePointer(1);

  bool prev_isOverloading = isOverloading;
  isOverloading = false;

  short inLeft = 0;
  short inRight = 0;
  short outLeft, outRight;
  for (size_t i = 0; i < buffer.getNumSamples(); i++) {
    float drySampleL = channelDataL[i];
    float drySampleR = channelDataR[i];

    if (fabsf(drySampleL) > 1.3f || fabsf(drySampleR) > 1.3f) {
      isOverloading = true;
    }

    channelDataL[i] = 0;
    channelDataR[i] = 0;
  }

  if (isOverloading != prev_isOverloading) {
    sendChangeMessage();
  }
}

//==============================================================================
bool ChorusAudioProcessor::hasEditor() const {
  return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *ChorusAudioProcessor::createEditor() {
  return new ChorusAudioProcessorEditor(*this);
}

//==============================================================================
void ChorusAudioProcessor::getStateInformation(juce::MemoryBlock &destData) {
  std::unique_ptr<juce::XmlElement> xml(new juce::XmlElement("RRV10"));
  xml->setAttribute("enabled", (bool)*enabled);
  copyXmlToBinary(*xml, destData);
}

void ChorusAudioProcessor::setStateInformation(const void *data,
                                               int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xmlState(
      getXmlFromBinary(data, sizeInBytes));

  if (xmlState.get() != nullptr) {
    if (xmlState->hasTagName("RRV10")) {
      *enabled = (bool)xmlState->getBoolAttribute("enabled", true);
    }
  }

  sendChangeMessage();
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new ChorusAudioProcessor();
}
