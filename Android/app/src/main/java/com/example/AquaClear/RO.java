package com.example.AquaClear;

import android.os.Build;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.appcompat.widget.SwitchCompat;
import androidx.fragment.app.Fragment;

import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;

public class RO extends Fragment {
    boolean bypass = false;
    Integer ignore = 0;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_r_o, container, false);

        TextView mode = (TextView) view.findViewById(R.id.ROMode);
        TextView status = (TextView) view.findViewById(R.id.ROStatus);
        TextView tds = (TextView) view.findViewById(R.id.ROTDS);
        TextView temp = (TextView) view.findViewById(R.id.ROTemperature);
        TextView level = (TextView) view.findViewById(R.id.ROleveltxt);
        ProgressBar progressBar = (ProgressBar) view.findViewById(R.id.ROmeter);
        Button startbtn = (Button) view.findViewById(R.id.ROstartbtn);
        Button stopbtn = (Button) view.findViewById(R.id.ROstopbtn);
        SwitchCompat switchCompat = (SwitchCompat) view.findViewById(R.id.switchCombat);

//        FirebaseDatabase.getInstance("https://aquaclear-8646e-default-rtdb.firebaseio.com/")
//                .getReference("ROMode").get().addOnCompleteListener(new OnCompleteListener<DataSnapshot>() {
//            @Override
//            public void onComplete(@NonNull Task<DataSnapshot> task) {
//                System.out.println(task.getResult().getValue());
//                String txt = "\t\t";
//                if((long)task.getResult().getValue() == 1L){
//                    txt += "Ultra Filter";
//                    switchCompat.setChecked(true);
//                }
//                else{
//                    txt += "Filter";
//                    switchCompat.setChecked(false);
//                }
//                mode.setText(txt);
//            }
//        });

        startbtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                FirebaseDatabase rootnode = FirebaseDatabase.getInstance("https://aquaclear-8646e-default-rtdb.firebaseio.com/");
                DatabaseReference reference = rootnode.getReference("ROstart");

                reference.setValue(1);
            }
        });

        stopbtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                FirebaseDatabase rootnode = FirebaseDatabase.getInstance("https://aquaclear-8646e-default-rtdb.firebaseio.com/");
                DatabaseReference reference = rootnode.getReference("ROstart");

                reference.setValue(0);
            }
        });

        switchCompat.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                FirebaseDatabase rootnode = FirebaseDatabase.getInstance("https://aquaclear-8646e-default-rtdb.firebaseio.com/");
                DatabaseReference reference = rootnode.getReference("forceRO");

                if(switchCompat.isChecked())reference.setValue(1L);
                else reference.setValue(0L);
            }
        });


        final Handler handler = new Handler();
        final int delay = 300; // 1000 milliseconds == 1 second

        handler.postDelayed(new Runnable() {
            public void run() {
                FirebaseDatabase rootnode = FirebaseDatabase.getInstance("https://aquaclear-8646e-default-rtdb.firebaseio.com/");
                rootnode.getReference("ROStatus").get().addOnCompleteListener(new OnCompleteListener<DataSnapshot>() {
                    @Override
                    public void onComplete(@NonNull Task<DataSnapshot> task) {
                        System.out.println(task.getResult().getValue());
                        String txt = "\t\t";
                        if((long)task.getResult().getValue() == 0L) txt += "OFF";
                        else txt += "ON";
                        status.setText(txt);
                    }
                });
                rootnode.getReference("ROTDS").get().addOnCompleteListener(new OnCompleteListener<DataSnapshot>() {
                    @Override
                    public void onComplete(@NonNull Task<DataSnapshot> task) {
                        System.out.println(task.getResult().getValue());
                        String txt = "\t\t" + task.getResult().getValue().toString() + " ppm";

                        tds.setText(txt);
                    }
                });
                rootnode.getReference("ROtemperature").get().addOnCompleteListener(new OnCompleteListener<DataSnapshot>() {
                    @Override
                    public void onComplete(@NonNull Task<DataSnapshot> task) {
                        System.out.println(task.getResult().getValue());
                        String txt = "\t\t" + task.getResult().getValue().toString() + "°C";

                        temp.setText(txt);
                    }
                });
                rootnode.getReference("ROwaterlevel").get().addOnCompleteListener(new OnCompleteListener<DataSnapshot>() {
                    @Override
                    public void onComplete(@NonNull Task<DataSnapshot> task) {
                        System.out.println(task.getResult().getValue());

                        int val = (int)(long)(task.getResult().getValue());
                        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
                            progressBar.setProgress(val, true);
                        }

                        level.setText(task.getResult().getValue().toString() + "%");
                    }
                });
                rootnode.getReference("forceRO").get().addOnCompleteListener(new OnCompleteListener<DataSnapshot>() {
                    @Override
                    public void onComplete(@NonNull Task<DataSnapshot> task) {
                        System.out.println(task.getResult().getValue());
                        if((long)task.getResult().getValue() == 1L){
                            switchCompat.setChecked(true);
                        }
                        else{
                            switchCompat.setChecked(false);
                        }
                    }
                });
                rootnode.getReference("ROMode").get().addOnCompleteListener(new OnCompleteListener<DataSnapshot>() {
                    @Override
                    public void onComplete(@NonNull Task<DataSnapshot> task) {

                        System.out.println(task.getResult().getValue());
                        String txt = "\t\t";

                        if((long)task.getResult().getValue() == 1L){
                            txt += "RO";
                        }
                        else{
                            txt += "Normal";
                        }
                        mode.setText(txt);
                    }
                });

                handler.postDelayed(this, delay);
            }
        }, 0);

        return view;
    }
}