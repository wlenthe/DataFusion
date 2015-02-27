Generate Dataset Misorientation Colors {#generatedatasetmisorientationcolors}
=====

## Group (Subgroup) ##
DatasetMerging (DatasetMerging)


## Description ##
This filter generates misorientation colors between 2 orientations for a single voxel (refer to the help file for *Generate Misorientation Colors* for more details on this coloring scheme). Additionally a boolean array is created to flag voxels with a misorientation greater than the specified **Low Angle Flag Tolerance** to aid in visualizing differences between 2 datasets. Any voxels below the tolerance or for which a color cannot be calculated will be not be flagged.

## Parameters ##
| Name             | Type |
|------------------|------|
| Low Angle Flag Tolerance | Float |

## Required Arrays ##

| Type | Default Array Name | Description |
|------|--------------------|-------------|
| 4x Float  | Quats           | Dataset 1 Orientations |
| 4x Float  | Quats           | Dataset 2 Orientations        |
| Int  | Phases           | Dataset 1 Phases        |
| Int  | Phases           | Dataset 2 Phases        |
| Int  | CrystalStructures           | Dataset 1 Crystal Structures        |
| Int  | CrystalStructures           | Dataset 2 Crystal Structures        |
| Boolean  | ThresholdArray           | Good Voxels        |


## Created Arrays ##

| Type | Default Array Name | Description |
|------|--------------------|-------------|
| 3x Int  | MisorientationColor | RGB misorientation color |
| Boolean  | ThresholdArray | Low misorientation angle flag |



## Authors: ##

**Copyright:** Who holds the copyright for the code

**Contact Info:** email address

**Version:** x.y.z

**License:**  What is the license for this code


