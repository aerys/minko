package minko.plugin.htmloverlay;

import android.app.Activity;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.util.Log;
import android.webkit.JavascriptInterface;
import android.webkit.WebView;
import android.widget.Toast;
import java.io.IOException;
import java.io.File;
import java.io.OutputStreamWriter;
import java.io.FileOutputStream;
import android.os.Environment;

public class WebViewJSInterface
{
	private Activity _activity;

	public static boolean ResultReady = false;
	public static String Result = "[UNKNOWN]";

	// Native functions
	public native void minkoNativeOnMessage(String message);
	public native void minkoNativeOnEvent(String accessor, String eventData);
	
	public WebViewJSInterface(Activity activity)
	{
		_activity = activity;
	}

	@JavascriptInterface
	public void onMessage(String message)
	{
		minkoNativeOnMessage(message);
	}
	
	@JavascriptInterface
	public void onEvent(String accessor, String eventData)
	{
		Log.i("minko-java", "[WebViewJSInterface] onEvent: " + eventData);
		minkoNativeOnEvent(accessor, eventData);
	}

	@JavascriptInterface
	public void onCopy(String content, boolean showToast)
	{
		Log.i("minko-java", "[WebViewJSInterface] onCopy: " + content);

		// Gets a handle to the clipboard service.
        ClipboardManager clipboard = (ClipboardManager)_activity.getSystemService(Context.CLIPBOARD_SERVICE);
        ClipData clip = ClipData.newPlainText("text", content);

        clipboard.setPrimaryClip(clip);

        if (showToast)
        {
        	Toast.makeText(_activity, "Content successfully copied into clipboard.", Toast.LENGTH_LONG).show();
        }
	}
	
	@JavascriptInterface
	public void onNativeJSResult(String js, String jsResult)
	{
		Log.d("minko-java", "[WebViewJSInterface] onNativeJSResult: Get result of: \"" + js + "\"");
		Log.d("minko-java", "[WebViewJSInterface] onNativeJSResult: Result: \"" + jsResult + "\"");
		
		WebViewJSInterface.Result = jsResult;
		WebViewJSInterface.ResultReady = true;
	}

	@JavascriptInterface
	public void onSaveSnippetResult(String filename, String result)
	{
		Log.i("minko-java", "[WebViewJSInterface] onSaveSnippetResult: " + filename + ": " + result);
		try
		{
			int num = 0;
			File outputFile = new File(Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS), filename);

			// Separate name from format in filename
			int dotIndex = filename.lastIndexOf(".");
			String name = filename.substring(0, dotIndex);
			String format = filename.substring(dotIndex + 1);

			// Append a number to the name if a file with the same name already exists
			while (outputFile.exists())
			{
				num++;
				String newName = name + "(" + num + ")." + format;
				outputFile = new File(Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS), newName);
			}

			// Create new empty file
			outputFile.createNewFile();

			// Fill the newly created file
			if (outputFile.exists())
			{
				FileOutputStream fOut = new FileOutputStream(outputFile);
				OutputStreamWriter osw = new OutputStreamWriter(fOut);
				osw.write(result);
				osw.close();
				Toast.makeText(_activity, outputFile.getName() + " saved in Download.", Toast.LENGTH_LONG).show();
			}
		}
		catch (IOException ioe) 
		{
			ioe.printStackTrace();
		}
	}
}