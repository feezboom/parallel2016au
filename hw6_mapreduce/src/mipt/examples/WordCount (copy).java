package ru.mipt.examples;

import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;
import org.apache.hadoop.util.Tool;

import java.io.IOException;
import java.util.Iterator;

public class WordCount extends Configured implements Tool
{

	public static class MapCount extends Mapper<LongWritable, Text, Text, IntWritable>
	{
		private static final IntWritable one = new IntWritable(1);
		private Text word = new Text();
		public void map(LongWritable offset, Text line, Context context) throws IOException, InterruptedException
		{
			for(String element: line.toString().split("\\s+"))
			{
				element = element.replaceAll("^[^a-zA-Zа-яА-Я0-9\\s]+$", "");
				if(element.length() > 0 && Character.isUpperCase(element.charAt(0)))
				{
					word.set(element);
					context.write(word, one);
				}
			}
		}
	}

	public static class ReduceCount extends Reducer<Text, IntWritable, Text, IntWritable>
	{
		private IntWritable count = new IntWritable();
		public void reduce(Text word, Iterable<IntWritable> values, Context context) throws IOException, InterruptedException 
		{
			Iterator<IntWritable> it = values.iterator();
			int sum = 0;
			while (it.hasNext())
				sum += it.next().get();
			count.set(sum);
			context.write(word, count);
		}
	}

	public static class MapSort extends Mapper<LongWritable, Text, IntWritable, Text>
	{
		private IntWritable count = new IntWritable();
		private Text word = new Text();
		public void map(LongWritable offset, Text line, Context context) throws IOException, InterruptedException
		{
			String[] pair = line.toString().split("\\s+");
			word.set(pair[0]);
			count.set(Integer.parseInt(pair[1]));
			context.write(count, word);
		}
	}

	public static class ReduceSort extends Reducer<IntWritable, Text, IntWritable, Text >
	{
		private Text temp = new Text();
		public void reduce(IntWritable count, Iterable<Text> words, Context context) throws IOException, InterruptedException 
		{
			Iterator<Text> it = words.iterator();
			while(it.hasNext()){
				//temp.set(it.next().get());
				context.write(count, it.next());
			}
		}
	}

	@Override
	public int run(String[] strings) throws Exception
	{
		Path outputPath = new Path(strings[2]);
		Path tempPath = new Path(strings[1]);
		Path inputPath = new Path(strings[0]);

		Job job1 = Job.getInstance();
		Job job2 = Job.getInstance();
	
		job1.setJarByClass(WordCount.class);
		job1.setMapperClass(ru.mipt.examples.WordCount.MapCount.class);
		job1.setReducerClass(ru.mipt.examples.WordCount.ReduceCount.class);
		job1.setOutputKeyClass(Text.class);
		job1.setOutputValueClass(IntWritable.class);
		job1.setInputFormatClass(TextInputFormat.class);
		job1.setOutputFormatClass(TextOutputFormat.class);
		job1.setMapOutputKeyClass(Text.class);
		job1.setMapOutputValueClass(IntWritable.class);
		job1.setNumReduceTasks(8); 

		TextInputFormat.addInputPath(job1, inputPath);
		TextOutputFormat.setOutputPath(job1, tempPath);

		job1.waitForCompletion(true);
		
		job2.setJarByClass(WordCount.class);
		job2.setMapperClass(ru.mipt.examples.WordCount.MapSort.class);
		job2.setReducerClass(ru.mipt.examples.WordCount.ReduceSort.class);
		job2.setOutputKeyClass(IntWritable.class);
		job2.setOutputValueClass(Text.class);
		job2.setInputFormatClass(TextInputFormat.class);
		job2.setOutputFormatClass(TextOutputFormat.class);
		job2.setMapOutputKeyClass(IntWritable.class);
		job2.setMapOutputValueClass(Text.class);
		job2.setNumReduceTasks(8); 

		TextInputFormat.addInputPath(job2, tempPath);
		TextOutputFormat.setOutputPath(job2, outputPath);

		return job2.waitForCompletion(true) ? 0 : 1;
	}

	public static void main(String[] args) throws Exception
	{
		new WordCount().run(args);
	}
}

