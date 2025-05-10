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
  modeSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
  modeSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
  modeSlider.setColour(juce::Slider::rotarySliderFillColourId,
                       juce::Colour(58, 126, 94));
  modeSlider.setRange(0, 8, 1);
  modeSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.3f,
                                 juce::MathConstants<float>::pi * 2.7f, true);
  modeSlider.addListener(this);

  addAndMakeVisible(drySlider);
  drySlider.setLookAndFeel(&knobLF);
  drySlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
  drySlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
  drySlider.setColour(juce::Slider::rotarySliderFillColourId,
                      juce::Colour(228, 96, 4));
  drySlider.setRange(0, 1);
  drySlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.15f,
                                juce::MathConstants<float>::pi * 2.85f, true);
  drySlider.addListener(this);

  addAndMakeVisible(wetSlider);
  wetSlider.setLookAndFeel(&knobLF);
  wetSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
  wetSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
  wetSlider.setColour(juce::Slider::rotarySliderFillColourId,
                      juce::Colour(228, 96, 4));
  wetSlider.setRange(0, 1);
  wetSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.15f,
                                juce::MathConstants<float>::pi * 2.85f, true);
  wetSlider.addListener(this);

  addAndMakeVisible(decayTimeSlider);
  decayTimeSlider.setLookAndFeel(&knobLF);
  decayTimeSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
  decayTimeSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
  decayTimeSlider.setColour(juce::Slider::rotarySliderFillColourId,
                            juce::Colour(59, 124, 41));
  decayTimeSlider.setRange(0, 1);
  decayTimeSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.15f,
                                      juce::MathConstants<float>::pi * 2.85f,
                                      true);
  decayTimeSlider.addListener(this);

  addAndMakeVisible(preEqSlider);
  preEqSlider.setLookAndFeel(&knobLF);
  preEqSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
  preEqSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
  preEqSlider.setColour(juce::Slider::rotarySliderFillColourId,
                        juce::Colour(234, 157, 48));
  preEqSlider.setRange(0, 1);
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
  if (*audioProcessor.enabled) {
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
    *audioProcessor.mode = modeSlider.getValue();
    audioProcessor.emuLock.enter();
    audioProcessor.bossEmu.setParameters(*audioProcessor.mode,
                                         *audioProcessor.decayTime, 7);
    audioProcessor.emuLock.exit();
  }

  if (slider == &drySlider) {
    *audioProcessor.directLevel = drySlider.getValue();
  }

  if (slider == &wetSlider) {
    *audioProcessor.effectLevel = wetSlider.getValue();
  }

  if (slider == &preEqSlider) {
    *audioProcessor.preEq = preEqSlider.getValue();
  }

  if (slider == &decayTimeSlider) {
    *audioProcessor.decayTime = decayTimeSlider.getValue() * 20.0f;
    audioProcessor.emuLock.enter();
    audioProcessor.bossEmu.setParameters(*audioProcessor.mode,
                                         (int)(*audioProcessor.decayTime), 7);
    audioProcessor.emuLock.exit();
  }
}

void ReverbAudioProcessorEditor::buttonClicked(juce::Button *button) {
  if (button == &enabledToggle) {
  }
}

void ReverbAudioProcessorEditor::buttonStateChanged(juce::Button *button) {
  if (button == &enabledToggle) {
    if (button->getState() == juce::Button::ButtonState::buttonDown) {
      *audioProcessor.enabled = !(*audioProcessor.enabled);
    }
    this->repaint();
  }
}

void ReverbAudioProcessorEditor::changeListenerCallback(
    juce::ChangeBroadcaster *) {
  updateValues();
}

void ReverbAudioProcessorEditor::updateValues() {
  enabledToggle.setToggleState(*audioProcessor.enabled,
                               juce::dontSendNotification);
  modeSlider.setValue(*audioProcessor.mode, juce::dontSendNotification);
  drySlider.setValue(*audioProcessor.directLevel, juce::dontSendNotification);
  wetSlider.setValue(*audioProcessor.effectLevel, juce::dontSendNotification);
  preEqSlider.setValue(*audioProcessor.preEq, juce::dontSendNotification);
  decayTimeSlider.setValue((float)(*audioProcessor.decayTime) / 15.0f,
                           juce::dontSendNotification);

  this->repaint();
}
