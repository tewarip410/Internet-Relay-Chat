/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:

**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>
#include "dialog.h"
#include <QMessageBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QListWidgetItem>
#include <iostream>

#include <time.h>
#include <curses.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

char * host = "localhost";
char * user;
char * password;
char * sport;
int port = 2445;
bool inRoom = true;
char * currentRoom = strdup("");
char * args = (char *) malloc (300 * sizeof(char));

#define MAX_MESSAGES 100
#define MAX_MESSAGE_LEN 300
#define MAX_RESPONSE (20 * 1024)
using namespace std;

int Dialog::open_client_socket(char * host, int port) {
    // Initialize socket address structure
    struct  sockaddr_in socketAddress;

    // Clear sockaddr structure
    memset((char *)&socketAddress,0,sizeof(socketAddress));

    // Set family to Internet
    socketAddress.sin_family = AF_INET;

    // Set port
    socketAddress.sin_port = htons((u_short)port);

    // Get host table entry for this host
    struct  hostent  *ptrh = gethostbyname(host);
    if ( ptrh == NULL ) {
        perror("gethostbyname");
        exit(1);
    }

    // Copy the host ip address to socket address structure
    memcpy(&socketAddress.sin_addr, ptrh->h_addr, ptrh->h_length);

    // Get TCP transport protocol entry
    struct  protoent *ptrp = getprotobyname("tcp");
    if ( ptrp == NULL ) {
        perror("getprotobyname");
        exit(1);
    }

    // Create a tcp socket
    int sock = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    // Connect the socket to the specified server
    if (::connect(sock, (struct sockaddr *)&socketAddress,
            sizeof(socketAddress)) < 0) {
        perror("connect");
        exit(1);
    }

    return sock;
}

void Dialog::sendCommand(char * host, int port, char * command, char * user,
        char * password, char * args, char * response) {
    int sock = open_client_socket( host, port);

    // Send command
    write(sock, command, strlen(command));
    write(sock, " ", 1);
    write(sock, user, strlen(user));
    write(sock, " ", 1);
    write(sock, password, strlen(password));
    write(sock, " ", 1);
    write(sock, args, strlen(args));
    write(sock, "\r\n",2);

    // Keep reading until connection is closed or MAX_REPONSE
    int n = 0;
    int len = 0;
    while ((n=read(sock, response+len, MAX_RESPONSE - len))>0) {
        len += n;
    }

    //printf("response:%s\n", response);

    ::close(sock);
}

void Dialog::sendAction()
{
    printf("Send Button\n");
    //cout << inputMessage->toPlainText().toStdString().c_str() << endl;

    char buffer[MAX_RESPONSE];

    memset(buffer,'\0', sizeof(buffer));
    args = strdup(currentRoom);
    strcat(args," ");
    strcat(args, inputMessage->toPlainText().toStdString().c_str());

    sendCommand(host, port, "SEND-MESSAGE", user, password, args, buffer);

    cout << buffer << endl;


    char response2[MAX_RESPONSE];
    memset(response2, '\0', sizeof(response2));

    char *args2 = (char*)malloc(sizeof(char*));
    args2 = strdup("");
    strcat(args2, "-1 ");
    strcat(args2, currentRoom);

    sendCommand(host,port, "GET-MESSAGES", user, password, args2 , response2);
    cout << response2 << endl;
    allMessages->clear();

    allMessages->append(response2);

  /*  char * separate = strtok(response2, "\r\n");

    while(separate!=NULL)
    {
            allMessages->setText(separate);
            separate = strtok(NULL, "\r\n");
    }*/

    inputMessage->clear();
}

