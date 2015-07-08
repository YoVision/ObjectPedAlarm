package com.parse.tutorials.pushnotifications;

import android.util.Log;

import com.parse.Parse;
import com.parse.ParseException;
import com.parse.ParseInstallation;
import com.parse.PushService;
import com.parse.ParsePush;
import com.parse.SaveCallback;

public class Application extends android.app.Application {

  public Application() {
  }

  @Override
  public void onCreate() {
    super.onCreate();
/*
      ParsePush.subscribeInBackground("", new SaveCallback() {
          @Override
          public void done(ParseException e) {
              if (e == null) {
                  Log.d("com.parse.push", "successfully subscribed to the broadcast channel.");
              } else {
                  Log.e("com.parse.push", "failed to subscribe for push", e);
              }
          }
      });
*/
	// Initialize the Parse SDK.
	Parse.initialize(this, "FKwceNJVu71bnxNLUhqyE2JU0FV9FsSyEfFi701k", "hRIVV0KPIafMbzS12IAk54OZfFvqt2IYZOHLmkvq");

    PushService.setDefaultPushCallback(this, MainActivity.class);
    ParseInstallation.getCurrentInstallation().saveInBackground();

	// Specify an Activity to handle all pushes by default.
	PushService.setDefaultPushCallback(this, MainActivity.class);
  }
}