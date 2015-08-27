RegisterPointSets {#registerpointsets}
=============

## Group (Subgroup) ##
DataFusion (DataFusion)

## Description ##
Given two sets of matched points (e.g. centroids for 2 segmentations with matching feature ids) this filter computes the transformation (from 'moving' to 'reference' points) resulting in the least squares error. If **Use Good Points Arrays** is selected, only points flagged as good in both the moving and reference point set will be considered. If **Weight Pairs** is selected the transformation resulting in the weighted least squares error will be computed. The point sets centered to their centroid prior to calculation of the transformation. For full affine degrees of freedom the transformation is computed directly. For restricted degrees of freedom the least squares rotation is computed first (if allowed) using singular value decomposition. Next the least squares scaling for the computed rotation is found. The resulting transform is stored as a 4x4 augmented matrix.

## Parameters ##
| Name             | Type |
|------------------|------|
| Allow Translation | Boolean |
| Allow Rotation | Boolean |
| Allow Scaling | Boolean |
| Scaling Type | Choice |
| Allow Shearing | Boolean |
| Use Good Points | Boolean |
| Weight Pairs | Boolean |

## Required Arrays ##

| Type | Default Array Name | Description |
|------|--------------------|-------------|
| Float  | Reference Points | x, y, z coordinate of each point |
| Float  | Moving Points | x, y, z coordinate of each point |
| Boolean  | Reference Good Points | flag for each point |
| Boolean  | Moving Good Points | flag for each point |
| Float  | Weights | weighting for each point pair |


## Created Arrays ##

| Type | Default Array Name | Description |
|------|--------------------|-------------|
| Float  | Transformation  | 4x4 augmented matrix |

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users