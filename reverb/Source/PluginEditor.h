/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "PluginProcessor.h"
#include <JuceHeader.h>


//==============================================================================
/**
 */
class ReverbAudioProcessorEditor : public juce::AudioProcessorEditor,
                                   public juce::Slider::Listener,
                                   public juce::ComboBox::Listener,
                                   public juce::Button::Listener {
public:
  ReverbAudioProcessorEditor(ReverbAudioProcessor &);
  ~ReverbAudioProcessorEditor() override;

  //==============================================================================
  void resized() override;
  void sliderValueChanged(juce::Slider *) override;
  void comboBoxChanged(juce::ComboBox *) override;
  void buttonClicked(juce::Button *) override;
  void visibilityChanged() override;

  void updateValues();

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  ReverbAudioProcessor &audioProcessor;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverbAudioProcessorEditor)

  juce::ToggleButton enabledToggle;
  juce::Slider dryWetSlider;
  juce::Label dryWetLabel;
  juce::Slider decayTimeSlider;
  juce::Label decayTimeLabel;
  juce::Label modeLabel;
  juce::ComboBox modeComboBox;
};
