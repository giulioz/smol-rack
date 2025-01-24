/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "PluginProcessor.h"

static int bgWidth = 3024;
static int bgHeight = 591;
static float scaleFactor = 3;
static int uiWidth = bgWidth / scaleFactor;
static int uiHeight = bgHeight / scaleFactor;

//==============================================================================
ReverbAudioProcessorEditor::ReverbAudioProcessorEditor(ReverbAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
  addAndMakeVisible(enabledToggle);
  enabledToggle.addListener(this);
  enabledToggle.setAlpha(0.0f);

  addAndMakeVisible(modeSlider);
  modeSlider.setLookAndFeel(&knobLF);
  modeSlider.setSliderStyle(juce::Slider::Rotary);
  modeSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
  modeSlider.setColour(juce::Slider::rotarySliderFillColourId,
                       juce::Colour(58, 126, 94));
  modeSlider.setRange(0, 8, 1);
  modeSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.3f,
                                 juce::MathConstants<float>::pi * 2.7f, true);
  modeSlider.addListener(this);

  addAndMakeVisible(drySlider);
  drySlider.setLookAndFeel(&knobLF);
  drySlider.setSliderStyle(juce::Slider::Rotary);
  drySlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
  drySlider.setColour(juce::Slider::rotarySliderFillColourId,
                      juce::Colour(228, 96, 4));
  drySlider.setRange(0, 127);
  drySlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.15f,
                                juce::MathConstants<float>::pi * 2.85f, true);
  drySlider.addListener(this);

  addAndMakeVisible(wetSlider);
  wetSlider.setLookAndFeel(&knobLF);
  wetSlider.setSliderStyle(juce::Slider::Rotary);
  wetSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
  wetSlider.setColour(juce::Slider::rotarySliderFillColourId,
                      juce::Colour(228, 96, 4));
  wetSlider.setRange(0, 127);
  wetSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.15f,
                                juce::MathConstants<float>::pi * 2.85f, true);
  wetSlider.addListener(this);

  addAndMakeVisible(decayTimeSlider);
  decayTimeSlider.setLookAndFeel(&knobLF);
  decayTimeSlider.setSliderStyle(juce::Slider::Rotary);
  decayTimeSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
  decayTimeSlider.setColour(juce::Slider::rotarySliderFillColourId,
                            juce::Colour(59, 124, 41));
  decayTimeSlider.setRange(0, 100);
  decayTimeSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.15f,
                                      juce::MathConstants<float>::pi * 2.85f,
                                      true);
  decayTimeSlider.addListener(this);

  addAndMakeVisible(preEqSlider);
  preEqSlider.setLookAndFeel(&knobLF);
  preEqSlider.setSliderStyle(juce::Slider::Rotary);
  preEqSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
  preEqSlider.setColour(juce::Slider::rotarySliderFillColourId,
                        juce::Colour(234, 157, 48));
  preEqSlider.setRange(0, 127);
  preEqSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.15f,
                                  juce::MathConstants<float>::pi * 2.85f, true);
  preEqSlider.addListener(this);

  setResizeLimits(uiWidth, uiHeight, uiWidth, uiHeight);
  setSize(uiWidth, uiHeight);

  updateValues();

  p.addChangeListener(this);
}

ReverbAudioProcessorEditor::~ReverbAudioProcessorEditor() {
  modeSlider.setLookAndFeel(nullptr);
  drySlider.setLookAndFeel(nullptr);
  wetSlider.setLookAndFeel(nullptr);
  decayTimeSlider.setLookAndFeel(nullptr);
  preEqSlider.setLookAndFeel(nullptr);

  audioProcessor.removeChangeListener(this);
}

void ReverbAudioProcessorEditor::resized() {
  float scaleFactorCurrent = (float)bgWidth / getBounds().getWidth();

  enabledToggle.setBounds(188 / scaleFactorCurrent, 212 / scaleFactorCurrent,
                          130 / scaleFactorCurrent, 258 / scaleFactorCurrent);

  const int knobSize = 200 / scaleFactorCurrent;
  const int knobY = 230 / scaleFactorCurrent;
  modeSlider.setBounds(409 / scaleFactorCurrent, knobY, knobSize, knobSize);
  decayTimeSlider.setBounds(700 / scaleFactorCurrent, knobY, knobSize,
                            knobSize);
  preEqSlider.setBounds(995 / scaleFactorCurrent, knobY, knobSize, knobSize);
  wetSlider.setBounds(1285 / scaleFactorCurrent, knobY, knobSize, knobSize);
  drySlider.setBounds(1574 / scaleFactorCurrent, knobY, knobSize, knobSize);
}

