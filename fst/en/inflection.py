"""
Inflects English nouns - creates a plural form from singular.

Install Pynini package before running.
"""

import pynini as p

from pynini.lib import pynutil
from pynini.lib import rewrite

def _priority_union(q: p.Fst, r: p.Fst, sigma: p.Fst) -> p.Fst:
  complement_domain_q = sigma - p.project(q, 'input')
  return p.union(q, complement_domain_q @ r)

_v = p.union('a', 'e', 'i', 'o', 'u')
_c = p.union('b', 'c', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm', 'n',
             'p', 'q', 'r', 's', 't', 'v', 'w', 'x', 'y', 'z')
_sigma = p.union(_v, _c).closure().optimize()

_exceptions = p.string_map([
    # Zero plurals.
    'deer',
    'fish',
    'sheep',
    'series',
    'species',
    # Stem changes.
    ('child', 'children'),
    ('goose', 'geese'),
    ('man', 'men'),
    ('woman', 'women'),
    ('tooth', 'teeth'),
    ('foot', 'feet'),
    ('mouse', 'mice'),
    ('person', 'people'),
    ('penny', 'pence'),
    # Irregular suffixes.
    ('child', 'children'),
    ('ox', 'oxen'),
    # f -> v.
    ('wife', 'wives'),
    ('wolf', 'wolves'),
    # A few Greek and Latin plurals.
    ('analysis', 'analyses'),
    ('ellipsis', 'ellipses'),
    # More exceptions
    ('bus', 'busses'),
    ('fez', 'fezzes'),
    ('photo', 'photos'),
    ('piano', 'pianos'),
    ('halo', 'halos'),
])

# If a singular noun ends in -y and the letter before the -y is a consonant, change the ending to -ies.
# city - cities
# puppy - puppies
_ies = _sigma + _c + p.cross('y', 'ies')

# If the singular noun ends in -on, the plural ending is usually -a.
# phenomenon - phenomena
# criterion - criteria
_a = _sigma + p.cross('on', 'a')

# If the singular noun ends in -us, the plural ending is frequently -i.
# cactus - cacti
# focus - foci
_i = _sigma + p.cross('us', 'i')

# These three can act in parallel, no conflicts.
_ies_a_i = p.union(_ies, _a, _i)

# If the singular noun ends in -s, -ss, -sh, -ch, -x, -o or -z, you usually add -es to the end.
# iris - irises
# truss - trusses
# marsh - marshes
# lunch - lunches
# tax - taxes
# blitz - blitzes
# potato - potatoes

# Exceptions:
# bus - busses
# fez - fezzes
_es = _sigma + p.union('s', 'ss', 'sh', 'ch', 'o', 'x', 'z') + pynutil.insert('es')

# To make regular nouns plural, add -s to the end.
# cat - cats
# house - houses
_s = _sigma + pynutil.insert("s")

# Exclusions of rules are done through priority unions.
# Order is important, as _s rule should come in the end (catch all).
# _exceptions->(_ies, _a, _i)->_es->s
_plural = _priority_union(
   _exceptions, _priority_union(_ies_a_i, _priority_union(_es, _s, _sigma),
                                _sigma), _sigma).optimize()

def inflect(singular: str) -> str:
  return rewrite.one_top_rewrite(singular, _plural)
