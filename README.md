tofGeomAlign
============

Project for performing the TOF to TPC geometry alignment


## Workflow
1)	Use the TOF Calibration nTuple Maker to produce nTuples in the TOF picoDst format with the nTupler set to use the ideal geometry with the geometry tag set according the the production that is being calibrated. See TOF Calibration nTupler Project for more details.

2) Checkout this project into a clean working directory and build it: 
```
$ cd some-working-directory	
$ git clone <git-url>
$ cd tofGeomAlign/bin
$ ./fullbuild
```

3) Then run with a valid configuration file (See below) :
```
$ ./align path/to/configuration/file.xml > path/to/log.txt &
```

4) The alignment procedure will produce a QA root file and a QA PDF in addition to the GeometryAlignment.dat (or name given in config file). 

5) Rerun the TOF Calibration nTuple Maker with the Ideal geometry flag turned off and the GeometryAlignment.dat file loaded instead.

6) Repeat Steps 3-5 until the X, Y, and Z offsets are within acceptable limits around zero.

## Configuration File