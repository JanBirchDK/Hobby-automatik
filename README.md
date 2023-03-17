# Hobby-automatik
Automatisk vanding i haven. En styring der måler regnvejr, fugtighed i jorden, luftens temperatur og dagslyset. Ud fra disse doserer vandingen.

Havelys der viser vej til hoveddøren, ved at tænde lyset når man går ad stien mod huset.

Indendørs klimastyring. En styring der måler fugtighed, temperatur og CO2. Ud fra disse målinger starter en ventilator.

En automatisk skydedør. Når man går mod døren åbner den automatisk og lukker igen, når man er gået igennem.

Disse styringer er eksempler på automatik, der er sjovt at fremstille som gør det selv.

En Arduino er velegnet til at udføre styringen. Den har en masse indgange og udgange og det er nemt at tilslutte sensorer, motorer, relæer, lamper med mere.
Programmering af en Arduino gøres endnu lettere, ved at bygge med en samling software komponenter. Komponenterne bliver sat ind i programmet, konfigureret og så kører styringen.
## Repository indeholder
Der er vedlagt en beskrivelse af en automatik applikation. Dens formål, rammer og betingelser.

Dette repository indeholder en samling af software komponenter. Komponenter er samlet i biblioteker, hver med deres specifikke formål.

Hvert bibliotek:
- Har en beskrivelse
- Et UML klassediagram
- Kildekode til import i Arduino libraries

*Repository bliver uploaded løbende over de næste 2 uger.*

## Om Arduino platformen
Til Arduino platformen er der skabt mulighed for at bygge en applikation til automatisering af mindre anlæg.
Applikationen:
- Er fleksibel og kan løbende tilpasses, når nye krav og ønsker opstår.
- Bygget til styring, regulering og overvågning. Tilstandsmaskine udfører den overordnede koordination.
- Består primært af en række softwarekomponenter, der blot skal sættes ind og konfigureres.
. 
	
Består et anlæg af flere delsystemer, er det tanken at der bygges en applikation og der bruges en Arduino per delsystem. Delsystemerne bindes sammen med kommunikation. En Arduino applikation bliver specifik og møntet på et formål og gøres ikke generelt anvendelig.


## Arduino fordele og ulemper
**Fordele**
- Arduino bruger kun 1/4W. En kontrolboks kan stå tændt i døgndrift.
- En udgang kan trække 20mA. Der kan spares ekstern elektronik til strømforstærkning.
- Arduino kan sandsynligvis udføre alle funktioner i en kontrolboks.
- Det ser ud til, at Arduino er let at programmere til IR modtager.
- Der er et komplet udviklingsmiljø, der gør programmering og overførsel til Arduino let.
- Arduino er ikke forbundet til netværk og derfor beskyttet mod hackning.
- Arduino har ej boot eller shutdown. Den er nem at tænde og slukke.
 
Ulemper
- Programmeringssproget er C eller C++ og derfor low level og kan være vanskelig at programmere.
- Arduino har 2048 byte memory til data. Så der skal holdes øje med allokering af data.
- Vær obs på at allokere alle data inklusiv objekter ved programopstart.

Håndteres memory run-time giver det risiko for at fylde memory op, som derefter udløser stop i afvikling af applikationen. Derfor allokeres alle data ved programstart.

Det er standard at opdele kildekode i en <navn>.h og en  <navn>.cpp fil. Separat c++ fil giver mulighed for at udskifte kildekode til andre platforme. C++ til Arduino har en række begrænsninger, som gør det irellevant at gøre koden portabel. Derfor bliver kildekoden kun bygget med <navn>.ino og <navn>.h filer.
