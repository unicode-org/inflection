"""
Sample script for uploading lexicon data to Wikidata using tfsl library.
"""

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

# Depending on upload rate and amount of data you may need to use Bot account.
username = 'YOUR_WIKIDATA_ACCOUNT'
password = 'YOUR_WIKIDATA_PASSWORD'

# TODOs:
# Skip lexemes already in wikidata

already_done = [
	'glavni ministar',
	'diktator',
	'znanstveni pisac',
	'poslovni čovjek',
	'fotoreporter',
	'ronilac',
	'senator',
	'oružar',
	'arborist',
	'krvnik',
	'programer',
	'kupac',
	'sudac',
	'trener za snagu i kondiciju',
	'geolog',
	'tehnički crtač',
	'konstabl',
	'branič',
	'tehnolog',
	'vođa sastava',
	'youtuber',
	'zamjenik predsjednika vlade', # composed
	'bejzbolski trener', # composed
	'predsjednik gradske četvrti', # composed
	'ulema', # just one
	'guvernante', # something off?
	'supermodel', # just one
	'policajac',  # double meaning too close
	'kontrabasist',
	'vanjski igrač', # composed?
	'pravni savjetnik', # composed?
	'premijer', # double meaning too close
	'tiranin',
	'igrač ragbija', # composed
	'predak',
	'psihoterapeut',
	'endokrinolog',
	'moderni petobojac', # composed?
	'klarinetist',
	'prisjednik',
	'orkestrator',
	
]

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
