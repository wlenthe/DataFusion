Register Point Sets {#registerpointsets}
=====

## Group (Subgroup) ##
DatasetMerging (DatasetMerging)


## Description ##
Given two sets of matched points (e.g. centroids for 2 segmentations with matching feature ids) this filter computes the transformation (from 'moving' to 'reference' points) resulting in the least squares error. If **Use Good Points Arrays** is selected, only points flagged as good in both the moving and reference point set will be considered. If **Weight Pairs** is selected the transformation resulting in the weighted least squares error will be computed. The point sets centered to their centroid prior to calculation of the transformation. For full affine degrees of freedom the transformation is computed directly. For restricted degrees of freedom rotation is computed first (if allowed) then least squares scaling for the computed rotation. The resulting transform is stored as a 4x4 augmented matrix.

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

| Type | Default Array Name | Description | Comment |
|------|--------------------|-------------|---------|
| Int  | SomeName           | ....        | other   |



## Authors: ##

**Copyright:** Who holds the copyright for the code

**Contact Info:** email address

**Version:** x.y.z

**License:**  What is the license for this code


