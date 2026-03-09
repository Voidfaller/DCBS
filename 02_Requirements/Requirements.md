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

### 1.3 Non-Functionele Eisen
#### 1.3.1 Betrouwbaarheid
   Het systeem moet betrouwbaar functioneren onder verschillende omgevingsomstandigheden en gedurende langere perioden zonder falen.

#### 1.3.2 Veiligheid
   Het systeem moet veilig zijn in gebruik, zonder risico op elektrische schokken, oververhitting of andere gevaren.

#### 1.3.3 Gebruiksvriendelijkheid
   Het systeem moet eenvoudig te bedienen zijn, met een intuïtieve gebruikersinterface en duidelijke feedback over de status en werking.


#### 1.3.4 Opstarttijd
Het systeem moet binnen 10 seconden na inschakeling volledig operationeel zijn.

#### 1.3.5 Onderhoudbaarheid
   Het systeem moet gemakkelijk te onderhouden zijn, met modulaire componenten en duidelijke documentatie voor foutopsporing en reparatie.