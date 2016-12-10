package mipt.counter;

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


import javax.naming.Context;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Vector;

public class Counter extends Configured implements Tool {

    public static class WordMapper extends Mapper<LongWritable, Text, Text, IntWritable> {

        private static final IntWritable one = new IntWritable(1);

        private Text word = new Text();

        public void map(LongWritable offset, Text line, Context context) throws IOException, InterruptedException {
            for(String element: line.toString().split("[\\W]")) {
                if (element.length() > 0 && Character.isUpperCase(element.charAt(0))) {
                    word.set(element);
                    context.write(word, one);
                }
            }
        }
    }

    public static class CountReducer extends Reducer<Text, IntWritable, Text, IntWritable> {

        private IntWritable count = new IntWritable();

        public void reduce(Text word, Iterable<IntWritable> values, Context context)
                throws IOException, InterruptedException {
            Iterator<IntWritable> it = values.iterator();
            int sum = 0;
            while (it.hasNext()){
                sum += it.next().get();
            }

            count.set(sum);
            context.write(word, count);
        }

    }

    public static class WordMapper1 extends Mapper<LongWritable, Text, Text, IntWritable> {

        private static final IntWritable one = new IntWritable(1);

        private Text word = new Text();

        public void map(LongWritable offset, Text line, Context context) throws IOException, InterruptedException {
            for(String element: line.toString().split("[\\W]")) {
                if (element.length() > 0 && Character.isUpperCase(element.charAt(0))) {
                    word.set(element);
                    context.write(word, one);
                }
            }
        }
    }

    public static class CountReducer1 extends Reducer<Text, IntWritable, Text, IntWritable> {

        private IntWritable count = new IntWritable();

        public void reduce(Text word, Iterable<IntWritable> values, Context context)
                throws IOException, InterruptedException {

        }
    }

    @Override
    public int run(String[] strings) throws Exception {
        Path outputPath = new Path(strings[1]);
        Job job1 = Job.getInstance();
        job1.setJarByClass(Counter.class);
        job1.setMapperClass(WordMapper.class);
        job1.setReducerClass(CountReducer.class);
        job1.setOutputKeyClass(Text.class);
        job1.setOutputValueClass(IntWritable.class);
        job1.setInputFormatClass(TextInputFormat.class);
        job1.setOutputFormatClass(TextOutputFormat.class);
        job1.setMapOutputKeyClass(Text.class);
        job1.setMapOutputValueClass(IntWritable.class);
        job1.setNumReduceTasks(1);
        TextInputFormat.addInputPath(job1, new Path(strings[0]));
        TextOutputFormat.setOutputPath(job1, outputPath);
        return job1.waitForCompletion(true)? 0: 1;
    }


    public static void main(String[] args) throws Exception {
        new Counter().run(args);
//        new Counter().run1(args);
    }
}
