package minko.file;

import android.app.Activity;
import android.content.Context;
import android.content.res.AssetManager;
import android.content.pm.PackageManager;
import android.content.pm.ApplicationInfo;
import android.util.Log;
import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.zip.ZipInputStream;
import java.util.zip.ZipEntry;
import org.libsdl.app.SDLActivity;

public class AndroidUnzip
{
    private Activity _sdlActivity;
    private final Context _context;

    public AndroidUnzip(Activity sdlActivity)
    {
        _sdlActivity = sdlActivity;
        _context = SDLActivity.getContext();
    }

    public String extractFromAsset(String archivePath) throws IOException
    {
        Log.d("Minko/Java", "AndroidUnzip::extractFromAsset() " + archivePath);
        AssetManager assetManager = _context.getAssets();
        File targetDirectory = _context.getFilesDir();

        PackageManager packageManager =  _context.getPackageManager();

        long lastUpdateTime;

        try
        {
            ApplicationInfo appInfo = packageManager.getApplicationInfo(_context.getPackageName(), 0);
            String appFileDir = appInfo.sourceDir;
            File appFile = new File(appFileDir);
            lastUpdateTime = appFile.lastModified();
        }
        catch(PackageManager.NameNotFoundException e)
        {
            lastUpdateTime = 0;
        }

        File indexJs = new File(targetDirectory, "index.js");
        long lastUnzipTime = indexJs.lastModified();

        Log.d("Minko/Java", "AndroidUnzip::extractFromAsset() lastUnzipTime " + lastUnzipTime);
        Log.d("Minko/Java", "AndroidUnzip::extractFromAsset() lastUpdateTime " + lastUpdateTime);

        if (lastUpdateTime == 0 || lastUnzipTime < lastUpdateTime)
        {
            ZipInputStream zip = new ZipInputStream(new BufferedInputStream(assetManager.open(archivePath)));

            try
            {
                ZipEntry ze;
                int count;
                byte[] buffer = new byte[8192];

                while ((ze = zip.getNextEntry()) != null)
                {
                    File file = new File(targetDirectory, ze.getName());
                    File dir = ze.isDirectory() ? file : file.getParentFile();

                    if (!dir.isDirectory() && !dir.mkdirs())
                        throw new FileNotFoundException("Failed to ensure directory: " + dir.getAbsolutePath());

                    if (ze.isDirectory())
                        continue;

                    FileOutputStream fout = new FileOutputStream(file);

                    try
                    {
                        while ((count = zip.read(buffer)) != -1)
                            fout.write(buffer, 0, count);
                    }
                    finally
                    {
                        fout.close();
                    }
                }
            }
            finally
            {
                zip.close();
            }
        }
        else
        {
            Log.d("Minko/Java", "AndroidUnzip::extractFromAsset() server up to date, not extracting");
        }

        return targetDirectory.getPath();
    }
}
