RegisterOrientations {#registerorientations}
=============

## Group (Subgroup) ##
DataFusion (DataFusion)

## Description ##
This filter finds the average rotation from <B>Moving Quats</B> to <B>Reference Quats</B> (and then optionally applies it to <B>Moving Quats</B> to align their crystallographic reference frames). The average rotation is computed with the following algorithm:

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
| Name             | Type |
|------------------|------|
| Use Good Features| Boolean |
| Minimum Average Rotation Angle | Double |
| Apply Transformation | Boolean |

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

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users