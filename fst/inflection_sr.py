"""
Inflects Serbian nouns by case and number.

Lexicon entry looks like this, attributes are after ':'
субота: noun feminine nominative singular inanimate

Run from project root folder.

Install Pynini package before running.
"""

import os
import pynini as p

from pynini.lib import features
from pynini.lib import paradigms
from pynini.lib import pynutil
from pynini.lib import rewrite
from utils import parse_lexicon_entry
from utils import priority_union

# Keep stats on how many nouns per class.
stats = {
'Group1m': 0,
'Group1n': 0,
'Group1mi': 0,
'Group1mp': 0,
'Group2n': 0,
'Group2t': 0,
'Group3': 0,
'Group4': 0,
'Exception': 0
}

_v = p.union('а', 'е', 'и', 'о', 'у')
_v_up = p.union('А', 'Е', 'И', 'О', 'У')
_c = p.union('б', 'в', 'г', 'д', 'ђ', 'ж', 'з', 'ј', 'к', 'л', 'љ', 'м',
             'н', 'њ', 'п', 'р', 'с', 'т', 'ћ', 'ф', 'х', 'ц', 'ч', 'џ', 'ш')
_c_up = p.union('Б', 'В', 'Г', 'Д', 'Ђ', 'Ж', 'З', 'Ј', 'К', 'Л', 'Љ', 'М',
                'Н', 'Њ', 'П', 'Р', 'С', 'Т', 'Ћ', 'Ф', 'Х', 'Ц', 'Ч', 'Џ', 'Ш')
_sigma = p.union(_v, _v_up, _c, _c_up).closure().optimize()

# Noun group rules for classification.
# Match pattern and append group name to the end.

# Group 1:
#  - Masculine with nominative sg that ends with consonant, -о и -е.
#  - Masculine forms are affected by animate/inanimate forms
#  - Neuter with nominative sg that ends with -о и -е
#  - Masculine names ending in consonant usually have -e in vocative (vs -o)
#  - Base form doesn't change in all cases
_masc_coe = 'masculine:' + _sigma + p.union(_c, 'о', 'е') + pynutil.insert('|Group1m')
_neut_coe = 'neuter:' + _sigma + p.union('о', 'е') + pynutil.insert('|Group1n')
_masc_coe_inan = 'masculine:inanimate:' + _sigma + p.union(_c, 'о', 'е') + pynutil.insert('|Group1mi')
_masc_c_personal = 'masculine:person:' + p.union(_v_up, _c_up) + _sigma + _c + pynutil.insert('|Group1mp')

# Group 2(n,t):
#  - Neuter with nominative sg that ends with -е
#  - Base form gets expanded in all cases, except nominative and vocative sg, with consonants н, т.
_neut_en = 'neuter:srinsertn:' + _sigma + p.union('е') + pynutil.insert('|Group2n')
_neut_et = 'neuter:srinsertt:' + _sigma + p.union('е') + pynutil.insert('|Group2t')

# Group 3:
#  - All nouns with nominative sg that ends with -а.
_all_a = p.union('masculine:', 'feminine:', 'neuter:', 'masculine:person:', 'feminine:person:', 'neuter:person:') + _sigma + p.union('а') + pynutil.insert('|Group3')

# Group 4:
#  - Feminine with nominative sg that ends with consonant.
_fem_c = p.union('feminine:', 'feminine:personal:') + _sigma + _c + pynutil.insert('|Group4')

_classify = p.union(_masc_coe, _neut_coe, _masc_coe_inan, _masc_c_personal, _neut_en, _neut_et, _all_a, _fem_c).optimize()

# Load exceptions from the file.
_exceptions = p.string_file(os.path.normpath('data/sr/exceptions.tsv'))

# Generation rules for the language.
case = features.Feature('case', 'nom', 'gen', 'dat', 'acc', 'voc', 'ins', 'loc')
num = features.Feature('num', 'sg', 'pl')
noun = features.Category(case, num)
stem = paradigms.make_byte_star_except_boundary()

