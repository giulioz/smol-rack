/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
ReverbAudioProcessorEditor::ReverbAudioProcessorEditor(ReverbAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
  addAndMakeVisible(enabledToggle);
  enabledToggle.addListener(this);
  enabledToggle.setButtonText("Enabled");

  addAndMakeVisible(modeComboBox);
  modeComboBox.addListener(this);
  modeComboBox.addItem("Room 1 (Small)", 1);
  modeComboBox.addItem("Room 2 (Large)", 2);
  modeComboBox.addItem("Hall 1 (Small)", 3);
  modeComboBox.addItem("Hall 2 (Large)", 4);
  modeComboBox.addItem("Plate 1", 5);
  modeComboBox.addItem("Plate 2", 6);
  modeComboBox.addItem("Multi-tap 1 (Delay)", 7);
  modeComboBox.addItem("Multi-tap 2 (Reverse)", 8);
  modeComboBox.addItem("Gated Reverb", 9);
  addAndMakeVisible(modeLabel);
  modeLabel.setText("Mode", juce::dontSendNotification);
  modeLabel.attachToComponent(&modeComboBox, true);

  addAndMakeVisible(dryWetSlider);
  dryWetSlider.setRange(0, 127);
  dryWetSlider.addListener(this);
  addAndMakeVisible(dryWetLabel);
  dryWetLabel.setText("Dry/Wet", juce::dontSendNotification);
  dryWetLabel.attachToComponent(&dryWetSlider, true);

  addAndMakeVisible(decayTimeSlider);
  decayTimeSlider.setRange(0, 100);
  decayTimeSlider.addListener(this);
  addAndMakeVisible(decayTimeLabel);
  decayTimeLabel.setText("Decay Time", juce::dontSendNotification);
  decayTimeLabel.attachToComponent(&decayTimeSlider, true);

  setResizeLimits(600, 200, 600, 200);
  setSize(600, 200);

  updateValues();
}

ReverbAudioProcessorEditor::~ReverbAudioProcessorEditor() {}

void ReverbAudioProcessorEditor::resized() {
  enabledToggle.setBounds(120, 30 + 40 * 0, 400, 25);
  modeComboBox.setBounds(120, 30 + 40 * 1, 400, 30);
  dryWetSlider.setBounds(120, 30 + 40 * 2, 400, 25);
  decayTimeSlider.setBounds(120, 30 + 40 * 3, 400, 25);
}

void ReverbAudioProcessorEditor::sliderValueChanged(juce::Slider *slider) {
  if (slider == &dryWetSlider) {
    audioProcessor.currentPatch.effectLevel = dryWetSlider.getValue() / 127.0f;
    audioProcessor.currentPatch.directLevel =
        1 - dryWetSlider.getValue() / 127.0f;
  }

  if (slider == &decayTimeSlider) {
    audioProcessor.currentPatch.decayTime =
        decayTimeSlider.getValue() / 127.0f * 20;
    audioProcessor.bossEmu.setParameters(audioProcessor.currentPatch.mode,
                                         audioProcessor.currentPatch.decayTime,
                                         7);
  }
}

void ReverbAudioProcessorEditor::comboBoxChanged(juce::ComboBox *button) {
  if (button == &modeComboBox) {
    audioProcessor.currentPatch.mode = modeComboBox.getSelectedId() - 1;
    audioProcessor.bossEmu.setParameters(audioProcessor.currentPatch.mode,
                                         audioProcessor.currentPatch.decayTime,
                                         7);
  }
}

void ReverbAudioProcessorEditor::buttonClicked(juce::Button *button) {
  if (button == &enabledToggle) {
    audioProcessor.currentPatch.enabled = enabledToggle.getToggleState();
  }
}

void ReverbAudioProcessorEditor::visibilityChanged() { updateValues(); }

void ReverbAudioProcessorEditor::updateValues() {
  enabledToggle.setToggleState(audioProcessor.currentPatch.enabled,
                               juce::dontSendNotification);
  modeComboBox.setSelectedItemIndex(audioProcessor.currentPatch.mode + 1,
                                    juce::dontSendNotification);
  dryWetSlider.setValue(audioProcessor.currentPatch.effectLevel * 127,
                        juce::dontSendNotification);
  decayTimeSlider.setValue((float)audioProcessor.currentPatch.decayTime / 20 *
                               127,
                           juce::dontSendNotification);
}
