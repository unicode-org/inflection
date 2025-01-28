"""
Extracts lexicographic data from Wikidata for Unicode Inflection usage

First you need to download the lexicographic data wholesale from Wikidata.
You can do this by downloading the data from the Web, with either of the
following two commands.

The first one downloads a specifc version of the data. The following
command downloads the first version of 2025:

wget -O lexemes.json.bz2 https://dumps.wikimedia.org/wikidatawiki/entities/20250101/wikidata-20250101-lexemes.json.bz2

A list of all available versions can be found on the Web here:
https://dumps.wikimedia.org/wikidatawiki/entities/

Or you can download the most recent version using the following command:

wget -O lexemes.json.bz2 https://dumps.wikimedia.org/wikidatawiki/entities/latest-lexemes.json.bz2

After that run this script in the dictionary in which the lexemes.json.bz2 file is located.
The result will be a dictionary_xxx.lst file for the given language.
Per default the language is 'en', but you can use one of the permitted language code insteads.

== How to add a new language ==
Find the languages language code, name, and QID (you can use Wikidata for finding all of these).

Add the language in the object code2qid (around line 40) with a mapping from the language code
to the Wikidata QID for the language.
Add the language in the list of languages at the beginning of the qid2name object (around line 70)
with a mapping from the QID to the (English) language name.
Then run the script for the new language and add all missing QIDs with an identifier for the
given grammatical feature to the same dictionary until it runs without errors.
"""

import bz2
import json
import sys

linecount = 0
lexemecount = 0
languagelexemecount = 0
formcount = 0
errorcount = 0

code2qid = {
  'ar': 'Q13955',
  'da': 'Q9035',
  'de': 'Q188',
  'en': 'Q1860',
  'es': 'Q1321',
  'fr': 'Q150',
  'he': 'Q9288',
  'hi': 'Q1568',
  'hr': 'Q6654',
  'id': 'Q9240',
  'it': 'Q652',
  'ja': 'Q5287',
  'kai': 'Q3438770',
  'ko': 'Q9176',
  'ms': 'Q9237',
  'nb': 'Q25167',
  'nl': 'Q7411',
  'pt': 'Q5146',
  'ru': 'Q7737',
  'sr':'Q9299',
  'sv': 'Q9027',
  'th': 'Q9217',
  'tr': 'Q256',
  'uz': 'Q9264',
  'zh': 'Q7850',
}