# Case feature vectors
# Singular
nomsg = features.FeatureVector(noun, 'case=nom', 'num=sg')
gensg = features.FeatureVector(noun, 'case=gen', 'num=sg')
datsg = features.FeatureVector(noun, 'case=dat', 'num=sg')
accsg = features.FeatureVector(noun, 'case=acc', 'num=sg')
vocsg = features.FeatureVector(noun, 'case=voc', 'num=sg')
inssg = features.FeatureVector(noun, 'case=ins', 'num=sg')
locsg = features.FeatureVector(noun, 'case=loc', 'num=sg')
# Plural
nompl = features.FeatureVector(noun, 'case=nom', 'num=pl')
genpl = features.FeatureVector(noun, 'case=gen', 'num=pl')
datpl = features.FeatureVector(noun, 'case=dat', 'num=pl')
accpl = features.FeatureVector(noun, 'case=acc', 'num=pl')
vocpl = features.FeatureVector(noun, 'case=voc', 'num=pl')
inspl = features.FeatureVector(noun, 'case=ins', 'num=pl')
locpl = features.FeatureVector(noun, 'case=loc', 'num=pl')

# Group 1m rules
_slot_coe = [
  (stem, nomsg),
  (paradigms.suffix('+а', stem), gensg),
  (paradigms.suffix('+у', stem), datsg),
  (paradigms.suffix('+а', stem), accsg),
  (paradigms.suffix('+о', stem), vocsg),
  (paradigms.suffix('+ом', stem), inssg),
  (paradigms.suffix('+у', stem), locsg),
  (paradigms.suffix('+и', stem), nompl),
  (paradigms.suffix('+а', stem), genpl),
  (paradigms.suffix('+има', stem), datpl),
  (paradigms.suffix('+е', stem), accpl),
  (paradigms.suffix('+и', stem), vocpl),
  (paradigms.suffix('+има', stem), inspl),
  (paradigms.suffix('+има', stem), locpl),
]
_masc_coe_para = paradigms.Paradigm(
  category=noun,
  name='Group 1 masculine',
  slots=_slot_coe,
  lemma_feature_vector=nomsg,
  stems=[_sigma])

# Group 1mi inanimate rules
_slot_coe_i = [
  (stem, nomsg),
  (paradigms.suffix('+а', stem), gensg),
  (paradigms.suffix('+у', stem), datsg),
  (stem, accsg),
  (paradigms.suffix('+е', stem), vocsg),
  (paradigms.suffix('+ом', stem), inssg),
  (paradigms.suffix('+у', stem), locsg),
  (paradigms.suffix('+и', stem), nompl),
  (paradigms.suffix('+а', stem), genpl),
  (paradigms.suffix('+има', stem), datpl),
  (paradigms.suffix('+е', stem), accpl),
  (paradigms.suffix('+и', stem), vocpl),
  (paradigms.suffix('+има', stem), inspl),
  (paradigms.suffix('+има', stem), locpl),
]
_masc_coei_para = paradigms.Paradigm(
  category=noun,
  name='Group 1 masculine inanimate',
  slots=_slot_coe_i,
  lemma_feature_vector=nomsg,
  stems=[_sigma])

# Group 1n rules
_slot_oe = [
  (stem, nomsg),
  (paradigms.suffix('+а', stem + pynutil.delete(p.union('о', 'е'))), gensg),
  (paradigms.suffix('+у', stem + pynutil.delete(p.union('о', 'е'))), datsg),
  (stem, accsg),
  (stem, vocsg),
  (paradigms.suffix('+ом', stem + pynutil.delete(p.union('о', 'е'))), inssg),
  (paradigms.suffix('+у', stem + pynutil.delete(p.union('о', 'е'))), locsg),
  (paradigms.suffix('+а', stem + pynutil.delete(p.union('о', 'е'))), nompl),
  (paradigms.suffix('+а', stem + pynutil.delete(p.union('о', 'е'))), genpl),
  (paradigms.suffix('+има', stem + pynutil.delete(p.union('о', 'е'))), datpl),
  (paradigms.suffix('+а', stem + pynutil.delete(p.union('о', 'е'))), accpl),
  (paradigms.suffix('+а', stem + pynutil.delete(p.union('о', 'е'))), vocpl),
  (paradigms.suffix('+има', stem + pynutil.delete(p.union('о', 'е'))), inspl),
  (paradigms.suffix('+има', stem + pynutil.delete(p.union('о', 'е'))), locpl),
]
_neut_oe_para = paradigms.Paradigm(
  category=noun,
  name='Group 1 neuter',
  slots=_slot_oe,
  lemma_feature_vector=nomsg,
  stems=[_sigma])

