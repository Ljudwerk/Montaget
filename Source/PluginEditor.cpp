// Copyright (C) 2026 Montaget-teamet. Licens: AGPLv3, se LICENSE.
#include "PluginEditor.h"

namespace pb = mo::stil::panel;

MontagetEditor::MontagetEditor (MontagetProcessor& p) : AudioProcessorEditor (p), panel (p), proc (p)
{
    addAndMakeVisible (panel);
    setResizable (true, true);
    setResizeLimits ((int) (pb::bredd * 0.6f), (int) (pb::hojd * 0.6f), (int) (pb::bredd * 1.6f), (int) (pb::hojd * 1.6f));
    getConstrainer()->setFixedAspectRatio ((double) pb::bredd / (double) pb::hojd);
    const float s = juce::jlimit (0.6f, 1.6f, (float) (double) proc.apvts.state.getProperty ("storlek", 0.85));
    setSize ((int) std::round (pb::bredd * s), (int) std::round (pb::hojd * s));
}

void MontagetEditor::resized()
{
    const float s = (float) getWidth() / (float) pb::bredd;
    proc.apvts.state.setProperty ("storlek", s, nullptr);
    panel.setBounds (0, 0, pb::bredd, pb::hojd);
    panel.setTransform (juce::AffineTransform::scale (s));
}
