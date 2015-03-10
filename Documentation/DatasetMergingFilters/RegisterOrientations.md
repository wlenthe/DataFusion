Register Orientations {#registerorientations}
=====

## Group (Subgroup) ##
DatasetMerging (DatasetMerging)


## Description ##
This filter finds the average rotation from <B>Moving Quats</B> to <B>Reference Quats</B> and then applies it to <B>Moving Quats</B> to align their crystallographic reference frames. The average rotation is computed with the following algorithm:

- Create list of features that are good in both <B>Reference Good Features</B> and <B>Moving Good Features</B> (these are assumed to be the same feature and are checked to make sure they shared the same crystal structure).
- Loop over shared good features and for each:
	- Compute the rotation between <B>Reference Quats</B> and <B>Moving Quats</B> for the feature
	- Loop over the phase's symmetry operators and for each:
		- Compute the equivalent rotation ('<I>candidate rotation</I>')
		- Loop over every other pair of quaternions and for each:
			- Apply the '<I>candidate rotation</I>'
			- Compute the misorientation angle between modified <B>Reference</B> and <B>Moving</B> orientations
		- Find the average misorientation angle for this '<I>candidate rotation</I>'
	- Select the '<I>candidate rotation</I>' resulting in the minimum average misorientation angle and add to list of '<I>best rotations</I>'
- Average all '<I>best rotations</I>' with a corresponding average '<I>minimum rotation angle</I>' below the <B>Minimum Average Rotation Angle</B>

## Parameters ##
| Name             | Type | Comment |
|------------------|------|---------|
| Write Best Rotations to File | Boolean | if selected the '<I>best rotations</I>' for each pair along with their   corresponding average '<I>minimum rotation angle</I>' will be written to a csv |
| Output File | String | The file to write best rotation info to |
| Minimum Average Rotation Angle | Double | The threshold for average '<I>minimum rotation angle</I>' (degrees) |

## Required Arrays ##

| Type | Default Array Name | Description |
|------|--------------------|-------------|
| 4x Float  | AvgQuats           | Reference Features' Quaternions        |
| 4x Float  | AvgQuats           | Moving Features' Quaternions        |
| Int  | Phases           | Reference Features' Phases        |
| Int  | Phases           | Moving Features' Phases        |
| Int  | CrystalStructures           | Reference Features' Crystal Structures        |
| Int  | CrystalStructures           | Moving Features' Crystal Structures        |
| Boolean  | ThresholdArray           | Reference Features' Good Features        |
| Boolean  | ThresholdArray           | Moving Features' Good Features        |


## Created Arrays ##
none



## Authors: ##

**Copyright:** Who holds the copyright for the code

**Contact Info:** email address

**Version:** x.y.z

**License:**  What is the license for this code


