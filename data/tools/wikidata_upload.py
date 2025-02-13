"""
Sample script for uploading lexicon data to Wikidata using tfsl library.

Follow these instructions to get tfsl:
https://phabricator.wikimedia.org/source/tool-twofivesixlex/browse/main/

git clone https://phabricator.wikimedia.org/source/tool-twofivesixlex tfsl

Code expects tfsl checkout location next to inflection repository.
"""

#import qnames
#import requests
import argparse
import csv
import functools
import json
import operator
import os
import readline
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', '..', 'tfsl'))
import tfsl

# Easy conversion from humanly readable form to/from Wikidata codes.
WIKI_TYPES = {
'singular': 'Q110786',
'plural': 'Q146786',

'nominative': 'Q131105',
'genitive': 'Q146233',
'dative': 'Q145599',
'accusative': 'Q146078',
'vocative': 'Q185077',
'locative': 'Q202142',
'instrumental': 'Q192997',

'animate': 'Q51927507',

# Grammatical gender
'feminine': 'Q1775415',
'masculine': 'Q499327',
'neuter': 'Q1775461',

# To be used in "sex or gender" (P21) to indicate that the human subject is a male/female
# or "semantic gender" (P10339) to indicate that a word refers to a male/female person.
'female': 'Q6581072',
'male': 'Q6581097',

'gender': 'P5185',
'semanticgender': 'P10339',

'noun': 'Q1084',
'proper noun': 'Q147276',
'pronouns': 'Q36224',
'possesive pronouns': 'Q1502460',
'adjective': 'Q34698',
}
WIKI_TYPES.update(dict([reversed(i) for i in WIKI_TYPES.items()]))

def hr(s): return s @ tfsl.langs.hr_
def en(s): return s @ tfsl.langs.en_

