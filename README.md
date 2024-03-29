# kgc2010

**KGC** is an **unsupervised classification algorithm** developed with David Goodenough, Hao Chen and the Canadian Forest Service's **Advanced Forest Technologies (AFT) group** team. This implementation was developed between April 1 -- August 19th, 2010 for presentation at IGARSS 2020 while the author was funded by the U.Vic Math & Stats department, and is occasionally still being improved. Graphics functions adapted from Nigel Stewart [2].

## instructions (tested on Ubuntu 20)
To run the example:
```
  ./setup.sh
  
  make
  
  ./run.sh 
```
Detailed instructions for GUI, to follow..
.. also documentation of parameters!

## Firescar Delineation: 2002 Keg River Wildfire, Alberta, Canada
Thanks to JAXA for L-band fully-polarimetric SAR data. Shown: cluster selection from 3-d cluster plot window. The 3-d coordinates are the same as those used in the RGB encoding in the Image window. The color encoding and the 3-d coordinates for plotting are tied together in this implementation but can be switched on the fly to explore higher-dimensional data. A "very large" number of bands is supported.
<img src="output/out.gif " width="800">

# Instructions
## 3d cluster plot
* Zoom: hold down both mouse buttons and drag
* Pan: hold down right mouse button and drag
* Rotate: hold down left mouse button and drag

Click on cluster centres (blue) to visualize cluster in image domain (class window). This produces a binary classification
* after producing a binary classication, can press c (lower case) to switch back to multi-class view (in which pixels are coloured according to the mode to which they are assigned). In this representation the mode is the value of the "hilltop" determined

## Image plot
Band selection: [ add photo for switching bands] 
* type: r1 (then press return) to set band 1 as red
* type g2 (then press return) to set band 2 as green
* type b3 (then press return) to set band 3 as blue
Similarly, type r3 <return> g2 <return> b1 <return> for (red, green, blue) = (3, 2, 1) instead..

## biblio

[1] **Unsupervised Nonparametric Classification of Polarimetric SAR Data Using The K-nearest Neighbor Graph**, A. Richardson, D. G. Goodenough, H. Chen, G. Hobart, B. Moa, W. Myrvold, proc. IEEE IGARSS, Honolulu, Hawaii, July 2010.

[2] gltzpr by Nigel Stewart http://www.nigels.com/glt/gltzpr/zpr.h

[3] **Hierarchical Unsupervised Nonparametric Classification of Polarimetric SAR Time Series Data,** A. Richardson, D. Goodenough, H. Chen, proc. IEEE IGARSS, Québec City, Canada, July 2014

[4] **Mapping Fire Scars Using Radarsat-2 Polarimetric SAR Data,** D. Goodenough, H. Chen, A. Richardson, S. Cloude, W. Hong, Y. Li, Can. J. Remote Sensing, Vol. 37, No. 5, pp. 1-10, 2011
