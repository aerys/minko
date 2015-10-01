package {{PACKAGE}};

import android.app.Activity;
import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.Intent;
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

	public static void initialize() {
		Log.v("minko-java", "[MinkoActivity] initialize()");

        // The static nature of the singleton and Android quirkyness force us to initialize everything here
        // Otherwise, when exiting the app and returning to it, these variables *keep* their pre exit values
        _minkoActivitySingleton = null;
    }

	// Setup
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.v("minko-java", "[MinkoActivity] onCreate():" + _minkoActivitySingleton);
        super.onCreate(savedInstanceState);
        
        MinkoActivity.initialize();

        // So we can call stuff from static callbacks
        _minkoActivitySingleton = this;
    }

	public static Context getContext() {
        return _minkoActivitySingleton;
    }

	// Open an URL in the default browser
    public void openURL(String url)
    {
        Log.i("minko-java", "Open URL: "/* + url*/);

        try {
            if (!url.startsWith("https://") && !url.startsWith("http://")) {
                url = "http://" + url;
            }

            Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
            startActivity(intent);
        } catch (ActivityNotFoundException e) {
            Toast.makeText(this, "No application can handle this request."
                    + " Please install a webbrowser", Toast.LENGTH_LONG).show();
            e.printStackTrace();
        }
    }
}