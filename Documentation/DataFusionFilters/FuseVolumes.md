FuseVolumes {#fusevolumes}
=============

## Group (Subgroup) ##
DataFusion (DataFusion)

## Description ##
This filter fuses two 3D attribute matrices into one using the given affine transform (describing the desired transformation from moving to reference). An array is created in the **Reference Atribute Matrix** for each array in the **Moving Attribute Matrix** (named according to the selected **Prefix**). To fill the new arrays each _cell_ in the **Reference Atribute Matrix** is mapped to a _cell_ in the **Moving Attribute Matrix** (or 0 where there is no overlap). If the **Reference Atribute Matrix** and **Moving Attribute Matrix** belong to different _Data Containers_ all other _Attribute Matricies_ belonging to the same _Data Container_ as the **Moving Cell Attribute Matrix** will be copied into the **Reference Cell Atribute Matrix**'s _Data Containers_ (named according to the selected **Prefix**).

## Parameters ##
| Name             | Type |
|------------------|------|
| Array Prefix | String |
| Reference Attribute Matrix | String |
| Moving Attribute Matrix | String |
| Transformation Type | String |
| Transform | manually augmented transformation matrix (3x4 with translations in last column) |

## Required Arrays ##
| Name             | Type |
|------------------|------|
| Transform | 4x4 augmented transformation matrix |

## Created Arrays ##
Use dependent (see Description above).

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users