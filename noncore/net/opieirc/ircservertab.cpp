#include <qtextstream.h>
#include <qwhatsthis.h>

#include "ircservertab.h"
#include "ircmessageparser.h"
#include "ircchannelperson.h"


bool IRCServerTab::containsPing( const QString& text, IRCServerTab* tab ) {
  return (text.contains(IRCMessageParser::tr("Received a CTCP PING from "))) ||
         (text.find("ping") != -1 && text.find( tab->server()->nick() != -1));
}


IRCServerTab::IRCServerTab(IRCServer server, MainWindow *mainWindow, QWidget *parent, const char *name, WFlags f) : IRCTab(parent, name, f) {
    m_server = server;
    m_session = new IRCSession(&m_server);
    m_mainWindow = mainWindow;
    m_close = FALSE;
    m_lines = 0;
    m_description->setText(tr("Connecting to")+" <b>" + server.hostname() + ":" + QString::number(server.port()) + "</b>");
    m_textview = new QTextView(this);
    m_textview->setHScrollBarMode(QScrollView::AlwaysOff);
    m_textview->setVScrollBarMode(QScrollView::AlwaysOn);
    m_textview->setTextFormat(RichText);
    QWhatsThis::add(m_textview, tr("Server messages"));
    m_layout->add(m_textview);
    m_field = new IRCHistoryLineEdit(this);
    connect(m_field, SIGNAL(nextTab()), this, SIGNAL(nextTab()));
    connect(m_field, SIGNAL(prevTab()), this, SIGNAL(prevTab()));
    connect(m_field, SIGNAL(closeTab()), this, SIGNAL(closeTab()));
    connect(this, SIGNAL(editFocus()), m_field, SLOT(setEditFocus()));

    QWhatsThis::add(m_field, tr("Type commands here. A list of available commands can be found inside the OpieIRC help"));
    m_layout->add(m_field);
    connect(m_field, SIGNAL(returnPressed()), this, SLOT(processCommand()));
    connect(m_session, SIGNAL(outputReady(IRCOutput)), this, SLOT(display(IRCOutput)));
    connect(m_mainWindow, SIGNAL(updateScroll()), this, SLOT(scrolling()));
    connect(m_session, SIGNAL(updateChannels()), this, SLOT(slotUpdateChannels()));
    settingsChanged();

    m_field->setFocus();
    m_field->setActiveWindow();

}

void IRCServerTab::scrolling(){
  m_textview->ensureVisible(0, m_textview->contentsHeight());
}


void IRCServerTab::appendText(QString text) {
    /* not using append because it creates layout problems */
    QString txt = m_textview->text() + IRCTab::appendTimestamp( text );



    if (m_maxLines > 0 && m_lines >= m_maxLines) {
        int firstBreak = txt.find('\n');
        if (firstBreak != -1) {
            txt = "<qt bgcolor=\"" + m_backgroundColor + "\"/>" + txt.right(txt.length() - (firstBreak + 1));
        }
    } else {
        m_lines++;
    }
    m_textview->setText(txt);
    m_textview->ensureVisible(0, m_textview->contentsHeight());
    emit changed(this);
}

IRCServerTab::~IRCServerTab() {
    delete m_session;
}

void IRCServerTab::removeChannelTab(IRCChannelTab *tab) {
    m_channelTabs.remove(tab);
}

void IRCServerTab::removeQueryTab(IRCQueryTab *tab) {
    m_queryTabs.remove(tab);
}

void IRCServerTab::addQueryTab(IRCQueryTab *tab) {
    m_queryTabs.append(tab);
}

QString IRCServerTab::title() {
    return "Server";
}

IRCSession *IRCServerTab::session() {
    return m_session;
}
/*
QString *IRCServerTab::mynick() {
	return (*m_server->nick());
} */

IRCServer *IRCServerTab::server() {
    return &m_server;
}

void IRCServerTab::settingsChanged() {
    m_textview->setText("<qt bgcolor=\"" + m_backgroundColor + "\"/>");
    m_lines = 0;
}

