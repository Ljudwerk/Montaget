# Montaget – plan

*Modell MO-85. Samplingsdatorn i en elektronmusikstudio, 1983–85: ljud klipps ur världen, ur radion och fabriken, och spelas som ett instrument.*

**Status:** Planen är skriven 6 november 2026. Fas 0 är inte påbörjad, och Röstverket går före.

Instrumentplugin (VST3, AU, CLAP) och fristående app för macOS. Bygger på samma stack som Analogen och Pianot. Allt i gränssnittet är på svenska.

## 1. Mål

**Montaget ska spela användarens egna ljud som ett 80-talssamplingsinstrument: klippa, vända, loopa, hacka och smutsa ned dem – och spela dem kromatiskt eller som trumplattor.**

Måttstockar:

- metallslagen och fabriksljuden på Depeche Modes mittenskivor
- radiorösterna och klippen hos Front 242
- Frontline Assemblys och Bigod 20:s samplade stötar
- Laibachs montage

Vi kopierar inget och har inga ljud med oss från dem, utan lyssnar för att veta när vi är framme.

Det här måste finnas:

- **Egna filer:** WAV, AIFF och FLAC dras in eller väljs. De läses i en egen tråd, och instrumentet spelar tyst tills ljudet är inne.
- **Inspelning från ingången:** sidokedjan i pluginet eller mikrofonen i appen, med tröskelstart. Montaget samplar det som spelas.
- **Saxen:** start, slut, loop (fram och fram-bak), baklänges, toning och hackning i 4–32 bitar efter transienter eller raster.
- **Datorns färg:** sampletakt 8–44,1 kHz utan filter före, 8/12/16 bitar och utfilter (rekonstruktion av, mjukt eller hårt). Det är 80-talssamplerns grovhet.
- **Spelsätten:** kromatiskt (en plats över hela klaviaturen), plattor (en plats per tangent, för trummor och stötar) och hackat (bitarna på tangenterna).
- **Analogt filter och envelopper:** lågpass 24 dB med resonans, filterenvelopp, förstärkarenvelopp och anslag.
- **16 platser** med egen nivå, panorering och utgångsgrupp.
- **Fabriksljuden genereras i kod:** Montaget har inga inspelade ljud med sig och kan därför spridas, till skillnad från Analogen, Pianot och Stråkarna.

### Inte i 1.0

- Tidssträckning och resyntes
- En inbyggd sekvenser (Kroppsmaskinen eller DAW:en sköter takten)
- Effektkedja utöver färgen och filtret (Pannrummet och Röstverket finns för det)
- Export av program med ljud som ett paket
- E-mu-bankerna som fabriksljud (de får inte spridas)
- Windows och Linux

## 2. Instrumentet

```
 FILER / INGÅNG ─▶ ARKIVET (16 platser) ─▶ SAXEN ─▶ DATORN ─▶ FILTRET ─▶ FÖRSTÄRKAREN ─▶ MIXERN ─▶ LIMITER ─▶ UT
                                                     ▲            ▲            ▲
 MIDI ─▶ KLAVIATUREN (KROMATISKT · PLATTOR · HACKAT) ┴── röster (16, poly) ────┘
```

### Arkivet – platserna

16 platser, och varje plats har ett ljud.

- **Att fylla en plats:** dra in en fil, välj en fil, spela in eller välj ett fabriksljud.
- **Inläsningen:** filen läses i en egen tråd (`krang/samplingar/Laddaren.h`) till 32-bitars flyttal i minnet, högst 60 s per plats.
- **Var platserna sparas:** se öppna frågor.

### Inspelningen

Läget **SPELA IN** fyller vald plats från ingången.

- **Start:** tröskeln startar inspelningen. Utan tröskel startar den vid nästa takt.
- **Längd:** fast längd i takter eller tills man trycker igen.
- **Utan allokering:** inspelningsbufferten förbereds i `prepareToPlay`, så ljudtråden skriver bara in i den.

### Saxen

Reglagen per plats:

- start, slut, loopstart och loopläge (**AV**, **FRAM**, **FRAM-BAK**)
- **BAKLÄNGES**
- toning in och ut
- hackning: antal bitar (4–32) och sätt (**TRANSIENT** eller **RASTER**)

Vågformen visas på fosforskärmen, och markörerna dras med musen (ljuspennan).

### Datorn

- **Sampletakt:** 8–44,1 kHz, utan filter före.
- **Bitar:** 8, 12 eller 16.
- **Utfilter:** **AV** (trappsteg), **MJUKT** eller **HÅRT**.
- **Tonhöjd:** ±24 halvtoner och fin ±50 cent.
- **Rotfrekvens:** grundtonen som spelas utan omstämning. Den detekteras när filen läses in och kan ändras för hand.

### Filtret och Förstärkaren

