package {{PACKAGE}};

import org.libsdl.app.*;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import java.util.Arrays;
import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;

/**
    Minko Activity, Java entry point
*/
public class MinkoActivity extends SDLActivity
{
	@Override
    protected void onCreate(Bundle savedInstanceState) 
	{
		super.onCreate(savedInstanceState);
		
		Intent intent = getIntent();
		
		nativeDropFile(intent);
	}
	
	@Override
	protected void onNewIntent(Intent intent)
	{
		super.onNewIntent(intent);
		
		nativeDropFile(intent);
	}
	
	// Get real filepath from intent URI and fire SDL DropFile event
	private void nativeDropFile(Intent intent)
	{
		String filepath = null;
	
		if (intent.getData() != null) 
		{
			Log.d("minko-java", "[MinkoActivity] Intent DataString: " + intent.getDataString());
			Log.d("minko-java", "[MinkoActivity] Intent Scheme: " + intent.getData().getScheme());
				
			// Content URI
			if (intent.getData().getScheme() != null && intent.getData().getScheme().equals("content"))
			{
				Cursor cursor = this.getContentResolver().query(intent.getData(), null, null, null, null);
			
				Log.d("minko-java", "[MinkoActivity] Intent Host: " + intent.getData().getHost());
				
				String dataColumnName = "_data";
				String displayNameColumnName = "_display_name";
				
				// We can access to the filepath (like browser apps)
				if(Arrays.asList(cursor.getColumnNames()).contains(dataColumnName)) 
				{
					int columnIndex = cursor.getColumnIndexOrThrow(dataColumnName);
					if (cursor.moveToFirst())
						filepath = cursor.getString(columnIndex);
				}
				// We can't access to the filepath => we download the stream to a temporary file (like mail-based apps)
				else
				{
					Log.d("minko-java", "[MinkoActivity] Unable to find \"_data\" value into column names from this content URI."); 
					
					try 
					{
						cursor.moveToFirst();
						
						String filename = cursor.getString(cursor.getColumnIndex(displayNameColumnName));
						
						String tempFolders = Environment.getExternalStorageDirectory().getPath() + "/Download/Minko";
						String tempFilepath = tempFolders + "/" + filename;
						
						// Create temporary folders if they don't exist
						File folder = new File(tempFolders);
						boolean success = true;
						
						if (!folder.exists())
						{
							Log.i("minko-java", "[MinkoActivity] Temporary folder doesn't exist, we have to create it.");
							success = folder.mkdir();
						}
						
						if (success) 
						{
							Log.i("minko-java", "[MinkoActivity] Download file " + filename + " from content URI to temporary file. (location: " + tempFilepath + ")");

							InputStream is = getContentResolver().openInputStream(intent.getData());
							OutputStream os = new FileOutputStream(tempFilepath);
						
							byte[] buffer = new byte[1024];
							int bytesRead = 0;
							
							while((bytesRead = is.read(buffer)) >= 0) 
							{
								os.write(buffer, 0, bytesRead);
							}
							
							os.close();
							is.close();
							
							filepath = tempFilepath;
						} 
						else
							Log.e("minko-java", "[MinkoActivity] Can't create the temporary folders, please check the permissions into your AndroidManifest.xml file.");
					} 
					catch (Exception e) 
					{
						Log.e("minko-java", "[MinkoActivity] Couldn't download file from mail URI: \"" + e.getMessage() + "\"");
						
						cursor.close();
					}
				}
				
				cursor.close();
			}
			// We have the filepath directly (like Dropbox or file browser apps)
			else if (intent.getData().getPath() != null)
			{
				filepath = intent.getData().getPath();
			}
			
			Log.i("minko-java", "[MinkoActivity] Filepath for SDL DropFile event: " + filepath);
			SDLActivity.onNativeDropFile(filepath);
		}
	}
}