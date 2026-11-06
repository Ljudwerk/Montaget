// Montagets parametrar. ID:na har formen mo_<del>_<vad> (per plats mo_p01_start … mo_p16_solo) och får
// ALDRIG döpas om när de väl finns i ett bygge som använts. Nya parametrar läggs sist i listan.
//
// Alla 16 platsers parametrar och de gemensamma skapas från fas 0, så att ID:n och ordning ligger fast.
// Platserna kommer först, plats för plats i samma ordning; de gemensamma sist.
//
// Copyright (C) 2026 Montaget-teamet. Licens: AGPLv3, se LICENSE.
#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace mo
{
inline juce::String svt (const char* utf8) { return juce::String::fromUTF8 (utf8); }

inline constexpr int antalPlatser = 16;

namespace id
{
    // En plats parameter: mo_p01_start osv. 'vad' är delen efter platsnumret.
    inline juce::String plats (int p, const char* vad) { return "mo_p" + juce::String (p + 1).paddedLeft ('0', 2) + "_" + vad; }

    // Delarna, i den ordning de skapas för varje plats.
    inline constexpr const char* start = "start", *slut = "slut", *loopstart = "loopstart", *loop = "loop", *baklanges = "baklanges",
        *tonaIn = "tonain", *tonaUt = "tonaut", *hack = "hack", *hackSatt = "hacksatt",
        *takt = "takt", *bitar = "bitar", *utfilter = "utfilter", *tonhojd = "tonhojd", *fin = "fin", *rot = "rot",
        *cutoff = "cutoff", *resonans = "resonans", *filtEnv = "filtenv", *filtAnslag = "filtanslag", *tangent = "tangent",
        *fA = "fa", *fD = "fd", *fS = "fs", *fR = "fr", *aA = "aa", *aD = "ad", *aS = "as", *aR = "ar",
        *anslag = "anslag", *lage = "lage", *stryp = "stryp", *niva = "niva", *pan = "pan", *utgang = "utgang",
        *tyst = "tyst", *solo = "solo";

    // Gemensamma
    inline constexpr const char* spelsatt      = "mo_spelsatt";
    inline constexpr const char* kromPlats     = "mo_krom_plats";     // platsen som spelas KROMATISKT och HACKAT
    inline constexpr const char* glid          = "mo_glid";
    inline constexpr const char* mono          = "mo_mono";
    inline constexpr const char* inspTroskel   = "mo_insp_troskel";
    inline constexpr const char* inspLangd     = "mo_insp_langd";
    inline constexpr const char* utNiva        = "mo_niva";
    inline constexpr const char* limiter       = "mo_limiter";
}

namespace lagen
{
    inline juce::StringArray loop()     { return { "Av", "Fram", "Fram-bak" }; }
    inline juce::StringArray hack()     { return { "4", "8", "16", "32" }; }
    inline juce::StringArray hackSatt() { return { "Transient", "Raster" }; }
    inline juce::StringArray bitar()    { return { "8", "12", "16" }; }
    inline juce::StringArray utfilter() { return { "Av", "Mjukt", svt ("Hårt") }; }
    inline juce::StringArray lage()     { return { svt ("Hållen"), svt ("En gång") }; }
    inline juce::StringArray stryp()    { return { "Ingen", "1", "2", "3", "4" }; }
    inline juce::StringArray utgang()   { return { "Huvud", "Grupp 1", "Grupp 2", "Grupp 3", "Grupp 4" }; }
    inline juce::StringArray spelsatt() { return { "Kromatiskt", "Plattor", "Hackat" }; }
    inline juce::StringArray inspLangd(){ return { "1 takt", "2 takter", "4 takter", "8 takter", "Fri" }; }
}

// Svenska tonnamn: H, inte B.
inline juce::String tonnamn (int ton)
{
    static const char* n[] { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "H" };
    return juce::String (n[((ton % 12) + 12) % 12]) + juce::String (ton / 12 - 1);
}

inline constexpr int parametrarPerPlats = 36;
inline constexpr int antalGemensamma = 8;
inline constexpr int antalParametrar = antalPlatser * parametrarPerPlats + antalGemensamma;   // 584

inline juce::AudioProcessorValueTreeState::ParameterLayout skapaLayout()
{
    using namespace juce;
    AudioProcessorValueTreeState::ParameterLayout l;
    const int version = 1;
    auto pid  = [&] (const juce::String& s) { return ParameterID (s, version); };
    auto flyt = [&] (const juce::String& s, const juce::String& namn, float lo, float hi, float std, const char* enhet = "", float mitt = -1.0f)
    {
        NormalisableRange<float> r (lo, hi);
        if (mitt > lo && mitt < hi) r.setSkewForCentre (mitt);
        l.add (std::make_unique<AudioParameterFloat> (pid (s), namn, r, std, AudioParameterFloatAttributes().withLabel (svt (enhet))));
    };
    auto val  = [&] (const juce::String& s, const juce::String& namn, const StringArray& lista, int std)
    {
        l.add (std::make_unique<AudioParameterChoice> (pid (s), namn, lista, std));
    };
    auto bry  = [&] (const juce::String& s, const juce::String& namn, bool std)
    {
        l.add (std::make_unique<AudioParameterBool> (pid (s), namn, std));
    };

    for (int p = 0; p < antalPlatser; ++p)
    {
        auto P = [p] (const char* vad) { return id::plats (p, vad); };
        auto N = [p] (const char* vad) { return "Plats " + juce::String (p + 1) + " " + svt (vad); };
        // Saxen
        flyt (P (id::start), N ("start"), 0.0f, 1.0f, 0.0f);
        flyt (P (id::slut), N ("slut"), 0.0f, 1.0f, 1.0f);
        flyt (P (id::loopstart), N ("loopstart"), 0.0f, 1.0f, 0.0f);
        val  (P (id::loop), N ("loop"), lagen::loop(), 0);
        bry  (P (id::baklanges), N ("baklänges"), false);
        flyt (P (id::tonaIn), N ("toning in"), 0.0f, 500.0f, 0.0f, "ms", 50.0f);
        flyt (P (id::tonaUt), N ("toning ut"), 0.0f, 2000.0f, 5.0f, "ms", 100.0f);
        val  (P (id::hack), N ("hackning"), lagen::hack(), 2);
        val  (P (id::hackSatt), N ("hackning sätt"), lagen::hackSatt(), 0);
        // Datorn
        flyt (P (id::takt), N ("sampletakt"), 4000.0f, 44100.0f, 44100.0f, "Hz", 16000.0f);
        val  (P (id::bitar), N ("bitar"), lagen::bitar(), 2);
        val  (P (id::utfilter), N ("utfilter"), lagen::utfilter(), 1);
        flyt (P (id::tonhojd), N ("tonhöjd"), -24.0f, 24.0f, 0.0f, "st");
        flyt (P (id::fin), N ("fin"), -50.0f, 50.0f, 0.0f, "ct");
        l.add (std::make_unique<AudioParameterInt> (pid (P (id::rot)), N ("rotton"), 24, 96, 60, AudioParameterIntAttributes()
                  .withStringFromValueFunction ([] (int v, int) { return tonnamn (v); })));
        // Filtret
        flyt (P (id::cutoff), N ("filter"), 30.0f, 20000.0f, 20000.0f, "Hz", 1200.0f);
        flyt (P (id::resonans), N ("resonans"), 0.0f, 1.0f, 0.0f);
        flyt (P (id::filtEnv), N ("filterenvelopp"), -100.0f, 100.0f, 0.0f, "%");
        flyt (P (id::filtAnslag), N ("filter anslag"), 0.0f, 1.0f, 0.0f);
        flyt (P (id::tangent), N ("tangentföljning"), 0.0f, 1.0f, 0.0f);
        flyt (P (id::fA), N ("filter attack"), 0.5f, 5000.0f, 1.0f, "ms", 100.0f);
        flyt (P (id::fD), N ("filter decay"), 1.0f, 10000.0f, 300.0f, "ms", 400.0f);
        flyt (P (id::fS), N ("filter sustain"), 0.0f, 1.0f, 0.0f);
        flyt (P (id::fR), N ("filter release"), 1.0f, 10000.0f, 200.0f, "ms", 400.0f);
        // Förstärkaren
        flyt (P (id::aA), N ("attack"), 0.5f, 5000.0f, 0.5f, "ms", 100.0f);
        flyt (P (id::aD), N ("decay"), 1.0f, 10000.0f, 300.0f, "ms", 400.0f);
        flyt (P (id::aS), N ("sustain"), 0.0f, 1.0f, 1.0f);
        flyt (P (id::aR), N ("release"), 1.0f, 10000.0f, 80.0f, "ms", 400.0f);
        flyt (P (id::anslag), N ("anslag"), 0.0f, 1.0f, 0.6f);
        val  (P (id::lage), N ("läge"), lagen::lage(), 0);
        val  (P (id::stryp), N ("strypgrupp"), lagen::stryp(), 0);
        // Mixern
        flyt (P (id::niva), N ("nivå"), -60.0f, 6.0f, 0.0f, "dB");
        flyt (P (id::pan), N ("panorering"), -100.0f, 100.0f, 0.0f, "%");
        val  (P (id::utgang), N ("utgång"), lagen::utgang(), 0);
        bry  (P (id::tyst), N ("tyst"), false);
        bry  (P (id::solo), N ("solo"), false);
    }

    // Gemensamma
    val  (id::spelsatt, svt ("Spelsätt"), lagen::spelsatt(), 0);
    l.add (std::make_unique<AudioParameterInt> (pid (id::kromPlats), svt ("Spelad plats"), 1, antalPlatser, 1));
    flyt (id::glid, svt ("Glid"), 0.0f, 2000.0f, 0.0f, "ms", 200.0f);
    bry  (id::mono, svt ("Mono"), false);
    flyt (id::inspTroskel, svt ("Inspelning tröskel"), -80.0f, 0.0f, -40.0f, "dB");
    val  (id::inspLangd, svt ("Inspelning längd"), lagen::inspLangd(), 4);
    flyt (id::utNiva, svt ("Utnivå"), -24.0f, 6.0f, 0.0f, "dB");
    bry  (id::limiter, svt ("Limiter"), true);
    return l;
}

} // namespace mo
