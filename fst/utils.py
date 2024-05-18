"""
Helper functions
"""

import pynini as p

def parse_lexicon_entry(lexicon_entry: str) -> tuple[str, list[str]]:
    """
    Parses the lexicon entry in form of word: attr, attr(, attr)*
    into word: str and [attr,attr,...]: list[str]
    """
    none_tuple = (None, None)

    lexicon_entry = lexicon_entry.strip()
    if not lexicon_entry:
        return none_tuple
    
    word_attributes = lexicon_entry.split(':')
    # If string is not empty split will return at least one element.
    word = word_attributes[0].strip()
    if not word:
        return none_tuple

    if len(word_attributes) == 1:
        return (word, None)
    
    attributes_list = word_attributes[1].strip().split(' ')
    result = []
    for attribute in attributes_list:
        attribute = attribute.strip()
        if attribute:
            result.append(attribute)

    if len(result):
        return (word, result)
    else:
        return (word, None)

def priority_union(q: p.Fst, r: p.Fst, sigma: p.Fst) -> p.Fst:
  """
  If q matches the input, don't execute r. Otherwise do r.
  """
  complement_domain_q = sigma - p.project(q, 'input')
  return p.union(q, complement_domain_q @ r)
