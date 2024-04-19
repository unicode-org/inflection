from inflection import inflect
import unittest


class TestClassification(unittest.TestCase):

    def test_exceptions(self):
        self.assertEqual('busses', inflect('bus'))
        self.assertEqual('fezzes', inflect('fez'))
        self.assertEqual('wives', inflect('wife'))
        self.assertEqual('wolves', inflect('wolf'))    
        self.assertEqual('photos', inflect('photo'))
        self.assertEqual('pianos', inflect('piano'))
        self.assertEqual('halos', inflect('halo'))
        self.assertEqual('feet', inflect('foot'))
        self.assertEqual('pence', inflect('penny'))
        self.assertEqual('sheep', inflect('sheep'))
        self.assertEqual('series', inflect('series'))
        self.assertEqual('species', inflect('species'))
        self.assertEqual('deer', inflect('deer'))
        self.assertEqual('children', inflect('child'))
        self.assertEqual('geese', inflect('goose'))
        self.assertEqual('men', inflect('man'))
        self.assertEqual('women', inflect('woman'))
        self.assertEqual('teeth', inflect('tooth'))
        self.assertEqual('mice', inflect('mouse'))
        self.assertEqual('people', inflect('person'))

    def test_s(self):
        self.assertEqual('cats', inflect('cat'))
        self.assertEqual('houses', inflect('house'))
        self.assertEqual('roofs', inflect('roof'))
    
    def test_ies(self):
        self.assertEqual('cities', inflect('city'))
        self.assertEqual('puppies', inflect('puppy'))

    def test_i(self):
        self.assertEqual('cacti', inflect('cactus'))
        self.assertEqual('foci', inflect('focus'))

    def test_a(self):
        self.assertEqual('phenomena', inflect('phenomenon'))
        self.assertEqual('criteria', inflect('criterion'))

    def test_es(self):
        self.assertEqual('irises', inflect('iris'))
        self.assertEqual('trusses', inflect('truss'))
        self.assertEqual('marshes', inflect('marsh'))
        self.assertEqual('lunches', inflect('lunch'))
        self.assertEqual('taxes', inflect('tax'))
        self.assertEqual('blitzes', inflect('blitz'))
        self.assertEqual('potatoes', inflect('potato'))

if __name__ == '__main__':
    unittest.main()