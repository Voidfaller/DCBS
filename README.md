# Eindproject

## Doel van dit document
Dit document dient als overzicht van het eindproject voor Embedded Systems. 

## Links
Git: https://gitlab.cmi.hro.nl/embedded-systems/eindproject
Jira: https://giovannidegroot.atlassian.net/?continue=https%3A%2F%2Fgiovannidegroot.atlassian.net%2Fwelcome%2Fsoftware%3FprojectId%3D10033&atlOrigin=eyJpIjoiMjkwNTViM2RjZGJjNDJkNTk4ZDQ4OTVjMzFjNjI1ZDUiLCJwIjoiamlyYS1jb3JlIn0

Overleaf English Paper Link: https://www.overleaf.com/6747122719rhbrqhtbdkbg#aa452c


## Inhoud
- [Eindproject](#eindproject)
  - [Doel van dit document](#doel-van-dit-document)
  - [Teamleden](#teamleden)
  - [Links](#links)
  - [Inhoud](#inhoud)
  - [Projectomschrijving](#projectomschrijving)
  - [Projectmap (waar vind ik wat?)](#projectmap-waar-vind-ik-wat)
    - [01. Projectanalyse](#01-projectanalyse)
    - [02. Requirements](#02-requirements)
    - [03. Onderzoek](#03-onderzoek)
    - [04. Ontwerpen](#04-ontwerpen)
    - [05. Testen](#05-testen)
    - [06. Firmware](#06-firmware)
    - [07. Gebruikersinformatie](#07-gebruikersinformatie)
    - [08. Onderwijsdocumenten](#08-onderwijsdocumenten)
  - [Route voor docenten](#route-voor-docenten)

## Projectomschrijving
Het doel van dit project is het ontwerpen en implementeren van een embedded systeem.

## Projectmap (waar vind ik wat?)

Gebruik dit overzicht als snelle navigatie door de repository.

### 01. Projectanalyse
- `01_Projectanalyse/`: initiële analyse en afbakening
- `01_Projectanalyse/docx/Problem Description.docx`: probleemdefinitie
- `01_Projectanalyse/docx/Risk Analysis.docx`: risicoanalyse
- `01_Projectanalyse/docx/BoM.docx`: Bill of Materials
- `01_Projectanalyse/docx/StakeholderAnalysis.docx`: stakeholderanalyse
- `01_Projectanalyse/scope/docx/scope.docx`: scopebeschrijving

### 02. Requirements
- `02_Requirements/docx/Requirements.docx`: functionele en niet-functionele eisen

### 03. Onderzoek
- `03_Onderzoek/docx/Microcontroller.docx`: keuze en onderbouwing microcontroller
- `03_Onderzoek/docx/sensor.docx`: sensoronderzoek
- `03_Onderzoek/docx/actuator.docx`: actuatoronderzoek

### 04. Ontwerpen
- `04_Ontwerpen/Systeemarchitectuur/`: systeemarchitectuurontwerp
- `04_Ontwerpen/Software/`: software-architectuur en componentflowcharts
- `04_Ontwerpen/Hardware ontwerp/`: hardwareontwerp (KiCad)
- `04_Ontwerpen/mechanisch_ontwerp/`: mechanisch ontwerp
- `04_Ontwerpen/UI/`: UI-ontwerp en features

### 05. Testen
- `05_Testen/Testplannen/docx/`: functioneel testplan en testrapportages
- `05_Testen/Testplannen/docx/testreport_*.docx`: testrapportages per component

### 06. Firmware
- `06_Firmware/DCBS/main/`: entrypoint van de firmware (`main.c`)
- `06_Firmware/DCBS/components/`: componentgebaseerde firmwaremodules
- `06_Firmware/DCBS/components/*/user_manual.docx`: componentdocumentatie (waar aanwezig)
- `06_Firmware/DCBS/components/*/user_manual.md`: bronversies van componentdocumentatie
- `06_Firmware/DCBS/components/watchdog_service/user_manual.docx`
- `06_Firmware/DCBS/components/gyro_driver/user_manual.docx`
- `06_Firmware/DCBS/components/pid_control/user_manual.docx`
- `06_Firmware/DCBS/components/state_estimator/user_manual.docx`
- `06_Firmware/DCBS/components/servo_driver/User_manual.docx`
- `06_Firmware/DCBS/CMakeLists.txt`: buildconfiguratie
- `06_Firmware/DCBS/build/`: gegenereerde build-output

### 07. Gebruikersinformatie
- `07_GebruikersInformatie/docx/User_manual.docx`: gebruikershandleiding
- `07_GebruikersInformatie/docx/advisory_report.docx`: adviesrapport
- `07_GebruikersInformatie/md/`: bronversies (.md)

### 08. Onderwijsdocumenten
- `08_Onderwijsdocumenten/docx/Leerdoel-verantwoording.docx`: verantwoording leerdoelen

## Route voor docenten
Voor beoordeling van de leeruitkomsten en onderbouwing is dit het beste startpunt:

- [08_Onderwijsdocumenten/docx/Leerdoel-verantwoording.docx](08_Onderwijsdocumenten/docx/Leerdoel-verantwoording.docx)

Bronversie (markdown):

- [08_Onderwijsdocumenten/md/Leerdoel-verantwoording.md](08_Onderwijsdocumenten/md/Leerdoel-verantwoording.md)

In dit document staat ook een technische bewijsmatrix met directe links naar relevante code, testresultaten en ontwerpdocumenten.




