#Tof Geometry Alignment

For conducting the TOF detector alignment before final data production.

===

###Procedure
#### Obtaining a data list
create a list file with the MuDsts of interest.
For instance for the Run14 AuAu15 calibration 
```
ls /star/data84/reco/production_15GeV_2014/ReversedFullField/P14ii_TOF_dEdx/2014/*/*.MuDst.root > sthlt_good.lis
```

In /ntupler/schedule_alignment.xml replace the input path to the list file you just made


#### Creating the picoDSTs

