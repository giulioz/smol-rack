/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "BossEmu.h"

struct ReverbState {
  bool enabled = true;
  float effectLevel = 0.5; // 0-1
  float directLevel = 0.5; // 0-1
  int mode = 0;            // 0-8
  int decayTime = 5;       // 0-31
  float preEq = 0.5;       // 0-1
};

//==============================================================================
/**
 */
class ReverbAudioProcessor : public juce::AudioProcessor,
                             public juce::ChangeBroadcaster {
public:
  ReverbState currentPatch;
  BossEmu bossEmu;
  bool isOverloading = false;
  float filterTempL = 0;
  float filterTempR = 0;

  //==============================================================================
  ReverbAudioProcessor();
  ~ReverbAudioProcessor() override;

  //==============================================================================
  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

  bool isBusesLayoutSupported(const BusesLayout &layouts) const override;

  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

  //==============================================================================
  juce::AudioProcessorEditor *createEditor() override;
  bool hasEditor() const override;

  //==============================================================================
  const juce::String getName() const override;

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  //==============================================================================
  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int index, const juce::String &newName) override;

  //==============================================================================
  void getStateInformation(juce::MemoryBlock &destData) override;
  void setStateInformation(const void *data, int sizeInBytes) override;

private:
  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverbAudioProcessor)
};
