# Régulation thermique par ventilateur — STM32F411VE (C++ orienté objet)

Système embarqué de **régulation thermique** : la température mesurée pilote la vitesse d'un
ventilateur DC en PWM, la vitesse réelle est mesurée par tachymètre et l'ensemble est
supervisé via une liaison série UART.

La particularité du projet est sa **couche d'abstraction C++** : chaque périphérique du
STM32 (GPIO, ADC, Timer, PWM, UART) est encapsulé dans une classe, au-dessus du HAL
STM32Cube. Le programme applicatif (`main.cpp`) ne manipule plus de registres ni de
`handles` HAL, mais des objets.

> Projet réalisé à l'École Nationale d'Ingénieurs de Tunis (ENIT), cycle ingénieur
> Génie Électrique.

\---

## 1\. Principe de fonctionnement

```
Capteur de température ──> ADC (12 bits) ──> Loi de régulation ──> PWM ──> Ventilateur
                                                     ^                        │
                                                     │                        │ signal
                                                     └──── RPM mesurés <──────┘ tachymétrique
                                                          (Input Capture)
                                                              │
                                                              v
                                                   UART ──> terminal série (monitoring)
```

1. **Acquisition** — la tension issue du capteur de température est convertie par l'ADC.
2. **Décision** — la température est comparée à la consigne ; le rapport cyclique du PWM
en est déduit.
3. **Action** — le timer génère le PWM appliqué au ventilateur.
4. **Retour vitesse** — le signal tachymétrique du ventilateur est compté pour en déduire
la vitesse en tours/minute.
5. **Supervision** — température, rapport cyclique et RPM sont envoyés sur l'UART.

\---

## 2\. Matériel

|Élément|Rôle|
|-|-|
|Carte STM32F411VE (Cortex-M4, 100 MHz)|Unité de traitement|
|Capteur de température (sortie analogique)|Mesure de la grandeur régulée|
|Ventilateur DC 12 V avec sortie tachymétrique|Actionneur + retour vitesse|
|Étage de puissance (transistor / MOSFET + diode de roue libre)|Commande du ventilateur|
|Convertisseur USB–TTL|Liaison série vers le PC|

> Les broches exactes sont définies dans le fichier de configuration
> `Mini\_Projet\_CPP\_STM32411.ioc` (ouvrable avec STM32CubeMX / STM32CubeIDE).

\---

## 3\. Architecture logicielle

```
Core/
├── Inc/                    en-têtes des classes
├── Src/
│   ├── main.cpp            programme applicatif (boucle de régulation)
│   ├── STM\_Gpiogroupe.cpp  abstraction d'un groupe de broches GPIO
│   ├── ADC.cpp             acquisition analogique (température, consigne)
│   ├── Timer.cpp           base de temps / configuration des timers
│   ├── PWM.cpp             génération du signal de commande du ventilateur
│   ├── FanTachometer.cpp   mesure de la vitesse de rotation (RPM)
│   └── UART.cpp            liaison série de supervision
└── Startup/                code de démarrage
Drivers/                    HAL et CMSIS STM32F4 (générés par STM32CubeMX)
```

### Classes

|Classe|Responsabilité|
|-|-|
|`STM\_Gpiogroupe`|Configuration et manipulation d'un ensemble de broches|
|`ADC`|Démarrage des conversions et lecture des valeurs analogiques|
|`Timer`|Configuration prescaler / période, base de temps|
|`PWM`|Génération du PWM et réglage du rapport cyclique|
|`FanTachometer`|Comptage des impulsions tachymétriques et calcul des RPM|
|`UART`|Émission des trames de supervision|

Cette organisation rend chaque périphérique **testable et réutilisable** indépendamment
de l'application, et permet d'ajouter un nouvel actionneur ou capteur sans toucher au
reste du code.

\---

## 4\. Compilation et programmation

**Prérequis :** [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html).

```bash
git clone https://github.com/maramturki-lang/stm32-fan-thermal-control-cpp.git
```

1. `File > Open Projects from File System…` et sélectionner le dossier cloné.
2. Compiler : `Project > Build All` (Ctrl+B).
3. Programmer la carte : `Run > Run As > STM32 C/C++ Application` (ST-LINK).

**Supervision série :** ouvrir un terminal (PuTTY, `screen`, moniteur série de CubeIDE)
sur le port COM de la carte.

\---

## 5\. Compétences mises en œuvre

* Programmation orientée objet en C++ sur cible embarquée à ressources limitées
* Conception d'une couche d'abstraction matérielle (HAL wrapper)
* Génération PWM et mesure de fréquence par timer
* Conversion analogique–numérique et mise à l'échelle des mesures
* Boucle de régulation temps réel et communication série

\---

## 6\. Auteur

**Maram TURKI** — élève ingénieure en Génie Électrique, ENIT

