package com.example.AquaClear;

import android.os.Looper;
import android.widget.Toast;
import android.os.Handler;

import androidx.annotation.NonNull;
import androidx.core.app.NotificationCompat;
import androidx.core.app.NotificationManagerCompat;

import com.google.firebase.messaging.FirebaseMessagingService;
import com.google.firebase.messaging.RemoteMessage;

public class MyFirebaseMessagingService extends FirebaseMessagingService {
    @Override
    public void onMessageReceived(@NonNull RemoteMessage remoteMessage) {
        super.onMessageReceived(remoteMessage);
        getmsg(remoteMessage.getNotification().getTitle(), remoteMessage.getNotification().getBody());
    }

    public void getmsg(String title, String msg){
        NotificationCompat.Builder builder = new NotificationCompat.Builder(this, "MyFirebaseChannel")
                .setSmallIcon(R.drawable.ic_baseline_notifications_active_24)
                .setContentTitle(title)
                .setContentText(msg)
                .setAutoCancel(true)
                .setPriority(NotificationManagerCompat.IMPORTANCE_HIGH);

        NotificationManagerCompat manager = NotificationManagerCompat.from(this);
        manager.notify(101, builder.build());

        new Handler(Looper.getMainLooper()).post(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(getApplicationContext(), title, Toast.LENGTH_SHORT).show();
            }
        });
    }
}
