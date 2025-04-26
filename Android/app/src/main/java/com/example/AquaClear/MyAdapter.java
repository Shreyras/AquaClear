package com.example.AquaClear;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentStatePagerAdapter;
import androidx.viewpager.widget.PagerAdapter;
import androidx.viewpager2.adapter.FragmentStateAdapter;

import java.util.ArrayList;
import java.util.List;

public class MyAdapter extends FragmentStatePagerAdapter {
//    Context context;
//    ArrayList<Integer> arrayList;
//    LayoutInflater inflater;
    List<Fragment> fragmentList;

    public MyAdapter(FragmentManager fm, List<Fragment> fragmentList) {
        super(fm);
        this.fragmentList = fragmentList;
    }
//    public MyAdapter(Context context, ArrayList<Integer> arrayList) {
//        this.context = context;
//        this.arrayList = arrayList;
//        inflater = LayoutInflater.from(context);
//    }

    @NonNull
    @Override
    public Fragment getItem(int position) {
        return fragmentList.get(position);
    }

    @Override
    public int getCount() {
        return fragmentList.size();
    }

//    @Override
//    public int getCount() {
//        return arrayList.size();
//    }
//
//    @Override
//    public boolean isViewFromObject(@NonNull View view, @NonNull Object object) {
//        return view.equals(object);
//    }
//
//    @Override
//    public Object instantiateItem(@NonNull ViewGroup container, int position) {
//        View view = inflater.inflate(arrayList.get(position), container, false);
//        container.addView(view);
//        return view;
//    }
//
//    @Override
//    public void destroyItem(@NonNull ViewGroup container, int position, @NonNull Object object) {
//        container.removeView((View) object);
//    }
}
