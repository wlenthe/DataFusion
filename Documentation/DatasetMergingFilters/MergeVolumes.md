Merge Volumes {#mergevolumes}
=====

## Group (Subgroup) ##
DatasetMerging (DatasetMerging)

## Description ##
This filter merges two 3D attribute matrices into one using the given affine transform (describing the desired transformation from moving to reference).  An array is created in the **Reference Cell Atribute Matrix** for each array in the **Moving Cell Attribute Matrix** (named according to the selected **Prefix**). To fill the new arrays each _cell_ in the **Reference Cell Atribute Matrix** is mapped to a _cell_ in the **Moving Cell Attribute Matrix** (or zero filled where there is no overlap). If the **Reference Cell Atribute Matrix** and **Moving Cell Attribute Matrix** belong to different _Data Containers_ all other _Attribute Matricies_ belonging to the same _Data Container_ as the **Moving Cell Attribute Matrix** will be copied into the **Reference Cell Atribute Matrix**'s _Data Containers_ (named according to the selected **Prefix**).

## Parameters ##
| Name             | Type |
|------------------|------|
| Reference Cell Attribute Matrix | String |
| Moving Cell Attribute Matrix | String |
| Affine Transform | 4x4 matrix |
| Array Prefix | String |

## Required Arrays ##
none

## Created Arrays ##
Use dependent (see Description above).


## Authors: ##

**Copyright:** Who holds the copyright for the code

**Contact Info:** email address

**Version:** x.y.z

**License:**  What is the license for this code