void IRCServerTab::executeCommand(IRCTab *tab, QString line) {
    QTextIStream stream(&line);
    QString command;
    stream >> command;
    command = command.upper().right(command.length()-1);

   //JOIN
  if (command == "JOIN" || command == "J") {
        QString channel;
        stream >> channel;
        /* According to RFC 1459 */
        if (channel.length() > 0 && channel.length() < 200 &&
            channel.find(",") == -1 && channel.find("") == -1) {
            
            if (!channel.startsWith("#") && !channel.startsWith("&")) {
                channel = channel.prepend("#");
            }
            m_session->join(channel);
        } else {
            tab->appendText("<font color=\"" + m_errorColor + "\">Unknown channel format!</font><br>");
        }
    }

  //KICK
	else if (command == "KICK"){
		QString nickname;
        stream >> nickname;
        if (nickname.length() > 0) {
            if (line.length() > 7 + nickname.length()) {
                QString text = line.right(line.length()-nickname.length()-7);
                IRCPerson person;
                person.setNick(nickname);
                m_session->kick(((IRCChannelTab *)tab)->channel(), &person, text);
            } else {
                IRCPerson person;
                person.setNick(nickname);
                m_session->kick(((IRCChannelTab *)tab)->channel(), &person);
            }
        }
	}

 else if (command == "OP"){
		QString nickname;
        stream >> nickname;
        if (nickname.length() > 0) {
                QString text = line.right(line.length()-nickname.length()-5);
                IRCPerson person;
                person.setNick(nickname);
                m_session->op(((IRCChannelTab *)tab)->channel(), &person);
            }
        }

  //SEND MODES
  else if (command == "MODE"){
     QString text = line.right(line.length()-6);
     if (text.length() > 0) {
       m_session->mode(text);
     } else {
       tab->appendText("<font color=\"" + m_errorColor + "\">/mode channel {[+|-]|o|p|s|i|t|n|b|v} [limit] [user] [ban mask]<br>/mode nickname {[+|-]|i|w|s|o}</font><br>");
     }
	}
  //SEND RAW MESSAGE TO SERVER, COMPLETELY UNCHECKED - anything in the RFC...or really anything you want
  else if (command == "RAW"){
     QString text = line.right(line.length()-5);
     if (text.length() > 0) {
       m_session->raw(text);
     }
  }
  else if (command == "SUSPEND"){
     QString text = line.right(line.length()-9);
      if (text.upper() == "ON") {
          QCopEnvelope( "QPE/System", "setScreenSaverMode(int)" ) << QPEApplication::Enable;
      }
      else if (text.upper() == "OFF"){
          QCopEnvelope( "QPE/System", "setScreenSaverMode(int)" ) << QPEApplication::Disable;
      } else {
          tab->appendText("<font color=\"" + m_errorColor + "\">Line: "+ line +"</font><br>Text: "+text);
      }
  }

  else if (command == "QUIT"){
     QString text = line.right(line.length()-6);
     if (text.length() > 0) {
       m_session->quit(text);
     } else {
       m_session->quit();
     }
	}

  //SEND ACTION
	else if (command == "ME") {
        QString text = line.right(line.length()-4);
        if (text.length() > 0) {
            if (tab->isA("IRCChannelTab")) {
                tab->appendText("<font color=\"" + m_selfColor + "\">*" + IRCOutput::toHTML(m_server.nick()) + " " + IRCOutput::toHTML(text) + "</font><br>");
                m_session->sendAction(((IRCChannelTab *)tab)->channel(), text);
            } else if (tab->isA("IRCQueryTab")) {
                tab->appendText("<font color=\"" + m_selfColor + "\">*" + IRCOutput::toHTML(m_server.nick()) + " " + IRCOutput::toHTML(text) + "</font><br>");
                m_session->sendAction(((IRCQueryTab *)tab)->person(), text);
            } else {
                tab->appendText("<font color=\"" + m_errorColor + "\">Invalid tab for this command</font><br>");
            }
        }
    }
  //SEND PRIVMSG
  else if (command == "MSG") {
        QString nickname;
        stream >> nickname;
        if (nickname.length() > 0) {
            if (line.length() > 6 + nickname.length()) {
                QString text = line.right(line.length()-nickname.length()-6);
                IRCPerson person;
                person.setNick(nickname);
                tab->appendText("<font color=\"" + m_textColor + "\">&gt;</font><font color=\"" + m_otherColor + "\">"+IRCOutput::toHTML(nickname)+"</font><font color=\"" + m_textColor + "\">&lt; "+IRCOutput::toHTML(text)+"</font><br>");
                m_session->sendMessage(&person, text);
            }
        }
    }
	else {
        tab->appendText("<font color=\"" + m_errorColor + "\">Unknown command</font><br>");
    }
}

