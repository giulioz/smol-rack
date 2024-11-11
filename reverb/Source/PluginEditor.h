/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "KnobLF.h"
#include "PluginProcessor.h"
#include <JuceHeader.h>

//==============================================================================
/**
 */
class ReverbAudioProcessorEditor : public juce::AudioProcessorEditor,
                                   public juce::Slider::Listener,
                                   public juce::Button::Listener {
public:
  ReverbAudioProcessorEditor(ReverbAudioProcessor &);
  ~ReverbAudioProcessorEditor() override;

  //==============================================================================
  void resized() override;
  void paint(juce::Graphics &g) override;
  void visibilityChanged() override;

  void sliderValueChanged(juce::Slider *) override;
  void buttonClicked(juce::Button *) override;
  void buttonStateChanged(juce::Button *) override;

  void updateValues();

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  ReverbAudioProcessor &audioProcessor;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverbAudioProcessorEditor)

  juce::ToggleButton enabledToggle;
  juce::Slider modeSlider;
  juce::Slider drySlider;
  juce::Slider wetSlider;
  juce::Slider decayTimeSlider;
  juce::Slider preEqSlider;

  KnobLF knobLF;
};
