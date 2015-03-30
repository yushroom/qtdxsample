QtDXSample is a Qt sample based on DirectX SDK sample 'HLSLWithoutFX'.

This source code is substantively in the public domain and is thus free for use for any purpose.

Requirements:
  * Visual Studio 2008 or later
  * [Qt libraries 4.7 or later for Windows](http://qt.nokia.com/downloads/)
  * [Visual Studio Add-in 1.1.9 or later for Qt](http://qt.nokia.com/downloads)
  * [DirectX SDK June 2010](http://www.microsoft.com/downloads/details.aspx?FamilyID=3021d52b-514e-41d3-ad02-438a3ba730ba&displaylang=en) (Optional)
  * [Windows SDK 7.1](http://www.microsoft.com/downloads/details.aspx?familyid=35AEDA01-421D-4BA5-B44B-543DC8C33A20&displaylang=en) (Optional)
Features:
  * Easy-to-use custom Qt widgets for DirectX 9/10.1/11 and Direct2D
  * Easy-to-use dynamic property editors for numerical entry
  * Maya-like camera operation & attribute editor
  * Platform independent core module (based on Sony vector math library)
  * Support handling DirectX device lost and resizing swap-chain
  * Automatical detection of MSAA capability (default : up to x4 sampling)
Snapshots:

![http://qtdxsample.googlecode.com/svn/wiki/images/DX9Test.png](http://qtdxsample.googlecode.com/svn/wiki/images/DX9Test.png)

Operation:
  * Tumble Tool
    * (Alt)+LMB: Rotate the point of view around the center of interest
    * LMB Drag: Use LMB or MMB to tumble. Shift to constrain rotaion(not supported)
  * Track Tool
    * (Alt)+MMB: Track the camera
    * MMB Drag: Use LMB or MMB to track. Shift to constrain translation(not supported)
  * Dolly Tool
    * (Alt)+RMB: Move ehe camera in and out
    * RMB Drag: Use mouse to dolly
  * Wheel
    * Zoom Tool: Zoom the camera in and out
To Do:
  * Support OpenGL