void IRCServerTab::processCommand() {
    QString text = m_field->text();
    if (text.startsWith("/") && !text.startsWith("//")) {
        /* Command mode */
        executeCommand(this, text);
    }
    m_field->clear();
}

void IRCServerTab::doConnect() {
    m_session->beginSession();
}

void IRCServerTab::remove() {
    /* Close requested */
    if (m_session->isSessionActive()) {
        /* While there is a running session */
        m_close = TRUE;
        m_session->endSession();
    } else {
        /* Session has previously been closed */
        m_channelTabs.first();
        while (m_channelTabs.current() != 0) {
            m_mainWindow->killTab(m_channelTabs.current(), true);
        }
        m_queryTabs.first();
        while (m_queryTabs.current() != 0) {
            m_mainWindow->killTab(m_queryTabs.current(), true);
        }
        m_mainWindow->killTab(this);
    }
}

IRCChannelTab *IRCServerTab::getTabForChannel(IRCChannel *channel) {
    QListIterator<IRCChannelTab> it(m_channelTabs);

    for (; it.current(); ++it) {
        if (it.current()->channel() == channel)
            return it.current();
    }
    return 0;
}

IRCQueryTab *IRCServerTab::getTabForQuery(IRCPerson *person) {
    QListIterator<IRCQueryTab> it(m_queryTabs);

    for (; it.current(); ++it) {
        if (it.current()->person()->nick() == person->nick())
            return it.current();
    }
    return 0;
}

