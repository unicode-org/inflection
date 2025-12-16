## How to update ICU version

ICU team releases new version twice a year, around April and Novemeber.
We have a strong dependancy on ICU and should keep project in sync with that release cycle.

### Install new version of ICU
Follow [ICU instructions](https://unicode-org.github.io/icu/download/) on how to download and install a new version.

### Test the project
Test your update locally and fix any problems - most likely to happen due to CLDR changes.
If there are errors, you'll need to update the CI actions before submitting.

### Update Actions on GitHub
Edit all yml files under .github/workflows that contain:

```
env:
  ICU_MAJOR: 'xx'
  ICU_MINOR: 'y'
```
and update major and minor ICU versions, e.g. 78.2.

### Clear the GitHub Cache if necessary
Go to [Caches](https://github.com/unicode-org/inflection/actions/caches) page, and clear all ICU related caches on the right.