# Group 1mp rules (personal names)
_slot_cp = [
  (stem, nomsg),
  (paradigms.suffix('+а', stem), gensg),
  (paradigms.suffix('+у', stem), datsg),
  (paradigms.suffix('+а', stem), accsg),
  (paradigms.suffix('+е', stem), vocsg),
  (paradigms.suffix('+ом', stem), inssg),
  (paradigms.suffix('+у', stem), locsg),
  (paradigms.suffix('+и', stem), nompl),
  (paradigms.suffix('+а', stem), genpl),
  (paradigms.suffix('+има', stem), datpl),
  (paradigms.suffix('+е', stem), accpl),
  (paradigms.suffix('+и', stem), vocpl),
  (paradigms.suffix('+има', stem), inspl),
  (paradigms.suffix('+има', stem), locpl),
]
_masc_cp_para = paradigms.Paradigm(
  category=noun,
  name='Group 1mp masculine',
  slots=_slot_cp,
  lemma_feature_vector=nomsg,
  stems=[_sigma])

# Group 2n rules
_slot_neut_en = [
  (stem, nomsg),
  (paradigms.suffix('+на', stem), gensg),
  (paradigms.suffix('+ну', stem), datsg),
  (stem, accsg),
  (stem, vocsg),
  (paradigms.suffix('+ном', stem), inssg),
  (paradigms.suffix('+ну', stem), locsg),
  (paradigms.suffix('+на', stem), nompl),
  (paradigms.suffix('+на', stem), genpl),
  (paradigms.suffix('+нима', stem), datpl),
  (paradigms.suffix('+на', stem), accpl),
  (paradigms.suffix('+на', stem), vocpl),
  (paradigms.suffix('+нима', stem), inspl),
  (paradigms.suffix('+нима', stem), locpl),
]
_neut_en_para = paradigms.Paradigm(
  category=noun,
  name='Neuter group2 n',
  slots=_slot_neut_en,
  lemma_feature_vector=nomsg,
  stems=[_sigma])

_slot_neut_et = [
  (stem, nomsg),
  (paradigms.suffix('+та', stem), gensg),
  (paradigms.suffix('+ту', stem), datsg),
  (stem, accsg),
  (stem, vocsg),
  (paradigms.suffix('+том', stem), inssg),
  (paradigms.suffix('+ту', stem), locsg),
  (paradigms.suffix('+та', stem), nompl),
  (paradigms.suffix('+та', stem), genpl),
  (paradigms.suffix('+тима', stem), datpl),
  (paradigms.suffix('+та', stem), accpl),
  (paradigms.suffix('+та', stem), vocpl),
  (paradigms.suffix('+тима', stem), inspl),
  (paradigms.suffix('+тима', stem), locpl),
]
_neut_et_para = paradigms.Paradigm(
  category=noun,
  name='Neuter group2 t',
  slots=_slot_neut_et,
  lemma_feature_vector=nomsg,
  stems=[_sigma])

# Group 3 rules
_multi_3 = p.union(_v + _c.star + _v, _c.plus + _v + _c.star + _v)
_multi_personal_3 = p.union(_v_up + _c.star + _v, _c_up + _c.star + _v + _c.star + _v)
# Vocative singular special rules
_ica_multi = _multi_3 + _sigma + 'иц' + p.cross('а', 'е')
_ska_country = p.union(_c_up, _v_up) + _sigma + p.union('ска', 'чка', 'шка')
_person_3 = _multi_personal_3 + _sigma + 'а'
_all_3 = _sigma + p.cross('а', 'о')
_spec_3 = p.union(_ica_multi, _person_3, _ska_country)
_voc_3 = priority_union(_spec_3, _all_3, _sigma).optimize()
# Genitive plural special rules
_i_3 = p.cdrewrite(p.cross('а', 'и'), p.union('тњ', 'дњ', 'пт', 'лб', 'рв'), '', _sigma)
_volatile_a = p.cdrewrite(p.string_map([('јк', 'јак'), ('мљ', 'маљ'), ('вц', 'вац'),
                                        ('тк', 'так'), ('тк', 'дак'), ('пк', 'пак')]), '', 'а', _sigma)
_spec_g_3 = _i_3 @ _volatile_a
_all_g_3 = _sigma + 'a'
_gen_3 = priority_union(_spec_g_3, _all_g_3, _sigma).optimize()

