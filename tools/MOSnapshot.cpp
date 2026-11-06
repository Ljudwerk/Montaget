// MOSnapshot – ritar Montagets panel och stilprov till PNG, så att gränssnittet kan granskas utan DAW.
//
//   MOSnapshot fil.png [skala] [id=värde ...]         den riktiga panelen med givna värden
//   MOSnapshot --stilprov fil.png [skala]             datorstudions stilprov (fasta exempelvärden)
//   MOSnapshot --stilprov-detaljer fil.png [skala]    närbilder av reglagen och skärmen
//   MOSnapshot --tid                                  ritningstider för bakgrunden och reglagen
//
// Skalan är fysisk (2 = Retina). Stilproven använder varken processorn eller parametrarna.
//
// Copyright (C) 2026 Montaget-teamet. Licens: AGPLv3, se LICENSE.
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginEditor.h"
#include "ui/stil/Stilprov.h"
#include <iostream>

static int sparaPNG (juce::Component& c, const juce::File& fil, float skala, const char* vad)
{
    const auto bild = c.createComponentSnapshot (c.getLocalBounds(), true, skala);
    fil.deleteFile();
    fil.getParentDirectory().createDirectory();
    juce::FileOutputStream ut (fil);
    juce::PNGImageFormat png;
    if (! ut.openedOk() || ! png.writeImageToStream (bild, ut))
    {
        std::cout << "Kunde inte skriva " << fil.getFullPathName() << "\n";
        return 1;
    }
    std::cout << vad << " ritat till " << fil.getFullPathName() << "\n";
    return 0;
}

int main (int argc, char* argv[])
{
    juce::ScopedJuceInitialiser_GUI juce;
    if (argc < 2)
    {
        std::cout << "Användning: MOSnapshot fil.png [skala] [id=värde ...] | --stilprov | --stilprov-detaljer fil.png [skala] | --tid\n";
        return 1;
    }
    const juce::String lage (argv[1]);
    const auto cwd = juce::File::getCurrentWorkingDirectory();
    namespace pb = mo::stil::panel;

    if (lage == "--tid")
    {
        for (float skala : { 1.0f, 2.0f })
        {
            juce::Image bild (juce::Image::RGB, (int) (pb::bredd * skala), (int) (pb::hojd * skala), true);
            juce::Graphics g (bild);
            g.addTransform (juce::AffineTransform::scale (skala));
            const auto t = mo::stil::prov::exempel();
            auto start = juce::Time::getMillisecondCounterHiRes();
            pb::bakgrund (g, t, skala);
            const auto bak = juce::Time::getMillisecondCounterHiRes() - start;
            start = juce::Time::getMillisecondCounterHiRes();
            for (int i = 0; i < 10; ++i) pb::reglage (g, t);
            const auto reg = (juce::Time::getMillisecondCounterHiRes() - start) / 10.0;
            std::cout << "Skala " << skala << ": bakgrund " << juce::String (bak, 1) << " ms (en gång), reglage "
                      << juce::String (reg, 1) << " ms per omritning\n";
        }
        return 0;
    }
    if (! lage.startsWith ("--"))
    {
        MontagetProcessor p;
        p.setRateAndBufferSizeDetails (48000.0, 512);
        for (int i = 3; i < argc; ++i)
        {
            const juce::String a (argv[i]);
            if (a.startsWith ("vald=")) { p.apvts.state.setProperty ("vald", a.getTrailingIntValue() - 1, nullptr); continue; }
            if (auto* par = p.apvts.getParameter (a.upToFirstOccurrenceOf ("=", false, false)))
                par->setValueNotifyingHost (par->convertTo0to1 (a.fromFirstOccurrenceOf ("=", false, false).getFloatValue()));
        }
        p.prepareToPlay (48000.0, 512);
        std::unique_ptr<juce::AudioProcessorEditor> ed (p.createEditor());
        juce::MessageManager::getInstance()->runDispatchLoopUntil (60);
        auto& panel = dynamic_cast<MontagetEditor&> (*ed).panel;
        panel.setTransform ({});
        const float skala = argc > 2 ? (float) juce::String (argv[2]).getDoubleValue() : 1.0f;
        return sparaPNG (panel, cwd.getChildFile (argv[1]), skala, "Panelen");
    }
    if (argc < 3) return 1;
    const juce::File fil = cwd.getChildFile (argv[2]);
    const float skala = argc > 3 ? (float) juce::String (argv[3]).getDoubleValue() : 1.0f;
    if (lage == "--stilprov")
    {
        mo::stil::prov::Panel panel;
        panel.setBounds (0, 0, mo::stil::prov::Panel::bredd, mo::stil::prov::Panel::hojd);
        return sparaPNG (panel, fil, skala, "Stilprovet");
    }
    if (lage == "--stilprov-detaljer")
    {
        mo::stil::prov::Detaljer d;
        d.setBounds (0, 0, mo::stil::prov::Detaljer::bredd, mo::stil::prov::Detaljer::hojd);
        return sparaPNG (d, fil, skala, "Detaljarket");
    }
    std::cout << "Okänt läge: " << lage << "\n";
    return 1;
}