void Dialog::newUserAction()
{
    printf("New User Button\n");

    cout<<un->text().toStdString()<<endl;
    cout<<pw->text().toStdString()<<endl;

    user = strdup(un->text().toStdString().c_str());
    password = strdup(pw->text().toStdString().c_str());

    char newbuffer[MAX_RESPONSE];

    memset(newbuffer,'\0', sizeof(newbuffer));

    sendCommand(host, port, "ADD-USER", user, password, user, newbuffer);

    cout << newbuffer << endl;
    QMessageBox msgBox;

    if(!strcmp(newbuffer,"DENIED\r\n"))
    {
        msgBox.setText("User already exists");
        msgBox.exec();

    }
    else
    {
        msgBox.setText("Account creation successful!");
        msgBox.exec();
    }
    currentUserLineEdit->setText(user);

    timer = new QTimer(this);
    connect(timer, SIGNAL (timeout()), this, SLOT (timerAction()));
    timer->start(1000);


   /* un->setHidden(true);
    pw->setHidden(true);*/

        un->clear();
        pw->clear();


}

void Dialog::createRoomAction()
{
    printf("Create Room Button\n");

    cout << cr->text().toStdString().c_str()<<endl;

    char response[MAX_RESPONSE];
    memset(response, '\0', sizeof(response));

    sendCommand(host, port, "CREATE-ROOM", user, password,strdup(cr->text().toStdString().c_str()) , response);
    cout << response << endl;

    char response2[MAX_RESPONSE];
    memset(response2, '\0', sizeof(response2));

    sendCommand(host,port, "LIST-ROOMS", user, password, strdup(cr->text().toStdString().c_str()), response2);
    cout << response2 << endl;

    roomsList->clear();

    char * separate = strtok(response2, "\r\n");

    while(separate!=NULL)
    {
            roomsList->addItem(separate);
            separate = strtok(NULL, "\r\n");
    }

   // roomsList->itemDoubleClicked();
    QMessageBox msgBox;
    msgBox.setText("New room creation successful!");
    msgBox.exec();

    cr->clear();
}

void Dialog::enterRoomAction(QListWidgetItem* item)
{

    //Enters room

    printf("Enter Room\n");

    char response[MAX_RESPONSE];
    memset(response, '\0', sizeof(response));

    string rTemp = item->text().toStdString();

    char*rName = new char[rTemp.length()+1];
    strcpy(rName,rTemp.c_str());
    //currentRoom = rName;
    strcpy(currentRoom, rName);

    sendCommand(host, port, "ENTER-ROOM", user, password, rName, response);
    cout << response << endl;

    char thisbuffer[MAX_RESPONSE];

    memset(thisbuffer,'\0', sizeof(thisbuffer));
    args = strdup(rName);
    strcat(args," has entered room");
    sendCommand(host, port, "SEND-MESSAGE", user, password, args, thisbuffer);

    currentRoomLineEdit->setText(rName);


}

void Dialog::leaveRoomAction()
{
      printf("Leave Room\n");



      cout<<lr->text().toStdString()<<endl;

      char buffer2[MAX_RESPONSE];

      memset(buffer2,'\0', sizeof(buffer2));
      args = strdup(currentRoom);
      strcat(args," has left room");
      sendCommand(host, port, "SEND-MESSAGE", user, password, args, buffer2);

      char buffer[MAX_RESPONSE];

      memset(buffer,'\0', sizeof(buffer));

      sendCommand(host, port, "LEAVE-ROOM", user, password, strdup(currentRoom), buffer);
      cout << buffer << endl;

      char response2[MAX_RESPONSE];
      memset(response2, '\0', sizeof(response2));

      sendCommand(host, port, "GET-USERS-IN-ROOM", user, password, strdup(currentRoom), response2);
      cout << response2 << endl;
      usersList->clear();

      char * separate = strtok(response2, "\r\n");

      while(separate!=NULL)
      {
              usersList->addItem(separate);
              separate = strtok(NULL, "\r\n");
      }

      lr->clear();
}

