package com.example.AquaClear;

import android.os.Bundle;
import android.util.Log;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.Fragment;
import androidx.viewpager.widget.PagerAdapter;
import androidx.viewpager.widget.ViewPager;

import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.firebase.messaging.FirebaseMessaging;

import java.util.ArrayList;
import java.util.List;


public class MainActivity extends AppCompatActivity{
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        FirebaseMessaging.getInstance().subscribeToTopic("Notifications")
                .addOnCompleteListener(new OnCompleteListener<Void>() {
                    @Override
                    public void onComplete(@NonNull Task<Void> task) {
                        String msg = "done";
                        if (!task.isSuccessful()) {
                            msg = "failed";
                        }
                        Log.i("Subscription", "onComplete: bhai subscribe kr liya");
                    }
                });

        ViewPager viewPager = (ViewPager)findViewById(R.id.pager);
        List<Fragment> list = new ArrayList<>();
        list.add(new Motor());
        list.add(new RO());

        PagerAdapter pagerAdapter = new MyAdapter(getSupportFragmentManager(), list);
        viewPager.setAdapter(pagerAdapter);
    }
}