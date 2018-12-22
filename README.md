Hadoop3DTerrainGenerator
---------------------------------------------------------------------------------------------
It uses Hadoop/Hipi in a multicluster environment to average height image data from a bundle of images and then pull that average image for use in a OpenGL 3D rendering program.
---------------------------------------------------------------------------------------------

Technologies used: 

Java with Hadoop and Hipi was used for the MapReduce. 

C++ with OpenGL to render terrain. 


	--FreeType for text fonts, stb_image for loading textures.
----------------------------------------------------------------------------------------------

The prerequisites for running this project is:

1: A Set up Hadoop Cluster.
2: A output.hib and output.hib.data file located in the following directory on hdfs: /hipi/images2/
	- this is because it calls for /hipi/images2/output.hib
3: Textures in the Assets Folder (they're already there, just make sure they stay!)

If you have all of these you can just run the START.cmd in the root directory and it should run everything.

--- The TerrainRenderer.exe will NOT run if you dont have the image.jpeg height image in the root directory again. It deletes it and pulls the new one from HDFS automatically in the START.cmd, so make sure to keep it there. If you dont have the above, then DONT run START.cmd ---


----------------------------------------------------------------------------------------------

Settings of the renderer can be editted in the config.cfg file.

heightscale = the highest value possible (when the color is white)
blockscale = how many vertices per pixel
radius = the radius length of the light source in the renderer.
ambient r/g/b = the ambient colors of the light. 
