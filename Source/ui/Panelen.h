// Panelen: datorstudion ritad från processorns parametrar. I fas 0 visar den den valda platsens värden;
// platsknapparna väljer plats (sparas i tillståndet), annars nås parametrarna i DAW:ens generiska vy.
// Skärmen visar TOM PLATS och remsan FAS 0 – EJ INKOPPLAD tills arkivet och rösterna finns (fas 1).
//
// Copyright (C) 2026 Montaget-teamet. Licens: AGPLv3, se LICENSE.
#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <krang/ui/ValueFormat.h>
#include "../PluginProcessor.h"
#include "stil/Panelbild.h"

namespace mo::ui
{
using stil::panel::Del;
namespace pb = stil::panel;

class Panelen : public juce::Component, private juce::Timer
{
public:
    explicit Panelen (MontagetProcessor& p) : proc (p)
    {
        setOpaque (true);
        tillstand = bygg();
        startTimerHz (30);
    }

    void paint (juce::Graphics& g) override { cache.rita (g, tillstand); }

    int vald() const { return juce::jlimit (0, antalPlatser - 1, (int) proc.apvts.state.getProperty ("vald", 0)); }

    pb::Tillstand bygg() const
    {
        pb::Tillstand t;
        const int v = vald();
        t.vald = v;
        auto P = [v] (const char* vad) { return id::plats (v, vad); };
        auto& s = t.sektioner;
        s[0] = { "Saxen", {
            { ratt (P (id::start), "Start"), ratt (P (id::slut), "Slut"), ratt (P (id::loopstart), "Loopstart"),
              ratt (P (id::tonaIn), "Tona in"), ratt (P (id::tonaUt), "Tona ut") },
            { valjare (P (id::loop), "Loop"), vippa (P (id::baklanges), svt ("Baklänges")), valjare (P (id::hack), "Hack"),
              valjare (P (id::hackSatt), svt ("Sätt"), { "Trans.", "Raster" }) } } };
        s[1] = { "Datorn", {
            { ratt (P (id::takt), "Takt"), ratt (P (id::tonhojd), svt ("Tonhöjd")), ratt (P (id::fin), "Fin"), ratt (P (id::rot), "Rot") },
            { valjare (P (id::bitar), "Bitar"), valjare (P (id::utfilter), "Utfilter") } } };
        s[2] = { "Filtret", {
            { ratt (P (id::cutoff), "Filter"), ratt (P (id::resonans), "Res."), ratt (P (id::filtEnv), "Env."),
              ratt (P (id::filtAnslag), "Anslag"), ratt (P (id::tangent), "Tangent") },
            { ratt (P (id::fA), "A"), ratt (P (id::fD), "D"), ratt (P (id::fS), "S"), ratt (P (id::fR), "R") } } };
        s[3] = { svt ("Förstärkaren"), {
            { ratt (P (id::aA), "A"), ratt (P (id::aD), "D"), ratt (P (id::aS), "S"), ratt (P (id::aR), "R") },
            { ratt (P (id::anslag), "Anslag"), valjare (P (id::lage), svt ("Läge")), valjare (P (id::stryp), "Stryp", { "-", "1", "2", "3", "4" }) } } };
        s[4] = { "Mixern", {
            { ratt (P (id::niva), svt ("Nivå")), ratt (P (id::pan), "Pan") },
            { valjare (P (id::utgang), svt ("Utgång"), { "H", "1", "2", "3", "4" }), vippa (P (id::tyst), "Tyst"), vippa (P (id::solo), "Solo") } } };
        t.spel = { valjare (id::spelsatt, svt ("Spelsätt"), { "Krom.", "Plattor", "Hackat" }), ratt (id::kromPlats, "Plats"),
                   ratt (id::glid, "Glid"), vippa (id::mono, "Mono") };
        t.skarmTopp = "PLATS " + juce::String (v + 1).paddedLeft ('0', 2);
        t.skarmHorn = "MINNE 0 %";
        t.skarmMitt = "TOM PLATS";
        t.skarmBotten = svt ("ARKIVET OCH RÖSTERNA KOMMER I FAS 1");
        t.inDb = proc.inNivaDb.load(); t.utDb = proc.utNivaDb.load();
        t.programNr = "01"; t.programNamn = "Init";
        t.utniva = ratt (id::utNiva, svt ("Utnivå"));
        t.limiterPa = las (id::limiter) > 0.5f;
        t.fas = svt ("FAS 0 – EJ INKOPPLAD");
        return t;
    }

    void mouseDown (const juce::MouseEvent& e) override
    {
        for (int i = 0; i < antalPlatser; ++i)
            if (pb::geo::platsKnapp (i).contains (e.position))
            {
                proc.apvts.state.setProperty ("vald", i, nullptr);
                tillstand = bygg();
                repaint();
                return;
            }
    }

private:
    void timerCallback() override
    {
        auto ny = bygg();
        if (std::abs (ny.inDb - tillstand.inDb) > 0.2f || std::abs (ny.utDb - tillstand.utDb) > 0.2f || andrad())
        {
            tillstand = std::move (ny);
            repaint();
        }
    }

    // Har någon parameter ändrats sedan förra ritningen? (Fas 0: enkel jämförelse av alla värden.)
    bool andrad()
    {
        bool a = false;
        const auto& ps = proc.getParameters();
        if (forra.size() != (size_t) ps.size()) { forra.assign ((size_t) ps.size(), -1.0f); a = true; }
        for (int i = 0; i < ps.size(); ++i)
        {
            const float v = ps[i]->getValue();
            if (std::abs (v - forra[(size_t) i]) > 1.0e-4f) { forra[(size_t) i] = v; a = true; }
        }
        return a;
    }

    juce::RangedAudioParameter& par (const juce::String& pid) const { return *proc.apvts.getParameter (pid); }
    float las (const juce::String& pid) const { return proc.apvts.getRawParameterValue (pid)->load(); }

    Del ratt (const juce::String& pid, const juce::String& namn) const
    {
        auto& p = par (pid);
        const float v = las (pid);
        auto d = Del::ratt (namn, p.convertTo0to1 (v), krang::ui::formatValue (p, v)); d.pid = pid; return d;
    }
    Del valjare (const juce::String& pid, const juce::String& namn, juce::StringArray kort = {}) const
    {
        auto* c = dynamic_cast<juce::AudioParameterChoice*> (&par (pid));
        auto d = Del::valjare (namn, kort.isEmpty() && c != nullptr ? c->choices : kort, c != nullptr ? c->getIndex() : 0); d.pid = pid; return d;
    }
    Del vippa (const juce::String& pid, const juce::String& namn) const { auto d = Del::vippa (namn, las (pid) > 0.5f); d.pid = pid; return d; }

    MontagetProcessor& proc;
    pb::Tillstand tillstand;
    pb::Cache cache;
    std::vector<float> forra;
};

} // namespace mo::ui
