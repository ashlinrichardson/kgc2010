# w41k1k1

this software written by A. Richardson April 1, 2010 -- August 19th, 2010 while funded as an M.Sc. student by Dr. R. Illner at the department of Mathematics and Statistics (U. Victoria) is a reinterpretation of a code originally written in a command-line utility "scg" (standing for S. Cloude graphics) written under the supervision of Dr. D. Goodenough and H. Chen at Natural Resources Canada (NRCAN). 

this research direction towards data-driven analysis and implementation was continued with the support of Dr. D. Leckie who supervised me as a physics scientist at NRCAN and also by Dr. B. Gooch my first PhD supervisor at U. Victoria. The author conducted research relating to adaptations of derived software to time-series data applications at U. Victoria with Dr. Goodenough as his PhD student, and with physics scientist H. Chen at NRCAN. The author is thankful to all of the above gentlemen for inspiring a lifelong interest in mathematical modelling, remote sensing, image analysis, and computer vision. 

Dr. Y. Coady who also supported this work was instrumental in helping me develop a philosophy to always look on the bright side of things-- a special thank-you goes to her. It is with gratitude I recognize D. Marinakis and D. Fairall for welcoming me as part of their community at Kinsol Research, and as inspiring leaders and community builders.

the command-line utility "scg" also implemented an algorithm similar to [1]. "scg" was intended for rapid interactive radar data exploration and included special features for visualizing radar data inspired by the great Dr. S. R. Cloude and the physically explanatory radar image visualisation techniques he develops.

this software produced results shown in the 2010 IGARSS presentation given to a full room at the Waikiki Hilton [1] which led to an invitation by Prof. Eric Pottier (u. Rennes) to submit the same software to the ESA-administered PolSARPro software package, subject to the implementation of a number of recommendations made in the presentation to make the software useful. Until those improvements are implemented here, this version should not be considered intended as useful for any particular purpose other than the author's personal reflection. 

instructions (tested on mac os-X 10.11.6 and ubuntu 16. LTS) 

  make 
  
  ./run.sh 

biblio

[1] Unsupervised Nonparametric Classification of Polarimetric SAR Data Using The K-nearest Neighbor Graph, A. Richardson, D. G. Goodenough, H. Chen, G. Hobart, B. Moa, W. Myrvold, proc. IEEE IGARSS, Honolulu, Hawaii, July 2010.

[2] gltzpr by Nigel Stewart http://www.nigels.com/glt/gltzpr/zpr.h
