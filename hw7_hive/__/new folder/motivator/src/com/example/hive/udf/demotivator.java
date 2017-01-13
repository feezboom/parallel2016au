package com.example.hive.udf;

import org.apache.hadoop.hive.ql.exec.UDF;
import org.apache.hadoop.io.Text;

public final class demotivator extends UDF {
    public Text evaluate(final Text t) {
        if (t == null) {
        	return null;
        }
        String s = t.toString();
        Integer value = 100 - Integer.parseInt(s);
        return new Text(new String (value.toString()));
    }
}
