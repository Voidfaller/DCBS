# Requirements – Dynamisch Comfort Ventilatie Systeem (DCVS)
 ### 1.1 Epics
#### 1.1.1 Adaptieve Comfortregeling
   Het systeem moet thermisch comfort dynamisch reguleren door een ventilator automatisch aan te sturen op basis van omgevings- en fysiologische sensorinput.
#### 1.1.2 Sensorintegratie en Dataverwerking
Het systeem moet relevante omgevings- en fysiologische data betrouwbaar verzamelen, verwerken en interpreteren op een embedded platform.
    
#### 1.1.3 Personalisatie en Comfortmodellering
   Het systeem moet individuele thermische voorkeuren en fysiologische variaties ondersteunen door middel van instelbare parameters of een adaptief comfortmodel.

#### 1.1.4 Betrouwbare en Veilige Embedded Architectuur
   Het systeem moet stabiel, veilig en energie-efficiënt functioneren binnen verschillende gebruiksomgevingen (kantoor, thuis, onderwijs, recreatie).
### 1.2 Functionele Eisen
#### 1.2.1 Omgevingstemperatuurmeting
   Het systeem moet de omgevingstemperatuur kunnen meten met een geschikte sensor
#### 1.2.2 Fysiologische meting
Het systeem moet fysiologische signalen zoals huidtemperatuur kunnen meten met geschikte sensoren.
#### 1.2.3 Realtime dataverwerking 
Het systeem moet sensorwaarden lokaal verwerken met voldoende snelheid om realtime ventilatoraansturing mogelijk te maken.
#### 1.2.4 Beslissingsalgoritme
   Het systeem moet een algoritme implementeren dat op basis van de verzamelde data bepaalt wanneer en hoe de ventilator aan te sturen.
#### 1.2.5 Ventilatorbesturing
   Het systeem moet een ventilator kunnen aansturen (aan/uit, snelheidsregeling) op basis van de output van het beslissingsalgoritme. en een PID-regelaar implementeren voor nauwkeurige temperatuurregeling.
#### 1.2.6 Handmatige override
   Het systeem moet een handmatige override-functie bieden waarmee gebruikers de ventilatorinstellingen kunnen aanpassen, zelfs wanneer automatische regeling actief is.

#### 1.2.7 Gebruikersinstellingen
Het systeem moet gebruikersinstellingen kunnen opslaan en toepassen, zoals voorkeurstemperaturen of gevoeligheidsniveaus voor fysiologische signalen.

#### 1.2.8 Gebruikersinterface
   Het systeem moet een eenvoudige gebruikersinterface bieden (bijv. knoppen, display) voor het weergeven van statusinformatie en het aanpassen van instellingen.

#### 1.2.9 Veiligheid en foutafhandeling
   Het systeem moet watchdog-timers implementeren en foutcondities detecteren (bijv. sensorfouten, ventilatorstoringen) om veilige werking te garanderen.

#### 1.2.10 Energie-efficiëntie
   Het systeem moet energiezuinig zijn, met functies zoals slaapstand of verminderde samplingfrequentie wanneer geen activiteit wordt gedetecteerd.

### Non-Functionele Eisen
#### 1.3.1 Betrouwbaarheid
   Het systeem moet betrouwbaar functioneren onder verschillende omgevingsomstandigheden en gedurende langere perioden zonder falen.

#### 1.3.2 Veiligheid
   Het systeem moet veilig zijn in gebruik, zonder risico op elektrische schokken, oververhitting of andere gevaren.

#### 1.3.3 Gebruiksvriendelijkheid
   Het systeem moet eenvoudig te bedienen zijn, met een intuïtieve gebruikersinterface en duidelijke feedback over de status en werking.

#### 1.3.4 Energie-efficiëntie
   Het systeem moet energiezuinig zijn, met een laag stroomverbruik en functies om energie te besparen wanneer mogelijk.

#### 1.3.5 Opstarttijd
Het systeem moet binnen 10 seconden na inschakeling volledig operationeel zijn.