void IRCServerTab::display(IRCOutput output) {

    /* All messages to be displayed inside the GUI get here */
    switch (output.type()) {
        case OUTPUT_CONNCLOSE:
            if (m_close) {
                m_channelTabs.first();
                while (m_channelTabs.current() != 0) {
                    m_mainWindow->killTab(m_channelTabs.current(), true);
                }
                m_queryTabs.first();
                while (m_queryTabs.current() != 0) {
                    m_mainWindow->killTab(m_queryTabs.current(), true);
                }
                m_mainWindow->killTab(this);
            } else {
                appendText("<font color=\"" + m_errorColor + "\">" + output.htmlMessage() +"</font><br>");
                QListIterator<IRCChannelTab> it(m_channelTabs);
                for (; it.current(); ++it) {
                    it.current()->appendText("<font color=\"" + m_serverColor + "\">" + output.htmlMessage() +"</font><br>");
                }
            }
            break;
        case OUTPUT_SELFJOIN: {
                IRCChannelTab *channeltab = new IRCChannelTab((IRCChannel *)output.getParam(0), this, m_mainWindow, (QWidget *)parent());
                m_channelTabs.append(channeltab);
                m_mainWindow->addTab(channeltab);
            }
            break;
        case OUTPUT_CHANPRIVMSG: {
                IRCChannelTab *channelTab = getTabForChannel((IRCChannel *)output.getParam(0));
                channelTab->appendText("<font color=\"" + m_textColor + "\">&lt;</font><font color=\"" + m_otherColor + "\">"+IRCOutput::toHTML(((IRCChannelPerson *)output.getParam(1))->nick())+"</font><font color=\"" + m_textColor + "\">&gt; " + output.htmlMessage()+"</font><br>");
            }
            break;
        case OUTPUT_QUERYACTION:
        case OUTPUT_QUERYPRIVMSG: {
                IRCQueryTab *queryTab = getTabForQuery((IRCPerson *)output.getParam(0));
                if (!queryTab) {
                    queryTab = new IRCQueryTab((IRCPerson *)output.getParam(0), this, m_mainWindow, (QWidget *)parent());
                    m_queryTabs.append(queryTab);
                    m_mainWindow->addTab(queryTab);
                }
                queryTab->display(output);
            }
            break;
        case OUTPUT_SELFPART: {
                IRCChannelTab *channelTab = getTabForChannel((IRCChannel *)output.getParam(0));
                if (channelTab)
                    m_mainWindow->killTab(channelTab, true);
            }
            break;
        case OUTPUT_SELFKICK: {
                appendText("<font color=\"" + m_errorColor + "\">" + output.htmlMessage() + "</font><br>");
                IRCChannelTab *channelTab = getTabForChannel((IRCChannel *)output.getParam(0));
                if (channelTab)
                    m_mainWindow->killTab(channelTab, true);
            }
            break;
        case OUTPUT_CHANACTION: {
                IRCChannelTab *channelTab = getTabForChannel((IRCChannel *)output.getParam(0));
                channelTab->appendText("<font color=\"" + m_otherColor + "\">"+output.htmlMessage()+"</font><br>");
            }
            break;
        case OUTPUT_TOPIC: {
                IRCChannel *channel = (IRCChannel *) output.getParam(0);
                if (channel) {
                    IRCChannelTab *channelTab = getTabForChannel(channel);
                    if (channelTab) {
                        channelTab->appendText("<font color=\"" + m_notificationColor + "\">"+output.htmlMessage()+"</font><br>");
                        return;
                    }
                }
                IRCChannelTab::enqueue(channel->channelname(), "<font color=\"" + m_notificationColor + "\">"+output.htmlMessage()+"</font><br>");
            }
            break;
        case OUTPUT_QUIT: {
                QString nick = ((IRCPerson *)output.getParam(0))->nick();
                QListIterator<IRCChannelTab> it(m_channelTabs);
                for (; it.current(); ++it) {
                    if (it.current()->list()->hasPerson(nick)) {
                        it.current()->appendText("<font color=\"" + m_notificationColor + "\">"+output.htmlMessage()+"</font><br>");
                        it.current()->list()->update();
                    }
                }
            }
            break;
        case OUTPUT_NICKCHANGE: {
                QString *nick = static_cast<QString*>(output.getParam(0));
                if(!nick) {
                    appendText("<font color=\"" + m_notificationColor + "\">"+output.htmlMessage()+"</font><br>");
                    break;
                }
                QListIterator<IRCChannelTab> it(m_channelTabs);
                for (; it.current(); ++it) {
                    if (it.current()->list()->hasPerson(*nick)) {
                        it.current()->appendText("<font color=\"" + m_notificationColor + "\">"+output.htmlMessage()+"</font><br>");
                    }
                }
                delete nick;
            }
            break;
        case OUTPUT_OTHERJOIN:
        case OUTPUT_OTHERKICK:
        case OUTPUT_CHANPERSONMODE:
        case OUTPUT_OTHERPART: {
                IRCChannelTab *channelTab = getTabForChannel((IRCChannel *)output.getParam(0));
                channelTab->appendText("<font color=\"" + m_notificationColor + "\">"+output.htmlMessage()+"</font><br>");
                channelTab->list()->update();
            }
            break;
        case OUTPUT_CTCP:
            appendText("<font color=\"" + m_notificationColor + "\">" + output.htmlMessage() + "</font><br>");
            break;
        case OUTPUT_ERROR:
            appendText("<font color=\"" + m_errorColor + "\">" + output.htmlMessage() + "</font><br>");
            break;
        case OUTPUT_TITLE:
            m_description->setText(output.message());
            break;
        default:
            appendText("<font color=\"" + m_serverColor + "\">" + output.htmlMessage() + "</font><br>");
            break;
    }
}

void IRCServerTab::slotUpdateChannels() {
    QListIterator<IRCChannelTab> it(m_channelTabs);
    for (; it.current(); ++it) {
        it.current()->list()->update();
    }
}
                                                                                                 
