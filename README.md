# Projektbeskrivelse gruppe 6:

Vores program, "OptiMill", skal kunne identificere optimale placeringer til vindmøller ud fra givne parametre. Brugeren kan vælge om programmet skal prioritere højst mulige produktion af energi, eller lavest mulige omkostninger for placering og valg af én vindmølle. Dette udregnes efter programmets foruddefinerede "områder", som hver indeholder landhøjde, vindhastighed, ruhed, afstand til elnet, afstand til nærmeste hus og nærmeste by m.m.. Beregningerne foretages ud fra reelle formler og data, som gør resultaterne så præcise som muligt. Derudover kan brugeren også vælge hvilken region af Danmark, som programmet skal lede efter områdernes data samt hvilken model af vindmølle, som skal placeres i området.
Det endelige mål ved programmet er både at øge mængden af grøn energi, som produceres af Danmarks vindmøller, samt at nedsætte omkostningerne for installation af vindmøller.

Hele programmet er udarbejdet i C og compiled med gcc.

Gruppemedlemmer:

* Emil Ramsbæk
* Jakob Saadbye
* Johan Riedel
* Laurits Lippert
* Nicolai Mandrup
* Sture Svensson

# Run/Compile

Hent github repository ved brug af git clone eller download zip filen.

Compile og run på Mac 
```console
  gcc-10 Optimill.c -o optimill
  ./optimill.out
```

Compile og run på Windows 10
```console
  gcc Optimill.c -o optimill
  optimill.exe
```

