package com.example.ece387_finalproject;

import android.content.Context;
import android.os.Bundle;
import android.content.Intent;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.Toast;

import java.util.List;

public class MainActivity  extends BlunoLibrary {
    private Button buttonScan;
    private Button buttonSerialQueryMeasure;
    private Button buttonSerialSendCounter;
    private Button buttonSerialQueryCounter;
    private Button buttonSerialSend;
    private EditText serialSendText;
    private TextView serialReceivedText;
    private int area;
    private int people;
    private int state;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        request(1000, new OnPermissionsResult() {
            @Override
            public void OnSuccess() {
                Toast.makeText(MainActivity.this,"权限请求成功",Toast.LENGTH_SHORT).show();
            }

            @Override
            public void OnFail(List<String> noPermissions) {
                Toast.makeText(MainActivity.this,"权限请求失败",Toast.LENGTH_SHORT).show();
            }
        });

        onCreateProcess();														//onCreate Process by BlunoLibrary


        serialBegin(115200);													//set the Uart Baudrate on BLE chip to 115200

        serialReceivedText=(TextView) findViewById(R.id.serialReveicedText);	//initial the EditText of the received data
        //serialSendText=(EditText) findViewById(R.id.serialSendText);			//initial the EditText of the sending data

        buttonScan = (Button) findViewById(R.id.buttonScan);					//initial the button for scanning the BLE device
        buttonScan.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                // TODO Auto-generated method stub

                buttonScanOnClickProcess();										//Alert Dialog for selecting the BLE device
            }
        });

        buttonSerialSendCounter = (Button) findViewById(R.id.buttonSerialSendCounter);		//initial the button for sending the data
        buttonSerialSendCounter.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                // TODO Auto-generated method stub

                serialSend(Integer.toString(area));				//send the data to the BLUNO
            }
        });

        buttonSerialQueryMeasure = (Button) findViewById(R.id.buttonSerialQueryMeasure);		//initial the button for sending the data
        buttonSerialQueryMeasure.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                // TODO Auto-generated method stub

                serialSend("query");				//send the data to the BLUNO
                state = 1;
            }
        });

        buttonSerialQueryCounter = (Button) findViewById(R.id.buttonSerialQueryCounter);		//initial the button for sending the data
        buttonSerialQueryCounter.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                // TODO Auto-generated method stub

                serialSend("query");				//send the data to the BLUNO
                state = 2;
            }
        });

        buttonSerialSend = (Button) findViewById(R.id.buttonSerialSend);		//initial the button for sending the data
        buttonSerialSend.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                // TODO Auto-generated method stub
                serialSend(serialSendText.getText().toString());				//send the data to the BLUNO
            }
        });
    }

    protected void onResume(){
        super.onResume();
        System.out.println("BlUNOActivity onResume");
        onResumeProcess();														//onResume Process by BlunoLibrary
    }



    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        onActivityResultProcess(requestCode, resultCode, data);					//onActivityResult Process by BlunoLibrary
        super.onActivityResult(requestCode, resultCode, data);
    }

    @Override
    protected void onPause() {
        super.onPause();
        onPauseProcess();														//onPause Process by BlunoLibrary
    }

    protected void onStop() {
        super.onStop();
        onStopProcess();														//onStop Process by BlunoLibrary
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        onDestroyProcess();														//onDestroy Process by BlunoLibrary
    }

    @Override
    public void onConectionStateChange(connectionStateEnum theConnectionState) {//Once connection state changes, this function will be called
        switch (theConnectionState) {											//Four connection state
            case isConnected:
                buttonScan.setText("Connected");
                break;
            case isConnecting:
                buttonScan.setText("Connecting");
                break;
            case isToScan:
                buttonScan.setText("Scan");
                break;
            case isScanning:
                buttonScan.setText("Scanning");
                break;
            case isDisconnecting:
                buttonScan.setText("isDisconnecting");
                break;
            default:
                break;
        }
    }

    @Override
    public void onSerialReceived(String theString) {							//Once connection data received, this function will be called
        // TODO Auto-generated method stub
        serialReceivedText.setText(theString);							//append the text into the EditText
        //The Serial data from the BLUNO may be sub-packaged, so using a buffer to hold the String is a good choice.
        ((ScrollView)serialReceivedText.getParent()).fullScroll(View.FOCUS_DOWN);

        if (state == 1) {
            area = Integer.valueOf(theString);
            state = 0;
        }
        else if (state == 2) {
            people = Integer.valueOf(theString);
            state = 0;
        }
    }

}