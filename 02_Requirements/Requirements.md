# Requirements – Dynamisch Beker Balanceer Systeem (DBBS)
 ### 1.1 Epics
#### 1.1.1 Adaptieve Stabilisatie
  Het systeem moet de hellingshoek van een beker dynamisch reguleren door servos automatisch aan te sturen op basis van omgevings- en bewegingsdata.
#### 1.1.2 Sensorintegratie en Dataverwerking
Het systeem moet relevante bewegings data betrouwbaar verzamelen, verwerken en interpreteren op een embedded platform.
    
#### 1.1.3 Personalisatie en Comfortmodellering
  Het systeem moet individuele voorkeuren of scenario’s ondersteunen (bijv. snelheid van correcties, maximale hellingstolerantie) door middel van instelbare parameters.

#### 1.1.4 Betrouwbare en Veilige Embedded Architectuur
   Het systeem moet stabiel, veilig en energie-efficiënt functioneren binnen verschillende gebruiksomgevingen (tafel, bureau, onderwijs, recreatie).
### 1.2 Functionele Eisen
#### 1.2.1 Bekerhellingmeting
Het systeem moet de hellingshoek van de beker kunnen meten of bijhouden met een geschikte sensor.

#### 1.2.2 Bewegingsdetectie
Het systeem moet bewegingen van de ondergrond of externe verstoringen kunnen detecteren met geschikte sensoren.
#### 1.2.3 Realtime dataverwerking 
Het systeem moet sensorwaarden lokaal verwerken met voldoende snelheid om realtime servo-aansturing mogelijk te maken.

#### 1.2.4 Beslissingsalgoritme
Het systeem moet een algoritme implementeren dat op basis van de verzamelde data bepaalt wanneer en hoe de servos aan te sturen.


#### 1.2.5 Servo Aansturing
   Het systeem moet servos kunnen aansturen (positie en snelheid) op basis van de output van het beslissingsalgoritme en een PID-regelaar implementeren voor nauwkeurige bekerstabiliteit.

   
#### 1.2.6 Handmatige override
   Het systeem moet een handmatige override-functie bieden waarmee gebruikers de servo-instellingen kunnen aanpassen, zelfs wanneer automatische regeling actief is.

#### 1.2.7 Gebruikersinstellingen
Het systeem moet gebruikersinstellingen kunnen opslaan en toepassen, zoals maximale helling, correctiesnelheid of gevoeligheidsniveaus voor externe bewegingen.

#### 1.2.8 Gebruikersinterface
   Het systeem moet een eenvoudige gebruikersinterface bieden (bijv. knoppen, display) voor het weergeven van statusinformatie en het aanpassen van instellingen.

#### 1.2.9 Veiligheid en foutafhandeling
   Het systeem moet watchdog-timers implementeren en foutcondities detecteren (bijv. sensorfouten, servo-storingen) om veilige werking te garanderen.

#### 1.2.10 Energie-efficiëntie
   Het systeem moet energiezuinig zijn, met functies zoals slaapstand of verminderde samplingfrequentie wanneer geen beweging of activiteit wordt gedetecteerd.

### 1.3 Non-Functionele Eisen
#### 1.3.1 Betrouwbaarheid
Het systeem moet betrouwbaar functioneren onder verschillende omgevingsomstandigheden en gedurende langere perioden zonder falen.
#### 1.3.2 Veiligheid
Het systeem moet veilig zijn in gebruik, zonder risico op schade aan de beker, servos of gebruiker.
#### 1.3.3 Gebruiksvriendelijkheid
Het systeem moet eenvoudig te bedienen zijn, met een intuïtieve gebruikersinterface en duidelijke feedback over de status en werking.

#### 1.3.4 Opstarttijd
Het systeem moet binnen 10 seconden na inschakeling volledig operationeel zijn.
#### 1.3.5 Onderhoudbaarheid
Het systeem moet gemakkelijk te onderhouden zijn, met modulaire componenten en duidelijke documentatie voor foutopsporing en reparatie.