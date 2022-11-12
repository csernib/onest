# ONEST Calculator

Program intended to be used for ONEST (Observers Needed to Evaluate Subjective Tests) calculations. The methodology is described in the supplementary material of the following paper:

_Cserni B, Bori R, Csörgő E, Oláh-Németh O, Pancsa T, Sejben A, Sejben I, Vörös A, Zombori T, Nyári T, Cserni G._  
_The additional value of ONEST (Observers Needed to Evaluate Subjective Tests) in assessing reproducibility of oestrogen receptor, progesterone receptor and Ki67 classification in breast cancer._  
_Virchows Arch 2021;479(6):1101-1109. [doi: 10.1007/s00428-021-03172-9](https://doi.org/10.1007/s00428-021-03172-9)_

The program is able to parse CSV (comma-separated values) files exported from Microsoft Excel, LibreOffice Calc or created via other means. The imported CSV is then interpreted as an assessment matrix for ONEST calculation using an optional user-defined categorizer. The resulting ONEST and simplified ONEST plots are displayed to the user, who can then choose to analyze them right within the program or export them for further processing.

Further details can be found in the [user guide](doc/UserGuide.md).

Building the program requires CMake and a C++ 20 compliant compiler. Currently only Microsoft Windows is supported, but the program may also build fine on other operating systems. Please make sure to clone with `--recursive` to also clone the submodules for the dependencies.
