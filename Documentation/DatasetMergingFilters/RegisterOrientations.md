RegisterOrientations {#registerorientations}
=====

## Group (Subgroup) ##
DatasetMerging (DatasetMerging)


## Description ##
This filter finds the average rotation from <B>Moving Quats</B> to <B>Reference Quats</B> and then applies it to <B>Moving Quats</B> align their reference frames. The average rotation is computed with the following algorithm:

- Create list features that are good in both <B>Reference Good Features</B> and <B>Moving Good Features</B> (these are assumed to be the same feature and are check to make sure they shared the same crystal structure).
- Loop over shared good feature and for each:
	- Compute the rotation between <B>Reference Quats</B> and <B>Moving Quats</B> for the feature
	- Loop over the phase's symmetry operators and for each:
		- Compute the equivalent rotation expression ('<I>candidate rotation</I>')
		- Loop over every other pair of quaternions and for each:
			- Apply the '<I>candidate rotation</I>'
			- Compute the rotation between modified <B>Reference</B> and <B>Moving</B>
			- Loop over the phase's symmetry operators and for each:
				- Compute the equivalent rotation expression ('<I>resulting rotation candidate</I>')
			- Find the '<I>resulting rotation candidate</I>' with the lowest rotation angle ('<I>minimum rotation angle</I>')
		- Find the average '<I>minimum rotation angle</I>' for this '<I>candidate rotation</I>'
	- Select the '<I>candidate rotation</I>' resulting in the minimum a'<I>minimum rotation angle</I>' and add to list ('<I>best rotations</I>')
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

None



## Authors: ##

**Copyright:** 2014 Will Lenthe (UCSB)

**Contact Info:** willlenthe@gmail.com

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.


