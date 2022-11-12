# ONEST Calculator - User Guide

When using this program, please consider quoting the article below, for which it was developed:

_Cserni B, Kilmartin D, O'Loughlin M, Andreu X, Bagó-Horváth Z, Bianchi S, Chmielik E, Figueiredo P, Floris G, Foschini MP, Kovács A, Heikkilä P, Kulka J, Laenkholm AV, Liepniece-Karele I, Marchiò C, Provenzano E, Regitnig P, Reiner A, Ryška A, Sapino A, Specht Stovgaard E, Quinn C, Zolota V, Webber M, Glynn SA, Bori R, Csörgő E, Oláh-Németh O, Pancsa T, Sejben A, Sejben I, Vörös A, Zombori T, Nyári T, Callagy G, Cserni G._  
_ONEST (Observers Needed to Evaluate Subjective Tests) analysis of stromal tumour infiltrating lymphocytes (sTILs) in breast cancer and its limitations._  
_Source to be specified later._

## Intended use-case

The program is intended to be used for ONEST (Observers Needed to Evaluate Subjective Tests) calculations. The methodology is described in the supplementary material of the following paper:

_Cserni B, Bori R, Csörgő E, Oláh-Németh O, Pancsa T, Sejben A, Sejben I, Vörös A, Zombori T, Nyári T, Cserni G._  
_The additional value of ONEST (Observers Needed to Evaluate Subjective Tests) in assessing reproducibility of oestrogen receptor, progesterone receptor and Ki67 classification in breast cancer._  
_Virchows Arch 2021;479(6):1101-1109. [doi: 10.1007/s00428-021-03172-9](https://doi.org/10.1007/s00428-021-03172-9)_

The program is able to parse CSV (comma-separated values) files exported from Microsoft Excel, LibreOffice Calc or created via other means. The imported CSV is then interpreted as an assessment matrix for ONEST calculation using an optional user-defined categorizer. The resulting ONEST and simplified ONEST plots are displayed to the user, who can then choose to analyze them right within the program or export them for further processing.


## Importing assessments

The program expects regular UTF-8 encoded CSV files using either commas or semi-colons as value separators and double quotes as string separators. Such files can be created in most spreadsheet editors (including Microsoft Excel) using their "Save As..." menu and selecting the correct file type. The program expects the columns to represent the observers, and the rows to represent the cases. An optional header line (for example for naming the observers) is also allowed. The program is also able to ignore columns during calculation, so columns like "Case Number" are also safe to export. The intersection of row _R_ and column _C_ represent the assessment made by the _Cth_ observer for the _Rth_ case. Assessments can be of any form but are typically numeric.

To import a CSV file, click on the first button in the toolbar ("Open CSV input..."), and make sure you select the correct value separator (semi-colons by default).

Example assessment table:

|        | Observer 1 | Observer 2 | Observer 3 |
| ------ | ---------- | ---------- | ---------- |
| Case 1 |     45     |     40     |     45     |
| Case 2 |     40     |     30     |     45     |
| Case 3 |     10     |     15     |     25     |
| Case 4 |     70     |     75     |     50     |
| Case 5 |     85     |     90     |     80     |


## Filtering out the table header or individual columns from calculation

When opening a new CSV file, the program will automatically try to detect whether the first row is a header. This can also be toggled via the header toggle button on the right side of the toolbar.

Individual columns can also be filtered out by clicking on their header (there is a tickbox showing whether the column is enabled or not). Filtered out columns are colored black and will not participate in ONEST calculation. This feature is useful for filtering out meta columns like case labels or checking how ONEST would look if we removed certain observers.


## Toggling randomization and calculation modes

By default, the program will select only 100 distinct pseudorandom observer permutations out of all possible permutations for calculation, provided that at least five observers are present (otherwise all permutations are used, since the number of possible permutations is below 100). Randomization happens by default via a deterministically seeded pseudorandom number generator, which means that for the same number of observers, the program is always going to use the same 100 random permutations.

If a non-deterministic seed is desired for pseudorandom number generation, so that the selected 100 random permutations change between each calculation, it can be done via the dice button in the toolbar.

The program also gives the possibility to use all possible permutations for ONEST calculation instead of just 100. Switching between the two modes is done via the "100/ALL" button in the toolbar. Please note that as the number of observers grows, calculation using all possible permutations may become computationally infeasible. ("Calculating ONEST..." appears in the left bottom corner while the process is running.)


## Categorization

By default, the assessments of each observer are compared literally. This means that two assessments are considered equal if and only if the values within the two cells match exactly. While this may already be fine if the input already contains discrete categories, that is not always the case. For example, if the assessments are numeric, we may want to categorize them based on thresholds. For example, values less than 50 should form category A, while values greater or equal to 50 should form category B. The _Categorizer_ input field in the program allows such rules to be defined.

The syntax for defining a categorizer is as follows:

```
	<categorizer>        ::= <rule_1> ";" <rule_2> ";" <rule_3> ...
	<rule>               ::= <or_rule> | <threshold_rule> | <literal_rule>
	<or_rule>            ::= <sub_rule_1> "," <sub_rule_2> "," <sub_rule_3> ...
	<sub_rule>           ::= <threshold_rule> | <literal_rule>
	<threshold_rule>     ::= <left_bound_rule> | <right_bound_rule> | <mixed_bound_rule_a> | <mixed_bound_rule_b>
	<left_bound_rule>    ::= <number> ( "<" | "<=" | ">" | ">=") ["X" | "x"]
	<right_bound_rule>   ::= ["X" | "x"] ( "<" | "<=" | ">" | ">=") <number>
	<mixed_bound_rule_a> ::= <number> ( "<" | "<=" ) ["X" | "x"] ( "<" | "<=" ) <number>
	<mixed_bound_rule_b> ::= <number> ( ">" | ">=" ) ["X" | "x"] ( ">" | ">=" ) <number>
	<number>             ::= any regular number
	<literal_rule>       ::= any text (including numbers)
```

As visible from the syntax definition, the categorizer is made up of rules, which are separated by semi-colons. Each rule defines a category distinct from that of any other rules. Rules are matched from left to right on all assessed cells. The cell will be categorized based on the first rule that fits it.

There are three main types of rules: "OR rules", "threshold rules" and "literal rules".

OR rules contain threshold or literal rules (or a mixture of these) separated by commas. The OR rule matches if any of its sub-rules matches.

Threshold rules only match numeric cells whose value are above, below or between the given threshold(s) (with equality allowed conditionally). For example, `<=50` means "less than or equal to 50", `90<` means "greater than 90". The syntax also allows for both the lower and the upper bounds to be defined. For example, `0.3<<=9.2` means "greater than 0.3, but less than or equal to 9.2". To make the rules easier to read, an optional "X" or "x" sign is also allowed on the other side of the relational operators. The previous three examples thus could have also been written like this: `X<=50`, `90<x`, `0.3<X<=9.2`.

Literal rules match if the cell they are matched against contains exactly the same text as the rule itself. For example, "ABC" will match only cells having "ABC" as their value.

Any cell not matching a rule will form its own category. Other cells containing the same value will be put into the same category.

### Example 1

We ask three observers to give a subjective estimate on the percentage of a certain substance being present in various samples:

| Observer 1 | Observer 2 | Observer 3 |
| ---------- | ---------- | ---------- |
|     45     |     40     |     45     |
|     40     |     30     |     45     |
|     10     |     15     |     25     |
|     70     |     75     |     50     |
|     85     |     90     |     80     |

Based on the estimates, we would like to have three categories: one for values less than 50, one for values between 50 and 75 (both inclusive) and one for values greater than 75.
We would then use (for example) the following categorizer: `<50;50<=<=75;75<`
This would categorize the above data table like this:

| Observer 1 | Observer 2 | Observer 3 |
| ---------- | ---------- | ---------- |
|     A      |     A      |     A      |
|     A      |     A      |     A      |
|     A      |     A      |     A      |
|     B      |     B      |     B      |
|     C      |     C      |     C      |

The resulting ONEST plot would then show a constant line at 1.0, because all the observers for all possible selections are in 100% agreement with each other.

Other equivalent categorizers for this example include:
 - `75>=X>=50;<50;75<X`, because distinct ranges can be reordered, and the use of x or X is optional
 - `50<=<=75;<=50;>=75`, because matching is done from left to right, i.e. it does not matter that we also allow equality for the latter two rules
 - `50<<75,50,75;<50;>75`, because the OR rule "greater than 50 and less than 75 or 50 or 75" is identical to `50<=X<=75`

### Example 2

We ask four observers to test the flavouring of certain drinks. There are three possible flavours (apple, orange and raspberry), and either of them can be gathered from natural resources or produced artificially. The task of each observer is to taste each of our five (not necessarily different) drinks, try to tell the flavour of each, and also try to determine whether the flavour comes from natural or artificial sources.

| Taster 1             | Taster 2             | Taster 3             | Taster 4             | Control           |
| -------------------- | -------------------- | -------------------- | -------------------- | ----------------- |
| artificial orange    | artificial orange    | natural orange       | artificial orange    | artificial orange |
| artificial orange    | natural orange       | artificial apple     | artificial orange    | artificial apple  |
| natural apple        | natural apple        | natural apple        | artificial apple     | artificial apple  |
| artificial raspberry | artificial raspberry | artificial raspberry | artificial raspberry | natural raspberry |
| natural apple        | natural apple        | natural apple        | natural apple        | natural apple     |

Let us examine three scenarios:
1. If no categorizer is given, each type will form a category of its own, so there are going to be 6 categories. In this case we are testing whether the observer was able to correctly determine both the flavour and the source of the drink. As an alternative to leaving the categorizer empty, we could also provide the literal rules explicitly: `natural apple;artificial apple;natural orange;artificial orange;natural raspberry;artificial raspberry`
2. Let us say we are only interested whether the observers are able to detect the difference between natural and artificial sources, for example because producing drinks from artificial sources would be cheaper. We would then use the following categorizer: `natural apple,natural orange,natural raspberry;artificial apple,artificial orange,artificial raspberry`
3. Another scenario may be where we want to check how reliably can the observers recognize the different flavours, regardless of their origin. The following categorizer could be used for this case: `natural apple,artificial apple;natural orange,artificial orange;natural raspberry,artificial raspberry`


## Reading the results

Immediately under the categorizer input field, a table displays the various categories detected by the program, along with their count and percentage compared to the number of active cells. The results are sorted by count from left to right in descending order.

Below the category table, the key ONEST values are displayed. _OPA(N)_ is the overall percent agreement of all observers. _Bandwidth_ is the range between the minimum and maximum OPA(2) values, i.e., the maximum difference in agreement between two observers based on the number of permutations analysed. _Observers needed_ is the number of observers required for a reliable estimation of reproducibility, i.e., the observer count from which the overall percent agreement curves no longer decrease. This last value can also be "?" if there is not enough data to determine this number or "∞" if ONEST diverges. (Please note, that the _Observers needed_ value expressed here is defined so that no further decrease in OPA occurs with further increase in the number of observers; however, the plots may help to read a more practical value with no "substantial" decrease in OPA with more observers)

Under the ONEST values, there are two ONEST plots. The first one shows all the resulting curves, whereas the bottom one only shows the "simplified ONEST" plot, i.e. the minimum, median and maximum curves.


## Saving and exporting

Both ONEST plots can be saved as PNG images by right-clicking on them.

The _overall percent agreement curves (OPACs)_ making up ONEST can also be exported as CSV files for further processing. This is done via the export icon on the toolbar.


## Common problems

### International characters are not displayed correctly

Make sure that the input CSV file is saved using UTF-8 encoding. This can be selected within the file type section of the "Save As..." dialog of Microsoft Excel or the character set field in the case of LibreOffice Calc.

### The imported table contains only a single column with all values mixed into it

Make sure to select the correct value separator during import. This is likely the semi-colon for a CSV exported from Microsoft Excel and the comma for exports from LibreOffice Calc. LibreOffice Calc allows to change also the string separator during CSV export. This has to be the double quote sign (") in every case.

### Attempting to import CSV causes an error

This is likely caused by the above issue.


## Attribution and license

### Attribution

When using this program, please consider quoting the article below, for which it was developed:

_Cserni B, Kilmartin D, O'Loughlin M, Andreu X, Bagó-Horváth Z, Bianchi S, Chmielik E, Figueiredo P, Floris G, Foschini MP, Kovács A, Heikkilä P, Kulka J, Laenkholm AV, Liepniece-Karele I, Marchiò C, Provenzano E, Regitnig P, Reiner A, Ryška A, Sapino A, Specht Stovgaard E, Quinn C, Zolota V, Webber M, Glynn SA, Bori R, Csörgő E, Oláh-Németh O, Pancsa T, Sejben A, Sejben I, Vörös A, Zombori T, Nyári T, Callagy G, Cserni G._  
_ONEST (Observers Needed to Evaluate Subjective Tests) analysis of stromal tumour infiltrating lymphocytes (sTILs) in breast cancer and its limitations._  
_Source to be specified later._

### License

Copyright 2022 Bálint Cserni

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
