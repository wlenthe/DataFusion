Rotate Quaternion Reference Frame {#rotatequatreferenceframe}
=============

## Group ##
DataFusion

## Description ##
This **Filter** rotates the *Quaternion* at each **Element** by the supplied *Rotation*

## Parameters ##
| Name | Type | Description |
|------|------|------|
| Rotation Axis | float (3x) | Axis that the Euler reference frame will be rotated about |
| Rotation Angle | float | Angle (in degrees) that the Euler reference frame will be rotated around the rotation axis |


## Required Objects ##
| Type | Default Array Name | Description |
|------|--------------------|-------------|
| Float | AvgQuats | Quaternions to rotate |

## Created Objects ##
None

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM.3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users