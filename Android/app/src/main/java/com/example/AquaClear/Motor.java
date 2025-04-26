package com.example.AquaClear;

import android.os.Build;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;

import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.messaging.FirebaseMessaging;

public class Motor extends Fragment {
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_motor, container, false);

        TextView temp = (TextView)view.findViewById(R.id.Temperature);
        TextView tds = (TextView)view.findViewById(R.id.TDS);
        TextView motor = (TextView)view.findViewById(R.id.MotorStatus);
        ProgressBar waterlevel = (ProgressBar)view.findViewById(R.id.watermeter);
        TextView waterleveltxt = (TextView)view.findViewById(R.id.waterleveltxt);
        Button startbtn = (Button) view.findViewById(R.id.startbtn);
        Button stopbtn = (Button) view.findViewById(R.id.stopbtn);
        Button forcestartbtn = (Button) view.findViewById(R.id.forcestartbtn);

        startbtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                FirebaseDatabase rootnode = FirebaseDatabase.getInstance("https://aquaclear-8646e-default-rtdb.firebaseio.com/");
                DatabaseReference reference = rootnode.getReference("start");

                reference.setValue(1);
            }
        });

        stopbtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                FirebaseDatabase rootnode = FirebaseDatabase.getInstance("https://aquaclear-8646e-default-rtdb.firebaseio.com/");
                DatabaseReference reference = rootnode.getReference("start");

                reference.setValue(0);
                rootnode.getReference("forcestart").setValue(0);
            }
        });

        forcestartbtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                FirebaseDatabase rootnode = FirebaseDatabase.getInstance("https://aquaclear-8646e-default-rtdb.firebaseio.com/");
                DatabaseReference reference = rootnode.getReference("forcestart");
                rootnode.getReference("start").setValue(1);
                reference.setValue(1);
            }
        });

//        Log.i("shivam", "onCreateView: shivam sharma");

        final Handler handler = new Handler();
        final int delay = 300; // 1000 milliseconds == 1 second

        handler.postDelayed(new Runnable() {
            public void run() {
                Log.i("shivam sharma", "run: " + FirebaseDatabase.getInstance("https://aquaclear-8646e-default-rtdb.firebaseio.com/").toString());
                FirebaseDatabase rootnode = FirebaseDatabase.getInstance("https://aquaclear-8646e-default-rtdb.firebaseio.com/");
                rootnode.getReference("motorStatus").get().addOnCompleteListener(new OnCompleteListener<DataSnapshot>() {
                    @Override
                    public void onComplete(@NonNull Task<DataSnapshot> task) {
                        System.out.println(task.getResult().getValue());
                        String txt = "\t\t";
                        if((long)task.getResult().getValue() == 0L) txt += "OFF";
                        else txt += "ON";
                        motor.setText(txt);
                    }
                });
                rootnode.getReference("TDS").get().addOnCompleteListener(new OnCompleteListener<DataSnapshot>() {
                    @Override
                    public void onComplete(@NonNull Task<DataSnapshot> task) {
                        System.out.println(task.getResult().getValue());
                        String txt = "\t\t" + task.getResult().getValue().toString() + " ppm";

                        tds.setText(txt);
                    }
                });
                rootnode.getReference("temperature").get().addOnCompleteListener(new OnCompleteListener<DataSnapshot>() {
                    @Override
                    public void onComplete(@NonNull Task<DataSnapshot> task) {
                        System.out.println(task.getResult().getValue());
                        String txt = "\t\t" + task.getResult().getValue().toString() + "°C";

                        temp.setText(txt);
                    }
                });
                rootnode.getReference("waterlevel").get().addOnCompleteListener(new OnCompleteListener<DataSnapshot>() {
                    @Override
                    public void onComplete(@NonNull Task<DataSnapshot> task) {
                        System.out.println(task.getResult().getValue());

                        int val = (int)(long)(task.getResult().getValue());
                        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
                            waterlevel.setProgress(val, true);
                        }

                        waterleveltxt.setText(task.getResult().getValue().toString() + "%");
                    }
                });

                handler.postDelayed(this, delay);
            }
        }, 0);


        return view;
    }
}