void Dialog::timerAction()
{

    printf("Timer wakeup\n");



    char response2[MAX_RESPONSE];
    memset(response2, '\0', sizeof(response2));

    sendCommand(host, port, "LIST-ROOMS", user, password, "", response2);
    cout << response2 << endl;
     char * separate = strtok(response2, "\r\n");
        roomsList->clear();
        while(separate!=NULL)
        {
                roomsList->addItem(separate);
                separate = strtok(NULL, "\r\n");
        }

    char response3[MAX_RESPONSE];
    memset(response3, '\0', sizeof(response3));

    sendCommand(host, port, "GET-USERS-IN-ROOM", user, password, strdup(currentRoom), response3);
    cout << response3 << endl;


    usersList->clear();
        char * separate2 = strtok(response3, "\r\n");

        while(separate2!=NULL)
        {
                usersList->addItem(separate2);
                separate2 = strtok(NULL, "\r\n");
        }

        char responsemsg[MAX_RESPONSE];
        memset(responsemsg, '\0', sizeof(responsemsg));

        char *args2 = (char*)malloc(sizeof(char*));
        args2 = strdup("");
        strcat(args2, "-1 ");
        strcat(args2, currentRoom);

        sendCommand(host,port, "GET-MESSAGES", user, password, args2 , responsemsg);
        cout << responsemsg << endl;
        allMessages->clear();

        allMessages->append(responsemsg);
}