void ReverbAudioProcessorEditor::paint(juce::Graphics &g) {
  if (audioProcessor.currentPatch.enabled) {
    g.drawImage(juce::ImageCache::getFromMemory(BinaryData::ui_bg_png,
                                                BinaryData::ui_bg_pngSize),
                getLocalBounds().toFloat());
  } else {
    g.drawImage(juce::ImageCache::getFromMemory(BinaryData::ui_bg_off_png,
                                                BinaryData::ui_bg_off_pngSize),
                getLocalBounds().toFloat());
  }

  if (audioProcessor.isOverloading) {
    float scaleFactorCurrent = (float)bgWidth / getBounds().getWidth();
    g.setColour(juce::Colours::red);
    g.fillEllipse(343 / scaleFactorCurrent, 113 / scaleFactorCurrent,
                  30 / scaleFactorCurrent, 30 / scaleFactorCurrent);
  }
}

void ReverbAudioProcessorEditor::visibilityChanged() { updateValues(); }

void ReverbAudioProcessorEditor::sliderValueChanged(juce::Slider *slider) {
  if (slider == &modeSlider) {
    audioProcessor.currentPatch.mode = modeSlider.getValue();
    audioProcessor.emuLock.enter();
    audioProcessor.bossEmu.setParameters(audioProcessor.currentPatch.mode,
                                         audioProcessor.currentPatch.decayTime,
                                         7);
    audioProcessor.emuLock.exit();
  }

  if (slider == &drySlider) {
    audioProcessor.currentPatch.directLevel = drySlider.getValue() / 127.0f;
  }

  if (slider == &wetSlider) {
    audioProcessor.currentPatch.effectLevel = wetSlider.getValue() / 127.0f;
  }

  if (slider == &preEqSlider) {
    audioProcessor.currentPatch.preEq = preEqSlider.getValue() / 127.0f;
  }

  if (slider == &decayTimeSlider) {
    audioProcessor.currentPatch.decayTime =
        decayTimeSlider.getValue() / 127.0f * 20;
    audioProcessor.emuLock.enter();
    audioProcessor.bossEmu.setParameters(audioProcessor.currentPatch.mode,
                                         audioProcessor.currentPatch.decayTime,
                                         7);
    audioProcessor.emuLock.exit();
  }
}

void ReverbAudioProcessorEditor::buttonClicked(juce::Button *button) {
  if (button == &enabledToggle) {
    audioProcessor.currentPatch.enabled = enabledToggle.getToggleState();
    this->repaint();
  }
}

void ReverbAudioProcessorEditor::buttonStateChanged(juce::Button *button) {
  if (button == &enabledToggle) {
    audioProcessor.currentPatch.enabled = enabledToggle.getToggleState();
    if (button->getState() == juce::Button::ButtonState::buttonDown) {
      audioProcessor.currentPatch.enabled =
          !audioProcessor.currentPatch.enabled;
    }
    this->repaint();
  }
}

void ReverbAudioProcessorEditor::changeListenerCallback(
    juce::ChangeBroadcaster *) {
  updateValues();
}

void ReverbAudioProcessorEditor::updateValues() {
  enabledToggle.setToggleState(audioProcessor.currentPatch.enabled,
                               juce::dontSendNotification);
  modeSlider.setValue(audioProcessor.currentPatch.mode,
                      juce::dontSendNotification);
  drySlider.setValue(audioProcessor.currentPatch.directLevel * 127,
                     juce::dontSendNotification);
  wetSlider.setValue(audioProcessor.currentPatch.effectLevel * 127,
                     juce::dontSendNotification);
  preEqSlider.setValue(audioProcessor.currentPatch.preEq * 127,
                       juce::dontSendNotification);
  decayTimeSlider.setValue((float)audioProcessor.currentPatch.decayTime / 20 *
                               127,
                           juce::dontSendNotification);

  this->repaint();
}
