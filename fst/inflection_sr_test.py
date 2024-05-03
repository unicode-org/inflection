from inflection_sr import inflect, classify
import unittest


class TestClassification(unittest.TestCase):

    def test_categorize_group1(self):
        self.assertEqual('Group1', classify('Марко', ['noun', 'masculine']))
        self.assertEqual('Group1', classify('камен', ['noun', 'masculine']))
        self.assertEqual('Group1', classify('Павле', ['noun', 'masculine']))
        self.assertEqual('Group1', classify('село', ['noun', 'neuter']))
        self.assertEqual('Group1', classify('поље', ['noun', 'neuter']))

    def test_categorize_group2(self):
        self.assertEqual('Group2n', classify('име', ['noun', 'neuter', 'srinsertn']))
        self.assertEqual('Group2t', classify('дете', ['noun', 'neuter', 'srinsertt']))

    def test_categorize_group3(self):
        self.assertEqual('Group3', classify('жена', ['noun', 'feminine']))
        self.assertEqual('Group3', classify('судија', ['noun', 'masculine']))
        self.assertEqual('Group3', classify('доба', ['noun', 'neuter']))

    def test_categorize_group4(self):
        self.assertEqual('Group4', classify('ствар', ['noun', 'feminine']))

    def test_inflect_group2(self):
        self.assertEqual('име', inflect('име: noun neuter srinsertn', 'nom', 'sg'))
        self.assertEqual('имена', inflect('име: noun neuter srinsertn', 'gen', 'sg'))
        self.assertEqual('именом', inflect('име: noun neuter srinsertn', 'ins', 'sg'))
        self.assertEqual('именима', inflect('име: noun neuter srinsertn', 'loc', 'pl'))
        self.assertEqual('дугме', inflect('дугме: noun neuter srinsertt', 'nom', 'sg'))
        self.assertEqual('дугметима', inflect('дугме: noun neuter srinsertt', 'dat', 'pl'))

if __name__ == '__main__':
    unittest.main()