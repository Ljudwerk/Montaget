// Datorstudion som bild: tillståndet, geometrin och ritningen, delade av stilprovet och den riktiga panelen.
//
// panel::Tillstand – allt som syns (vald plats, skärmen, sektionernas reglage, platsknapparna, mätarna).
// panel::geo       – var allt sitter, så att panelen och musträffarna aldrig glider isär.
// panel::bakgrund  – lådan, fronten, galler, rubriker och tryck (cachas).
// panel::reglage   – skärmen, rattar, knappar, lysdioder och klaviaturen ovanpå.
//
// Copyright (C) 2026 Montaget-teamet. Licens: AGPLv3, se LICENSE.
#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <array>
#include <vector>
#include "Datorstudion.h"

namespace mo::stil::panel
{

constexpr int bredd = 1200, hojd = 800;

// ---------------------------------------------------------------------------------------------
// Tillståndet

struct Del
{
    enum class Typ { ratt, valjare, vippa };
    Typ typ = Typ::ratt;
    juce::String namn;
    float varde = 0.0f;              // ratt 0..1
    juce::String text;               // ratt: värdet i klartext
    juce::StringArray lagen;         // väljare
    int lage = 0;
    bool pa = false;                 // vippa
    juce::String pid;                // parametern (tom i stilprovet)

