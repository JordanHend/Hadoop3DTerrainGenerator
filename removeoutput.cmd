echo Deleting output directory on HDFS (If it exists)...
@ECHO OFF
hadoop fs -rm  -r /hipioutput/output3
