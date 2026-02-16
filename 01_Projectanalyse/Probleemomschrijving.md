# Probleem Omschrijving

## Context van het Project

Voor het vak TINLAB - Embedded Systems wordt een systeem ontworpen en ontwikkeld dat adaptieve ventilatie biedt op basis van omgevings- en fysiologische sensorinput, om comfort en concentratie te verbeteren.

### Wetenschappelijke Achtergrond
Recent onderzoek toont aan dat:

1. **Thermisch comfort beïnvloedt cognitieve prestaties en stress** (Lan et al., 2017): Studies tonen aan dat een omgevingstemperatuur buiten het comfortgebied leidt tot verminderde concentratie en hogere stressniveaus.

2. **Fysiologische signalen voorspellen comfort** (Lin et al., 2022): Huidtemperatuur, lichaamstemperatuur en andere metingen kunnen gebruikt worden om individueel thermisch comfort te voorspellen, waardoor ventilatie dynamisch kan worden aangepast.

3. **ADHD-medicatie beïnvloedt thermoregulatie** (CDC, 2023; Springer, 2021): Stimulantia zoals methylfenidaat kunnen de lichaamstemperatuur verhogen en het vermogen van het lichaam om warmte af te voeren beïnvloeden, wat invloed kan hebben op concentratie en comfort.

## Probleemstelling
Hoewel dynamische ventilatie en temperatuurregeling bewezen effect hebben op comfort en cognitieve prestaties, zijn veel bestaande oplossingen:

- Statisch ingesteld (constante ventilator- of airco-instellingen)

- Niet afgestemd op individuele fysiologische of omgevingscondities

- Duur of complex in implementatie

Er ontbreekt een praktisch, adaptief embedded systeem dat:
- De ventilator automatisch aanstuurt op basis van omgevingstemperatuur en fysiologische signalen zoals huidtemperatuur
- Individueel comfort en stressniveau ondersteunt, inclusief situaties waarin medicatie thermoregulatie beïnvloedt
- Geschikt is voor verschillende contexten (kantoor, thuis, onderwijs, recreatie)

## Opdracht Omschrijving

Ontwerp en implementeer een **Dynamisch Ambient Sound System** (DASS) dat automatisch ambient geluiden afspeelt en aanpast op basis van sensor input en omgevingsanalyse.


### Doelgroepen:
- Personen met ADHD die thermoregulatie gevoelig kunnen zijn
- Professionals die in focus-gevoelige omgevingen werken
- Studenten die hun concentratie willen optimaliseren
- Iedereen die stress wil verminderen door een comfortabele omgevingstemperatuur