_slot_a = [
  (paradigms.suffix('+а', stem + pynutil.delete('а')), nomsg),
  (paradigms.suffix('+е', stem + pynutil.delete('а')), gensg),
  (paradigms.suffix('+и', stem + pynutil.delete('а')), datsg),
  (paradigms.suffix('+у', stem + pynutil.delete('а')), accsg),
  (stem @ _voc_3, vocsg),
  (paradigms.suffix('+ом', stem + pynutil.delete('а')), inssg),
  (paradigms.suffix('+и', stem + pynutil.delete('а')), locsg),
  (paradigms.suffix('+е', stem + pynutil.delete('а')), nompl),
  (stem @ _gen_3, genpl),
  (paradigms.suffix('+ама', stem + pynutil.delete('а')), datpl),
  (paradigms.suffix('+е', stem + pynutil.delete('а')), accpl),
  (paradigms.suffix('+е', stem + pynutil.delete('а')), vocpl),
  (paradigms.suffix('+ама', stem + pynutil.delete('а')), inspl),
  (paradigms.suffix('+ама', stem + pynutil.delete('а')), locpl),
]
_a_para = paradigms.Paradigm(
  category=noun,
  name='Group 3',
  slots=_slot_a,
  lemma_feature_vector=nomsg,
  stems=[_sigma])

# Group 4 rules
_jy = _sigma + pynutil.insert('ју')
# These changes happen for consonants placed before j.
_pbmv = p.cdrewrite(p.string_map([('пј', 'пљ'), ('бј', 'бљ'), ('мј', 'мљ'), ('вј', 'вљ')]), '', 'у', _sigma)
_palatial = p.cdrewrite(p.string_map([('ђј', 'ђ'), ('ћј', 'ћ'), ('љј', 'љ'),
                                      ('њј', 'њ'), ('жј', 'ж'), ('шј', 'ш')]), '', 'у', _sigma)
_j_union = p.cdrewrite(p.string_map([('дј', 'ђ'), ('тј', 'ћ'), ('лј', 'љ'), ('нј', 'њ'), ('зј', 'ж'), ('сј', 'ш')]),
                                    '', 'у', _sigma)
# At this point we need to check if any 'с', 'з' are in front of 'ђ', 'ћ', 'љ', 'њ', 'ж', 'ш'
# and replace them with 'ш' and 'ж'.
_sz_repl = p.cdrewrite(p.string_map([('с', 'ш'), ('з', 'ж')]), '', p.union('ђ', 'ћ', 'љ', 'њ', 'ж', 'ш'), _sigma)
# Final incantation for the whole chain of changes in instrumental.
_ins = (_jy @ _pbmv @ _palatial @ _j_union @ _sz_repl).optimize()

_slot_fem_c = [
  (stem, nomsg),
  (paradigms.suffix('+и', stem), gensg),
  (paradigms.suffix('+и', stem), datsg),
  (stem, accsg),
  (paradigms.suffix('+и', stem), vocsg),
  (stem @ _ins, inssg),
  (paradigms.suffix('+и', stem), locsg),
  (paradigms.suffix('+и', stem), nompl),
  (paradigms.suffix('+и', stem), genpl),
  (paradigms.suffix('+има', stem), datpl),
  (paradigms.suffix('+и', stem), accpl),
  (paradigms.suffix('+и', stem), vocpl),
  (paradigms.suffix('+има', stem), inspl),
  (paradigms.suffix('+има', stem), locpl),
]
_fem_c_para = paradigms.Paradigm(
  category=noun,
  name='Feminine group 4',
  slots=_slot_fem_c,
  lemma_feature_vector=nomsg,
  stems=[_sigma])

def classify(singular: str, attributes: list[str]):
  """
  Returns which inflection group noun belongs to.
  We expect attributes for gender, anim/inanimate and n/t insertion.
  Attributes will be inserted before the noun, : delimited to aid classification.
  """
  reduced_attributes = [attrib for attrib in attributes if attrib in ['masculine', 'feminine', 'neuter', 'srinsertt', 'srinsertn', 'inanimate', 'person']]
  prefix = ':'.join(reduced_attributes) + ':'
  result = rewrite.one_top_rewrite(prefix + singular, _classify)
  return result.split('|')[1]


