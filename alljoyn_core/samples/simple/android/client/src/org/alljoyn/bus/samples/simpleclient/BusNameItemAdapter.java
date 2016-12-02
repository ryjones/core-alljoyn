/*
 *  * 
 *    Copyright (c) 2016 Open Connectivity Foundation and AllJoyn Open
 *    Source Project Contributors and others.
 *    
 *    All rights reserved. This program and the accompanying materials are
 *    made available under the terms of the Apache License, Version 2.0
 *    which accompanies this distribution, and is available at
 *    http://www.apache.org/licenses/LICENSE-2.0

 *
 */
package org.alljoyn.bus.samples.simpleclient;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

public class BusNameItemAdapter extends ArrayAdapter<BusNameItem> {

    private Client client;
    private Context context;

    public BusNameItemAdapter(Context context, Client client) {
        super(context, R.id.BusName);
        this.context = context;
        this.client = client;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        LinearLayout itemLayout;
        itemLayout = (LinearLayout) LayoutInflater.from(context).inflate(R.layout.service, parent, false);

        final BusNameItem n = getItem(position);
        final TextView busNameView = (TextView) itemLayout.findViewById(R.id.BusName);
        final Client c = client;

        final Button connectButton = (Button) itemLayout.findViewById(R.id.Connect);
        final Button pingButton = (Button) itemLayout.findViewById(R.id.Ping);
        final BusNameItemAdapter adapter = this;

        connectButton.setText(n.isConnected() ? "Disconnect" : "Connect");
        connectButton.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                final int id = n.getSessionId();
                if (id != 0) {
                    c.getBusHandler().post(new Runnable() {
                        public void run() {
                            c.leaveSession(id);
                            c.getUIHandler().post(new Runnable() {
                                public void run() {
                                    n.setSessionId(0);
                                    connectButton.setText("Connect");
                                    pingButton.setEnabled(false);
                                    c.hideInputMethod();
                                    busNameView.invalidate();
                                }
                            });
                        }
                    });
                }
                else {
                    c.getBusHandler().post(new Runnable() {
                        public void run() {
                            final int sessionId = c.joinSession(n.getBusName());
                            c.getUIHandler().post(new Runnable() {
                                public void run() {
                                    if (sessionId != 0) {
                                        n.setSessionId(sessionId);
                                        connectButton.setText("Disconnect");
                                        pingButton.setEnabled(true);
                                    }
                                    else {
                                        adapter.remove(n);
                                    }

                                    c.hideInputMethod();
                                    busNameView.invalidate();
                                }
                            });
                        }
                    });
                }
                //c.hideInputMethod();
                //busNameView.invalidate();
            }
        });

        pingButton.setEnabled(n.isConnected());
        pingButton.setTag(R.id.client, client);
        pingButton.setTag(R.id.foundName, n);
        pingButton.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                c.ping(n.getSessionId(), n.getBusName());
                c.hideInputMethod();
            }
        });

        busNameView.setText(n.getBusName());
        return itemLayout;
    }
}