Match Feature Ids {#renumberfeatures}
=============

## Group (Subgroup) ##
DataFusion (DataFusion)

## Description ##
Renumbers **Feature Ids** such that the values in **Scalar Array** are in Ascending/Descending **Order**. **Feature Ids** will be updated and all arrays in the feature attribute matrix containing **Scalar Array** reordered appropriately. For example if *Volumes* is selected for **Scalar Array** and *Descending* for **Order**, feature 1 will be the largest, feature 2 the second largest, etc. (feature 0 is always unchanged). 

## Parameters ##
| Type | Name             | Description |
|---|------------------|------|
| Choice | Order | sort direction |

## Required Arrays ##
| Type | Default Array Name | Description |
|------|--------------------|-------------|
| Int | Feature Ids | features to renumber |
| Any (Scalar) | Scalar Array | value to renumber by |

## Created Arrays ##
None

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users