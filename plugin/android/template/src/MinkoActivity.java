package {{PACKAGE}};

import org.libsdl.app.*;
import android.app.Activity;
import android.content.Intent;
import android.util.Log;
import android.os.Bundle;

/**
    Minko Activity, Java entry point
*/
public class MinkoActivity extends SDLActivity
{
	@Override
    protected void onCreate(Bundle savedInstanceState) 
	{
		super.onCreate(savedInstanceState);
	}

	@Override
	protected void onNewIntent(Intent intent)
	{
		super.onNewIntent(intent);

		if (intent != null && intent.getData() != null && intent.getData().getPath() != null)
		{
			String filename = intent.getData().getPath();

			Log.v("MINKOJAVA", "Get filename: " + filename);

			SDLActivity.onNativeDropFile(filename);
		}
	}
}