- **Filtret:** lågpass 24 dB (ladder) med cutoff, resonans, envelopp-mängd, anslag och tangentföljning. Utgår från Analogens filter.
- **Envelopperna:** ADSR för filtret och förstärkaren.
- **Läge:** **EN GÅNG**, eller spela tills tangenten släpps.
- **Stryp-grupp:** platser i samma grupp tystar varandra, som en öppen och en stängd hi-hat.

### Mixern

Varje plats har nivå, panorering, utgång (huvud eller en av fyra stereogrupper till DAW:en) och mute/solo. Sist kommer en mjuk limiter (`krang/dsp/Limiter.h`).

### Klaviaturen och rösterna

Tre spelsätt:

- **KROMATISKT:** vald plats över hela klaviaturen, med rotfrekvensen på rätt tangent.
- **PLATTOR:** plats 1–16 på C1–D#2, och varje plats kan stämmas.
- **HACKAT:** vald plats bitar på tangenterna från C3.

Rösterna:

- 16 röster, poly. Röststöld tonas ut, aldrig med klick.
- Glid i monoläge.
- Anslaget styr nivå och filter.

### Fabriksljuden

Fabriksljuden genereras i kod vid första start och sparas i `~/Library/Application Support/Montaget/Fabriksljud/`. Exempel:

- metallslag (modal syntes, som Plåten i Pannrummet)
- slag på rör och plåt
- ångstötar
- radiobrus och kortvågssvep
- orkesterstöt (syntetisk, som Hugget i Kroppsmaskinen)
- kör-"aah" (formantsyntes)
- trumljud från Stampverket

Det gör att fabriksbandet fungerar direkt, utan inspelade ljud.

## 3. Gränssnittet – datorstudion

### Idén

**En samplingsdator i en elektronmusikstudio, 1983–85.** Det är inte verkstaden, salongen eller fabriken, utan rummet med datorn.

- **Chassit:** en ljusbeige plåtlåda med ventilationsgaller och en svart frontpanel med silvertryck. Tangentbordet har tjocka grå tangenter med tryckt text.
- **Skärmen:** en grön fosfor-CRT med lätt rundning, sveplinjer och efterlysning. Den visar vågformen, markörerna, bitarna och platsernas lista som text i punktmatris.
- **Ljuspennan:** en markör som pennspets på skärmen.
- **Reglagen:** svarta rattar med vitt streck, platsknappar 1–16 med lysdioder och en diskettficka vars lampa tänds när ett ljud läses in.
- **Instrumenten:** två lysdiodsstaplar (IN och UT) och ett sjusegmentsfönster för platsen.

Färger: beige, svart, fosforgrönt, varnings-bärnsten och vitt silvertryck. Ingen rosa synthwave och inga logotyper.

### Layout (designstorlek 1200 × 800, skalas 60–160 %)

```
┌──────────────────────────────────────────────────────────────────────────────┐
│ [MONTAGET MO-85]                     [diskettficka ▭•]   [IN ▮▮▮][UT ▮▮▮]       │
├───────────────────────────────────────────────┬──────────────────────────────┤
│  FOSFORSKÄRMEN – vågformen, markörer, bitar   │  SAXEN  start slut loop hack │
│                                               │  DATORN takt bitar utfilter  │
├───────────────────────────────────────────────┴──────────────────────────────┤
│ PLATSERNA [1][2][3]…[16]  · SPELSÄTT · SPELA IN │ FILTRET │ FÖRSTÄRKAREN │ MIX │
├──────────────────────────────────────────────────────────────────────────────┤
│ [klaviatur med platsernas omfång markerade]    [PROGRAM ◀ ▶ SPARA MENY]         │
└──────────────────────────────────────────────────────────────────────────────┘
```

Det är en vy, där platsknapparna väljer vilken plats rattarna visar. Alla platsers parametrar finns som parametrar och nås via platsknapparna.

### Teknik

- Allt ritas i kod, inklusive fosforn, sveplinjerna och punktmatrisskriften.
- Vågformen ritas i en bakgrundstråd till en bild. Bara markörer, staplar och lampor ritas om, högst 30 bilder/s.
- `MOSnapshot` ritar panelen och stilprovet till PNG.

## 4. Kod och struktur

Stacken är JUCE 8, CMake, clap-juce-extensions, C++20 och AGPLv3.

| Vad | Värde |
| --- | --- |
| Namnrum | `mo` |
| Parameter-ID:n | `mo_<del>_<vad>`, per plats `mo_p01_start` … `mo_p16_…` |
| Tillverkare | `Mntg` |
| Plugin-kod | `Mo85` |
| Bundle-ID | `org.montaget.mo85` |
| PRODUCT_NAME | `Montaget` |
| Programändelse | `.moprog` |
| Verktyg | `MORender`, `MOSnapshot` |

