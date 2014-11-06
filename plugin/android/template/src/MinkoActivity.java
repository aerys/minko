package {{PACKAGE}};

import org.libsdl.app.*;
import android.app.Activity;
import android.content.Intent;
import android.util.Log;
import android.os.Bundle;
import android.provider.OpenableColumns;
import android.database.Cursor;
import java.io.InputStream;
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
			String filename = null;
			Long filesize = null;
			Cursor cursor = null;
			
			try 
			{
				cursor = this.getContentResolver().query(
					intent.getData(), 
					new String[] { OpenableColumns.DISPLAY_NAME, OpenableColumns.SIZE }, 
					null, null, null
				);
				
				if (cursor != null && cursor.moveToFirst()) 
				{
					filename = cursor.getString(0);
					filesize = cursor.getLong(1);
				}
			} 
			finally 
			{
				if (cursor != null)
					cursor.close();
			}
			
			Log.v("MINKOJAVA", "[onNewIntent]Filename: " + filename);
		
			if (filename == null)
				filename = intent.getData().getPath();
			
			Log.v("MINKOJAVA", "[onNewIntent]Get filename: " + filename);
			
			SDLActivity.onNativeDropFile(filename);
		}
	}
}