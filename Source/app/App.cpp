// Den fristående appen: Montaget i ett eget fönster (krang::app::FristaendeApp). Byggs bara in i appen
// (JUCE_USE_CUSTOM_PLUGIN_STANDALONE_APP).
//
// Copyright (C) 2026 Montaget-teamet. Licens: AGPLv3, se LICENSE.
#include <juce_audio_plugin_client/juce_audio_plugin_client.h>

#if JucePlugin_Build_Standalone && JUCE_USE_CUSTOM_PLUGIN_STANDALONE_APP

#include <krang/app/Fristaende.h>
#include "../PluginProcessor.h"
#include "../ui/stil/Palett.h"

juce::JUCEApplicationBase* juce_CreateApplication()
{
    return new krang::app::FristaendeApp<MontagetProcessor> ("Montaget", mo::stil::farg::front);
}

#endif
