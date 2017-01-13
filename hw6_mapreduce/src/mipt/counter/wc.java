package ru.mipt.code;

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

import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;

/**

 * Created by instructor on 06.11.16.

 */

public class wc extends Configured implements Tool{

    /**

     * Маппер. На вход получаем сплит (фрагмент файла размером с HDFS-блок).

     * На выходе - множество пар (слово, 1).

     */

    public static class WordMapper extends Mapper<LongWritable, Text, Text,

            IntWritable>{

        private static final IntWritable one = new IntWritable(1);

        private IntWritable sum = new IntWritable(0);
        private Text word = new Text();

        /*
             * @param offset номер строки, начиная от начала входного сплита (не будет

            использован ни в этом примере, ни в ДЗ).

             * @param line строка текста.

             * @param context объект, отвечающий за сохранение результата.

         */

        public void map(LongWritable offset, Text line, Context context) throws

                IOException, InterruptedException {


            String[] info = line.toString().split("\\s+");
            //for(String lin: line.toString().split("\n")) {

            // List<String> info = new ArrayList<String>(Arrays.asList(line.toString().split("\\s+")));
            word.set(info[0]);
//		System.out.println(info[0]);
            //List<String> replic = new ArrayList<String>(Arrays.asList(info.get(1).toString().split("\\W|\\s")));
            //replic.removeAll(Arrays.asList(""));
            sum.set(info.length - 1);
            context.write(word, sum);

//            for(String element: info[1].toString().split("\\s+")){
//                sum +=
//
//                context.write(word, one);
//
//            }
            //  }
        }



    }


    public static class ReplicMapper extends Mapper<LongWritable, Text, Text,

            IntWritable>{

        private static final IntWritable one = new IntWritable(1);

        private IntWritable sum = new IntWritable(0);
        private Text word = new Text();

        /*
             * @param offset номер строки, начиная от начала входного сплита (не будет

            использован ни в этом примере, ни в ДЗ).

             * @param line строка текста.

             * @param context объект, отвечающий за сохранение результата.

         */

        public void map(LongWritable offset, Text line, Context context) throws

                IOException, InterruptedException {


            String[] info = line.toString().split("\\s+");
//            for(String lin: line.toString().split("\n")) {
            //              List<String> info = new ArrayList<String>(Arrays.asList(lin.split("->")));


            word.set(info[0]);
//            List<String> replic = new ArrayList<String>(info[1].toString().split("\\W|\\s"));
//            replic.removeAll(Arrays.asList(""));

            context.write(word, one);

//            for(String element: info[1].toString().split("\\s+")){
//                sum +=
//
//                context.write(word, one);
//
//            }
        }
//        }



    }


    /**

     * Редьюсер. Суммирует пары (слово, 1) по ключу (слово).

     * На выходе получаем пары (уникальн_слово, кол-во).

     * В поставке Hadoop уже есть простейшие predefined reducers. Функционал

     данного редьюсера реализован в IntSumReducer.

     */

    public static class CountReducer extends Reducer<Text, IntWritable, Text,

            IntWritable>{

//Пустой IntWritable-объект для экономии памяти, чтоб не создавать его при

        // каждом выполнении reduce-функции

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


    public static class ReplicReducer extends Reducer<Text, IntWritable, Text,

            IntWritable>{

//Пустой IntWritable-объект для экономии памяти, чтоб не создавать его при

        // каждом выполнении reduce-функции

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

    @Override

    public int run(String[] strings) throws Exception {

        Path outputPath = new Path(strings[1]);
        Path outputPath2 = new Path(strings[2]);

// настройка Job'ы

        Job job1 = Job.getInstance();

        job1.setJarByClass(wc.class);

        job1.setMapperClass(WordMapper.class);

        job1.setReducerClass(CountReducer.class);

        job1.setOutputKeyClass(Text.class);

        job1.setOutputValueClass(IntWritable.class);

        job1.setInputFormatClass(TextInputFormat.class);

        job1.setOutputFormatClass(TextOutputFormat.class);

        job1.setMapOutputKeyClass(Text.class);

        job1.setMapOutputValueClass(IntWritable.class);

        job1.setNumReduceTasks(8); // по умолчанию задаётся 1 reducer

        Job job2 = Job.getInstance();

        job2.setJarByClass(wc.class);

        job2.setMapperClass(ReplicMapper.class);

        job2.setReducerClass(ReplicReducer.class);

        job2.setOutputKeyClass(Text.class);

        job2.setOutputValueClass(IntWritable.class);

        job2.setInputFormatClass(TextInputFormat.class);

        job2.setOutputFormatClass(TextOutputFormat.class);

        job2.setMapOutputKeyClass(Text.class);

        job2.setMapOutputValueClass(IntWritable.class);

        job2.setNumReduceTasks(8); // по умолчанию задаётся 1 reducer

        TextInputFormat.addInputPath(job2, new Path(strings[0]));

        TextOutputFormat.setOutputPath(job2, outputPath2);


        TextInputFormat.addInputPath(job1, new Path(strings[0]));

        TextOutputFormat.setOutputPath(job1, outputPath);

        return job1.waitForCompletion(true) && job2.waitForCompletion(true) ? 0: 1; //ждём пока закончится Job и

        //   возвращаем результат

    }

    public static void main(String[] args) throws Exception {

        new wc().run(args);

    }

}