```
montaget/
  Source/
    PluginProcessor.*, PluginEditor.*, Parameters.h
    motor/    Arkivet, Platsen, Rosten, Saxen, Datorn, Inspelningen, Fabriksljud
    ui/       Panelen.h, Skarmen.h; ui/stil/ (Palett, Datorstudion, Fosfor, Stilprov)
    presets/  Fabriksbandet.h
  tools/      MORender.cpp, MOSnapshot.cpp, reaper/
```

| Från / till krang-libs | Ursprung | Fas |
| --- | --- | --- |
| `krang/dsp/Grund.h`, `Limiter.h`, `presets/Programbank.h` | Finns | 0–3 |
| `krang/samplingar/Laddaren.h` | Finns. Montaget laddar enstaka filer, inte paket. | 1 |
| `krang/app/Fristaende.h`, `Inspelaren.h`, `ui/Datortangentbord.h` | Finns | 3 |
| Analogens ladder-filter | Flyttas till `krang/dsp/Ladder.h` när Montaget behöver det | 2 |
| Plåtens modala resonatorer (Pannrummet) | Flyttas till `krang/dsp/Modal.h` för fabriksljuden | 2 |

Regler:

- **Parameter-ID:n döps aldrig om.** Alla 16 platsers parametrar skapas i fas 0.
- Ljudtråden allokerar inget och låser inget. Ljuden byts med atomisk pekare och lämnas till en städtråd.
- Inga ljudfiler checkas in. Fabriksljuden genereras, och testljud görs av MORender.

## 5. Faser och grindar

### Fas 0 – Grunden och utseendet

- Repo, CMake, tomt instrument som laddar i Reaper och är tyst, och appen.
- Alla parametrar, och `MORender --test` för tystnad och parametrarnas ordning.
- **Stilprov:** datorstudion med fosforskärmen (vågform, markörer, punktmatris), platsknapparna, diskettfickan, rattarna och staplarna.

**Grind 0:** Stilprovet är godkänt, pluginet laddar i Reaper och appen startar.

### Fas 1 – Rösterna och Arkivet

- Filinläsning i egen tråd, KROMATISKT spel med 16 röster, Saxen (start, slut, loop, baklänges, toning) och förstärkarenveloppen.
- Testfall:
  - ingen allokering i ljudtråden
  - byte av ljud under spel utan klick
  - rätt tonhöjd i cent
  - loopsömmar utan klick
  - röststöld

**Grind 1:** Renderingar A/B: ett metallslag spelat kromatiskt, en röstfras loopad fram-bak, och ett trumslag baklänges.

### Fas 2 – Klippet och smutsen

- Hackningen (TRANSIENT och RASTER), PLATTOR och HACKAT, Datorn, Filtret, stryp-grupperna, mixern med gruppernas utgångar, inspelningen från ingången och fabriksljuden.

**Grind 2:** En testlåt i Reaper med hackad röst, metallplattor och stötar över Kroppsmaskinen.

### Fas 3 – Panelen

- Hela panelen, fosforskärmen med ljuspennan, dra-och-släpp och appen med inspelning från mikrofonen.

**Grind 3:** Alla parametrar nås utan den generiska vyn, och appen kan sampla och spela utan DAW.

### Fas 4 – Fabriksbandet (1.0)

- 25 program som kit och instrument: EBM-kit, Industri, Stötar, Röster och Radio – allt på fabriksljuden.
- Manual, Reaper-guide, pluginval nivå 5 och auval.

**Grind 4:** Någon som inte byggt Montaget gör ett spår med fabriksbandet och några egna inspelade ljud.

### Fas 5 – Efter 1.0 (kandidater)

- Tidssträckning, resyntes och en enkel stegsekvenser per plats.

## 6. Risker

| Risk | Motåtgärd |
| --- | --- |
| Projekt som tappar sina ljud när filer flyttas | Se öppna frågor. Saknade ljud visas tydligt på skärmen, och en sökfunktion finns. |
| Minnet växer med 16 långa ljud | Högst 60 s per plats. Minnet mäts och visas i MENY. |
| Klick vid hackning och loop | Korta toningar vid varje bit och sömmarna testas i MORender. |
| Omfånget växer mot en DAW i pluginet | "Inte i 1.0" gäller. Ingen sekvenser och ingen effektkedja. |

## 7. Beslut

- **Namnet är Montaget, modell MO-85** (beslut 6 november 2026).
- *Förslag (6 november 2026):* världen **datorstudion**, inga inspelade fabriksljud (pluginet kan spridas) och koderna ovan.

Öppna frågor:

- **Var sparas ljuden i ett projekt?** Alternativen:
  - som sökväg
  - som sökväg plus en kopia i `~/Library/Application Support/Montaget/Ljud/`
  - inbäddat i projektets tillstånd (projektet blir stort)
  
  Doktor Krangs förslag är sökväg plus kopia.
