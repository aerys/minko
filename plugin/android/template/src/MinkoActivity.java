package {{PACKAGE}};

import org.libsdl.app.*;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.util.Log;
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
		if (intent != null && intent.getData() != null && intent.getData().getPath() != null)
		{	
			String filename = null; 
			String intentDataString = intent.getDataString();
		
			if (intentDataString.startsWith("content://"))
			{
				Uri uri = Uri.parse(intentDataString);
				filename = getRealPathFromURI(this.getApplicationContext(), uri);
			}
			else
				filename = intent.getData().getPath();
			
			Log.v("minko-java", "[MinkoActivity]Get filename: " + filename);
			
			SDLActivity.onNativeDropFile(filename);
		}
	}
	
	private String getRealPathFromURI(Context context, Uri contentUri) 
	{
        Cursor cursor = null;
		
        try 
		{
			String filePath;
			String[] filePathColumn = {MediaStore.Images.Media.DATA};

			cursor = context.getContentResolver().query(contentUri, filePathColumn, null, null, null);
			cursor.moveToFirst();
			
			int columnIndex = cursor.getColumnIndex(filePathColumn[0]);
			
			filePath = cursor.getString(columnIndex);
			
			cursor.close();
			
			return filePath;
        }
		finally 
		{
            if (cursor != null) 
			{
                cursor.close();
            }
        }
    }
}