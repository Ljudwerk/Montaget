// Montaget – editorn: datorstudion (ui/Panelen.h) i vald storlek, 60–160 % (85 % från början).
// Storleken ändras genom att dra i hörnet och sparas med projektet.
//
// Copyright (C) 2026 Montaget-teamet. Licens: AGPLv3, se LICENSE.
#pragma once

#include "PluginProcessor.h"
#include "ui/Panelen.h"

class MontagetEditor : public juce::AudioProcessorEditor
{
public:
    explicit MontagetEditor (MontagetProcessor& p);
    void resized() override;

    mo::ui::Panelen panel;

private:
    MontagetProcessor& proc;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MontagetEditor)
};
