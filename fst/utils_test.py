import unittest
import utils

class TestClassification(unittest.TestCase):

    def test_parse_lexicon_entry(self):
        self.assertEqual(('word', ['attr', 'attr']), utils.parse_lexicon_entry('word: attr attr'))
        self.assertEqual(('word', ['attr', 'attr']), utils.parse_lexicon_entry(' word :   attr    attr  '))
        # Edge cases
        self.assertEqual((None, None), utils.parse_lexicon_entry(''))
        self.assertEqual(('word', None), utils.parse_lexicon_entry('word'))
        self.assertEqual(('word', None), utils.parse_lexicon_entry('word:'))
        self.assertEqual((None, None), utils.parse_lexicon_entry(':'))
        self.assertEqual((None, None), utils.parse_lexicon_entry(': attr attr'))
        # Complicated word, with spaces and punctuation, but not an error.
        self.assertEqual(('word; attr attr', None), utils.parse_lexicon_entry('word; attr attr'))

if __name__ == '__main__':
    unittest.main()