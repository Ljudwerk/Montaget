// Copyright (C) 2026 Montaget-teamet. Licens: AGPLv3, se LICENSE.
#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace mo;

juce::AudioProcessor::BusesProperties MontagetProcessor::bussar()
{
    auto b = BusesProperties().withOutput ("Huvud", juce::AudioChannelSet::stereo(), true);
    if (juce::PluginHostType::getPluginLoadedAs() != juce::AudioProcessor::wrapperType_AudioUnit)
    {
        for (int g = 1; g <= 4; ++g) b = b.withOutput ("Grupp " + juce::String (g), juce::AudioChannelSet::stereo(), false);
        b = b.withInput (juce::String::fromUTF8 ("Ingång"), juce::AudioChannelSet::stereo(), true);
    }
    return b;
}

MontagetProcessor::MontagetProcessor()
    : AudioProcessor (bussar()),
      apvts (*this, nullptr, "Montaget", skapaLayout())
{
}

bool MontagetProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto ut = layouts.getMainOutputChannelSet();
    if (ut != juce::AudioChannelSet::mono() && ut != juce::AudioChannelSet::stereo()) return false;
    for (int i = 1; i < layouts.outputBuses.size(); ++i)
    {
        const auto g = layouts.getChannelSet (false, i);
        if (! g.isDisabled() && g != juce::AudioChannelSet::stereo()) return false;
    }
    if (! layouts.inputBuses.isEmpty())
    {
        const auto in = layouts.getChannelSet (true, 0);
        if (! in.isDisabled() && in != juce::AudioChannelSet::mono() && in != juce::AudioChannelSet::stereo()) return false;
    }
    return true;
}

void MontagetProcessor::prepareToPlay (double samplingsfrekvens, int)
{
    fs = samplingsfrekvens;
    inTopp = 0.0f;
}

void MontagetProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals ingaDenormaler;
    const int n = buffer.getNumSamples();
    // Ingångens nivå (för inspelningen från fas 2), före allt annat.
    float topp = 0.0f;
    if (getBusCount (true) > 0 && getBus (true, 0)->isEnabled())
    {
        auto in = getBusBuffer (buffer, true, 0);
        for (int k = 0; k < in.getNumChannels(); ++k) topp = juce::jmax (topp, in.getMagnitude (k, 0, n));
    }
    inTopp = juce::jmax (topp, inTopp * std::pow (10.0f, -1.0f * (float) n / (float) fs));
    inNivaDb.store (inTopp > 1.0e-6f ? 20.0f * std::log10 (inTopp) : -120.0f);

    // Fas 0: instrumentet är tyst på alla utgångar.
    buffer.clear();
    utNivaDb.store (-120.0f);
}

void MontagetProcessor::getStateInformation (juce::MemoryBlock& data)
{
    auto tillstand = apvts.copyState();
    tillstand.setProperty ("moFormat", formatVersion, nullptr);
    if (auto xml = tillstand.createXml()) copyXmlToBinary (*xml, data);
}

void MontagetProcessor::setStateInformation (const void* data, int storlek)
{
    if (auto xml = getXmlFromBinary (data, storlek))
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessorEditor* MontagetProcessor::createEditor() { return new MontagetEditor (*this); }

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new MontagetProcessor(); }
