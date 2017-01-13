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
import java.util.*;

import static com.sun.org.apache.xalan.internal.xsltc.compiler.util.Type.Text;

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
    static Map<Integer, String> map = new TreeMap<Integer, String>();
    public static class CountReducer extends Reducer<Text, IntWritable, Text, IntWritable> {

        private IntWritable count = new IntWritable();

        public void reduce(Text word, Iterable<IntWritable> values, Context context)
                throws IOException, InterruptedException {
            Iterator<IntWritable> it = values.iterator();
            int sum = 0;

            for(Integer key : map.keySet()) {
                System.out.println(map.get(key));
            }

            while (it.hasNext()){
                sum += it.next().get();
            }

            map.put(sum, word.toString());

            count.set(sum);
            context.write(word, count);

            context.write(new Text(word.toString() + output), new IntWritable(average));
        }

    }



    public static class WordMapperSorter extends Mapper<LongWritable, Text, Text, IntWritable> {

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
    public static class CountReducerSorter extends Reducer<Text, IntWritable, Text, IntWritable> {

        private IntWritable count = new IntWritable();

        public void reduce(Text word, Iterable<IntWritable> values, Context context)
                throws IOException, InterruptedException {
            Iterator<IntWritable> it = values.iterator();
            int sum = 0;

            for(Integer key : map.keySet()) {
                System.out.println(map.get(key));
            }

            while (it.hasNext()){
                sum += it.next().get();
            }

            map.put(sum, word.toString());

            count.set(sum);
            context.write(word, count);

            context.write(new Text(word.toString() + output), new IntWritable(average));
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
        job1.setNumReduceTasks(1); // 1 редьюсер
        TextInputFormat.addInputPath(job1, new Path(strings[0]));
        TextOutputFormat.setOutputPath(job1, outputPath);

        Job job2 = Job.getInstance();

        return job1.waitForCompletion(true)? 0: 1;
    }

    public static void main(String[] args) throws Exception {
        new Counter().run(args);
        Map<Integer, String> res = MapUtil.sortByValue(map);
        for(Integer key : res.keySet()) {
            System.out.println(key + " " + res.get(key));
        }
    }
}

class MapUtil
{
    public static <K, V extends Comparable<? super V>> Map<K, V>
    sortByValue( Map<K, V> map )
    {
        List<Map.Entry<K, V>> list =
                new LinkedList<Map.Entry<K, V>>( map.entrySet() );
        Collections.sort( list, new Comparator<Map.Entry<K, V>>()
        {
            public int compare( Map.Entry<K, V> o1, Map.Entry<K, V> o2 )
            {
                return (o1.getValue()).compareTo( o2.getValue() );
            }
        } );

        Map<K, V> result = new LinkedHashMap<K, V>();
        for (Map.Entry<K, V> entry : list)
        {
            result.put( entry.getKey(), entry.getValue() );
        }
        return result;
    }
}
