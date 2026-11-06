// Datorstudions färger: ljusbeige plåt, svart frontpanel med silvertryck, fosforgrönt på skärmen,
// bärnsten och rött i lysdioderna. Inga synthwave-färger.
//
// Copyright (C) 2026 Montaget-teamet. Licens: AGPLv3, se LICENSE.
#pragma once

#include <juce_graphics/juce_graphics.h>

namespace mo::stil::farg
{
    // Lådan
    inline const juce::Colour beige        { 0xffd6ccb2 };   // ljusbeige lackad plåt
    inline const juce::Colour beigeMork    { 0xff9f9478 };
    inline const juce::Colour front        { 0xff161616 };   // svart frontpanel
    inline const juce::Colour frontLjus    { 0xff2a2a2a };
    inline const juce::Colour fog          { 0xff050505 };

    // Tryck
    inline const juce::Colour silver       { 0xffd9d8d2 };   // silvertryck på fronten
    inline const juce::Colour silverMatt   { 0xff8a8984 };
    inline const juce::Colour orange       { 0xffd9822b };   // rubriklinjer (dämpad, som på 80-talets datorer)

    // Skärmen
    inline const juce::Colour fosfor       { 0xff42ff7a };
    inline const juce::Colour fosforMatt   { 0xff1d8a40 };
    inline const juce::Colour fosforSvag   { 0xff0e3b1d };
    inline const juce::Colour skarmBotten  { 0xff031207 };

    // Lysdioder
    inline const juce::Colour rod          { 0xffff3424 };
    inline const juce::Colour barnsten     { 0xffffae1a };
    inline const juce::Colour gron         { 0xff3cf05a };

    // Reglage
    inline const juce::Colour ratt         { 0xff1c1c1c };
    inline const juce::Colour rattKant     { 0xff3a3a3a };
    inline const juce::Colour knapp        { 0xff8e8c84 };   // grå tangenter och platsknappar
    inline const juce::Colour knappLjus    { 0xffb9b6ac };
    inline const juce::Colour vitTangent   { 0xffe9e5da };
    inline const juce::Colour svartTangent { 0xff202020 };
}
