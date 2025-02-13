"""
Sample script for uploading lexicon data to Wikidata using tfsl library.

Follow these instructions to get tfsl:
https://phabricator.wikimedia.org/source/tool-twofivesixlex/browse/main/

git clone https://phabricator.wikimedia.org/source/tool-twofivesixlex tfsl

Code expects tfsl checkout location next to inflection repository.
"""

import argparse
import json
import os
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
'personal pronoun': 'Q468801',
# Word or form that substitutes for another word, broader scope than pronoun.
'pro-form': 'Q2006180',
}
# Make it a bidirectional dictionary.
WIKI_TYPES.update(dict([reversed(i) for i in WIKI_TYPES.items()]))

def hr(s): return s @ tfsl.langs.hr_
def en(s): return s @ tfsl.langs.en_

"""
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
def make_search_key(lemma, cat, lang):
    """Makes a key for searching through wikidata for duplicates.

    Args:
        lemma (str): Lemma
        cat (str): gramatical category
        lang (str): language
    """
    return '-'.join([lemma, cat, lang])

def load_lexemes(lang, input_file):
    """
    Loads lexemes from the input_file.

    Args:
        lang (str): Requested language.
        input_file (str): Path to JSON file with new lexemes.
    """
    print("Processing lexemes...")
    try:
     with open(input_file, 'r', encoding='utf-8') as file:
        data = json.load(file)
        for lexeme in data:
            # Validate before further processing.
            if (lexeme['language'] != lang or
                lexeme['grammaticalCategory'] not in WIKI_TYPES):
                print(f'Invalid entry {lang}, {lexeme["grammaticalCategory"]} in\n{lexeme}')
                exit(-1)
            for form in lexeme['forms']:
                for feature in form['grammaticalFeatures']:
                    if feature not in WIKI_TYPES:
                        print(f'Invalid feature: {feature} in\n{lexeme}, ')
                        exit(-1)
    except FileNotFoundError:
        print(f"Error: File '{input_file}' not found.")
    except json.JSONDecodeError:
        print(f"Error: Invalid JSON in file '{input}'.")
    print(f'Collected {len(data)} items.')
    return data

def load_wikidata(lang, wiki_file):
    """
    Loads lemma, language and grammatical category for easy lookup.

    Args:
        lang (str): Language to filter by.
        wiki_file (str): Path to wikidata JSON file.
    """
    print("Processing wikidata, it may take a while...")
    # Lemma-language-grammatical category set to allow for duplication of
    # different types.
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
                result.add(make_search_key(lemma, cat, lang))
            except json.JSONDecodeError:
                # We can't do much except to skip.
                continue
    except FileNotFoundError:
        print(f"Error: File '{wiki_file}' not found.")
    except PermissionError:
        print(f"Error: Permission denied to read file '{wiki_file}'.")
    print(f'Collected {count} items for {lang}.')
    return result

def filter_duplicates(lexemes, wikidata):
    """Filters out lexemes already in wikidata.

    Args:
        lexemes (list): List of loaded lexemes.
        wikidata (list): Wikidata dump.
    """
    count = 0
    new_lexemes = []
    for lexeme in lexemes:
        key = make_search_key(lexeme['lemma'],
                              WIKI_TYPES[lexeme['grammaticalCategory']],
                              lexeme['language'])
        if key not in wikidata:
            count +=1
            new_lexemes.append(lexeme)
    print(f'New lexemes: {count}, duplicates: {len(lexemes)-count}.')
    return new_lexemes

def build_tfsl_lexemes(new_lexemes):
    """For each new_lexeme builds a corresponding tfsl one.

    Args:
        new_lexemes (list): New lexemes from the input_file.
    """
    tfsl_lexemes = []
    return tfsl_lexemes

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
    lexemes = load_lexemes(lang, input_file)
    wikidata = load_wikidata(lang, wiki_file)
    new_lexemes = filter_duplicates(lexemes, wikidata)
    tfsl_lexemes = build_tfsl_lexemes(new_lexemes)
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