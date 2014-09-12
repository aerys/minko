package minko.plugin.htmloverlay;

import android.app.Activity;
import android.webkit.WebView;
import java.util.Vector;
import java.util.concurrent.Callable;
import java.util.concurrent.FutureTask;
import java.lang.Exception;
import static android.util.Log.*;

public class EvalJSManager 
{
	private Activity _sdlActivity;
	private WebView _webView;
	public static Vector<Integer> UniqueIds;
	
	public EvalJSManager(Activity sdlActivity, WebView webView)
	{
		_sdlActivity = sdlActivity;
		_webView = webView;
		UniqueIds = new Vector<Integer>();
	}
	
	public String evalJS(String js, int uniqueId)
	{
		EvalJSCallable evalJSCallable = new EvalJSCallable(_webView, js, uniqueId);
		FutureTask<String> task = new FutureTask<String>(evalJSCallable);

		EvalJSManager.UniqueIds.add(uniqueId);
		
		d("MINKOJAVA", "IndexOf(" + uniqueId + ") = " + EvalJSManager.UniqueIds.indexOf(uniqueId));
		
        _sdlActivity.runOnUiThread(task);
		
		String returnValue = "";
		try 
		{
			returnValue = task.get();
			EvalJSManager.UniqueIds.remove(uniqueId);
		}
		catch (Exception e) 
		{
			returnValue = e.getMessage();
		}
		
		return returnValue;
	}
}
