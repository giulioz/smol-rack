/*
  ==============================================================================

    KnobLF.h
    Created: 11 Nov 2024 12:58:34pm
    Author:  Giulio Zausa

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
 */
class KnobLF : public juce::LookAndFeel_V3 {
public:
  KnobLF() {}
  ~KnobLF() {}

  void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height,
                        float sliderPos, const float rotaryStartAngle,
                        const float rotaryEndAngle, juce::Slider &slider) {
    const float centerX = x + width * 0.5f;
    const float centerY = y + height * 0.5f;
    const float angle =
        rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    g.drawImage(juce::ImageCache::getFromMemory(BinaryData::knob_low_png,
                                                BinaryData::knob_low_pngSize),
                juce::Rectangle<float>(x, y, width, height));

    g.setColour(slider.findColour(juce::Slider::rotarySliderFillColourId));
    g.fillEllipse(centerX - (71.5f / 97.5f) * width / 2,
                  centerY - (72.0f / 99.0f) * height / 2,
                  (71.5f / 97.5f) * width, (72.0f / 99.0f) * height);

    // pointer
    juce::Path p;
    const float radius = juce::jmin(width / 2, height / 2) - 4.0f;
    const float pointerLength = radius * 0.8f;
    const float pointerThickness = 4.5f;
    p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness,
                   pointerLength);
    p.applyTransform(
        juce::AffineTransform::rotation(angle).translated(centerX, centerY));
    g.setColour(juce::Colours::black);
    g.fillPath(p);

    g.drawImage(juce::ImageCache::getFromMemory(BinaryData::knob_high_png,
                                                BinaryData::knob_high_pngSize),
                juce::Rectangle<float>(x, y, width, height));
  }

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KnobLF)
};
