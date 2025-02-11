# Adding data to a lexicon

When there is insufficient lexical data from Wikidata for a specific language, you may need to generate and contribute significant quantities of lexical data to Wikidata. This will ensure that the data is maintained, and others can reuse your results. After you have reviewed such data for correctness, and contributed it to Wikidata, the [dictionary-parser](../inflection/tools/dictionary-parser/README.md) can be used to extract the data and use for this project.

If only a few words need to be added or modified, it's recommended that you contribute those changes directly through Wikidata instead of through the tools mentioned here.

## Adding data directly
Data needs to be formatted properly before addition. See [here](#preparing-data) for details.

After properly formatting the data it's simple to run the local dictionary parser tool to build a lexicon (see above).

It is possible to mix multiple files in the input, e.g. wikidata input and local file to expand the local lexicon - just list all of them in the command line.

This approach is useful for local testing before submitting new data to Wikidata.

## Adding data to Wikidata
The main benefit of adding data to Wikidata is expanding lexicon for all users of the language, unrelated to the Inflection project.

There are tools that can help with bulk upload, e.g. [tfsl for Python](https://phabricator.wikimedia.org/source/tool-twofivesixlex/), [Wikidata toolkit](https://www.mediawiki.org/wiki/Wikidata_Toolkit) and [WikibaseIntegrator for Python](https://www.wikidata.org/wiki/Wikidata:Tools/WikibaseIntegrator). A [sample upload script](../data/tools/wikidata_upload.py) is a good example of using tfsl library.

Lexical info can also be entered [directly](https://www.wikidata.org/wiki/Special:NewLexeme) using Wikidata interface, but that's recommended only for a small amount of data.

Depending on the size and rate of upload, you may need to create a Bot account with Wikidata.

Once you upload/enter data, it may take a few days for it to show up in the regular dumps - depending on review requirements.

### Preparing data

Wikidata and directory-parser expect data in a specific format. A minimal set of JSON keys to add a noun is proveded [in this file](../data/tools/wikidata_sample.json). Full file specification is available at [Wikidata](https://www.wikidata.org/wiki/Wikidata:Lexicographical_data/Documentation).

Category values, e.g. Q1084, are defined in [Grammar file](../inflection/tools/dictionary-parser/src/main/java/org/unicode/wikidata/Grammar.java).
