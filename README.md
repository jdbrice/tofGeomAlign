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

All configuration tags are case insensitive and must follow xml tag format
The default value is given for optional tags.

###Jobtype
* Default : calibration
### 1. calibrate
* performs a geometry alignment job
### 2. plot
* plots the histograms showing the yLocal and zLocal hit distributions but does not do a full calibration

###baseName
Default : ""
* The name to be prepended to all output files for easier record keeping. For instance give a basename of "run14AuAu14.6GeV" to make all root, PDF, etc appear as run14AuAu14.6GeV.{root, pdf, etc. } 

###rootOutput
* Default : "qa.root"
* The name specific to the root output file name. The full name will be baseName+rootOutput. The '.root' suffix will be added if needed.

###reportOutput
* Default : "qa.pdf"
* The name specific to the pdf report output file name. The full name will be baseName+reportOutput. The '.pdf' suffix should be specified.

###geometryOutput
* Default : "geometryAlignment.dat"
* The name specific to the data output file. The full name will be baseName+geometryOutput. The '.dat' suffix should be specified.

###dataDir
* REQUIRED
* The full path to the directory containing the TOF calibration picoDsts

###maxFiles
* Default : 10000
* The maximum number of files to load from the <dataDir> directory for processing

###ptCut
* Default : 0.5 [GeV]
* The pt cut applied to tracks matched to the tof hits

###vzCut
* Default : 30.0 [cm]
* The cut applied to the TPC zVertex. TRacks must have a zVertex less than the cut value.

###minHitsFit
* Default : 25
* The minimum number of hits in the track fit. Tracks with fewer hits used for the fit will not be used for the calibration.


## Sample Configuration
A sample configuration file:
```xml
<config>

	<!-- perform the geometry alignment -->
	<jobType>calibrate</jobType>

	<!-- names for root, report, data file etc. -->
	<baseName></baseName>
	<rootOutput>qa</rootOutput>
	<reportOutput>qa.pdf</reportOutput>
	<geometryOutput>geometryAlignment.dat</geometryOutput>
	
	<!-- The directory containing the picoDsts -->
	<dataDir>/star/institutions/rice/jdb/run13/pp510/tofCalibrationRun13/MuDstOutput/idealGeometry/output/</dataDir>
	<!-- number of files to process -->
	<maxFiles> 200 </maxFiles>

	<!-- cuts to apply -->
	<ptCut>0.5</ptCut>
	<vzCut>30.0</vzCut>
	<minHitsFit>25</minHitsFit>

</config>
```