    static Del ratt (const juce::String& n, float v, const juce::String& t)       { Del d; d.typ = Typ::ratt; d.namn = n; d.varde = v; d.text = t; return d; }
    static Del valjare (const juce::String& n, const juce::StringArray& l, int i) { Del d; d.typ = Typ::valjare; d.namn = n; d.lagen = l; d.lage = i; return d; }
    static Del vippa (const juce::String& n, bool p)                              { Del d; d.typ = Typ::vippa; d.namn = n; d.pa = p; return d; }
};

using Rad = std::vector<Del>;

struct Sektion
{
    juce::String titel;
    std::vector<Rad> rader;
};

struct Plats { juce::String namn; bool harLjud = false; float spelar = 0.0f; };

struct Tillstand
{
    int vald = 0;                                    // vald plats (0..15)
    std::array<Plats, 16> platser;
    std::array<Sektion, 5> sektioner;                // Saxen, Datorn, Filtret, Förstärkaren, Mixern
    Rad spel;                                        // platsraden: spelsätt, spelad plats, glid, mono
    Vagbild vag;
    juce::String skarmTopp, skarmBotten, skarmHorn;  // skärmens textrader
    juce::String skarmMitt;                          // stor text mitt på skärmen (t.ex. TOM PLATS)
    float inDb = -120.0f, utDb = -120.0f, diskett = 0.0f;
    bool spelarIn = false;
    juce::String programNr = "01", programNamn = "Init";
    Del utniva;
    bool limiterPa = true; float limiterLampa = 0.0f;
    std::vector<Omfang> omfang;
    juce::String fas;                                // "FAS 0 – EJ INKOPPLAD" o.d., tom = inget
};

// ---------------------------------------------------------------------------------------------
// Geometrin

namespace geo
{
    inline const juce::Rectangle<float> front    { 14.0f, 40.0f, 1172.0f, 746.0f };
    inline const juce::Rectangle<float> diskett  { 560.0f, 54.0f, 230.0f, 42.0f };
    inline const juce::Rectangle<float> stapelIn { 820.0f, 58.0f, 340.0f, 12.0f };
    inline const juce::Rectangle<float> stapelUt { 820.0f, 80.0f, 340.0f, 12.0f };
    inline const juce::Rectangle<float> skarm    { 30.0f, 112.0f, 762.0f, 360.0f };
    inline const std::array<juce::Rectangle<float>, 5> sektion {{
        { 806.0f, 112.0f, 364.0f, 178.0f },    // Saxen
        { 806.0f, 296.0f, 364.0f, 176.0f },    // Datorn
        { 516.0f, 486.0f, 250.0f, 166.0f },    // Filtret
        { 776.0f, 486.0f, 226.0f, 166.0f },    // Förstärkaren
        { 1012.0f, 486.0f, 158.0f, 166.0f } }};// Mixern
    inline const juce::Rectangle<float> platser  { 30.0f, 486.0f, 476.0f, 166.0f };
    inline juce::Rectangle<float> platsKnapp (int i)
    {
        const float b = 54.0f, h = 40.0f;
        return { platser.getX() + 8.0f + (float) (i % 8) * (b + 4.6f), platser.getY() + 18.0f + (float) (i / 8) * (h + 6.0f), b, h };
    }
    inline const juce::Rectangle<float> spelRad  { 30.0f, 604.0f, 476.0f, 48.0f };
    inline const juce::Rectangle<float> spelaIn  { 438.0f, 612.0f, 60.0f, 34.0f };
    inline const juce::Rectangle<float> klaviatur{ 30.0f, 666.0f, 824.0f, 108.0f };
    inline const juce::Rectangle<float> program  { 868.0f, 676.0f, 70.0f, 40.0f };
    inline const std::array<juce::Rectangle<float>, 4> knappar {{
        { 868.0f, 730.0f, 40.0f, 34.0f }, { 914.0f, 730.0f, 40.0f, 34.0f }, { 960.0f, 730.0f, 58.0f, 34.0f }, { 1024.0f, 730.0f, 54.0f, 34.0f } }};
    inline const juce::Point<float> utniva  { 1128.0f, 712.0f };
    inline const juce::Point<float> limiter { 1128.0f, 758.0f };
    constexpr int lagstaTon = 36, hogstaTon = 96;
}

// ---------------------------------------------------------------------------------------------
// Reglagen i sektionerna

inline juce::Point<float> centrum (const Del& d, juce::Rectangle<float> cell)
{
    return { cell.getCentreX(), cell.getCentreY() + (d.typ == Del::Typ::valjare ? 5.0f : 2.0f) };
}
inline float radie (const Del& d, float r) { return d.typ == Del::Typ::valjare ? r * 0.75f : (d.typ == Del::Typ::vippa ? 8.0f : r); }

// Går igenom alla reglage med cell och radie: f (del, cell, r). Sektionerna först, sist platsraden.
template <typename F>
void forVarjeDel (const Tillstand& t, F&& f)
{
    for (size_t s = 0; s < t.sektioner.size(); ++s)
    {
        const auto& sek = t.sektioner[s];
        const auto inn = geo::sektion[s].withTrimmedTop (16.0f).reduced (4.0f, 2.0f);
        const int n = (int) sek.rader.size();
        const float r = n > 1 ? 11.5f : 15.0f;
        for (int ri = 0; ri < n; ++ri)
        {
            const auto rr = inn.withHeight (inn.getHeight() / (float) n).translated (0.0f, inn.getHeight() / (float) n * (float) ri);
            const auto& rad = sek.rader[(size_t) ri];
            const float w = rr.getWidth() / (float) juce::jmax<size_t> (1, rad.size());
            for (size_t k = 0; k < rad.size(); ++k) f (rad[k], rr.withX (rr.getX() + w * (float) k).withWidth (w), r);
        }
    }
    const auto sr = geo::spelRad.withRight (geo::spelaIn.getX() - 6.0f);
    const float w = sr.getWidth() / (float) juce::jmax<size_t> (1, t.spel.size());
    for (size_t k = 0; k < t.spel.size(); ++k) f (t.spel[k], sr.withX (sr.getX() + w * (float) k).withWidth (w), 11.0f);
}

inline void del (juce::Graphics& g, const Del& d, juce::Rectangle<float> cell, float r)
{
    const auto namn = d.namn.toUpperCase();
    const auto c = centrum (d, cell);
    switch (d.typ)
    {
        case Del::Typ::ratt:
            skriv (g, namn, { cell.getX(), c.y - r * 1.5f - 10.0f, cell.getWidth(), 7.0f }, 5.2f, farg::silver);
            ratt (g, c, r, d.varde);
            skriv (g, d.text, { cell.getX(), c.y + r * 1.35f + 1.0f, cell.getWidth(), 7.0f }, 4.8f, farg::barnsten.withAlpha (0.9f));
            break;
        case Del::Typ::valjare:
            valjare (g, c, radie (d, r), d.lagen, d.lage, 4.6f);
            skriv (g, namn, { cell.getX(), c.y + radie (d, r) * 1.3f + 2.0f, cell.getWidth(), 7.0f }, 5.2f, farg::silver);
            break;
        case Del::Typ::vippa:
            skriv (g, namn, { cell.getX(), c.y - 22.0f, cell.getWidth(), 7.0f }, 5.2f, farg::silver);
            vippa (g, c, 16.0f, d.pa);
            led (g, { c.x, c.y + 17.0f }, 2.0f, farg::rod, d.pa ? 1.0f : 0.0f);
            break;
    }
}

// ---------------------------------------------------------------------------------------------
// Bakgrunden

inline void bakgrund (juce::Graphics& g, const Tillstand& t, float skala)
{
    juce::ignoreUnused (t);
    // Den beige lådan
    {
        juce::Rectangle<float> yta (0.0f, 0.0f, (float) bredd, (float) hojd);
        juce::Path p; p.addRectangle (yta);
        krang::stil::fyllMedTextur (g, p, plat (bredd, hojd, skala, 1983), yta, skala);
        juce::ColourGradient skugga (juce::Colours::white.withAlpha (0.15f), 0.0f, 0.0f, juce::Colours::black.withAlpha (0.15f), 0.0f, (float) hojd, false);
        g.setGradientFill (skugga);
        g.fillRect (yta);
        galler (g, { 260.0f, 12.0f, 680.0f, 16.0f }, 40);
        skriv (g, juce::String::fromUTF8 ("MONTAGET · SAMPLINGSDATOR"), { 30.0f, 14.0f, 220.0f, 12.0f }, 6.0f, farg::beigeMork.darker (0.5f), juce::Justification::centredLeft);
        skriv (g, "MO-85", { 960.0f, 14.0f, 210.0f, 12.0f }, 6.0f, farg::beigeMork.darker (0.5f), juce::Justification::centredRight);
    }
    // Fronten
    front (g, geo::front, skala);
    // Modellskylten i silvertryck
    {
        g.setColour (farg::silver);
        ls::rita (g, "MONTAGET", { 32.0f, 56.0f, 300.0f, 24.0f }, text (20.0f, 2.6f, 3.0f), juce::Justification::centredLeft);
        g.setColour (farg::orange);
        g.fillRect (32.0f, 86.0f, 250.0f, 2.0f);
        skriv (g, "MO-85   SAMPLINGSDATOR", { 32.0f, 90.0f, 300.0f, 9.0f }, 6.0f, farg::silverMatt, juce::Justification::centredLeft);
        skriv (g, "DISKETT", { geo::diskett.getX(), geo::diskett.getBottom() + 3.0f, 80.0f, 7.0f }, 5.0f, farg::silverMatt, juce::Justification::centredLeft);
    }
    // Sektionsrubriker
    const char* titlar[] { "SAXEN", "DATORN", "FILTRET", "FÖRSTÄRKAREN", "MIXERN" };
    for (size_t s = 0; s < geo::sektion.size(); ++s)
    {
        rubrik (g, juce::String::fromUTF8 (titlar[s]), geo::sektion[s].withHeight (12.0f));
        g.setColour (juce::Colours::white.withAlpha (0.04f));
        g.fillRoundedRectangle (geo::sektion[s].withTrimmedTop (14.0f), 4.0f);
    }
    rubrik (g, "PLATSERNA", geo::platser.withHeight (12.0f));
    g.setColour (juce::Colours::white.withAlpha (0.04f));
    g.fillRoundedRectangle (geo::platser.withTrimmedTop (14.0f), 4.0f);
    skriv (g, "PROGRAM", { geo::program.getX(), geo::program.getY() - 11.0f, 80.0f, 7.0f }, 5.2f, farg::silver, juce::Justification::centredLeft);
    skriv (g, juce::String::fromUTF8 ("UTNIVÅ"), { geo::utniva.x - 30.0f, geo::utniva.y - 30.0f, 60.0f, 7.0f }, 5.2f, farg::silver);
    skriv (g, "LIMITER", { geo::limiter.x - 40.0f, geo::limiter.y - 3.0f, 30.0f, 7.0f }, 4.8f, farg::silverMatt, juce::Justification::centredRight);
}

// ---------------------------------------------------------------------------------------------
// Det som rör sig

inline void reglage (juce::Graphics& g, const Tillstand& t)
{
    // Skärmen
    {
        const auto s = skarmStart (g, geo::skarm);
        const auto gl = s.glas;
        punktText (g, t.skarmTopp, gl.getX() + 18.0f, gl.getY() + 12.0f, 9.0f, farg::fosfor);
        if (t.skarmHorn.isNotEmpty())
            punktText (g, t.skarmHorn, gl.getRight() - 18.0f - punktBredd (t.skarmHorn, 7.0f), gl.getY() + 14.0f, 7.0f, farg::fosfor, 0.75f);
        const auto vagRuta = juce::Rectangle<float> (gl.getX() + 18.0f, gl.getY() + 34.0f, gl.getWidth() - 36.0f, gl.getHeight() - 76.0f);
        if (t.skarmMitt.isNotEmpty())
        {
            const float b = punktBredd (t.skarmMitt, 16.0f);
            punktText (g, t.skarmMitt, vagRuta.getCentreX() - b * 0.5f, vagRuta.getCentreY() - 8.0f, 16.0f, farg::fosfor, 0.85f);
        }
        else vagform (g, vagRuta, t.vag);
        punktText (g, t.skarmBotten, gl.getX() + 18.0f, gl.getBottom() - 26.0f, 7.5f, farg::fosfor, 0.85f);
        skarmSlut (g, s);
    }
    // Mätarna och diskettfickan
    stapel (g, geo::stapelIn, t.inDb, "IN");
    stapel (g, geo::stapelUt, t.utDb, "UT");
    diskettficka (g, geo::diskett, t.diskett);

    // Sektionerna och platsraden
    forVarjeDel (t, [&] (const Del& d, juce::Rectangle<float> cell, float r) { del (g, d, cell, r); });

    // Platsknapparna: röd lysdiod = vald, grön = har ett ljud, bärnsten = spelar.
    for (int i = 0; i < 16; ++i)
    {
        const auto& p = t.platser[(size_t) i];
        const bool vald = i == t.vald;
        const auto f = p.spelar > 0.05f ? farg::barnsten : (vald ? farg::rod : farg::gron);
        const float ljus = p.spelar > 0.05f ? p.spelar : (vald ? 1.0f : (p.harLjud ? 0.6f : 0.0f));
        knapp (g, geo::platsKnapp (i), juce::String (i + 1), f, ljus, vald);
    }
    knapp (g, geo::spelaIn, "SPELA IN", farg::rod, t.spelarIn ? 1.0f : 0.0f, t.spelarIn);

    // Klaviaturen
    klaviatur (g, geo::klaviatur, geo::lagstaTon, geo::hogstaTon, t.omfang);

    // Programmet och utgången
    sjusegment (g, geo::program, t.programNr);
    skriv (g, t.programNamn.toUpperCase(), { geo::program.getRight() + 8.0f, geo::program.getY() + 4.0f, 200.0f, 10.0f }, 7.0f, farg::silver, juce::Justification::centredLeft);
    const char* kt[] { "\xe2\x97\x80", "\xe2\x96\xb6", "SPARA", "MENY" };
    for (int i = 0; i < 4; ++i) knapp (g, geo::knappar[(size_t) i], juce::String::fromUTF8 (kt[i]), {}, -1.0f);
    ratt (g, geo::utniva, 13.0f, t.utniva.varde);
    skriv (g, t.utniva.text, { geo::utniva.x - 30.0f, geo::utniva.y + 18.0f, 60.0f, 7.0f }, 4.8f, farg::barnsten.withAlpha (0.9f));
    led (g, geo::limiter, 3.0f, farg::rod, t.limiterPa ? juce::jmax (0.15f, t.limiterLampa) : 0.0f);

    // Fasen: en Dymo-remsa över skärmens nederkant så länge ljudet inte är inkopplat.
    if (t.fas.isNotEmpty())
    {
        const float b = ls::bredd (t.fas, text (7.0f)) + 20.0f;
        auto r = juce::Rectangle<float> (b, 16.0f).withCentre ({ geo::skarm.getCentreX(), geo::skarm.getBottom() - 2.0f });
        g.setColour (juce::Colour (0xffb01818));
        g.fillRect (r);
        skriv (g, t.fas, r, 7.0f, juce::Colours::white);
    }
}

inline void rita (juce::Graphics& g, const Tillstand& t)
{
    bakgrund (g, t, krang::stil::fysiskSkala (g));
    reglage (g, t);
}

// Bakgrunden cachad per fysisk skala.
class Cache
{
public:
    void rita (juce::Graphics& g, const Tillstand& t)
    {
        const float skala = krang::stil::fysiskSkala (g);
        if (! bild.isValid() || std::abs (skala - bildSkala) > 0.01f)
        {
            bildSkala = skala;
            bild = juce::Image (juce::Image::RGB, (int) std::ceil (bredd * skala), (int) std::ceil (hojd * skala), true);
            juce::Graphics bg (bild);
            bg.addTransform (juce::AffineTransform::scale (skala));
            bakgrund (bg, t, skala);
        }
        g.drawImageTransformed (bild, juce::AffineTransform::scale (1.0f / bildSkala));
        reglage (g, t);
    }

private:
    juce::Image bild;
    float bildSkala = 0.0f;
};

} // namespace mo::stil::panel