qid2name = {
  'Q13955': 'Arabic',
  'Q9035': 'Danish',
  'Q188': 'German',
  'Q1860': 'English',
  'Q1321': 'Spanish',
  'Q150': 'French',
  'Q9288': 'Hebrew',
  'Q1568': 'Hindi',
  'Q6654': 'Croatian',
  'Q9240': 'Indonesian',
  'Q652': 'Italian',
  'Q5287': 'Japanese',
  'Q3438770': 'Karai-karai',
  'Q9176': 'Korean',
  'Q9237': 'Malay',
  'Q25167': 'Norwegian Boksmal',
  'Q7411': 'Dutch',
  'Q5146': 'Portuguese',
  'Q7737': 'Russian',
  'Q9299': 'Serbian',
  'Q9027': 'Swedish',
  'Q9217': 'Thai',
  'Q256': 'Turkish',
  'Q9264': 'Uzbek',
  'Q7850': 'Chinese',
  
  'Q1084': 'noun',
  'Q24905': 'verb',
  'Q63116': 'numeral',
  'Q576271': 'determiner',
  'Q34698': 'adjective',
  'Q4833830': 'preposition',
  'Q36484': 'conjunction',
  'Q83034': 'interjection',
  'Q36224': 'pronoun',
  'Q102047': 'suffix',
  'Q134830': 'prefix',
  'Q161873': 'postposition',
  'Q7884789': 'toponym',
  'Q66614509': 'nominal-suffix',
  'Q9788': 'letter',
  'Q2865743': 'definite-article',
  'Q29888377': 'nominal-locution',
  'Q12734432': 'attributive-locution',
  'Q2304610': 'interrogative-word',
  'Q101244': 'acronym',
  'Q1668170': 'interrogative-adverb',
  'Q56042915': 'prepositional phrase',
  'Q187931': 'phrase',
  'Q468801': 'personal-pronoun',
  'Q3976700': 'adjectival-suffix',
  'Q217438': 'demonym',
  'Q7250170': 'proper-adjective',
  'Q1964223': 'name-suffix',
  'Q184511': 'idiom',
  'Q81058955': 'demonym',
  'Q1153504': 'interfix',
  'Q5978305': 'conjunctive-locution',
  'Q130266209': 'reflexive-personal-pronoun',
  'Q1527589': 'phrasal-verb',
  'Q1122269': 'collocation',
  'Q30515': 'slogan',
  'Q35102': 'proverb',
  'Q5': 'human', 
  'Q10976085': 'verbal-locution',
  'Q1778442': 'verb-phrase',
  'Q43369910': 'compound-noun',
  'Q80071': 'symbol',
  'Q62155': 'affix',
  'Q191494': 'digraph',
  'Q7233569': 'postpositive-adjective',
  'Q184943': 'grammatical-particle',
  'Q182990': 'uracil',
  'Q43229': 'organization',
  'Q4335462': 'definite-pronoun',
  'Q10319520': 'interjectional-locution',
  'Q10319522': 'prepositional-locution',
  'Q41796': 'sentence',
  'Q43249': 'morpheme',
  'Q1167104': 'conjunctive-adverb',
  'Q10535365': 'infinitive-marker',
  'Q5456361': 'fixed-expression',
  'Q357760': 'adjectival-phrase',
  'Q3257782': 'locution',
  'Q1787727': 'agent-noun',
  'Q10265745': 'demonstrative-determiner',
  'Q8171': 'word',
  'Q918270': 'initalism',
  
  'Q10345583': 'present',
  'Q110786': 'singular',
  'Q3910936': 'present',
  'Q51929074': 'third',
  'Q1230649': 'past',
  'Q1392475': 'past',
  'Q1380802': 'causal-adverb',
  'Q146786': 'plural',
  'Q146233': 'genitive',
  'Q131105': 'nominative',
  'Q4818723': 'attributive',
  'Q1931259': 'predicative',
  'Q2105891': 'possessive',
  'Q3482678': 'positive',
  'Q14169499': 'comparative',
  'Q1817208': 'superlative',
  'Q122477358': 'double-comparative',
  'Q65248385': 'comparative-adverb',
  'Q1861696': 'possessive',
  'Q30619513': 'abbreviation',
  'Q21470140': 'possessive',
  'Q1478451': 'negation',
  'Q501405': 'future',
  'Q51929049': 'second',
  'Q56682909': 'present-indicative',
  'Q125286251': 'past-indicative',
  'Q55965516': 'disjunctive-conjunction',
  'Q8185162': 'lowercase-letter',
  'Q98912': 'uppercase-letter',
  'Q102786': 'abbreviation',
  'Q57156426': 'standalone-form',
  'Q57156560': 'affixed-form',
  'Q126473': 'contraction',
  'Q1233197': 'oblique',
  'Q5051': 'possessive-determiner',
  'Q1502460': 'possessive-pronoun',
  'Q98772589': 'expanded-contraction',
  'Q953129': 'reflexive-pronoun',
  'Q59342809': 'alternative-form',
  'Q3813849': 'indefinite article',
  'Q1401131': 'noun-phrase',
  'Q489168': 'noun',
  'Q332375': 'absolute-adjective',
  'Q192613': 'present',
  'Q21714344': 'first',
  'Q228503': 'uninflected-word',
  'Q188224': 'mode',
  'Q1994301': 'past',
  'Q442485': 'preterite',
  'Q473746': 'subjunctive',
  'Q138246': 'plurale-tantum',
  'Q604984': 'singulare-tantum',
  'Q38918': 'roman-numerals',
  'Q179230': 'infinitive',
  'Q12717679': 'past',
  'Q3026787': 'saying',
  'Q8102': 'slang',
  'Q126897884': 'denominal',
  'Q682111': 'indicative',
  'Q22716': 'imperative',
  'Q181970': 'archaism',
  'Q380057': 'adverb',
  'Q96406487': 'short-form',
  'Q185077': 'vocative',
  'Q78191294': 'singular-possessive',
  'Q78191289': 'plural-possessive',
  'Q147276': 'proper-noun',
  'Q107614077': 'combining-form',
  'Q956030': 'indefinite-pronoun',
  'Q53997851': 'definite',
  'Q53997857': 'indefinite',
  'Q1941737': 'modal-adverb',
  'Q12274060': 'future-in-the-past',
  'Q1329258': 'cardinal-numeral',
  'Q1775415': 'feminine',
  'Q499327': 'masculine',
  'Q1923028': 'gerund',
  'Q1240211': 'present-perfect',
  'Q901711': 'colloquial-language',
  'Q54310231': 'interrogative-pronoun',
  'Q1522423': 'locative-adverb',
  'Q1984758': 'misspelling',
  'Q117605926': 'perfective-verbal-expression',
  'Q65048529': 'lowercase-text',
  'Q12812139': 'technical-term',
  'Q126729404': 'deverbal',
  'Q131431824': 'proper-verb',
  'Q113076880': 'postpositive-adverb',
  'Q130270424': 'interrogative-expression',
  'Q2034977': 'prepositional-adverb',
  'Q5978303': 'adverbial-locution',
  'Q65280376': 'collocation',
  'Q106610283': 'adverbial-suffix',
  'Q65248545': 'superlative-adverb',
  'Q1774805': 'verb',
  'Q113198319': 'adverbial-particle',
  'Q2034977': 'prepositional-adverb',
  'Q3517796': 'simple-tense',
  'Q146078': 'accusative',
  'Q145599': 'dative',
  'Q1317831': 'active',
  'Q1775461': 'neuter',
  'Q7977953': 'weak-inflection',
  'Q3481903': 'strong-inflection',
  'Q96407524': 'mixed-inflection',
  'Q55685962': 'subjunctive',
  'Q54671845': 'subjunctive-2',
  'Q101583900': 'alternative-spelling',
  'Q55228835': 'colloquial',
  'Q100952920': 'zu-infinitive',
  'Q623742': 'pluperfect',
  'Q371427': 'imperfective',
  'Q625420': 'perfect',
  'Q54792077': 'prefixoid',
  'Q358417': 'circumposition',
  'Q34793275': 'demonstrative-pronoun',
  'Q1050744': 'relative-pronoun',
  'Q77768790': 'formal',
  'Q7188068': 'phrasal-template',
  'Q52434245': 'present-infinitive',
  'Q15841920': 'idiom',
  'Q55698475': 'statal-passive',
  'Q55698471': 'processual-passive',
  'Q1424306': 'perfective',
  'Q210523': 'word-stem',
  'Q202142': 'locative',
  'Q192997': 'instrumental',
  'Q1194697': 'passive',
  'Q53998049': 'indefinite',
  'Q51927507': 'animate',
  'Q51927539': 'inanimate',
  'Q108524486': 'imperfect',
  'Q216497': 'aorist',
  'Q12721117': 'continuous',
  'Q12547192': 'past-imperfect',
  'Q545779': 'pejorative',
  'Q109267112': 'affirmative',
  'Q23199274': 'affirmation',
  'Q1234617': 'future-perfect',
  'Q923933': 'ordinal',
  'Q904896': 'transgressive',
  'Q114752182': 'preconsonantal',
  'Q109371067': 'emphatic',
  'Q113289507': 'emphatic',
  'Q3502544': 'past-subjunctive',
  'Q3502553': 'present-subjunctive',
  'Q23663136': 'past-perfect',
  'Q2006180': 'pro-form',
}
name2qid = {v: k for k, v in qid2name.items()}

