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

_v = p.union('а', 'е', 'и', 'о', 'у', 'А', 'Е', 'И', 'О', 'У')
_c = p.union('б', 'в', 'г', 'д', 'ђ', 'ж', 'з', 'ј', 'к', 'л', 'љ', 'м',
             'н', 'њ', 'п', 'р', 'с', 'т', 'ћ', 'ф', 'х', 'ц', 'ч', 'џ', 'ш',
             'Б', 'В', 'Г', 'Д', 'Ђ', 'Ж', 'З', 'Ј', 'К', 'Л', 'Љ', 'М',
             'Н', 'Њ', 'П', 'Р', 'С', 'Т', 'Ћ', 'Ф', 'Х', 'Ц', 'Ч', 'Џ', 'Ш')
_sigma = p.union(_v, _c).closure().optimize()

# Noun group rules for classification.
# Match pattern and append group name to the end.

# Group 1:
#  - Masculine with nominative sg that ends with consonant, -о и -е.
#  - Masculine forms are affected by animate/inanimate forms
#  - Neuter with nominative sg that ends with -о и -е
#  - Base form doesn't change in all cases
_masc_coe = 'masculine:' + _sigma + p.union(_c, 'о', 'е') + pynutil.insert('|Group1m')
_neut_coe = 'neuter:' + _sigma + p.union('о', 'е') + pynutil.insert('|Group1n')
_masc_coe_inan = 'masculine:inanimate:' + _sigma + p.union(_c, 'о', 'е') + pynutil.insert('|Group1mi')

# Group 2(n,t):
#  - Neuter with nominative sg that ends with -е
#  - Base form gets expanded in all cases, except nominative and vocative sg, with consonants н, т.
_neut_en = 'neuter:srinsertn:' + _sigma + p.union('е') + pynutil.insert('|Group2n')
_neut_et = 'neuter:srinsertt:' + _sigma + p.union('е') + pynutil.insert('|Group2t')

# Group 3:
#  - All nouns with nominative sg that ends with -а.
_all_a = p.union('masculine:', 'feminine:', 'neuter:') + _sigma + p.union('а') + pynutil.insert('|Group3')

# Group 4:
#  - Feminine with nominative sg that ends with consonant.
_fem_c = 'feminine:' + _sigma + _c + pynutil.insert('|Group4')

_classify = p.union(_masc_coe, _neut_coe, _masc_coe_inan, _neut_en, _neut_et, _all_a, _fem_c).optimize()

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
_slot_a = [
  (paradigms.suffix('+а', stem + pynutil.delete('а')), nomsg),
  (paradigms.suffix('+е', stem + pynutil.delete('а')), gensg),
  (paradigms.suffix('+и', stem + pynutil.delete('а')), datsg),
  (paradigms.suffix('+у', stem + pynutil.delete('а')), accsg),
  (paradigms.suffix('+о', stem + pynutil.delete('а')), vocsg),
  (paradigms.suffix('+ом', stem + pynutil.delete('а')), inssg),
  (paradigms.suffix('+и', stem + pynutil.delete('а')), locsg),
  (paradigms.suffix('+е', stem + pynutil.delete('а')), nompl),
  (paradigms.suffix('+а', stem + pynutil.delete('а')), genpl),
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
  reduced_attributes = [attrib for attrib in attributes if attrib in ['masculine', 'feminine', 'neuter', 'srinsertt', 'srinsertn', 'inanimate']]
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
    return(rewrite.one_top_rewrite(':'.join([noun, noun_case, number]), _exceptions))
  except rewrite.Error:
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
      return(_masc_coe_para.inflect(noun, feature_vector)[0])
    case 'Group1n':
      return(_neut_oe_para.inflect(noun, feature_vector)[0])
    case 'Group1mi':
      return(_masc_coei_para.inflect(noun, feature_vector)[0])
    case 'Group2n':
      return(_neut_en_para.inflect(noun, feature_vector)[0])
    case 'Group2t':
      return(_neut_et_para.inflect(noun, feature_vector)[0])
    case 'Group3':
      return(_a_para.inflect(noun, feature_vector)[0])
    case 'Group4':
      return(_fem_c_para.inflect(noun, feature_vector)[0])

# Remove after generation.
import sys
def main() -> int:
    """ Output data for training """
    word_list = [
      ('име: noun neuter srinsertn', 'n'),
      ('дугме: noun neuter srinsertt', 'n'),
      ('ствар: noun feminine', 'f'),
      ('пећ: noun feminine', 'f'),
      ('љубав: noun feminine', 'f'),
      ('младост: noun feminine', 'f'),
      ('чађ: noun feminine', 'f'),
      ('памет: noun feminine', 'f'),
      ('кћи: noun feminine', 'f'),
      ('мати: noun feminine', 'f'),
      ('судија: noun masculine', 'm'),
      ('владика: noun masculine', 'm'),
      ('бурегџија: noun masculine', 'm'),
      ('жена: noun feminine', 'f'),
      ('учитељица: noun feminine', 'f'),
      ('Небојша: noun masculine', 'm'),
      ('Француска: noun feminine', 'f'),
      ('Италија: noun feminine', 'f'),
      ('Кина: noun feminine', 'f'),
      ('претња: noun feminine', 'f'),
      ('девојка: noun feminine', 'f'),
      ('земља: noun feminine', 'f'),
      ('овца: noun feminine', 'f'),
      ('боца: noun feminine', 'f'),
      ('коза: noun feminine', 'f'),
      ('ташта: noun feminine', 'f'),
      ('недеља: noun feminine', 'f'),
    ]
    with open("training_data.tsv", "w") as file: 
      for word, gender in word_list:
        for case, num, enc in [('nom', 'sg', '11'), ('gen', 'sg', '21'), ('dat', 'sg', '31'), ('acc', 'sg', '41'), ('voc', 'sg', '51'), ('ins', 'sg', '61'), ('loc', 'sg', '71'),
                               ('nom', 'pl', '12'), ('gen', 'pl', '22'), ('dat', 'pl', '32'), ('acc', 'pl', '42'), ('voc', 'pl', '52'), ('ins', 'pl', '62'), ('loc', 'pl', '72')]:
          stem = inflect(word, 'nom', 'sg')
          result = inflect(word, case, num)
          file.write(stem + gender + enc + '\t' + result + '\n')

    return 0

if __name__ == '__main__':
    sys.exit(main())