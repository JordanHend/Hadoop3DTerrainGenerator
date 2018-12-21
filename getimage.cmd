echo Grabbing image off of HDFS and starting renderer...
@ECHO OFF
hadoop fs -get /image.jpeg
START TerrainRenderer.exe