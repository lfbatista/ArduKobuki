package pt.ipg.ardukobuki.controller;

import android.app.Activity;
import android.os.AsyncTask;
import android.os.Bundle;
import android.speech.tts.TextToSpeech;
import android.util.Log;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.Locale;

import edu.cmu.pocketsphinx.Assets;
import edu.cmu.pocketsphinx.Hypothesis;
import edu.cmu.pocketsphinx.RecognitionListener;
import edu.cmu.pocketsphinx.SpeechRecognizer;
import edu.cmu.pocketsphinx.demo.R;

import static android.widget.Toast.makeText;
import static edu.cmu.pocketsphinx.SpeechRecognizerSetup.defaultSetup;

public class MainActivity extends Activity implements
        RecognitionListener, TextToSpeech.OnInitListener {

    private static final String DEVICE_ADDRESS = "20:14:10:15:07:28";

    private static final String KWS_SEARCH = "wakeup";
    private static final String COMMANDS_SEARCH = "commands";
    private static final String KEYPHRASE = "hello robot";

    private SpeechRecognizer recognizer;
    private HashMap<String, Integer> captions;
    private TextToSpeech tts;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

        tts = new TextToSpeech(this, this);

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

    // Views
    public void onClickBtnActivate(View v)
    {
        Toast.makeText(this, "Hello robot!", Toast.LENGTH_SHORT).show();
        Amarino.sendDataToArduino(this, DEVICE_ADDRESS, ' ', "hello robot");
        tts.speak("Hello Robot!", TextToSpeech.QUEUE_FLUSH, null);
    }

    public void onClickBtnExplore(View v)
    {
        Toast.makeText(this, "Go explore!", Toast.LENGTH_SHORT).show();
        Amarino.sendDataToArduino(this, DEVICE_ADDRESS, ' ', "go explore");
        tts.speak("Kobuki, explore!", TextToSpeech.QUEUE_FLUSH, null);
    }

    public void onClickBtnUp(View v)
    {
        Toast.makeText(this, "Go forward!", Toast.LENGTH_SHORT).show();
        Amarino.sendDataToArduino(this, DEVICE_ADDRESS, ' ', "go forward");
        tts.speak("Kobuki, go forward!", TextToSpeech.QUEUE_FLUSH, null);
    }

    public void onClickBtnLeft(View v)
    {
        Toast.makeText(this, "Turn left!", Toast.LENGTH_SHORT).show();
        Amarino.sendDataToArduino(this, DEVICE_ADDRESS, ' ', "turn left");
        tts.speak("Kobuki, turn left!", TextToSpeech.QUEUE_FLUSH, null);
    }

    public void onClickBtnPause(View v)
    {
        Toast.makeText(this, "Stop!", Toast.LENGTH_SHORT).show();
        Amarino.sendDataToArduino(this, DEVICE_ADDRESS, ' ', "stop");
        tts.speak("Kobuki, stop!", TextToSpeech.QUEUE_FLUSH, null);
    }

    public void onClickBtnRight(View v)
    {
        Toast.makeText(this, "Turn right!", Toast.LENGTH_SHORT).show();
        Amarino.sendDataToArduino(this, DEVICE_ADDRESS, ' ', "turn right");
        tts.speak("Kobuki, turn right!", TextToSpeech.QUEUE_FLUSH, null);
    }

    public void onClickBtnDown(View v)
    {
        Toast.makeText(this, "Go backwards!", Toast.LENGTH_SHORT).show();
        Amarino.sendDataToArduino(this, DEVICE_ADDRESS, ' ', "backwards");
        tts.speak("Kobuki, go backwards!", TextToSpeech.QUEUE_FLUSH, null);
    }

    public void onClickBtnFaster(View v)
    {
        Toast.makeText(this, "Go faster!", Toast.LENGTH_SHORT).show();
        Amarino.sendDataToArduino(this, DEVICE_ADDRESS, ' ', "go faster");
        tts.speak("Kobuki, go faster!", TextToSpeech.QUEUE_FLUSH, null);
    }

    public void onClickBtnSlower(View v)
    {
        Toast.makeText(this, "Go slower!", Toast.LENGTH_SHORT).show();
        Amarino.sendDataToArduino(this, DEVICE_ADDRESS, ' ', "go slower");
        tts.speak("Kobuki, go slower!", TextToSpeech.QUEUE_FLUSH, null);
    }

    public void onClickBtnPanic(View v)
    {
        Toast.makeText(this, "Kobuki stoped.", Toast.LENGTH_SHORT).show();
        Amarino.sendDataToArduino(this, DEVICE_ADDRESS, ' ', "exit");
        tts.speak("Kobuki stoped.", TextToSpeech.QUEUE_FLUSH, null);
    }

    @Override
    public void onStart() {
        super.onStart();
        Amarino.connect(this, DEVICE_ADDRESS);
    }

    @Override
    public void onDestroy() {
        if (tts != null) {
            tts.stop();
            tts.shutdown();
        }
        super.onDestroy();
    }

    @Override
    public void onInit(int status) {
        // TODO Auto-generated method stub

        if (status == TextToSpeech.SUCCESS) {
            int result = tts.setLanguage(Locale.US);

            if (result == TextToSpeech.LANG_MISSING_DATA
                    || result == TextToSpeech.LANG_NOT_SUPPORTED) {
                Log.e("TTS", "Language is not supported");
            } else {
                Log.e("TTS", "Everything is set...");
            }

        } else {
            Log.e("TTS", "Initilization Failed");
        }
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

    @Override
    public void onBeginningOfSpeech() {    }

    private void setupRecognizer(File assetsDir) throws IOException {
        // The recognizer can be configured to perform multiple searches
        // of different kind and switch between them
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

        // Grammar
        File commandsGrammar = new File(assetsDir, "commands.gram");
        recognizer.addGrammarSearch(COMMANDS_SEARCH, commandsGrammar);
        recognizer.startListening(COMMANDS_SEARCH, 10000);
    }

    @Override
    public void onEndOfSpeech() {
        if (!recognizer.getSearchName().equals(KWS_SEARCH))
            switchSearch(KWS_SEARCH);
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
            makeText(getApplicationContext(), text + "!", Toast.LENGTH_SHORT).show();
            tts.speak("Kobuki!" + text, TextToSpeech.QUEUE_FLUSH, null);
            Amarino.sendDataToArduino(this, DEVICE_ADDRESS, ' ', text);
        }
    }

    @Override
    public void onTimeout() {
        switchSearch(KWS_SEARCH);
    }

    @Override
    public void onError(Exception e) {    }
}