def inflect(lexicon_entry: str, noun_case: str, number: str) -> str:
  """
  Case is one of nom, gen, dat, acc, voc, ins, loc.
  Number is one of sg, pl.
  """
  (noun, attributes) = parse_lexicon_entry(lexicon_entry)
  if not noun or not attributes:
    return ''

  # Check exceptions before doing heavy work.
  try:
    stats['Exception'] += 1
    return(rewrite.one_top_rewrite(':'.join([noun, noun_case, number]), _exceptions))
  except rewrite.Error:
    stats['Exception'] -= 1
    pass

  # See which rule applies to the noun.
  group = classify(noun, attributes)

  # Map user input (case and number) to our categories.
  case_num_mapping = [('nom', 'sg', nomsg), ('nom', 'pl', nompl), ('gen', 'sg', gensg), ('gen', 'pl', genpl),
                      ('dat', 'sg', datsg), ('dat', 'pl', datpl), ('acc', 'sg', accsg), ('acc', 'pl', accpl),
                      ('voc', 'sg', vocsg), ('voc', 'pl', vocpl), ('ins', 'sg', inssg), ('ins', 'pl', inspl),
                      ('loc', 'sg', locsg), ('loc', 'pl', locpl)]
  feature_vector = [feature for ncase, num, feature in case_num_mapping if ncase == noun_case and num == number][0]
  
  # We can probably connect classify and match into one FST and avoid this match/case section.
  # But it's ok for the first iteration.
  match group:
    case 'Group1m':
      stats['Group1m'] += 1
      return(_masc_coe_para.inflect(noun, feature_vector)[0])
    case 'Group1n':
      stats['Group1n'] += 1
      return(_neut_oe_para.inflect(noun, feature_vector)[0])
    case 'Group1mi':
      stats['Group1mi'] += 1
      return(_masc_coei_para.inflect(noun, feature_vector)[0])
    case 'Group1mp':
      stats['Group1mp'] += 1
      return(_masc_cp_para.inflect(noun, feature_vector)[0])
    case 'Group2n':
      stats['Group2n'] += 1
      return(_neut_en_para.inflect(noun, feature_vector)[0])
    case 'Group2t':
      stats['Group2t'] += 1
      return(_neut_et_para.inflect(noun, feature_vector)[0])
    case 'Group3':
      stats['Group3'] += 1
      return(_a_para.inflect(noun, feature_vector)[0])
    case 'Group4':
      stats['Group4'] += 1
      return(_fem_c_para.inflect(noun, feature_vector)[0])

