Match Feature Ids (Manual) {#matchfeatureidsmanual}
=============

## Group (Subgroup) ##
DataFusion (DataFusion)

## Description ##
Given the correspondence between two sets of feature ids renumbers one set of features to match the other:
| Reference Feature ID |Moving Feature ID (before filter) | Moving Feature ID (after filter) |
|----|---|---|
| 1 | - | - |
| 2 | 6 | 2 |
| 3 | - | - |
| 4 | 1 | 4 |
| 5 | 4 | 5 |
| - | 2 | 6 |
| - | 3 | 7 |
| - | 5 | 8 |
Unpaired feature IDs in the reference set are skipped. Unpaired feature IDs in the moving set are assigned a higher feature ID than the highest ID in the reference set. 

## Parameters ##
| Type | Name             | Description |
|---|------------------|------|
| Table | Pairs | Pairs of grains to match |

## Required Arrays ##

| Type | Default Array Name | Description |
|------|--------------------|-------------|
| Int | Reference Feature Ids | 'target' feature ids |
| Int | Moving Feature Ids | features ids to match |

## Created Arrays ##

| Type | Default Array Name | Description |
|------|--------------------|-------------|
| Boolean | UniqueFeatures | flag for unmatched reference features |
| Boolean | UniqueFeatures | flag for unmatched moving features |

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users