filterpos = [
  'Q187931', 'Q184511', 'Q9788', 'Q30515', 'Q35102',
  'Q1401131', 'Q5', 'Q80071', 'Q191494', 'Q182990',
  'Q43229', 'Q41796', 'Q43249', 'Q5456361',
  'Q3438770',
]

dictionary = {}

languagecode = 'en'
if len(sys.argv) > 1:
  languagecode = sys.argv[1]
if languagecode not in code2qid:
  print('Unknown language code', languagecode)
  exit(1)

for line in bz2.open('lexemes.json.bz2'):
  linecount += 1
  if not line.startswith(b'{'): continue
  line = line.decode()
  line = line.strip()
  if line.endswith(','): line = line[:-1]
  #print(line)
  lexeme = json.loads(line)
  if lexeme['type'] != 'lexeme': continue
  lexemecount += 1
  if lexeme['language'] != code2qid[languagecode]: continue
  languagelexemecount += 1
  if not languagecode in lexeme['lemmas']: continue
  # print(lexeme['lemmas'][languagecode]['value'])
  pos = lexeme['lexicalCategory']
  if pos not in qid2name:
    print('***** missing', pos)
    errorcount += 1
    continue
  if pos in filterpos: continue
  for form in lexeme['forms']:
  	if not languagecode in form['representations']: continue
  	word = form['representations'][languagecode]['value']
  	if word.startswith('-') or word.endswith('-'): continue
  	if word not in dictionary:
  	  dictionary[word] = []
  	if qid2name[pos] not in dictionary[word]:
  	  dictionary[word].append( qid2name[pos] )
  	for feature in form['grammaticalFeatures']:
  	  if feature not in qid2name:
  	    print('***** missing', feature)
  	    errorcount += 1
  	    continue
  	  if qid2name[feature] not in dictionary[word]:
  	    dictionary[word].append( qid2name[feature] )

dictionary_file = open('dictionary_' + languagecode + '.lst', 'w')
for word in sorted(dictionary.keys()):
  dictionary_file.write(word + ': ' + ' '.join(dictionary[word]) + '\n')

dictionary_file.write('=============================================\n')
dictionary_file.write('Processed lines: ' + str(linecount) + '\n')
dictionary_file.write('Processed lexemes: ' + str(lexemecount) + '\n')
dictionary_file.write('Processed ' + qid2name[code2qid[languagecode]] + ' lexemes: ' + str(languagelexemecount) + '\n')
dictionary_file.write('Resulting entries: ' + str(len(dictionary.keys())) + '\n')
dictionary_file.write('Source file: https://dumps.wikimedia.org/wikidatawiki/entities/20250101/wikidata-20250101-lexemes.json.gz\n')
dictionary_file.write('Transforming file: transform-lexemes-unicode.py\n')
print('Processed ' + str(languagelexemecount) + ' entries resulting ' + str(len(dictionary.keys())) + ' in dictionary_' + languagecode + '.lst')
if errorcount > 0:
  print(str(errorcount) + ' errors')
