"""
Inflects English nouns - creates a plural form from singular.

Run from project root folder.

Install Pynini package before running.
"""

import os
import pynini as p

from pynini.lib import pynutil
from pynini.lib import rewrite
from utils import priority_union

_v = p.union('a', 'e', 'i', 'o', 'u', 'A', 'E', 'I', 'O', 'U')
_c = p.union('b', 'c', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm', 'n',
             'p', 'q', 'r', 's', 't', 'v', 'w', 'x', 'y', 'z',
             'B', 'C', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N',
             'P', 'Q', 'R', 'S', 'T', 'V', 'W', 'X', 'Y', 'Z')
_sigma = p.union(_v, _c).closure().optimize()

# Load exceptions from the file.
_exceptions = p.string_file(os.path.normpath('data/en/exceptions.tsv'))

# If a singular noun ends in -y and the letter before the -y is a consonant, change the ending to -ies.
# city - cities
# puppy - puppies
_ies = _sigma + _c + p.cross('y', 'ies')

# If the singular noun ends in -s, -ss, -sh, -ch, -x or -z, you usually add -es to the end.
# truss - trusses
# marsh - marshes
# lunch - lunches
# tax - taxes
# blitz - blitzes

# Exceptions:
# iris - irises
# bus - busses
# fez - fezzes
_es = _sigma + p.union('ss', 'sh', 'ch', 'x', 'z') + pynutil.insert('es')

# To make regular nouns plural, add -s to the end.
# cat - cats
# house - houses
_s = _sigma + pynutil.insert("s")

# Exclusions of rules are done through priority unions.
# Order is important, as _s rule should come in the end (catch all).
# _exceptions->(_ies, _a, _i)->_es->s
_plural = priority_union(
   _exceptions, priority_union(_ies, priority_union(_es, _s, _sigma),
                                _sigma), _sigma).optimize()

def inflect(singular: str) -> str:
  return rewrite.one_top_rewrite(singular, _plural)