# Remove after generation.
import json
import sys
def main() -> int:
    """ Output data for training """
    word_list = [
      ('август: noun masculine inanimate', 'm', 'August'),
      ('авенија: noun feminine', 'f', 'Avenue'),
      ('авион: noun masculine inanimate', 'm', 'aeroplane'),
      ('Ана: noun feminine person', 'f', ''),
      ('Ангелина: noun feminine person', 'f', ''),
      ('април: noun masculine inanimate', 'm', 'April'),
      ('ауто: noun masculine inanimate', 'm', 'car'),
      ('аутобус: noun masculine inanimate', 'm', 'bus'),
      ('бара: noun feminine', 'f', 'pond'),
      ('бик: noun masculine', 'm', 'bull'),
      ('Биљана: noun feminine', 'f', ''),
      ('Бојана: noun feminine person', 'f', ''),
      ('боца: noun feminine', 'f', 'bottle'),
      ('Бранимир: noun masculine person', 'm', ''),
      ('брдо: noun neuter', 'n', 'hill'),
      ('брод: noun masculine inanimate', 'm', 'ship'),
      ('булевар: noun masculine', 'm', 'boulevard'),
      ('бурегџија: noun masculine', 'm', ''),
      ('ваздух: noun masculine inanimate', 'm', 'air'),
      ('ватра: noun feminine', 'f', 'fire'),
      ('виме: noun neuter srinsertn', 'n', 'udder'),
      ('владика: noun masculine', 'm', 'bishop'),
      ('Владимир: noun masculine person', 'm', ''),
      ('време: noun neuter srinsertn', 'n', 'time'),
      ('гадост: noun feminine', 'f', 'nastiness'),
      ('Горан: noun masculine person', 'm', ''),
      ('Гордана: noun feminine person', 'f', ''),
      ('дворац: noun masculine inanimate', 'm', 'castle'),
      ('дебло: noun neuter', 'n', 'tree trunk'),
      ('девојка: noun feminine', 'f', 'girl'),
      ('дете: noun neuter srinsertt', 'n', 'child'),
      ('децембар: noun masculine inanimate', 'm', 'December'),
      ('Драган: noun masculine person', 'm', ''),
      ('Драгана: noun feminine person', 'f', ''),
      ('Драгиша: noun masculine person', 'm', ''),
      ('дугме: noun neuter srinsertt', 'n', 'button'),
      ('дупе: noun neuter srinsertt', 'n', 'butt'),
      ('Душан: noun masculine person', 'm', ''),
      ('жена: noun feminine', 'f', 'woman'),
      ('Загорка: noun feminine person', 'f', ''),
      ('запрега: noun feminine', 'f', 'ox cart'),
      ('зграда: noun feminine', 'f', 'building'),
      ('земља: noun feminine', 'f', 'soil'),
      ('знак: noun masculine inanimate', 'm', 'sign'),
      ('Зоран: noun masculine person', 'm', ''),
      ('Зорана: noun feminine person', 'f', ''),
      ('Ивана: noun feminine person', 'f', ''),
      ('име: noun neuter srinsertn', 'n', 'name'),
      ('Италија: noun feminine', 'f', 'Italy'),
      ('јануар: noun masculine inanimate', 'm', 'January'),
      ('јарац: noun masculine', 'm', 'billy goat'),
      ('једрилица: noun feminine', 'f', 'sailboat'),
      ('једро: noun neuter', 'n', 'sail'),
      ('језеро: noun neuter', 'n', 'lake'),
      ('Јелена: noun feminine', 'f', ''),
      ('јул: noun masculine inanimate', 'm', 'July'),
      ('јун: noun masculine inanimate', 'm', 'June'),
      ('јунак: noun masculine', 'm', 'heroe'),
      ('камен: noun masculine inanimate', 'm', 'rock'),
      ('камион: noun masculine inanimate', 'm', 'truck'),
      ('кафана: noun feminine', 'f', 'restaurant'),
      ('Кина: noun feminine', 'f', 'China'),
      ('киша: noun feminine', 'f', 'rain'),
      ('коза: noun feminine', 'f', 'goat'),
      ('коноба: noun feminine', 'f', 'inn'),
      ('крчма: noun feminine', 'f', 'pub'),
      ('кћи: noun feminine', 'f', 'doughter'),
      ('кућа: noun feminine', 'f', 'house'),
      ('љубав: noun feminine', 'f', 'love'),
      ('Љубица: noun feminine person', 'f', ''),
      ('Љубиша: noun masculine person', 'm', ''),
      ('мај: noun masculine inanimate', 'm', 'May'),
      ('Маја: noun feminine person', 'f', ''),
      ('мама: noun feminine', 'f', 'mom'),
      ('март: noun masculine inanimate', 'm', 'March'),
      ('мати: noun feminine', 'f', 'mommy'),
      ('Милена: noun feminine person', 'f', ''),
      ('Милица: noun feminine person', 'f', ''),
      ('Милка: noun feminine person', 'f', ''),
      ('Милош: noun masculine person', 'm', ''),
      ('Миљан: noun masculine person', 'm', ''),
      ('младост: noun feminine', 'f', 'youth'),
      ('море: noun neuter', 'n', 'sea'),
      ('мост: noun masculine inanimate', 'm', 'bridge'),
      ('Нада: noun feminine person', 'f', ''),
      ('надвожњак: noun masculine inanimate', 'm', 'overpass'),
      ('Небојша: noun masculine person', 'm', ''),
      ('недеља: noun feminine', 'f', 'Sunday'),
      ('новембар: noun masculine inanimate', 'm', 'November'),
      ('нога: noun feminine', 'f', 'leg'),
      ('обала: noun feminine', 'f', 'shore'),
      ('облак: noun masculine inanimate', 'm', 'cloud'),
      ('ован: noun masculine', 'm', 'ram'),
      ('овца: noun feminine', 'f', 'sheet'),
      ('октобар: noun masculine inanimate', 'm', 'October'),
      ('памет: noun feminine', 'f', 'intelligence'),
      ('парк: noun masculine inanimate', 'm', 'park'),
      ('петак: noun masculine inanimate', 'm', 'Friday'),
      ('пећ: noun feminine', 'f', 'furnace'),
      ('планина: noun feminine', 'f', 'mountain'),
      ('племе: noun neuter srinsertn', 'n', 'tribe'),
      ('пожар: noun masculine inanimate', 'm', 'wildfire'),
      ('понедељак: noun masculine inanimate', 'm', 'Monday'),
      ('поплава: noun feminine', 'f', 'flood'),
      ('поток: noun masculine inanimate', 'm', 'creek'),
      ('правац: noun masculine inanimate', 'm', 'direction'),
      ('превоз: noun masculine inanimate', 'm', 'transport'),
      ('претња: noun feminine', 'f', 'threat'),
      ('пропланак: noun masculine inanimate', 'm', 'glade'),
      ('пумпа: noun feminine', 'f', 'pump'),
      ('путоказ: noun masculine inanimate', 'm', 'signpost'),
      ('радост: noun feminine', 'f', 'happiness'),
      ('раме: noun neuter srinsertn', 'n', 'shoulder'),
      ('река: noun feminine', 'f', 'river'),
      ('ресторан: noun masculine inanimate', 'm', 'restaurant'),
      ('рука: noun feminine', 'f', 'hand'),
      ('салаш: noun feminine', 'f', 'farm'),
      ('септембар: noun masculine inanimate', 'm', 'September'),
      ('Синиша: noun masculine person', 'm', ''),
      ('скретање: noun neuter', 'n', 'turn'),
      ('слуга: noun masculine', 'm', 'servant'),
      ('смер: noun masculine inanimate', 'm', 'direction'),
      ('Соња: noun feminine person', 'f', ''),
      ('среда: noun feminine', 'f', 'Wednesday'),
      ('старост: noun feminine', 'f', 'old age'),
      ('ствар: noun feminine', 'f', 'thing'),
      ('Стева: noun masculine person', 'm', ''),
      ('Стеван: noun masculine person', 'm', ''),
      ('стена: noun feminine', 'f', 'boulder'),
      ('Стојан: noun masculine person', 'm', ''),
      ('субота: noun feminine', 'f', 'Saturday'),
      ('судија: noun masculine', 'm', 'judge'),
      ('Тамара: noun feminine person', 'f', ''),
      ('Тања: noun feminine person', 'f', ''),
      ('ташта: noun feminine', 'f', 'mother in law'),
      ('теме: noun neuter srinsertn', 'n', 'back of the head'),
      ('Тијана: noun feminine person', 'f', ''),
      ('Угљеша: noun masculine person', 'm', ''),
      ('уже: noun neuter srinsertt', 'n', 'rope'),
      ('улица: noun feminine', 'f', 'street'),
      ('уторак: noun masculine inanimate', 'm', 'Tuesday'),
      ('учитељица: noun feminine', 'f', 'teacher'),
      ('фебруар: noun masculine inanimate', 'm', 'February'),
      ('Француска: noun feminine', 'f', 'France'),
      ('херој: noun masculine', 'm', 'heroe'),
      ('хотел: noun masculine inanimate', 'm', 'hotel'),
      ('храм: noun masculine inanimate', 'm', 'temple'),
      ('црква: noun feminine', 'f', 'church'),
      ('чађ: noun feminine', 'f', 'tar'),
      ('чамац: noun masculine inanimate', 'm', 'boat'),
      ('четвртак: noun masculine inanimate', 'm', 'Thursday'),
      ('шума: noun feminine', 'f', 'forest'),
    ]
    with open("lexicon_data.json", "w", encoding='utf8') as file: 
      print("Processing total # words:", len(word_list))
      for word, gender, english in word_list:
        # Build JSON representation for all forms
        noun = {}
        noun['lemma'] = word.split(':')[0]
        noun['category'] = 'noun'
        noun['language'] = 'sr'
        noun['gender'] = gender
        noun['English'] = english
        if 'person' in word:
          noun['person'] = True
        if 'inanimate' in word:
          noun['inanimate'] = True
        for case, num in [('nom', 'sg'), ('gen', 'sg'), ('dat', 'sg'), ('acc', 'sg'), ('voc', 'sg'), ('ins', 'sg'), ('loc', 'sg'),
                          ('nom', 'pl'), ('gen', 'pl'), ('dat', 'pl'), ('acc', 'pl'), ('voc', 'pl'), ('ins', 'pl'), ('loc', 'pl')]:
          result = inflect(word, case, num)
          noun[case + ':' + num] = { 'noun': result, 'case': case, 'number': num}
        file.write(json.dumps(noun, ensure_ascii=False) + '\n')
      
      for key, value in stats.items():
        # 14 cases per class, reduce to 1.
        stats[key] = int(value / 14)
      print(stats)

    return 0

if __name__ == '__main__':
    sys.exit(main())