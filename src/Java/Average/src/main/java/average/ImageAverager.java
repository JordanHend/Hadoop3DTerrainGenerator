package average;

import org.hipi.image.FloatImage;
import org.hipi.image.HipiImageHeader;
import org.hipi.imagebundle.mapreduce.HibInputFormat;
import org.hipi.image.HipiImage;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import java.io.File;
import org.apache.hadoop.conf.Configuration;
import java.io.FileOutputStream;
import org.hipi.image.io.*;
import java.io.IOException;
import org.apache.hadoop.fs.FSDataOutputStream;
import org.hipi.image.HipiImageHeader.HipiColorSpace;
import java.io.File;
import java.io.FileOutputStream;
import org.hipi.image.HipiImageHeader.HipiImageFormat;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.FSDataOutputStream;
import org.apache.hadoop.fs.Path;


public class ImageAverager extends Configured implements Tool {
  
  public static class AverageMapper extends Mapper<HipiImageHeader, FloatImage, IntWritable, FloatImage> {
    public void map(HipiImageHeader key, FloatImage value, Context context) 
      throws IOException, InterruptedException {
    
  //Verify the images are ok
      if (value != null && value.getWidth() > 1 && value.getHeight() > 1) {



      //The standard dimentions of the data that we're forcing.
      int WIDTH = 500;
      int HEIGHT = 500;


        // Get dimensions of image
        int w = value.getWidth();
        int h = value.getHeight();



      //Temp image with the correct format ( we need it in jpeg and rgb color space to output it)
        FloatImage temp = new FloatImage(WIDTH, HEIGHT, 3, HipiImageHeader.HipiImageFormat.JPEG, HipiImageHeader.HipiColorSpace.RGB);

        //crop the image to be the right size (we can't perform addition operations unless they're the same size)
        value.crop(0, 0, 500, 500, temp);
  
        //and lastly, right.
        context.write(new IntWritable(1), temp);

      } 
      
    } 

  }
    
  
  
  public static class AverageReducer extends Reducer<IntWritable, FloatImage, IntWritable, FloatImage> {
    public void reduce(IntWritable key, Iterable<FloatImage> values, Context context) 
      throws IOException, InterruptedException 
      {
      //Again, standard dimensions...
      int WIDTH = 500;
      int HEIGHT = 500;

      //This is our output.
      FloatImage avg =new FloatImage(WIDTH, HEIGHT, 3, HipiImageHeader.HipiImageFormat.JPEG, HipiImageHeader.HipiColorSpace.RGB);

      // Loop through and add up images.
     int total = 0;
      for (FloatImage val : values) {
        avg.add(val);
        total++;
      }

      //Average them out...
      if (total > 0) {
       
        avg.scale(1.0f / total);

  
    //This is to write our output to an image...

    // Get the current job config.
		Configuration config = context.getConfiguration();
        // Use config to get filesystem.
        FileSystem fs = FileSystem.get(config);
        // Make the path to the image we want to make
         String inputfile = "/image.jpeg";
        Path filenamePath = new Path(inputfile);
 
        try
        {
            // If the file already exists delete it.
            if (fs.exists(filenamePath))
            {
                //Remove it.
                fs.delete(filenamePath, true);
            }
 
          //And lastly get the OutputStream to pass to the encodeImage function.    
          FSDataOutputStream fin = fs.create(filenamePath);
 		      JpegCodec.getInstance().encodeImage(avg, fin);
 		      fin.flush();
          fin.close();
        }
        catch (IOException ioe)
        {
            System.err.println("IOException during operation " + ioe.toString()); 
        }
        //Lastly write our output to HDFS.
        context.write(key, avg);

      }

    } 

  } 
  
  public int run(String[] args) throws Exception {
    // Check input arguments
    if (args.length != 2) {
      System.out.println("Usage: ImageAverager <input HIB> <output directory>");
      System.exit(0);
    }
    Configuration conf = new Configuration();
    // Initialize and configure MapReduce job
    Job job = new Job(conf);
    // Set input format class which parses the input HIB and spawns map tasks
    job.setInputFormatClass(HibInputFormat.class);
    // Set the driver, mapper, and reducer classes which express the computation
    job.setJarByClass(ImageAverager.class);
    job.setMapperClass(AverageMapper.class);
    job.setReducerClass(AverageReducer.class);
    // Set the types for the key/value pairs passed to/from map and reduce layers
    job.setMapOutputKeyClass(IntWritable.class);
    job.setMapOutputValueClass(FloatImage.class);
    job.setOutputKeyClass(IntWritable.class);
    job.setOutputValueClass(FloatImage.class);
    
    // Set the input and output paths on the HDFS
    FileInputFormat.setInputPaths(job, new Path(args[0]));
    FileOutputFormat.setOutputPath(job, new Path(args[1]));

    // Execute the MapReduce job and block until it complets
    boolean success = job.waitForCompletion(true);
    
    // Return success or failure
    return success ? 0 : 1;
  }
  
  public static void main(String[] args) throws Exception {
    ToolRunner.run(new ImageAverager(), args);
    System.exit(0);
  }
  
}