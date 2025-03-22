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
ChorusAudioProcessorEditor::ChorusAudioProcessorEditor(ChorusAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
  addAndMakeVisible(enabledToggle);
  enabledToggle.addListener(this);
  enabledToggle.setAlpha(0.0f);

  setResizeLimits(uiWidth, uiHeight, uiWidth, uiHeight);
  setSize(uiWidth, uiHeight);

  updateValues();

  p.addChangeListener(this);
}

ChorusAudioProcessorEditor::~ChorusAudioProcessorEditor() {
  audioProcessor.removeChangeListener(this);
}

void ChorusAudioProcessorEditor::resized() {
  float scaleFactorCurrent = (float)bgWidth / getBounds().getWidth();

  enabledToggle.setBounds(188 / scaleFactorCurrent, 212 / scaleFactorCurrent,
                          130 / scaleFactorCurrent, 258 / scaleFactorCurrent);
}

void ChorusAudioProcessorEditor::paint(juce::Graphics &g) {
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

void ChorusAudioProcessorEditor::visibilityChanged() { updateValues(); }

void ChorusAudioProcessorEditor::sliderValueChanged(juce::Slider *slider) {
}

void ChorusAudioProcessorEditor::buttonClicked(juce::Button *button) {
  if (button == &enabledToggle) {
  }
}

void ChorusAudioProcessorEditor::buttonStateChanged(juce::Button *button) {
  if (button == &enabledToggle) {
    if (button->getState() == juce::Button::ButtonState::buttonDown) {
      *audioProcessor.enabled = !(*audioProcessor.enabled);
    }
    this->repaint();
  }
}

void ChorusAudioProcessorEditor::changeListenerCallback(
    juce::ChangeBroadcaster *) {
  updateValues();
}

void ChorusAudioProcessorEditor::updateValues() {
  enabledToggle.setToggleState(*audioProcessor.enabled,
                               juce::dontSendNotification);

  this->repaint();
}
