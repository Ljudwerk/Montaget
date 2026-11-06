// Datorstudions ritverktyg: beige plåt med ventilationsgaller, svart front med silvertryck, den gröna
// fosforskärmen (rundad, sveplinjer, efterlysning, punktmatris), svarta rattar med vitt streck,
// vridomkopplare, vippor, platsknappar med lysdiod, diskettfickan, lysdiodsstaplar, sjusegment och
// klaviaturen. Allt ritas i kod, i designpixlar.
//
// Punktmatrisen: linjeskriftens streck samplas till punkter, så att texten ser ut att vara tänd punkt för
// punkt på skärmen – utan typsnittsfil.
//
// Copyright (C) 2026 Montaget-teamet. Licens: AGPLv3, se LICENSE.
#pragma once

#include <juce_graphics/juce_graphics.h>
#include <krang/stil/Linjeskrift.h>
#include <krang/stil/Textur.h>
#include "Palett.h"

namespace mo::stil
{
using krang::stil::Slump;
namespace ls = krang::stil::linjeskrift;
inline const float pi = juce::MathConstants<float>::pi;

// ---------------------------------------------------------------------------------------------
// Text

inline ls::Stil text (float hojd, float tjocklek = -1.0f, float spar = 1.6f, float smal = 1.0f)
{
    return { hojd, tjocklek > 0.0f ? tjocklek : juce::jmax (0.8f, hojd * 0.13f), spar, smal };
}

inline void skriv (juce::Graphics& g, const juce::String& t, juce::Rectangle<float> r, float hojd, juce::Colour c,
                   juce::Justification j = juce::Justification::centred, float tjocklek = -1.0f)
{
    g.setColour (c);
    ls::rita (g, t, r, text (hojd, tjocklek), j);
}

// Punktmatris på skärmen: strecken samplas till runda punkter, med glöd.
inline void punktText (juce::Graphics& g, const juce::String& t, float x, float y, float hojd, juce::Colour c, float ljus = 1.0f)
{
    const auto s = text (hojd, 1.0f, 2.2f);
    const auto bana = ls::bana (t, x, y, s);
    const float steg = hojd / 6.0f, r = hojd * 0.075f;
    juce::PathFlatteningIterator it (bana, {}, 0.2f);
    std::vector<juce::Point<float>> punkter;
    float rest = 0.0f;
    int del = -1;
    while (it.next())
    {
        const juce::Line<float> l (it.x1, it.y1, it.x2, it.y2);
        const float langd = l.getLength();
        float d = rest;
        if (it.subPathIndex != del) { d = 0.0f; del = it.subPathIndex; }
        for (; d <= langd; d += steg) punkter.push_back (l.getPointAlongLine (d));
        rest = d - langd;
    }
    g.setColour (c.withAlpha (0.18f * ljus));
    for (auto& p : punkter) g.fillEllipse (p.x - r * 2.6f, p.y - r * 2.6f, r * 5.2f, r * 5.2f);
    g.setColour (c.withAlpha (ljus));
    for (auto& p : punkter) g.fillEllipse (p.x - r, p.y - r, r * 2.0f, r * 2.0f);
}

inline float punktBredd (const juce::String& t, float hojd) { return ls::bredd (t, text (hojd, 1.0f, 2.2f)); }

// ---------------------------------------------------------------------------------------------
// Lådan

// Beige plåt: jämn lack med fint korn.
inline juce::Image plat (int w, int h, float skala, uint32_t fro)
{
    return krang::stil::plastnarv (w, h, skala, farg::beige, 2.5f, fro);
}

// Ventilationsgaller: en rad avlånga slitsar.
inline void galler (juce::Graphics& g, juce::Rectangle<float> r, int antal)
{
    const float b = r.getWidth() / (float) antal;
    for (int i = 0; i < antal; ++i)
    {
        auto s = juce::Rectangle<float> (r.getX() + b * (float) i + b * 0.2f, r.getY(), b * 0.6f, r.getHeight());
        g.setColour (farg::beigeMork.darker (0.6f));
        g.fillRoundedRectangle (s, s.getWidth() * 0.5f);
        g.setColour (juce::Colours::white.withAlpha (0.35f));
        g.drawLine (s.getX() + 1.0f, s.getBottom() + 0.6f, s.getRight() - 1.0f, s.getBottom() + 0.6f, 0.8f);
    }
}

// Den svarta frontpanelen med fin narv och en avfasad kant.
inline void front (juce::Graphics& g, juce::Rectangle<float> r, float skala)
{
    juce::Path p; p.addRoundedRectangle (r, 6.0f);
    auto tex = krang::stil::plastnarv ((int) r.getWidth(), (int) r.getHeight(), skala, farg::front, 3.0f, 85);
    krang::stil::fyllMedTextur (g, p, tex, r, skala);
    g.setColour (juce::Colours::white.withAlpha (0.10f));
    g.strokePath (p, juce::PathStrokeType (1.2f));
    g.setColour (juce::Colours::black.withAlpha (0.6f));
    g.drawRoundedRectangle (r.reduced (1.5f), 5.0f, 1.0f);
}

// En rubrik i silvertryck med en tunn orange linje efter, som på 80-talets datorer.
inline void rubrik (juce::Graphics& g, const juce::String& t, juce::Rectangle<float> r)
{
    const auto s = text (7.0f);
    const float b = ls::bredd (t, s);
    skriv (g, t, r.withWidth (b + 2.0f), 7.0f, farg::silver, juce::Justification::centredLeft);
    g.setColour (farg::orange.withAlpha (0.8f));
    g.drawLine (r.getX() + b + 8.0f, r.getCentreY(), r.getRight(), r.getCentreY(), 1.0f);
}

// ---------------------------------------------------------------------------------------------
// Skärmen

struct Skarm
{
    juce::Rectangle<float> glas;   // själva glaset, där innehållet ritas
};

// Ramen och glaset. Innehållet ritas mellan skarmStart och skarmSlut.
inline Skarm skarmStart (juce::Graphics& g, juce::Rectangle<float> r)
{
    // Ramen: grå plast med djup
    juce::ColourGradient ram (juce::Colour (0xff3b3b38), r.getX(), r.getY(), juce::Colour (0xff1a1a19), r.getX(), r.getBottom(), false);
    g.setGradientFill (ram);
    g.fillRoundedRectangle (r, 10.0f);
    auto glas = r.reduced (16.0f, 14.0f);
    g.setColour (juce::Colours::black);
    g.fillRoundedRectangle (glas.expanded (3.0f), 22.0f);
    juce::Path form; form.addRoundedRectangle (glas, 20.0f);
    juce::ColourGradient botten (farg::skarmBotten.brighter (0.25f), glas.getCentreX(), glas.getCentreY(),
                                 farg::skarmBotten.darker (0.8f), glas.getX(), glas.getY(), true);
    g.setGradientFill (botten);
    g.fillPath (form);
    g.saveState();
    g.reduceClipRegion (form);
    return { glas };
}

inline void skarmSlut (juce::Graphics& g, const Skarm& s)
{
    // Sveplinjer och rundningens glans
    for (float y = s.glas.getY(); y < s.glas.getBottom(); y += 2.0f)
    {
        g.setColour (juce::Colours::black.withAlpha (0.22f));
        g.fillRect (s.glas.getX(), y, s.glas.getWidth(), 0.8f);
    }
    juce::ColourGradient vinj (juce::Colours::transparentBlack, s.glas.getCentreX(), s.glas.getCentreY(),
                               juce::Colours::black.withAlpha (0.55f), s.glas.getX(), s.glas.getY(), true);
    g.setGradientFill (vinj);
    g.fillRect (s.glas);
    juce::ColourGradient glans (juce::Colours::white.withAlpha (0.07f), s.glas.getX(), s.glas.getY(),
                                juce::Colours::transparentWhite, s.glas.getX() + s.glas.getWidth() * 0.4f, s.glas.getY() + s.glas.getHeight() * 0.4f, false);
    g.setGradientFill (glans);
    g.fillRect (s.glas);
    g.restoreState();
    juce::Path form; form.addRoundedRectangle (s.glas, 20.0f);
    g.setColour (juce::Colours::black.withAlpha (0.7f));
    g.strokePath (form, juce::PathStrokeType (2.0f));
}

// En fosforlinje med glöd (för vågformen och markörerna).
inline void fosforBana (juce::Graphics& g, const juce::Path& p, float tjocklek, float ljus = 1.0f)
{
    g.setColour (farg::fosfor.withAlpha (0.10f * ljus));
    g.strokePath (p, juce::PathStrokeType (tjocklek * 5.0f));
    g.setColour (farg::fosfor.withAlpha (0.25f * ljus));
    g.strokePath (p, juce::PathStrokeType (tjocklek * 2.2f));
    g.setColour (farg::fosfor.withAlpha (ljus));
    g.strokePath (p, juce::PathStrokeType (tjocklek));
}

// Vågformen: min/max per kolumn, fylld, med markörer (start, loop, slut) och bitarnas streck.
struct Vagbild
{
    std::vector<float> minV, maxV;      // per kolumn, −1..1
    float start = 0.0f, slut = 1.0f, loopstart = 0.0f;
    bool loop = false, baklanges = false;
    std::vector<float> bitar;           // hackningens snitt, 0..1
};

inline void vagform (juce::Graphics& g, juce::Rectangle<float> r, const Vagbild& v)
{
    // Rutnät
    g.setColour (farg::fosforSvag);
    for (int i = 1; i < 8; ++i) g.drawLine (r.getX() + r.getWidth() * (float) i / 8.0f, r.getY(), r.getX() + r.getWidth() * (float) i / 8.0f, r.getBottom(), 0.5f);
    g.drawLine (r.getX(), r.getCentreY(), r.getRight(), r.getCentreY(), 0.6f);
    const int n = (int) v.minV.size();
    if (n > 1)
    {
        juce::Path fylld;
        for (int i = 0; i < n; ++i)
        {
            const float x = r.getX() + r.getWidth() * (float) i / (float) (n - 1);
            const float y = r.getCentreY() - v.maxV[(size_t) i] * r.getHeight() * 0.48f;
            if (i == 0) fylld.startNewSubPath (x, y); else fylld.lineTo (x, y);
        }
        for (int i = n - 1; i >= 0; --i)
        {
            const float x = r.getX() + r.getWidth() * (float) i / (float) (n - 1);
            fylld.lineTo (x, r.getCentreY() - v.minV[(size_t) i] * r.getHeight() * 0.48f);
        }
        fylld.closeSubPath();
        // Utanför start–slut är vågen svag.
        g.saveState();
        g.reduceClipRegion (r.withX (r.getX() + r.getWidth() * v.start).withRight (r.getX() + r.getWidth() * v.slut).toNearestInt());
        g.setColour (farg::fosfor.withAlpha (0.55f));
        g.fillPath (fylld);
        fosforBana (g, fylld, 0.8f, 0.9f);
        g.restoreState();
        g.setColour (farg::fosforMatt.withAlpha (0.35f));
        g.fillPath (fylld);
    }
    // Bitarnas snitt: prickade linjer
    for (float b : v.bitar)
    {
        const float x = r.getX() + r.getWidth() * b;
        for (float y = r.getY(); y < r.getBottom(); y += 5.0f)
        {
            g.setColour (farg::fosfor.withAlpha (0.6f));
            g.fillEllipse (x - 0.8f, y, 1.6f, 1.6f);
        }
    }
    // Markörerna
    auto markor = [&] (float pos, const char* namn, bool topp)
    {
        const float x = r.getX() + r.getWidth() * pos;
        juce::Path p; p.startNewSubPath (x, r.getY()); p.lineTo (x, r.getBottom());
        fosforBana (g, p, 1.2f);
        juce::Path flagga;
        const float y = topp ? r.getY() : r.getBottom() - 12.0f;
        flagga.addRectangle (x, y, 12.0f, 12.0f);
        g.setColour (farg::fosfor); g.fillPath (flagga);
        g.setColour (farg::skarmBotten);
        ls::rita (g, namn, { x, y, 12.0f, 12.0f }, text (7.0f, 1.3f));
    };
    markor (v.start, "S", true);
    markor (v.slut, "E", true);
    if (v.loop) markor (v.loopstart, "L", false);
}

// ---------------------------------------------------------------------------------------------
// Reglage

inline float rattVinkel (float v) { return (-0.75f + 1.5f * juce::jlimit (0.0f, 1.0f, v)) * pi; }

// Svart ratt med vitt streck och en tryckt skala på fronten.
inline void ratt (juce::Graphics& g, juce::Point<float> c, float r, float varde)
{
    for (int i = 0; i <= 10; ++i)
    {
        const float a = rattVinkel ((float) i / 10.0f);
        const float r1 = r * 1.25f, r2 = r * (i % 5 == 0 ? 1.45f : 1.35f);
        g.setColour (farg::silverMatt);
        g.drawLine (c.x + std::sin (a) * r1, c.y - std::cos (a) * r1, c.x + std::sin (a) * r2, c.y - std::cos (a) * r2, i % 5 == 0 ? 1.0f : 0.6f);
    }
    g.setColour (juce::Colours::black.withAlpha (0.6f));
    g.fillEllipse (c.x - r + 1.0f, c.y - r + 2.0f, r * 2, r * 2);
    juce::ColourGradient kjol (farg::rattKant.brighter (0.4f), c.x - r, c.y - r, farg::ratt.darker (0.6f), c.x + r, c.y + r, false);
    g.setGradientFill (kjol);
    g.fillEllipse (c.x - r, c.y - r, r * 2, r * 2);
    const float rt = r * 0.78f;
    juce::ColourGradient topp (juce::Colour (0xff3a3a3a), c.x - rt * 0.5f, c.y - rt, juce::Colour (0xff121212), c.x + rt * 0.5f, c.y + rt, false);
    g.setGradientFill (topp);
    g.fillEllipse (c.x - rt, c.y - rt, rt * 2, rt * 2);
    const float a = rattVinkel (varde);
    g.setColour (juce::Colours::white);
    g.drawLine (c.x + std::sin (a) * r * 0.2f, c.y - std::cos (a) * r * 0.2f, c.x + std::sin (a) * r * 0.95f, c.y - std::cos (a) * r * 0.95f, juce::jmax (1.2f, r * 0.12f));
}

// Vridomkopplare: liten svart pekarratt, lägena tryckta i en båge.
inline void valjare (juce::Graphics& g, juce::Point<float> c, float r, const juce::StringArray& lagen, int lage, float texthojd = 5.0f)
{
    const int n = juce::jmax (2, lagen.size());
    const float span = juce::jmin (1.4f * pi, (float) (n - 1) * 0.4f * pi);
    auto vinkel = [&] (int i) { return -span * 0.5f + span * (float) i / (float) (n - 1); };
    for (int i = 0; i < n; ++i)
    {
        const float a = vinkel (i);
        const bool vald = i == lage;
        const auto t = lagen[i].toUpperCase();
        const float tb = ls::bredd (t, text (texthojd));
        const float rt = r * 1.5f + tb * 0.5f * std::abs (std::sin (a)) + texthojd * 0.5f;
        skriv (g, t, juce::Rectangle<float> (tb + 2.0f, texthojd + 2.0f).withCentre ({ c.x + std::sin (a) * rt, c.y - std::cos (a) * rt }),
               texthojd, vald ? farg::silver : farg::silverMatt, juce::Justification::centred, vald ? texthojd * 0.17f : -1.0f);
        if (vald)
        {
            g.setColour (farg::barnsten);
            g.fillEllipse (c.x + std::sin (a) * r * 1.22f - 1.4f, c.y - std::cos (a) * r * 1.22f - 1.4f, 2.8f, 2.8f);
        }
    }
    ratt (g, c, r, 0.0f);   // själva ratten; pekaren ritas om nedan
    const float a = vinkel (juce::jlimit (0, n - 1, lage));
    g.setColour (juce::Colours::white);
    g.drawLine (c.x, c.y, c.x + std::sin (a) * r * 0.95f, c.y - std::cos (a) * r * 0.95f, 1.6f);
    g.setColour (farg::ratt);
    g.fillEllipse (c.x - r * 0.3f, c.y - r * 0.3f, r * 0.6f, r * 0.6f);
}

// Tryckknapp i grå plast med en lysdiod (platsknapparna, SPELA IN, ◀ ▶).
inline void knapp (juce::Graphics& g, juce::Rectangle<float> r, const juce::String& t, juce::Colour led, float ledLjus, bool nere = false)
{
    g.setColour (juce::Colours::black);
    g.fillRoundedRectangle (r.expanded (1.5f), 3.0f);
    auto k = nere ? r.reduced (0.5f).translated (0.0f, 0.8f) : r;
    juce::ColourGradient gr (farg::knappLjus, k.getX(), k.getY(), farg::knapp.darker (0.25f), k.getX(), k.getBottom(), false);
    g.setGradientFill (gr);
    g.fillRoundedRectangle (k, 2.5f);
    g.setColour (juce::Colours::white.withAlpha (0.35f));
    g.drawLine (k.getX() + 2.0f, k.getY() + 1.0f, k.getRight() - 2.0f, k.getY() + 1.0f, 0.8f);
    skriv (g, t, k.withTrimmedTop (k.getHeight() * 0.35f), juce::jmin (8.0f, k.getHeight() * 0.32f), juce::Colour (0xff1e1e1e));
    if (ledLjus >= 0.0f)
    {
        const juce::Point<float> c { k.getCentreX(), k.getY() + k.getHeight() * 0.2f };
        const float rl = 2.6f;
        if (ledLjus > 0.05f) { g.setColour (led.withAlpha (0.35f * ledLjus)); g.fillEllipse (c.x - rl * 2.4f, c.y - rl * 2.4f, rl * 4.8f, rl * 4.8f); }
        g.setColour (led.darker (2.0f).interpolatedWith (led.brighter (0.3f), ledLjus));
        g.fillEllipse (c.x - rl, c.y - rl, rl * 2, rl * 2);
    }
}

// Liten vippa (PÅ uppe, AV nere).
inline void vippa (juce::Graphics& g, juce::Point<float> c, float s, bool pa)
{
    g.setColour (juce::Colour (0xff2c2c2c));
    g.fillRoundedRectangle (c.x - s * 0.45f, c.y - s * 0.7f, s * 0.9f, s * 1.4f, 2.0f);
    g.setColour (juce::Colours::black);
    g.fillRect (c.x - s * 0.18f, c.y - s * 0.5f, s * 0.36f, s);
    const float y = pa ? c.y - s * 0.32f : c.y + s * 0.32f;
    juce::ColourGradient gr (juce::Colours::white, c.x - s * 0.3f, y, farg::knapp, c.x + s * 0.3f, y, false);
    g.setGradientFill (gr);
    g.fillRoundedRectangle (c.x - s * 0.3f, y - s * 0.22f, s * 0.6f, s * 0.44f, 1.5f);
}

// Lysdiod.
inline void led (juce::Graphics& g, juce::Point<float> c, float r, juce::Colour f, float ljus)
{
    if (ljus > 0.05f) { g.setColour (f.withAlpha (0.3f * ljus)); g.fillEllipse (c.x - r * 2.5f, c.y - r * 2.5f, r * 5.0f, r * 5.0f); }
    g.setColour (f.darker (2.2f).interpolatedWith (f.brighter (0.3f), ljus));
    g.fillEllipse (c.x - r, c.y - r, r * 2, r * 2);
    g.setColour (juce::Colours::white.withAlpha (0.4f));
    g.fillEllipse (c.x - r * 0.5f, c.y - r * 0.6f, r * 0.6f, r * 0.4f);
}

// Lysdiodsstapel (IN/UT): 16 segment, grönt, bärnsten, rött.
inline void stapel (juce::Graphics& g, juce::Rectangle<float> r, float db, const juce::String& namn)
{
    skriv (g, namn, r.withWidth (18.0f), 6.0f, farg::silver, juce::Justification::centredLeft);
    auto s = r.withTrimmedLeft (22.0f);
    const int n = 16;
    const float b = s.getWidth() / (float) n;
    for (int i = 0; i < n; ++i)
    {
        const float troskel = -48.0f + 48.0f * (float) i / (float) (n - 1);
        const auto f = i >= 14 ? farg::rod : (i >= 11 ? farg::barnsten : farg::gron);
        const bool tand = db >= troskel;
        auto seg = juce::Rectangle<float> (s.getX() + b * (float) i + 0.8f, s.getY(), b - 1.6f, s.getHeight());
        g.setColour (tand ? f : f.darker (2.4f));
        g.fillRect (seg);
        if (tand) { g.setColour (f.withAlpha (0.25f)); g.fillRect (seg.expanded (1.0f)); }
    }
}

// Sjusegmentsdisplay i rött bakom rökfärgat glas.
inline void sjusegment (juce::Graphics& g, juce::Rectangle<float> r, const juce::String& siffror)
{
    g.setColour (juce::Colour (0xff120606));
    g.fillRoundedRectangle (r, 3.0f);
    const int n = siffror.length();
    const float b = (r.getWidth() - 8.0f) / (float) juce::jmax (1, n);
    static const uint8_t seg[10] { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f };
    for (int i = 0; i < n; ++i)
    {
        const auto c = siffror[i];
        const uint8_t m = (c >= '0' && c <= '9') ? seg[c - '0'] : (c == '-' ? 0x40 : 0);
        auto d = juce::Rectangle<float> (r.getX() + 4.0f + b * (float) i, r.getY() + 4.0f, b, r.getHeight() - 8.0f).reduced (b * 0.14f, 0.0f);
        const float t = d.getWidth() * 0.16f, w = d.getWidth(), h = d.getHeight();
        auto streck = [&] (int s, float x, float y, float sw, float sh)
        {
            const bool pa = (m >> s) & 1;
            g.setColour (pa ? farg::rod : farg::rod.darker (3.5f));
            g.fillRoundedRectangle (d.getX() + x, d.getY() + y, sw, sh, t * 0.4f);
            if (pa) { g.setColour (farg::rod.withAlpha (0.25f)); g.fillRoundedRectangle (d.getX() + x - 1.0f, d.getY() + y - 1.0f, sw + 2.0f, sh + 2.0f, t); }
        };
        streck (0, t, 0, w - 2 * t, t);
        streck (1, w - t, t, t, h * 0.5f - 1.5f * t);
        streck (2, w - t, h * 0.5f + 0.5f * t, t, h * 0.5f - 1.5f * t);
        streck (3, t, h - t, w - 2 * t, t);
        streck (4, 0, h * 0.5f + 0.5f * t, t, h * 0.5f - 1.5f * t);
        streck (5, 0, t, t, h * 0.5f - 1.5f * t);
        streck (6, t, h * 0.5f - t * 0.5f, w - 2 * t, t);
    }
    g.setColour (juce::Colours::white.withAlpha (0.06f));
    g.fillRoundedRectangle (r.withHeight (r.getHeight() * 0.4f), 3.0f);
}

// Diskettfickan: en svart springa med en lucka och en lampa som tänds när ett ljud läses in.
inline void diskettficka (juce::Graphics& g, juce::Rectangle<float> r, float lampa)
{
    g.setColour (juce::Colour (0xff222220));
    g.fillRoundedRectangle (r, 4.0f);
    auto spring = r.reduced (14.0f, r.getHeight() * 0.38f).translated (-6.0f, 0.0f);
    g.setColour (juce::Colours::black);
    g.fillRoundedRectangle (spring, 2.0f);
    g.setColour (juce::Colour (0xff4a4a46));
    g.fillRect (spring.getX() + spring.getWidth() * 0.35f, spring.getBottom() - 2.0f, spring.getWidth() * 0.3f, 2.0f);
    led (g, { r.getRight() - 12.0f, r.getCentreY() }, 3.0f, farg::gron, lampa);
    g.setColour (juce::Colours::white.withAlpha (0.08f));
    g.drawRoundedRectangle (r, 4.0f, 1.0f);
}

// Klaviaturen: vita och svarta tangenter, och platsernas omfång som färgade band ovanför.
struct Omfang { int lag = 36, hog = 96; juce::Colour farg; juce::String namn; };

inline void klaviatur (juce::Graphics& g, juce::Rectangle<float> r, int lagsta, int hogsta, const std::vector<Omfang>& omfang, int nedtryckt = -1)
{
    auto vit = [] (int t) { const int n = t % 12; return n == 0 || n == 2 || n == 4 || n == 5 || n == 7 || n == 9 || n == 11; };
    int antalVita = 0; for (int t = lagsta; t <= hogsta; ++t) if (vit (t)) ++antalVita;
    const float bandH = 10.0f;
    auto tangenter = r.withTrimmedTop (bandH + 2.0f);
    const float vb = tangenter.getWidth() / (float) antalVita;
    std::vector<float> xFor ((size_t) (hogsta - lagsta + 1));
    int vi = 0;
    for (int t = lagsta; t <= hogsta; ++t)
    {
        if (vit (t)) { xFor[(size_t) (t - lagsta)] = tangenter.getX() + vb * (float) vi; ++vi; }
        else xFor[(size_t) (t - lagsta)] = tangenter.getX() + vb * (float) vi - vb * 0.3f;
    }
    // Banden
    for (auto& o : omfang)
    {
        const int a = juce::jlimit (lagsta, hogsta, o.lag), b = juce::jlimit (lagsta, hogsta, o.hog);
        const float x1 = xFor[(size_t) (a - lagsta)], x2 = xFor[(size_t) (b - lagsta)] + (vit (b) ? vb : vb * 0.6f);
        g.setColour (o.farg.withAlpha (0.85f));
        g.fillRoundedRectangle (x1 + 1.0f, r.getY(), x2 - x1 - 2.0f, bandH - 2.0f, 2.0f);
        if (o.namn.isNotEmpty()) skriv (g, o.namn, { x1, r.getY(), x2 - x1, bandH - 2.0f }, 5.0f, juce::Colours::black);
    }
    for (int t = lagsta; t <= hogsta; ++t)
        if (vit (t))
        {
            auto k = juce::Rectangle<float> (xFor[(size_t) (t - lagsta)], tangenter.getY(), vb, tangenter.getHeight()).reduced (0.5f, 0.0f);
            juce::ColourGradient gr (farg::vitTangent, k.getX(), k.getY(), farg::vitTangent.darker (t == nedtryckt ? 0.25f : 0.08f), k.getX(), k.getBottom(), false);
            g.setGradientFill (gr);
            g.fillRoundedRectangle (k, 1.5f);
            if (t % 12 == 0) skriv (g, "C" + juce::String (t / 12 - 1), k.withTrimmedTop (k.getHeight() - 10.0f), 4.5f, farg::silverMatt);
        }
    for (int t = lagsta; t <= hogsta; ++t)
        if (! vit (t))
        {
            auto k = juce::Rectangle<float> (xFor[(size_t) (t - lagsta)], tangenter.getY(), vb * 0.6f, tangenter.getHeight() * 0.62f);
            juce::ColourGradient gr (juce::Colour (0xff3a3a3a), k.getX(), k.getY(), farg::svartTangent, k.getX(), k.getBottom(), false);
            g.setGradientFill (gr);
            g.fillRoundedRectangle (k, 1.5f);
        }
}

} // namespace mo::stil
