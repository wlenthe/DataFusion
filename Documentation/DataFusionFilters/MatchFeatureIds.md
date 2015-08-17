Match Feature Ids {#matchfeatureids}
=============

## Group (Subgroup) ##
DataFusion (DataFusion)

## Description ##
Given two sets of feature ids for the same volume, this filter attempts to renumber one set of features so it corresponds to the other, as shown in the following table:
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
Feature IDs in the reference set for which a match cannot be found in the moving set are skipped. Feature IDs in the moving set for which a match cannot be found will have a higher feature ID than the highest ID in the reference set. Corresponding features are identified by computing a similarity metric for every pair and then performing a greedy match. Pairs with a value falling below the specified threshold will not be matched. An orientation tolerance can be additionally specified. The following similarity metrics are available:
| Metric Name | Dividend | Divisor |
|----|---|---|
| Jaccard | intersection of A and B | union of A and B |
| Sorensen-Dice | 2 * intersection of A and B | volume of A + volume of B |
| Ochiai (Cosine) |intersection of A and B | sqrt(volume of A * volume of B) |

## Parameters ##
| Type | Name             | Description |
|---|------------------|------|
| Choice | Similarity Coefficient | metric to match with |
| Float | Minimum Metric Value | minimum value to consider when matching pairs of features |
| Boolean | Require Orientation Match | if selected only features having orientations within the specified tolerance will be matched |
| Float | Orientation Tolerance Angle | maximum misorientation angle to consider when matching pairs of features |

## Required Arrays ##

| Type | Default Array Name | Description |
|------|--------------------|-------------|
| Int | Reference Feature Ids | 'target' feature ids |
| Int | Moving Feature Ids | features ids to match |
| Float  | Reference Feature Average Quats | 4x float describing feature orientation as quaternion |
| Int  | Reference Feature Phases |  |
| Int  | Reference Feature Crystal Structures |  |
| Float  | Moving Feature Average Quats | 4x float describing feature orientation as quaternion |
| Int  | Moving Feature Phases |  |
| Int  | Moving Feature Crystal Structures |  |

## Created Arrays ##

| Type | Default Array Name | Description |
|------|--------------------|-------------|
| Boolean | UniqueFeatures | flag for unmatched reference features |
| Boolean | UniqueFeatures | flag for unmatched moving features |
| Float | SimilarityMetric | similarity metric for matched pairs (0 for unmatched pairs) |

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users