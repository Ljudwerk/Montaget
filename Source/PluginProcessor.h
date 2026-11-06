// Montaget – processorn. Ett samplingsinstrument: 16 platser med egna ljud (filer eller inspelat från
// ingången), spelade kromatiskt, som plattor eller hackade. I fas 0 är det tyst: arkivet, rösterna och
// saxen byggs i fas 1.
//
// Bussarna: stereo ut (Huvud) och fyra stereogrupper (av från början, för platser som ska till egna spår),
// och en ingång för inspelning (sidokedjan i pluginet, mikrofonen i appen). I AU finns bara huvudutgången, som i Pannrummet – flera bussar
// i AU provas i en senare fas.
//
// Copyright (C) 2026 Montaget-teamet. Licens: AGPLv3, se LICENSE.
#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <krang/app/Inspelaren.h>
#include <atomic>
#include "Parameters.h"

class MontagetProcessor : public juce::AudioProcessor
{
public:
    MontagetProcessor();

    void prepareToPlay (double fs, int maxBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Montaget"; }
    bool acceptsMidi() const override  { return true; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return "Init"; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;

    juce::AudioProcessorValueTreeState apvts;

    // Appen: ljudkortet (sätts av krang::app::FristaendeApp) och inspelaren av utgången.
    juce::AudioDeviceManager* enheter = nullptr;
    krang::app::Inspelaren inspelaren { "Montaget" };

    // Mätare till panelen (dBFS).
    std::atomic<float> inNivaDb { -120.0f }, utNivaDb { -120.0f };

    static constexpr int formatVersion = 1;

private:
    static BusesProperties bussar();
    double fs = 48000.0;
    float inTopp = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MontagetProcessor)
};
