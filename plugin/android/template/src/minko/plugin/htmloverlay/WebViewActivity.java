package minko.plugin.htmloverlay;

import org.libsdl.app.*;
import android.app.Activity;
import android.content.Context;
import android.graphics.Color;
import android.view.LayoutInflater;
import android.view.View;
import android.webkit.WebView;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.os.Bundle;

import static android.util.Log.*;

/**
    WebView Activity
*/
public class WebViewActivity extends SDLActivity
{
    @Override
    protected void onCreate(Bundle savedInstanceState) 
    {
		super.onCreate(savedInstanceState);
/*		LinearLayout dynamic = (LinearLayout)findViewById(R.id.mainview);
		LayoutInflater inflater = (LayoutInflater)getSystemService(Context.LAYOUT_INFLATER_SERVICE);

		View v = (FrameLayout) inflater.inflate(R.layout.webview, null);

		WebView wv = (WebView)v.findViewById(R.id.webView);
		wv.setBackgroundColor(Color.GREEN);
		v.getRootView();
		mLayout.addView(v);
		v.setVisibility(View.VISIBLE);
		//wv.loadUrl("http://www.final-rpg.com");
		wv.loadUrl("file:///android_asset/iframe.html");*/

		initWebView();
	}

	public native void initWebView();

	public void helloWorld()
	{
		d("Minko", "HELLO WORLD!");
	}
}