"""
def countvalues(d):
	if len(d) == 0: return 0
	return functools.reduce(operator.add, map(len, d.values()))

def editable_input(prompt, default=""):
    # Set the default value for the input
    def hook():
        readline.insert_text(default)
        readline.redisplay()
    readline.set_pre_input_hook(hook)
    try:
        # Display the prompt and get user input
        result = input(prompt)
    finally:
        readline.set_pre_input_hook(None)  # Reset the hook
    return result

def readWordGraph(wikidata):
	senses = {}
	forms = {}
	lemmas = set()
	with open('wordgraph/hr_wikidata.tsv', 'r') as wordgraph_file:
		wordgraph = csv.reader(wordgraph_file, delimiter="\t")
		next(wordgraph)
		for line in wordgraph:
			(topic, relation, language, pos, lemma, form, features) = line
			if lemma in wikidata: continue
			if lemma in already_done: continue
			lemmas.add(lemma)
			if relation != 'Human denoting sense' and relation != 'Demonym adjective':
				print(relation)
			if pos != 'adjectival' and pos != 'nominal':
				print(pos)
			if relation == 'Human denoting sense' and pos != 'nominal':
				print(lemma)
			if relation == 'Demonym adjective' and pos != 'adjectival':
				print(lemma)
			if pos == 'adjectival': continue ### change this to adjectival once nominals are done
			
			if lemma not in senses: senses[lemma] = []
			if topic not in senses[lemma]: senses[lemma].append(topic)
			if lemma not in forms: forms[lemma] = {}
			if form not in forms[lemma]: forms[lemma][form] = []
			forms[lemma][form].append(features.split(','))
	print(len(lemmas), 'unique lemmas in WordGraph')
	print(countvalues(senses), 'senses in WordGraph')
	return senses, forms

def readWikidata():
	linecount = 0
	lexemecount = 0
	langcount = 0
	formcount = 0
	endline = 1000000000
	lemmas = set()
	senses = {}
	topics = set()
	for line in open('data/hr-lexemes.json'):
		linecount += 1
		if linecount > endline: break
		line = line.strip()
		if not line.startswith('{'): continue
		if line.endswith(','): line = line[:-1]
		lexeme = json.loads(line)
		if lexeme['type'] != 'lexeme': continue
		lexemecount += 1
		if lexeme['language'] != Croatian: continue
		langcount += 1
		if not 'hr' in lexeme['lemmas']:
			print('***')
			continue
		lemma = lexeme['lemmas']['hr']['value']
		lemmas.add(lemma)
		for sense in lexeme['senses']:
			# print(sense['claims'])
			for prop in sense['claims']:
				for claim in sense['claims'][prop]:
					snak = claim['mainsnak']
					if snak['snaktype'] != 'value': continue
					if snak['datatype'] != 'wikibase-item': continue
					topic = snak['datavalue']['value']['id']
					topics.add(topic)
					if not lemma in senses: senses[lemma] = []
					if not topic in senses[lemma]: senses[lemma].append(topic)
	# print(lexemecount, 'lexemes in Wikidata')
	# print(len(lemmas), 'unique lemmas in Wikidata')
	# print(len(topics), 'unique topics in Wikidata')
	# print(countvalues(senses), 'senses in Wikidata (possibly overcount)')
	return lemmas

def getform(forms, cas, numbr, gendr):
	for form in forms:
		for features in forms[form]:
			if cas not in features: continue
			if numbr not in features: continue
			if gendr not in features: continue
			if animate not in features: continue
			if len(features) != 4: continue
			return hr(form)
	print('*** something is off ***')
	print(forms)
	exit(-1)

def getgloss(default):
	return editable_input('> ', default)

def getwikidata(qid):
	url = 'https://www.wikidata.org/wiki/Special:EntityData/' + qid + '.json'
	response = requests.get(url)
	response.raise_for_status()
	result = response.json()
	item = result['entities'][qid]
	label_en = None
	if 'en' in item['labels']:
		label_en = item['labels']['en']['value']
	label_hr = None
	if 'hr' in item['labels']:
		label_hr = item['labels']['hr']['value']
	description_en = None
	if 'en' in item['descriptions']:
		description_en = item['descriptions']['en']['value']
	description_hr = None
	if 'hr' in item['descriptions']:
		description_en = item['descriptions']['hr']['value']
	return label_en, description_en, label_hr, description_hr

def makesense(lemma, topic, gendr):
	label_en, description_en, label_hr, description_hr = getwikidata(topic)
	g = 'female'
	if gendr == masculine:
		g = 'male'
	print(topic, lemma, ' ', g)
	print(label_hr, ': ', description_hr)
	print(label_en, ': ', description_en)
	gloss_hr = hr(getgloss(('ženska ' if gendr==feminine else 'muški ') + (description_hr if description_hr else '')))
	gloss_en = en(getgloss(label_en + ' (' + g + '); ' + description_en))
	sense = tfsl.LexemeSense( [ gloss_hr, gloss_en ] )
	itemconnection = tfsl.Statement( itemforthissense, tfsl.ItemValue(topic) )
	sense += itemconnection
	genderstatement = tfsl.Statement( semanticgender, tfsl.ItemValue(female if gendr == feminine else male))
	sense += genderstatement
	return sense

def createLexeme(lemma, topics, forms, gendr):
	if countvalues(forms) % 28 != 0:
		print('NUMBER OF FORMS OFF IN CREATE LEXEME')
		print(lemma)
		print(countvalues(forms))
		print(forms)
		print(topics)
		print(gendr)
		return False
		exit(-1)

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

wikidata = readWikidata()
senses, forms = readWordGraph(wikidata)

done = createLexemes(wikidata, senses, forms)
print(len(done), 'done')
"""
def load_wikidata(lang, wiki_file):
    """
    Loads lemma, language and grammatical category for easy lookup.

    Args:
        lang (str): Language to filter by.
        wiki_data (str): Path to wikidata JSON file.
    """
    # Lemma-language-grammatical category set to allow for duplication of
    # different types.
    print("Processing wikidata, it may take a while...")
    result = set()
    wiki_lang = tfsl.languages.get_first_lang(lang).item
    count = 0
    try:
     with open(wiki_file, 'r', encoding='utf-8') as file:
        for line in file:
            # File is large, ~5GB, so we load line by line.
            # Wikidata is an array of objects, this removes commas, newlines...
            line = line.strip('[],\n')
            try:
                data = json.loads(line)
                if data['language'] != wiki_lang or lang not in data['lemmas']:
                    continue
                count += 1
                lemma = data['lemmas'][lang]['value']
                cat = data['lexicalCategory']
                result.add('-'.join([lemma, cat, lang]))
            except json.JSONDecodeError:
                print(f"Warning: Invalid JSON line: {line}")
    except FileNotFoundError:
        print(f"Error: File '{wiki_file}' not found.")
    except PermissionError:
        print(f"Error: Permission denied to read file '{wiki_file}'.")
    print(f'Collected {count} items for {lang}.')
    return result
		

def upload_data(username, password, lang, input_file, wiki_file, test_only, delay_ms):
    """
    Processes the input file and uploads to Wikidata given username and password.

    Args:
        username (str): The username for authentication.
        password (str): The password for authentication.
		lang (str): ISO 639 language code.
        input_file (str): The path to the input lexeme file.
		wiki_file (str): Path to Wikidata JSON file.
        test_only (bool): Upload if false, print out if true.
        delay_ms (int): Upload delay in ms, for rate limiting.
    """
    # Load and validate input_file
    wikidata = load_wikidata(lang, wiki_file)
	# Skip entities that already exist in Wikifile
	# From the reminder build tfsl Lexemes
	# Upload or print depending on test flag

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Upload data to Wikidata requiring authentication.')
    parser.add_argument('username', help='Username for authentication.')
    parser.add_argument('password', help='Password for authentication.')
    parser.add_argument('lang', help='ISO 639 language code, e.g. en, sr, es...')
    parser.add_argument('input_file', help='Path to the input file.')
    parser.add_argument('wiki_file', help='Path to the wikidata JSON file.')
    parser.add_argument('--test', action='store_true', help='Test run, prints instead of upload.')
    parser.add_argument('--delay', type=int, default=1000, help='Milliseconds between uploads for rate limiting.')
    args = parser.parse_args()

    upload_data(args.username, args.password, args.lang, args.input_file, args.wiki_file, args.test, args.delay)