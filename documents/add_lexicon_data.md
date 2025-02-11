# Adding data to a lexicon

Data needs to be formatted properly before addition. See [here](#preparing-data) for details.

There are two ways of adding new data to the lexicon:
1. Running [dictionary-parser](../inflection/tools/dictionary-parser/README.md) on a local file.
1. Adding data to Wikidata first, then running step 1.

## Adding data directly
After properly formatting the data it's simple to run the local dictionary parser tool to build a lexicon (see above).

It is possible to mix multiple files in the input, e.g. wikidata input and local file to expand the local lexicon - just list all of them in the command line.

## Adding data to Wikidata
The main benefit of adding data to Wikidata is expanding lexicon for all users of the language, unrelated to the Inflection project.

There are tools that can help with bulk upload, e.g. [tfsl for Python](https://phabricator.wikimedia.org/source/tool-twofivesixlex/), [Wikidata toolkit](https://www.mediawiki.org/wiki/Wikidata_Toolkit) and [WikibaseIntegrator for Python](https://www.wikidata.org/wiki/Wikidata:Tools/WikibaseIntegrator). Below is an incomplete example of how to use tfsl for Croatian data.

Lexical info can also be entered [directly](https://www.wikidata.org/wiki/Special:NewLexeme) using Wikidata interface, but that's recommended only for a small amount of data.

Depending on the size and rate of upload, you may need to create a Bot account with Wikidata.

Once you upload/enter data, it may take a few days for it to show up in the regular dumps - depending on review requirements.

```python
import csv
import sys
import os
import qnames
import functools
import operator
import json
import readline
import requests

sys.path.append(os.path.join(os.path.dirname(__file__), 'twofivesixlex'))
import tfsl

# You may need to create a bot account, depending on upload size/rate.
username = '###YOUR-WIKIDATA-USERNAME'
password = '###YOUR-WIKIDATA-PASSWORD'

singular = 'Q110786'
plural = 'Q146786'

nominative = 'Q131105'
genitive = 'Q146233'
dative = 'Q145599'
accusative = 'Q146078'
vocative = 'Q185077'
locative = 'Q202142'
instrumental = 'Q192997'

animate = 'Q51927507'

feminine = 'Q1775415'
masculine = 'Q499327'
neuter = 'Q1775461'

female = 'Q6581072'
male = 'Q6581097'

gender = 'P5185'
itemforthissense = 'P5137'
semanticgender = 'P10339'

Croatian = 'Q6654'

noun = 'Q1084'

def hr(s): return s @ tfsl.langs.hr_
def en(s): return s @ tfsl.langs.en_

def createLexeme(lemma, topics, forms, gendr):
	lexeme = tfsl.Lexeme( getform(forms, nominative, singular, gendr), tfsl.langs.hr_, noun )

	genderstatement = tfsl.Statement( gender, tfsl.ItemValue(gendr) )

	lexeme += genderstatement
	
	for topic in topics:
		print(topics)
		sense = makesense(lemma, topic, gendr)
		lexeme += sense

	nom_sg = tfsl.LexemeForm([ getform(forms, nominative, singular, gendr) ])
	nom_sg += nominative
	nom_sg += singular
	lexeme += nom_sg

	gen_sg = tfsl.LexemeForm([ getform(forms, genitive, singular, gendr) ])
	gen_sg += singular
	gen_sg += genitive
	lexeme += gen_sg

	dat_sg = tfsl.LexemeForm([ getform(forms, dative, singular, gendr) ])
	dat_sg += singular
	dat_sg += dative
	lexeme += dat_sg

	acc_sg = tfsl.LexemeForm([ getform(forms, accusative, singular, gendr) ])
	acc_sg += singular
	acc_sg += accusative
	lexeme += acc_sg

	voc_sg = tfsl.LexemeForm([ getform(forms, vocative, singular, gendr) ])
	voc_sg += singular
	voc_sg += vocative
	lexeme += voc_sg

	loc_sg = tfsl.LexemeForm([ getform(forms, locative, singular, gendr) ])
	loc_sg += singular
	loc_sg += locative
	lexeme += loc_sg

	ins_sg = tfsl.LexemeForm([ getform(forms, instrumental, singular, gendr) ])
	ins_sg += singular
	ins_sg += instrumental
	lexeme += ins_sg

	nom_pl = tfsl.LexemeForm([ getform(forms, nominative, plural, gendr) ])
	nom_pl += plural
	nom_pl += nominative
	lexeme += nom_pl

	gen_pl = tfsl.LexemeForm([ getform(forms, genitive, plural, gendr) ])
	gen_pl += plural
	gen_pl += genitive
	lexeme += gen_pl

	dat_pl = tfsl.LexemeForm([ getform(forms, dative, plural, gendr) ])
	dat_pl += dative
	dat_pl += plural
	lexeme += dat_pl

	acc_pl = tfsl.LexemeForm([ getform(forms, accusative, plural, gendr) ])
	acc_pl += plural
	acc_pl += accusative
	lexeme += acc_pl

	voc_pl = tfsl.LexemeForm([ getform(forms, vocative, plural, gendr) ])
	voc_pl += plural
	voc_pl += vocative
	lexeme += voc_pl

	loc_pl = tfsl.LexemeForm([ getform(forms, locative, plural, gendr) ])
	loc_pl += plural
	loc_pl += locative
	lexeme += loc_pl

	ins_pl = tfsl.LexemeForm([ getform(forms, instrumental, plural, gendr) ])
	ins_pl += plural
	ins_pl += instrumental
	lexeme += ins_pl
	
	return lexeme

def createLexemes(wikidata, senses, forms):
	done = []
	for lemma in senses:
		if lemma in wikidata:
			continue
		if lemma in already_done:
			continue
		if ' ' in lemma:
			continue
		lexeme_m = createLexeme(lemma, senses[lemma], forms[lemma], masculine)
		lexeme_f = createLexeme(lemma, senses[lemma], forms[lemma], feminine)
		if lexeme_m == False or lexeme_f == False: continue
		
		more = publish([lexeme_f, lexeme_m])
		if not more: break
		done.append(lemma)
		x = input('Continue? (Y/n) > ')
		more = (x in ['y', 'Y', 'yes', '', 'YES', 'Yes'])
		if not more: break
	for entry in done:
		print("\t'" + entry + "',")
	return done
	
def publish(lexemes):
	for lexeme in lexemes:
		print(lexeme)
	x = input('Upload these two? (Y/n) > ')
	if (x not in ['y', 'Y', 'yes', '', 'YES', 'Yes']): return False
	session = tfsl.WikibaseSession(username, password)
	for lexeme in lexemes:
		session.push( lexeme )
	return True

# Read data
...

# Create lexemes and publish.
publish(createLexemes(wikidata, senses, forms))
```

### Preparing data

All approaches require data in a certain format which is used by Wikidata. A minimal set of JSON keys to add a noun is proveded below. Full file specification is available at [Wikidata](https://www.wikidata.org/wiki/Wikidata:Lexicographical_data/Documentation).

Category values, e.g. Q1084, are defined in [Grammar file](../inflection/tools/dictionary-parser/src/main/java/org/unicode/wikidata/Grammar.java).

```json
[
{
    "type": "lexeme",
    "lemmas": {
        "sr": {
            "language": "sr",
            "value": "тутумрак"
        }
    },
    "lexicalCategory": "Q1084",
    "language": "Q9299",
    "claims": {
        "P5185": [
            {
                "mainsnak": {
                    "snaktype": "value",
                    "property": "P5185",
                    "datavalue": {
                        "value": {
                            "id": "Q499327"
                        }
                    }
                }
            }
        ]
    },
    "forms": [
        {
            "representations": {
                "sr": {
                    "language": "sr",
                    "value": "тутумрак"
                }
            },
            "grammaticalFeatures": [
                "Q110786",
                "Q131105"
            ]
        },
        {
            "representations": {
                "sr": {
                    "language": "sr",
                    "value": "тутумрака"
                }
            },
            "grammaticalFeatures": [
                "Q110786",
                "Q146233"
            ]
        },
        {
            "representations": {
                "sr": {
                    "language": "sr",
                    "value": "тутумраку"
                }
            },
            "grammaticalFeatures": [
                "Q110786",
                "Q145599"
            ]
        {
            "representations": {
                "sr": {
                    "language": "sr",
                    "value": "тутумрака"
                }
            },
            "grammaticalFeatures": [
                "Q110786",
                "Q146078"
            ]
        },
        {
            "representations": {
                "sr": {
                    "language": "sr",
                    "value": "тутумраче"
                }
            },
            "grammaticalFeatures": [
                "Q110786",
                "Q185077"
            ]
        },
        {
            "representations": {
                "sr": {
                    "language": "sr",
                    "value": "тутумраком"
                }
            },
            "grammaticalFeatures": [
                "Q110786",
                "Q192997"
            ]
        },
        {
            "representations": {
                "sr": {
                    "language": "sr",
                    "value": "тутумраку"
                }
            },
            "grammaticalFeatures": [
                "Q110786",
                "Q202142"
            ]
        },
        {
            "representations": {
                "sr": {
                    "language": "sr",
                    "value": "тутумраци"
                }
            },
            "grammaticalFeatures": [
                "Q131105",
                "Q146786"
            ]
        },
        {
            "representations": {
                "sr": {
                    "language": "sr",
                    "value": "тутумрака"
                }
            },
            "grammaticalFeatures": [
                "Q146233",
                "Q146786"
            ]
        },
        {
            "representations": {
                "sr": {
                    "language": "sr",
                    "value": "тутумрацима"
                }
            },
            "grammaticalFeatures": [
                "Q145599",
                "Q146786"
            ]
        },
        {
            "representations": {
                "sr": {
                    "language": "sr",
                    "value": "тутумраке"
                }
            },
            "grammaticalFeatures": [
                "Q146078",
                "Q146786"
            ]
        },
        {
            "representations": {
                "sr": {
                    "language": "sr",
                    "value": "тутумраци"
                }
            },
            "grammaticalFeatures": [
                "Q146786",
                "Q185077"
            ]
        },
        {
            "representations": {
                "sr": {
                    "language": "sr",
                    "value": "тутумрацима"
                }
            },
            "grammaticalFeatures": [
                "Q146786",
                "Q192997"
            ]
        },
        {
            "representations": {
                "sr": {
                    "language": "sr",
                    "value": "тутумрацима"
                }
            },
            "grammaticalFeatures": [
                "Q146786",
                "Q202142"
            ]
        }
    ]
},
...
]
```