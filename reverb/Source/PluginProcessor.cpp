/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ReverbAudioProcessor::ReverbAudioProcessor()
    : AudioProcessor(
          BusesProperties()
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      bossEmu((unsigned char *)BinaryData::rrv10_bin, BinaryData::rrv10_binSize,
              BossEmu::RV_2_EMU_MODE) {

  addParameter(enabled =
                   new juce::AudioParameterBool("enabled", // parameterID
                                                "Enabled", // parameter name
                                                true));    // default value
  addParameter(effectLevel = new juce::AudioParameterFloat(
                   "effectLevel",  // parameterID
                   "Effect Level", // parameter name
                   0.0f,           // minimum value
                   1.0f,           // maximum value
                   0.4f));         // default value
  addParameter(directLevel = new juce::AudioParameterFloat(
                   "directLevel",                         // parameterID
                   "Direct Level",                        // parameter name
                   0.0f,                                  // minimum value
                   1.0f,                                  // maximum value
                   1.0f));                                // default value
  addParameter(mode = new juce::AudioParameterInt("mode", // parameterID
                                                  "Mode", // parameter name
                                                  0,      // minimum value
                                                  8,      // maximum value
                                                  0));    // default value
  addParameter(decayTime =
                   new juce::AudioParameterInt("decayTime",    // parameterID
                                               "Decay Time",   // parameter name
                                               0,              // minimum value
                                               31,             // maximum value
                                               5));            // default value
  addParameter(preEq = new juce::AudioParameterFloat("preEq",  // parameterID
                                                     "Pre Eq", // parameter name
                                                     0.0f,     // minimum value
                                                     1.0f,     // maximum value
                                                     0.5f));   // default value

  enabled->addListener(this);
  effectLevel->addListener(this);
  directLevel->addListener(this);
  mode->addListener(this);
  decayTime->addListener(this);
  preEq->addListener(this);
}

ReverbAudioProcessor::~ReverbAudioProcessor() {}

//==============================================================================
void ReverbAudioProcessor::parameterValueChanged(int parameterIndex,
                                                 float newValue) {
  sendChangeMessage();
}

void ReverbAudioProcessor::parameterGestureChanged(int parameterIndex,
                                                   bool gestureIsStarting) {}

//==============================================================================
const juce::String ReverbAudioProcessor::getName() const {
  return JucePlugin_Name;
}

bool ReverbAudioProcessor::acceptsMidi() const { return false; }

bool ReverbAudioProcessor::producesMidi() const { return false; }

bool ReverbAudioProcessor::isMidiEffect() const { return false; }

double ReverbAudioProcessor::getTailLengthSeconds() const {
  // TODO
  return 0.0;
}

int ReverbAudioProcessor::getNumPrograms() {
  return 1; // NB: some hosts don't cope very well if you tell them there are 0
            // programs, so this should be at least 1, even if you're not really
            // implementing programs.
}

int ReverbAudioProcessor::getCurrentProgram() { return 0; }

void ReverbAudioProcessor::setCurrentProgram(int index) {}

const juce::String ReverbAudioProcessor::getProgramName(int index) {
  return {};
}

void ReverbAudioProcessor::changeProgramName(int index,
                                             const juce::String &newName) {}

//==============================================================================
void ReverbAudioProcessor::prepareToPlay(double sampleRate,
                                         int samplesPerBlock) {
  emuLock.enter();
  bossEmu.reset();
  bossEmu.setParameters(*mode, *decayTime, 7);
  emuLock.exit();
}

void ReverbAudioProcessor::releaseResources() {
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

bool ReverbAudioProcessor::isBusesLayoutSupported(
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

void ReverbAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
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

    float filteredSampleL = drySampleL;
    float filteredSampleR = drySampleR;
    if (*preEq < 0.5f) {
      // Low-pass filter
      float cutoff = 1.0f - (0.5f - *preEq) * 2.0f;
      filteredSampleL = filterTempL + cutoff * (drySampleL - filterTempL);
      filteredSampleR = filterTempR + cutoff * (drySampleR - filterTempR);
    } else if (*preEq > 0.5f) {
      // High-pass filter
      float cutoff = (*preEq - 0.5f) * 2.0f;
      filteredSampleL =
          drySampleL - (filterTempL + cutoff * (drySampleL - filterTempL));
      filteredSampleR =
          drySampleR - (filterTempR + cutoff * (drySampleR - filterTempR));
    }
    filterTempL = filteredSampleL;
    filterTempR = filteredSampleR;

    float scaleFactor = 16383.0f;
    inLeft = filteredSampleL * scaleFactor;
    inRight = filteredSampleR * scaleFactor;
    emuLock.enter();
    bossEmu.process(&inLeft, &inRight, &outLeft, &outRight, 1);
    emuLock.exit();

    float wetSampleL = outLeft / scaleFactor;
    float wetSampleR = outRight / scaleFactor;
    if (*enabled) {
      channelDataL[i] = wetSampleL * *effectLevel + drySampleL * *directLevel;
      channelDataR[i] = wetSampleR * *effectLevel + drySampleR * *directLevel;
    }
  }

  if (isOverloading != prev_isOverloading) {
    sendChangeMessage();
  }
}

//==============================================================================
bool ReverbAudioProcessor::hasEditor() const {
  return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *ReverbAudioProcessor::createEditor() {
  return new ReverbAudioProcessorEditor(*this);
}

//==============================================================================
void ReverbAudioProcessor::getStateInformation(juce::MemoryBlock &destData) {
  std::unique_ptr<juce::XmlElement> xml(new juce::XmlElement("RRV10"));
  xml->setAttribute("enabled", (bool)*enabled);
  xml->setAttribute("effectLevel", (double)*effectLevel);
  xml->setAttribute("directLevel", (double)*directLevel);
  xml->setAttribute("mode", (int)*mode);
  xml->setAttribute("decayTime", (int)*decayTime);
  xml->setAttribute("preEq", (double)*preEq);
  copyXmlToBinary(*xml, destData);
}

void ReverbAudioProcessor::setStateInformation(const void *data,
                                               int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xmlState(
      getXmlFromBinary(data, sizeInBytes));

  if (xmlState.get() != nullptr) {
    if (xmlState->hasTagName("RRV10")) {
      *enabled = (bool)xmlState->getBoolAttribute("enabled", true);
      *effectLevel = (float)xmlState->getDoubleAttribute("effectLevel", 0.4f);
      *directLevel = (float)xmlState->getDoubleAttribute("directLevel", 1.0f);
      *mode = (int)xmlState->getIntAttribute("mode", 0);
      *decayTime = (int)xmlState->getIntAttribute("decayTime", 5);
      *preEq = (float)xmlState->getDoubleAttribute("preEq", 0.5f);
    }
  }

  emuLock.enter();
  bossEmu.reset();
  bossEmu.setParameters(*mode, *decayTime, 7);
  emuLock.exit();

  sendChangeMessage();
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new ReverbAudioProcessor();
}
