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



#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QListWidgetItem>

class QAction;
class QDialogButtonBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QMenu;
class QMenuBar;
class QPushButton;
class QTextEdit;
class QListWidget;

class Dialog : public QDialog
{
    Q_OBJECT
public:
    Dialog();
    
private:
    int messageCount;
    int open_client_socket(char *host, int port);
    void sendCommand(char * host, int port, char * command, char * user,
		char * password, char * args, char * response);
    void createMenu();
    void createHorizontalGroupBox();
    void createGridGroupBox();
    void createFormGroupBox();

protected:
    enum { NumGridRows = 3, NumButtons = 4 };

    QMenuBar *menuBar;

    QMenu *fileMenu;
    QAction *exitAction;

    QListWidget * roomsList;
    QListWidget * usersList;
    QTextEdit * allMessages;
    QTextEdit * inputMessage;
    QLineEdit * un;
    QLineEdit * pw;
    QLineEdit * cr;
    QLineEdit * lr;
    QLineEdit * currentUserLineEdit;

    QLineEdit * currentRoomLineEdit;
    QPushButton * sendButton;
    QPushButton * newUserButton;
    QTimer * timer;

private slots:
    void sendAction();
    void newUserAction();
    void createRoomAction();
    void timerAction();
    void enterRoomAction(QListWidgetItem*);
    void leaveRoomAction();
};


#endif // DIALOG_H