Dialog::Dialog()
{
    createMenu();

    QVBoxLayout *mainLayout = new QVBoxLayout;


    // Rooms List
    QVBoxLayout * roomsLayout = new QVBoxLayout();
    QLabel * currentUserLabel = new QLabel("CURRENT USER: ");
    currentUserLineEdit = new QLineEdit();
    currentUserLineEdit->setStyleSheet("background-color:rgb(207, 237, 250);");

    QLabel * roomsLabel = new QLabel("\nDouble click to enter a room\nCURRENT ROOM:");
    QLabel * currentRoomLabel = new QLabel("Rooms: ");
    currentRoomLineEdit = new QLineEdit();
    currentRoomLineEdit->setStyleSheet("background-color:rgb(207, 237, 250);");


    roomsList = new QListWidget();
   // roomsList->setStyleSheet("background-color:rgb(207, 237, 250);");

    QLabel * leaveRoomLabel = new QLabel("Which room do you want to leave?");
    lr = new QLineEdit();

    QPushButton * leaveRoomButton = new QPushButton("Leave Room");
    leaveRoomButton->setStyleSheet("background-color:rgb(196, 177, 223);");

    roomsLayout->addWidget(currentUserLabel);
    roomsLayout->addWidget(currentUserLineEdit);
    roomsLayout->addWidget(roomsLabel);
    roomsLayout->addWidget(currentRoomLineEdit);
    roomsLayout->addWidget(currentRoomLabel);
    roomsLayout->addWidget(roomsList);
   // roomsLayout->addWidget(leaveRoomLabel);
   // roomsLayout->addWidget(lr);
    roomsLayout->addWidget(leaveRoomButton);


    // Users List
    QVBoxLayout * usersLayout = new QVBoxLayout();
    QLabel * usersLabel = new QLabel("Users");
    usersList = new QListWidget();
   // usersList->setStyleSheet("background-color:rgb(207, 237, 250);");

    usersLayout->addWidget(usersLabel);
    usersLayout->addWidget(usersList);

    // Layout for rooms and users
    QHBoxLayout *layoutRoomsUsers = new QHBoxLayout;
    layoutRoomsUsers->addLayout(roomsLayout);
    layoutRoomsUsers->addLayout(usersLayout);

    // Textbox for all messages
    QVBoxLayout * allMessagesLayout = new QVBoxLayout();
    QLabel * allMessagesLabel = new QLabel("Messages");
    allMessages = new QTextEdit;
    allMessages->setStyleSheet("background-color:rgb(207, 237, 250);");

    allMessagesLayout->addWidget(allMessagesLabel);
    allMessagesLayout->addWidget(allMessages);

    // Textbox for input message
    QVBoxLayout * inputMessagesLayout = new QVBoxLayout();
    QLabel * inputMessagesLabel = new QLabel("Type your message:");
    inputMessage = new QTextEdit;
    //inputMessage->setStyleSheet("background-color:rgb(207, 237, 250);");

    inputMessagesLayout->addWidget(inputMessagesLabel);
    inputMessagesLayout->addWidget(inputMessage);

    // Send and new account buttons
    QHBoxLayout *layoutButtons = new QHBoxLayout;
    QPushButton * sendButton = new QPushButton("Send");
//    QPushButton * newUserButton = new QPushButton("New Account");
    sendButton->setStyleSheet("background-color:rgb(196, 177, 223);");

    layoutButtons->addWidget(sendButton);
    layoutButtons->addWidget(newUserButton);


    //Username
    QHBoxLayout * loginLayout = new QHBoxLayout();
    QLabel * usernameLabel = new QLabel("Username:");
    un = new QLineEdit;
    un->setStyleSheet("background-color:rgb(207, 237, 250);");

    loginLayout->addWidget(usernameLabel);
    loginLayout->addWidget(un);

    //Password
    QLabel * passwordLabel = new QLabel("Password:");
    pw = new QLineEdit;
    pw->setStyleSheet("background-color:rgb(207, 237, 250);");

    pw->setEchoMode(QLineEdit::Password);
    loginLayout->addWidget(passwordLabel);
    loginLayout->addWidget(pw);

    QPushButton * loginButton = new QPushButton("Login/Create Account");
    loginButton->setStyleSheet("background-color:rgb(196, 177, 223);");
    loginLayout->addWidget(loginButton);

	//Create room
    QHBoxLayout * createRoomLayout = new QHBoxLayout();
    QLabel * createRoomLabel = new QLabel("Create Room: ");
    cr = new QLineEdit;
    cr->setStyleSheet("background-color:rgb(207, 237, 250);");

    createRoomLayout->addWidget(createRoomLabel);
    createRoomLayout->addWidget(cr);

    QPushButton * createRoomButton = new QPushButton("Create Room");
    createRoomButton->setStyleSheet("background-color:rgb(196, 177, 223);");

    createRoomLayout->addWidget(createRoomButton);

	
    // Setup actions for buttons
    connect(sendButton, SIGNAL (released()), this, SLOT (sendAction()));
    connect(loginButton, SIGNAL (released()), this, SLOT (newUserAction()));
    connect(createRoomButton, SIGNAL (released()), this, SLOT (createRoomAction()));
    connect(leaveRoomButton, SIGNAL (released()), this, SLOT (leaveRoomAction()));
    connect(roomsList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(enterRoomAction(QListWidgetItem*)));
   // connect(roomsList, SIGNAL(itemSelectionChanged(QListWidgetItem*)), this, SLOT(leaveRoomAction(QListWidgetItem*)));

    // Add all widgets to window
    mainLayout->addLayout(loginLayout);
    mainLayout->addLayout(createRoomLayout);
    mainLayout->addLayout(layoutRoomsUsers);
    mainLayout->addLayout(allMessagesLayout);
    mainLayout->addLayout(inputMessagesLayout);
    mainLayout->addLayout(layoutButtons);

    // Populate rooms
  /*  for (int i = 0; i < 20; i++) {
        char s[50];
        sprintf(s,"Room %d", i);
        roomsList->addItem(s);
    }*/

    // Populate users
   /* for (int i = 0; i < 20; i++) {
        char s[50];
        sprintf(s,"User %d", i);
        usersList->addItem(s);
    }

    for (int i = 0; i < 20; i++) {
        char s[50];
        sprintf(s,"Message %d", i);
        allMessages->append(s);
    }*/

    // Add layout to main window
    setLayout(mainLayout);

    setWindowTitle(tr("CS240 IRC Client"));
    //timer->setInterval(5000);

    /*messageCount = 0;*/

    /*timer = new QTimer(this);
    connect(timer, SIGNAL (timeout()), this, SLOT (timerAction()));
    timer->start(1000);*/
}


void Dialog::createMenu()

{
    menuBar = new QMenuBar;
    fileMenu = new QMenu(tr("&File"), this);
    exitAction = fileMenu->addAction(tr("E&xit"));
    menuBar->addMenu(fileMenu);

    connect(exitAction, SIGNAL(triggered()), this, SLOT(accept()));
}
