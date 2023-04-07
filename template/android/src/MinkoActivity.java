package {{PACKAGE}};

import android.app.Activity;
import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.net.Uri;
import android.util.Log;
import android.widget.Toast;
import java.lang.Exception;
import java.util.Iterator;
import java.util.Set;
import org.libsdl.app.*;

/**
** Minko Activity, Java entry point
*/
public class MinkoActivity extends SDLActivity
{
	protected static MinkoActivity _minkoActivitySingleton;

    // We use the static library of SDL, so we do not need to load the shared
    // library.
    @Override
    protected String[] getLibraries() {
        return new String[] {
            "main"
        };
    }

	public static void initialize() {
		Log.v("Minko/Java", "[MinkoActivity] initialize()");

        // The static nature of the singleton and Android quirkyness force us to initialize everything here
        // Otherwise, when exiting the app and returning to it, these variables *keep* their pre exit values
        _minkoActivitySingleton = null;
    }

	// Setup
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.v("Minko/Java", "[MinkoActivity] onCreate():" + _minkoActivitySingleton);
        super.onCreate(savedInstanceState);

        MinkoActivity.initialize();

        // So we can call stuff from static callbacks
        _minkoActivitySingleton = this;
    }

	public static Context getContext() {
        return _minkoActivitySingleton;
    }

	// Open an URL in the default browser
    public static int openURL(String url)
    {
        Log.i("Minko/Java", "Open URL: "/* + url*/);

        try {
            if (!url.startsWith("https://") && !url.startsWith("http://")) {
                url = "http://" + url;
            }

            // Code from SDLActivity.java
            Intent i = new Intent(Intent.ACTION_VIEW);
            i.setData(Uri.parse(url));

            int flags = Intent.FLAG_ACTIVITY_NO_HISTORY | Intent.FLAG_ACTIVITY_MULTIPLE_TASK;
            if (Build.VERSION.SDK_INT >= 21) {
                flags |= Intent.FLAG_ACTIVITY_NEW_DOCUMENT;
            } else {
                flags |= Intent.FLAG_ACTIVITY_CLEAR_WHEN_TASK_RESET;
            }
            i.addFlags(flags);

            mSingleton.startActivity(i);
        } catch (Exception e) {
            Toast.makeText(mSingleton, "No application can handle this request."
                    + " Please install a webbrowser", Toast.LENGTH_LONG).show();
            e.printStackTrace();
            return -1;
        }
        return 0;
    }

    @Override
    public void setOrientationBis(int w, int h, boolean resizable, String hint) {
        // This method override is necessary to prevent the SDL library from
        // applying its own orientation settings, which might conflict with
        // the orientation settings specified in the AndroidManifest.xml
        // file of SmartShape App.
        // By leaving the method empty, we ensure that the app respects the
        // orientation settings defined in AndroidManifest.xml.
    }
}
