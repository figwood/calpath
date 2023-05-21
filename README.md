# calpath
Calpath, developed in C++, is a software used for calculating satellite tracks and sensor paths. During the execution, the program will retrieve the necessary inputs from the data folder, calculate the tracks and paths for the upcoming 7 days, and generate a new data file by writing the results into it. The default data folder is ../data.

Additionally, the orbitTools folder which is included serves as a reference to [orbitTools](http://www.zeptomoby.com/satellites/cppOrbitTools.zip) developed by Michael F. Henry, and is utilized to calculate satellite tracks based on SGP4 and TLE.

## database
Calpath retrieves data from satellites and sensors stored in a SQLite database. By default, the database is located at ../data/sat.db. Below are the table definitions.

### satellite
column name|type|description
----|----|----
id|int|primary key
noard_id|string|NOARD ID of satellite
name|string|custom satellite name
hex_color|string|color of satellite, in hex format 

### sensor
column name|type|description
----|----|----
id|int|primary key
sat_noard_id|string|NOARD ID of satellite
sat_name|string|satellite name
name|string|sensor name
resolution|float|resolution of sensor
width|float|width of sensor
right_side_angle|float|max right side angle, for side pendulum
left_side_angle|float|max left side angle, for side pendulum
observe_angle|float|observe angle of sensor, should not be null if width not provided
hex_color|string|color of sensor, in hex format
init_angle|float|angle to the right is positive, for special sensors

### sys_user
column name|type|description
----|----|----
id|int|primary key
user_name|string|user name
password|string|password, encrypted
email|string|email

### tle
column name|type|description
----|----|----
id|int|primary key
sat_noard_id|string|NOARD ID of satellite
time|int|timestamp of TLE
line1|string|line1 of TLE
line2|string|line2 of TLE

### tle_site
column name|type|description
----|----|----
id|int|primary key
site|string|site name
url|string|site url for updating TLE
description|string|description

## output
CalPath will save the results to SQLite databases located in the data folder. By default, this folder is ../data. The satellite track file will be saved as ../data/{NOARD_ID}.sqlite, while the sensor path file will be saved as ../data/{NOARD_ID}/{SENSOR_NAME}.sqlite.

### satellite result tables
#### track_info
column name|type|description
----|----|----
id|int|primary key
sat_noard_id|string|NOARD ID of satellite
sat_name|string|satellite name
start_time|int|calculation start time, in timestamp second

#### track
column name|type|description
----|----|----
id|int|primary key
time_offset|int|time offset after start_time
x|float|x coordinate 
y|float|y coordinate
z|float|z coordinate
vx|float|velocity on x coordinate 
vy|float|velocity on y coordinate 
vz|float|velocity on z coordinate
lon|float|longitude 
lat|float|latitude 
alt|float|altitude

### sensor result tables
#### path_info
column name|type|description
----|----|----
id|int|primary key
sat_noard_id|string|NOARD ID of satellite
sat_name|string|satellite name
sen_name|string|sensor name
start_time|int|calculation start time, in timestamp second

#### sen_path
column name|type|description
----|----|----
id|int|primary key
time_offset|int|time after start_time
lon1|float|longitude of left point 
lat1|float|latitude of left point 
lon2|float|longitude of right point 
lat2|float|latitude of right point 

## Build and run 
### Prerequisites
Ensure that you have installed all the necessary prerequisites on your development machine:
* g++
* make
* sqlite-dev

To rebuild and run, execute the command ```calPath/refresh.sh```. The output will be saved in the data folder.
