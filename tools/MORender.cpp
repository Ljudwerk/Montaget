// MORender – kör Montaget utan DAW, och testar instrumentet.
//
//   MORender --test                         testfallen (fas 0: tyst på alla utgångar, inget NaN, bussarna,
//                                           latensen, tillståndet, parametrarnas antal och ordning)
//   MORender --parametrar fil.txt           parametrarnas index, id, namn, intervall och grundvärde
//   MORender --state fil.xml [id=värde ...] pluginets läge som XML (för Reaper-skripten)
//
// Copyright (C) 2026 Montaget-teamet. Licens: AGPLv3, se LICENSE.
#include "PluginProcessor.h"
#include <juce_audio_formats/juce_audio_formats.h>
#include <iostream>

namespace
{
using namespace mo;

int fel = 0;
juce::String u (const char* s) { return juce::String::fromUTF8 (s); }
void kontroll (bool ok, const juce::String& vad)
{
    std::cout << (ok ? "  OK    " : "  FEL   ") << vad.toRawUTF8() << "\n";
    if (! ok) ++fel;
}

void satt (MontagetProcessor& p, const juce::String& pid, float varde)
{
    auto* par = p.apvts.getParameter (pid);
    if (par == nullptr) { std::cout << "  okänd parameter: " << pid << "\n"; return; }
    par->setValueNotifyingHost (par->convertTo0to1 (varde));
}

// Delarna per plats, i den ordning de skapas. Ändras aldrig – nya parametrar läggs sist.
const char* const perPlats[] { "start", "slut", "loopstart", "loop", "baklanges", "tonain", "tonaut", "hack", "hacksatt",
    "takt", "bitar", "utfilter", "tonhojd", "fin", "rot", "cutoff", "resonans", "filtenv", "filtanslag", "tangent",
    "fa", "fd", "fs", "fr", "aa", "ad", "as", "ar", "anslag", "lage", "stryp", "niva", "pan", "utgang", "tyst", "solo" };
const char* const gemensamma[] { "mo_spelsatt", "mo_krom_plats", "mo_glid", "mo_mono", "mo_insp_troskel", "mo_insp_langd", "mo_niva", "mo_limiter" };

int test()
{
    std::cout << "MORender --test (fas 0)\n";
    const double fs = 48000.0;
    {
        MontagetProcessor p;
        p.setRateAndBufferSizeDetails (fs, 512);
        p.prepareToPlay (fs, 512);
        juce::AudioBuffer<float> b (juce::jmax (p.getTotalNumInputChannels(), p.getTotalNumOutputChannels()), 512);
        juce::Random r (3);
        bool tyst = true, andligt = true;
        for (int blk = 0; blk < 200; ++blk)
        {
            for (int k = 0; k < b.getNumChannels(); ++k)
                for (int i = 0; i < 512; ++i) b.setSample (k, i, r.nextFloat() * 2.0f - 1.0f);   // ingången: brus
            juce::MidiBuffer m;
            m.addEvent (juce::MidiMessage::noteOn (1, 36 + blk % 48, (juce::uint8) 100), blk % 512);
            m.addEvent (juce::MidiMessage::noteOff (1, 36 + (blk + 47) % 48), 511 - blk % 512);
            p.processBlock (b, m);
            for (int k = 0; k < p.getTotalNumOutputChannels(); ++k)
                for (int i = 0; i < 512; ++i) { const float v = b.getSample (k, i); tyst = tyst && juce::exactlyEqual (v, 0.0f); andligt = andligt && std::isfinite (v); }
        }
        kontroll (tyst && andligt, u ("Fas 0: tyst på alla utgångar, med MIDI och brus på ingången"));
        kontroll (p.getLatencySamples() == 0, "Latensen rapporteras som 0");
    }
    {
        MontagetProcessor p;
        const auto layout = p.getBusesLayout();
        bool ok = layout.outputBuses.size() == 5 && layout.getMainOutputChannelSet() == juce::AudioChannelSet::stereo()
                  && layout.inputBuses.size() == 1 && layout.getChannelSet (true, 0) == juce::AudioChannelSet::stereo();
        for (int g = 1; g < 5 && ok; ++g) ok = layout.getChannelSet (false, g).isDisabled();
        kontroll (ok, u ("Bussarna: Huvud stereo, fyra grupper av från början, ingången stereo"));
        auto mono = layout; mono.outputBuses.getReference (0) = juce::AudioChannelSet::mono();
        auto allaGrupper = layout; for (int g = 1; g < 5; ++g) allaGrupper.outputBuses.getReference (g) = juce::AudioChannelSet::stereo();
        kontroll (p.checkBusesLayoutSupported (mono) && p.checkBusesLayoutSupported (allaGrupper), u ("Mono ut och alla fyra grupper går att slå på"));
    }
    {
        MontagetProcessor a, b;
        satt (a, id::plats (2, id::start), 0.25f); satt (a, id::plats (15, id::rot), 45.0f); satt (a, id::spelsatt, 1.0f);
        a.apvts.state.setProperty ("vald", 7, nullptr);
        juce::MemoryBlock mb; a.getStateInformation (mb);
        b.setStateInformation (mb.getData(), (int) mb.getSize());
        bool lika = true;
        for (int i = 0; i < a.getParameters().size(); ++i)
            lika = lika && std::abs (a.getParameters()[i]->getValue() - b.getParameters()[i]->getValue()) < 1.0e-6f;
        kontroll (lika && (int) b.apvts.state.getProperty ("vald") == 7 && b.apvts.state.getProperty ("moFormat").toString() == "1",
                  u ("Tillståndet sparas och läses in (formatversion 1, vald plats)"));
    }
    {
        MontagetProcessor p;
        const auto& ps = p.getParameters();
        juce::StringArray vantat;
        for (int pl = 0; pl < antalPlatser; ++pl) for (auto* d : perPlats) vantat.add (id::plats (pl, d));
        for (auto* g : gemensamma) vantat.add (g);
        bool ok = ps.size() == antalParametrar && vantat.size() == antalParametrar;
        for (int i = 0; ok && i < ps.size(); ++i) ok = dynamic_cast<juce::AudioProcessorParameterWithID*> (ps[i])->paramID == vantat[i];
        kontroll (ok, u ("Parametrarnas antal (") + juce::String (antalParametrar) + u (": 16 platser × 36 och 8 gemensamma) och ordning ligger fast"));
    }
    std::cout << (fel == 0 ? "Alla kontroller gick igenom.\n" : "Kontroller som inte gick igenom: ");
    if (fel) std::cout << fel << "\n";
    return fel == 0 ? 0 : 1;
}

int parametrar (const juce::File& fil)
{
    MontagetProcessor p;
    juce::String ut;
    int i = 0;
    for (auto* par : p.getParameters())
        if (auto* r = dynamic_cast<juce::RangedAudioParameter*> (par))
        {
            const auto ri = r->getNormalisableRange();
            ut << i++ << "\t" << r->getParameterID() << "\t" << r->getName (64) << "\t" << ri.start << "\t" << ri.end
               << "\t" << r->convertFrom0to1 (r->getDefaultValue()) << "\n";
        }
    if (! fil.replaceWithText (ut)) { std::cout << "Kunde inte skriva " << fil.getFullPathName() << "\n"; return 1; }
    std::cout << i << " parametrar skrivna till " << fil.getFullPathName() << "\n";
    return 0;
}

int state (const juce::File& fil, int argc, char* argv[])
{
    MontagetProcessor p;
    for (int i = 3; i < argc; ++i)
    {
        const juce::String a (argv[i]);
        satt (p, a.upToFirstOccurrenceOf ("=", false, false), a.fromFirstOccurrenceOf ("=", false, false).getFloatValue());
    }
    auto tillstand = p.apvts.copyState();
    tillstand.setProperty ("moFormat", MontagetProcessor::formatVersion, nullptr);
    auto xml = tillstand.createXml();
    return fil.replaceWithText (xml->toString (juce::XmlElement::TextFormat().singleLine().withoutHeader())) ? 0 : 1;
}

} // namespace

int main (int argc, char* argv[])
{
    juce::ScopedJuceInitialiser_GUI juce;
    const auto cwd = juce::File::getCurrentWorkingDirectory();
    const juce::String a1 = argc > 1 ? juce::String (argv[1]) : juce::String();
    if (a1 == "--test") return test();
    if (a1 == "--parametrar" && argc >= 3) return parametrar (cwd.getChildFile (argv[2]));
    if (a1 == "--state" && argc >= 3) return state (cwd.getChildFile (argv[2]), argc, argv);
    std::cout << "Användning: MORender --test | --parametrar fil.txt | --state fil.xml [id=värde ...]\n";
    return 1;
}
