# XmdvTool

XmdvTool is an interactive data visualization toolkit for data anlytics

### Overview

**XmdvTool** is a public-domain software package for the interactive visual exploration of multivariate data sets. It is available on all major platforms such as UNIX, LINUX, MAC and Windows. XmdvTool is developed using Qt and Eclipse CDT. It supports five methods for displaying flat form data and hierarchically clustered data:

* Scatterplots
* Star Glyphs
* Parallel Coordinates
* Dimensional Stacking
* Pixel-oriented Display

XmdvTool also supports a variety of interaction modes and tools, including brushing in screen, data, and structure spaces, zooming, panning, and distortion techniques, and the masking and reordering of dimensions. Univariate display and graphical summarization, via tree-maps and modified Tukey box plots, are also supported. Finally, color themes and user customizable color assignments permit tailoring of the aesthetics to the users.

XmdvTool has been applied to a wide range of application areas, some of which are highlighted in our Case Studies. Some of these domains include remote sensing, financial, geochemical, census, and simulation data. We are always looking for new applications, so if you've had some success with the system in your domain, we'd love to hear from you. See our contact page and join our user group if you'd like to contribute something or get further information.

You can learn more details via the slides ["XMDV Project Overview: 1997 to 2007 and Beyond"](http://davis.wpi.edu/xmdv/docs/overview.ppt).

### RECENT PROJECTS

* Model-Driven Visual Analytics on Streams: This project is supported by NSF under grant IIS-1117139.

* Managing Discoveries in Visual Analytics: This project is supported by NSF under grant IIS-0812027.

* Interactive Stream Views: Visual Analysis of Streaming Data: This project is supported by NSF under grant CCF-0811510.

* Quality Space Visualization: This project is supported by NSF under grant IIS-0414380.

The XmdvTool project has been supported by NSF under prior grants IIS-0119276, IIS-9732897 and IRIS-9729878.

### More details

Please visite our website: http://davis.wpi.edu/xmdv/


### How to compile

#### Mac
1. Open terminal and cd to the directory where you downloaded the source code of xmdv. run ```ls``` to make sure you see XmdvTool.pro file

2. If you are using xcode, the command below generate a xcode project file. For users not using xcode, skip step 2.
***qmake -spec macx-xcode XmdvTool.pro***
3. Create a make file 'Makefile.Release'
***qmake -r CONFIG+=release XmdvTool.pro***
4. Compile
***make -f Makefile.Release -j4***
5. Find XmdvTool App and run, you may copy it to your Application folder if you want.

#### Windows

#### Linux
