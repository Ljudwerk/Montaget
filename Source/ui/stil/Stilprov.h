// Stilprovet: datorstudion med fasta exempelvärden (prov::Panel), och närbilder av reglagen och skärmen
// (prov::Detaljer). Använder varken processorn eller parametrarna – bara bilder. Vågformen är räknad i
// kod (ett metallslag), inte läst från en fil.
//
// Copyright (C) 2026 Montaget-teamet. Licens: AGPLv3, se LICENSE.
#pragma once

#include "Panelbild.h"

namespace mo::stil::prov
{
using panel::Del;

inline juce::String u (const char* s) { return juce::String::fromUTF8 (s); }

// Ett metallslag: några isärstämda delton som klingar av, med ett brusigt anslag. Min/max per kolumn.
inline Vagbild metallslag (int kolumner)
{
    Vagbild v;
    Slump r (7);
    const int n = 48000;
    std::vector<float> x ((size_t) n);
    const float f[] { 180.0f, 431.0f, 677.0f, 1120.0f, 1733.0f }, a[] { 1.0f, 0.6f, 0.45f, 0.3f, 0.2f }, d[] { 0.35f, 0.22f, 0.15f, 0.09f, 0.05f };
    for (int i = 0; i < n; ++i)
    {
        const float t = (float) i / 48000.0f;
        float s = 0.0f;
        for (int k = 0; k < 5; ++k) s += a[k] * std::exp (-t / d[k]) * std::sin (2.0f * pi * f[k] * t);
        s += r.bipolar() * 0.8f * std::exp (-t / 0.012f);
        x[(size_t) i] = 0.45f * s;
    }
    for (int c = 0; c < kolumner; ++c)
    {
        float lo = 0.0f, hi = 0.0f;
        for (int i = c * n / kolumner; i < (c + 1) * n / kolumner; ++i) { lo = std::min (lo, x[(size_t) i]); hi = std::max (hi, x[(size_t) i]); }
        v.minV.push_back (lo); v.maxV.push_back (hi);
    }
    v.start = 0.02f; v.slut = 0.86f; v.loopstart = 0.52f; v.loop = true;
    for (int i = 1; i < 8; ++i) v.bitar.push_back (0.02f + 0.84f * (float) i / 8.0f);
    return v;
}

inline panel::Tillstand exempel()
{
    panel::Tillstand t;
    t.vald = 2;
    const char* namn[] { "Bastrumma", "Virvel", "Metallslag", "Ånga", "Radiobrus", "Kör aah", "Stöt", "", "", "Rör", "", "", "", "", "", "" };
    for (int i = 0; i < 16; ++i) { t.platser[(size_t) i].namn = u (namn[i]); t.platser[(size_t) i].harLjud = t.platser[(size_t) i].namn.isNotEmpty(); }
    t.platser[0].spelar = 0.8f;
    auto& s = t.sektioner;
    s[0] = { "Saxen", {
        { Del::ratt ("Start", 0.02f, "2 %"), Del::ratt ("Slut", 0.86f, "86 %"), Del::ratt ("Loopstart", 0.52f, "52 %"),
          Del::ratt ("Tona in", 0.0f, "0 ms"), Del::ratt ("Tona ut", 0.3f, "40 ms") },
        { Del::valjare ("Loop", { "Av", "Fram", "Fram-bak" }, 1), Del::vippa (u ("Baklänges"), false),
          Del::valjare ("Hack", { "4", "8", "16", "32" }, 1), Del::valjare (u ("Sätt"), { "Trans.", "Raster" }, 0) } } };
    s[1] = { "Datorn", {
        { Del::ratt ("Takt", 0.55f, "22,0 kHz"), Del::ratt (u ("Tonhöjd"), 0.5f, "0 st"), Del::ratt ("Fin", 0.5f, "0 ct"), Del::ratt ("Rot", 0.3f, "A2") },
        { Del::valjare ("Bitar", { "8", "12", "16" }, 1), Del::valjare ("Utfilter", { "Av", "Mjukt", u ("Hårt") }, 0) } } };
    s[2] = { "Filtret", {
        { Del::ratt ("Filter", 0.7f, "4,2 kHz"), Del::ratt ("Res.", 0.35f, "35 %"), Del::ratt ("Env.", 0.65f, "+30 %"),
          Del::ratt ("Anslag", 0.4f, "40 %"), Del::ratt ("Tangent", 0.0f, "0 %") },
        { Del::ratt ("A", 0.05f, "1 ms"), Del::ratt ("D", 0.4f, "300 ms"), Del::ratt ("S", 0.0f, "0 %"), Del::ratt ("R", 0.3f, "200 ms") } } };
    s[3] = { u ("Förstärkaren"), {
        { Del::ratt ("A", 0.0f, "1 ms"), Del::ratt ("D", 0.4f, "300 ms"), Del::ratt ("S", 1.0f, "100 %"), Del::ratt ("R", 0.25f, "80 ms") },
        { Del::ratt ("Anslag", 0.6f, "60 %"), Del::valjare (u ("Läge"), { u ("Hållen"), u ("En gång") }, 1), Del::valjare ("Stryp", { "-", "1", "2", "3", "4" }, 0) } } };
    s[4] = { "Mixern", {
        { Del::ratt (u ("Nivå"), 0.9f, "0,0 dB"), Del::ratt ("Pan", 0.5f, "0 %") },
        { Del::valjare (u ("Utgång"), { "H", "1", "2", "3", "4" }, 0), Del::vippa ("Tyst", false), Del::vippa ("Solo", false) } } };
    t.spel = { Del::valjare (u ("Spelsätt"), { "Krom.", "Plattor", "Hackat" }, 1), Del::ratt ("Plats", 0.13f, "3"),
               Del::ratt ("Glid", 0.0f, "0 ms"), Del::vippa ("Mono", false) };
    t.vag = metallslag (360);
    t.skarmTopp = u ("PLATS 03  METALLSLAG");
    t.skarmHorn = u ("MINNE 12 %");
    t.skarmBotten = u ("LÄNGD 1,00 S   ROT A2   22 KHZ 12 BIT   LOOP FRAM   8 BITAR");
    t.inDb = -40.0f; t.utDb = -9.0f; t.diskett = 1.0f;
    t.programNr = "07"; t.programNamn = "EBM-kit";
    t.utniva = Del::ratt (u ("Utnivå"), 0.8f, "0,0 dB");
    t.limiterPa = true; t.limiterLampa = 0.2f;
    t.omfang = { { 36, 36, farg::gron, "1" }, { 37, 37, farg::gron, "2" }, { 38, 38, farg::barnsten, "3" }, { 39, 39, farg::gron, "4" },
                 { 40, 40, farg::gron, "5" }, { 41, 41, farg::gron, "6" }, { 42, 42, farg::gron, "7" }, { 45, 45, farg::gron, "10" } };
    return t;
}

class Panel : public juce::Component
{
public:
    static constexpr int bredd = panel::bredd, hojd = panel::hojd;
    void paint (juce::Graphics& g) override { panel::rita (g, exempel()); }
};

// Närbilder i dubbel storlek på fronten.
class Detaljer : public juce::Component
{
public:
    static constexpr int bredd = 1200, hojd = 720;
    void paint (juce::Graphics& g) override
    {
        const float skala = krang::stil::fysiskSkala (g);
        front (g, { 0.0f, 0.0f, (float) bredd, (float) hojd }, skala);
        auto rub = [&] (const char* t, float x, float y) { rubrik (g, u (t), { x, y, 360.0f, 12.0f }); };

        rub ("RATTEN 0 · 5 · 10", 30.0f, 20.0f);
        for (int i = 0; i < 3; ++i) ratt (g, { 80.0f + 110.0f * (float) i, 100.0f }, 32.0f, 0.5f * (float) i);
        rub ("VRIDOMKOPPLAREN", 420.0f, 20.0f);
        for (int i = 0; i < 3; ++i) valjare (g, { 480.0f + 130.0f * (float) i, 105.0f }, 22.0f, { "Av", "Fram", "Fram-bak" }, i, 7.0f);
        rub ("VIPPAN OCH LYSDIODERNA", 860.0f, 20.0f);
        vippa (g, { 890.0f, 100.0f }, 36.0f, false);
        vippa (g, { 950.0f, 100.0f }, 36.0f, true);
        led (g, { 1020.0f, 80.0f }, 6.0f, farg::rod, 1.0f);   led (g, { 1060.0f, 80.0f }, 6.0f, farg::gron, 1.0f);
        led (g, { 1100.0f, 80.0f }, 6.0f, farg::barnsten, 1.0f); led (g, { 1140.0f, 80.0f }, 6.0f, farg::rod, 0.0f);

        rub ("SKÄRMEN – VÅGFORMEN OCH PUNKTMATRISEN", 30.0f, 180.0f);
        {
            const auto s = skarmStart (g, { 30.0f, 200.0f, 640.0f, 300.0f });
            punktText (g, u ("PLATS 03  METALLSLAG"), s.glas.getX() + 16.0f, s.glas.getY() + 12.0f, 11.0f, farg::fosfor);
            vagform (g, { s.glas.getX() + 16.0f, s.glas.getY() + 36.0f, s.glas.getWidth() - 32.0f, s.glas.getHeight() - 76.0f }, metallslag (300));
            punktText (g, u ("ÅÄÖ 0123456789 KHZ"), s.glas.getX() + 16.0f, s.glas.getBottom() - 28.0f, 9.0f, farg::fosfor, 0.85f);
            skarmSlut (g, s);
        }
        rub ("PLATSKNAPPARNA", 700.0f, 180.0f);
        knapp (g, { 700.0f, 210.0f, 90.0f, 70.0f }, "1", farg::rod, 1.0f, true);
        knapp (g, { 800.0f, 210.0f, 90.0f, 70.0f }, "2", farg::gron, 0.6f);
        knapp (g, { 900.0f, 210.0f, 90.0f, 70.0f }, "3", farg::barnsten, 1.0f);
        knapp (g, { 1000.0f, 210.0f, 90.0f, 70.0f }, "16", farg::gron, 0.0f);
        rub ("DISKETTFICKAN, STAPLARNA OCH SJUSEGMENTEN", 700.0f, 310.0f);
        diskettficka (g, { 700.0f, 340.0f, 300.0f, 56.0f }, 1.0f);
        stapel (g, { 700.0f, 420.0f, 460.0f, 16.0f }, -12.0f, "IN");
        stapel (g, { 700.0f, 446.0f, 460.0f, 16.0f }, -1.0f, "UT");
        sjusegment (g, { 1020.0f, 340.0f, 140.0f, 56.0f }, "16");

        rub ("KLAVIATUREN MED PLATSERNAS OMFÅNG", 30.0f, 530.0f);
        klaviatur (g, { 30.0f, 550.0f, 1140.0f, 150.0f }, 36, 84,
                   { { 36, 47, farg::gron, "PLATS 1" }, { 48, 59, farg::barnsten, "PLATS 3 – KROMATISKT" }, { 60, 67, farg::rod, "HACKAT" } });
    }
};

} // namespace mo::stil::prov
