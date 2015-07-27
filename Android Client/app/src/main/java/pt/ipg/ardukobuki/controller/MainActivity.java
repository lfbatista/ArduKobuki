package pt.ipg.ardukobuki.controller;

import android.app.Activity;
import android.os.AsyncTask;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.io.IOException;
import java.util.HashMap;

import edu.cmu.pocketsphinx.Assets;
import edu.cmu.pocketsphinx.Hypothesis;
import edu.cmu.pocketsphinx.RecognitionListener;
import edu.cmu.pocketsphinx.SpeechRecognizer;
import edu.cmu.pocketsphinx.demo.R;

import static android.widget.Toast.makeText;
import static edu.cmu.pocketsphinx.SpeechRecognizerSetup.defaultSetup;

public class MainActivity extends Activity implements
        RecognitionListener {

    private static final String DEVICE_ADDRESS = "20:14:10:15:07:28";

    private static final String KWS_SEARCH = "wakeup";
    private static final String COMMANDS_SEARCH = "commands";
    private static final String KEYPHRASE = "hello robot";

    private SpeechRecognizer recognizer;
    private HashMap<String, Integer> captions;

    @Override
    public void onCreate(Bundle state) {
        super.onCreate(state);

        Amarino.connect(this, DEVICE_ADDRESS);

        captions = new HashMap<String, Integer>();
        captions.put(KWS_SEARCH, R.string.kws_caption);
        captions.put(COMMANDS_SEARCH, R.string.commands_caption);
        setContentView(R.layout.main);
        ((TextView) findViewById(R.id.caption_text))
                .setText("Welcome!\n\nInitializing Kobuki Controller...");

        new AsyncTask<Void, Void, Exception>() {
            @Override
            protected Exception doInBackground(Void... params) {
                try {
                    Assets assets = new Assets(MainActivity.this);
                    File assetDir = assets.syncAssets();
                    setupRecognizer(assetDir);
                } catch (IOException e) {
                    return e;
                }
                return null;
            }

            @Override
            protected void onPostExecute(Exception result) {
                if (result != null) {
                    ((TextView) findViewById(R.id.caption_text))
                            .setText("Oh-oh, something has failed: " + result);
                } else {
                    switchSearch(KWS_SEARCH);
                }
            }
        }.execute();
    }

    public void onClickBtn1(View v)
    {
        Toast.makeText(this, "Kobuki stoped", Toast.LENGTH_SHORT).show();
        Amarino.sendDataToArduino(this, DEVICE_ADDRESS, ' ', "stop");
    }

    @Override
    public void onStart() {
        super.onStart();
        Amarino.connect(this, DEVICE_ADDRESS);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        //  recognizer.cancel();
     //   recognizer.shutdown();
    }

    @Override
    public void onStop(){
        super.onStop();
    }

    @Override
    public void onPartialResult(Hypothesis hypothesis) {
        if (hypothesis == null)
            return;

        String text = hypothesis.getHypstr();
        if (text.equals(KEYPHRASE))
            switchSearch(COMMANDS_SEARCH);
    }

    @Override
    public void onResult(Hypothesis hypothesis) {
        ((TextView) findViewById(R.id.result_text)).setText("");
        if (hypothesis != null) {
            String text = hypothesis.getHypstr();
            makeText(getApplicationContext(), text, Toast.LENGTH_SHORT).show();
            Amarino.sendDataToArduino(this, DEVICE_ADDRESS, ' ', text);
        }
    }

    @Override
    public void onBeginningOfSpeech() {}

    @Override
    public void onEndOfSpeech() {
        if (!recognizer.getSearchName().equals(KWS_SEARCH))
            switchSearch(KWS_SEARCH);
    }

    private void switchSearch(String searchName) {
        recognizer.stop();

        if (searchName.equals(KWS_SEARCH))
            recognizer.startListening(searchName);
        else
            recognizer.startListening(searchName, 20000);

        String caption = getResources().getString(captions.get(searchName));
        ((TextView) findViewById(R.id.caption_text)).setText(caption);
    }

    private void setupRecognizer(File assetsDir) throws IOException {
        recognizer = defaultSetup()
                .setAcousticModel(new File(assetsDir, "en-us-ptm"))
                .setDictionary(new File(assetsDir, "cmudict-en-us.dict"))
                // To disable logging of raw audio comment out this call (takes a lot of space on the device)
                .setRawLogDir(assetsDir)
                // Threshold to tune for keyphrase to balance between false alarms and misses
                .setKeywordThreshold(1e-45f)
                // Use context-independent phonetic search, context-dependent is too slow for mobile
                .setBoolean("-allphone_ci", true)
                .getRecognizer();
        recognizer.addListener(this);

        recognizer.addKeyphraseSearch(KWS_SEARCH, KEYPHRASE);

        File commandsGrammar = new File(assetsDir, "commands.gram");
        recognizer.addGrammarSearch(COMMANDS_SEARCH, commandsGrammar);
        recognizer.startListening(COMMANDS_SEARCH, 10000);
    }

    @Override
    public void onError(Exception error) {
        ((TextView) findViewById(R.id.caption_text)).setText(error.getMessage());
    }

    @Override
    public void onTimeout() {
        switchSearch(KWS_SEARCH);
    }
}