#### 1.3.6 Onderhoudbaarheid
   Het systeem moet gemakkelijk te onderhouden zijn, met modulaire componenten en duidelijke documentatie voor foutopsporing en reparatie.





   
## US-01 Automatische selectie
Als gebruiker wil ik dat het systeem automatisch een passend ambient geluid kiest op basis van omgevingsomstandigheden, zodat ik niet handmatig hoef te schakelen.

## US-02 Automatisch afspelen na activatie
Als gebruiker wil ik dat het systeem na activatie automatisch begint met afspelen, zodat ik direct effect merk.

## US-03 Dynamische volumeregeling
Als gebruiker wil ik dat het volume zich automatisch aanpast aan het omgevingsgeluid, zodat het geluid hoorbaar blijft zonder storend te worden.

## US-04 Dynamische profielwissel
Als gebruiker wil ik dat het systeem van geluidsprofiel kan wisselen wanneer de omgeving verandert, zodat het geluid passend blijft.

## US-05 Focusmodus (student)
Als student wil ik een focusmodus die helpt afleiding door omgevingsgeluid te verminderen, zodat ik efficiënter kan studeren.

## US-06 Kantoormodus (professional)
Als professional wil ik een kantoormodus die rekening houdt met een gedeelde omgeving, zodat het systeem niet storend is voor anderen.

## US-07 Ontspanningsmodus
Als gebruiker wil ik een ontspanningsmodus met natuurgeluiden, zodat ik stress kan verminderen.

## US-08 Handmatige override
Als gebruiker wil ik handmatig een geluidsprofiel kunnen kiezen, zodat ik controle heb als de automatische keuze niet klopt.

## US-09 Start/stop bediening
Als gebruiker wil ik het systeem kunnen pauzeren of uitschakelen, zodat ik het direct kan stoppen.

## US-10 Feedback over actieve modus
Als gebruiker wil ik kunnen zien welke modus/profiel actief is, zodat ik begrijp wat het systeem doet.

# Functionele Requirements – DASS (MoSCoW)

### FR-01 (M)
Het systeem moet het omgevingsgeluidsniveau kunnen meten via een sensor.

### FR-02 (M)
Het systeem moet sensorwaarden periodiek samplen en verwerken met een configureerbaar interval.

### FR-03 (M)
Het systeem moet automatisch een geluidsprofiel selecteren op basis van gemeten omgevingsparameters.

### FR-04 (M)
Het systeem moet binnen 5 seconden na activatie starten met het afspelen van een geluidsprofiel.

### FR-05 (M)
Het systeem moet minimaal drie geluidsprofielen ondersteunen: natuurgeluiden, white noise en neutrale ambient soundscapes.

### FR-06 (M)
Het systeem moet het outputvolume automatisch aanpassen wanneer het omgevingsgeluid een ingestelde drempel overschrijdt.

### FR-07 (M)
Het systeem moet een maximaal toegestaan volume afdwingen.

### FR-08 (M)
Het systeem moet gestart en gestopt kunnen worden via een gebruikersinterface.

### FR-09 (M)
Het systeem moet handmatige selectie van geluidsprofielen ondersteunen.

### FR-10 (M)
Het systeem moet aan de gebruiker aangeven welk profiel of welke modus actief is.

---

### FR-11 (S)
Het systeem zou bij significante verandering van omgevingsparameters automatisch moeten her-evalueren of een profielwissel nodig is.

### FR-12 (S)
Het systeem zou na een handmatige override automatische wissels tijdelijk moeten onderdrukken.

### FR-13 (S)
Het systeem zou foutcondities moeten detecteren en aan de gebruiker melden.

---

### FR-14 (C)
Het systeem zou meerdere contextmodi kunnen ondersteunen (bijv. office, garden, park, home).

### FR-15 (C)
Het systeem zou gebruikersinstellingen kunnen opslaan en herstellen na herstart.

---

### FR-16 (W)
Het systeem zal geen externe online data gebruiken voor profielkeuze.

### FR-17 (W)
Het systeem zal geen biometrische sensoren gebruiken voor stressdetectie.
