#include "ircsession.h"
#include "ircmessageparser.h"
#include "ircversion.h"

IRCSession::IRCSession(IRCServer *server) {
    m_server = server;
    m_connection = new IRCConnection(m_server);
    m_parser     = new IRCMessageParser(this);
    connect(m_connection, SIGNAL(messageArrived(IRCMessage *)), this, SLOT(handleMessage(IRCMessage *)));
    connect(m_parser, SIGNAL(outputReady(IRCOutput)), this, SIGNAL(outputReady(IRCOutput)));
    connect(m_connection, SIGNAL(outputReady(IRCOutput)), this, SIGNAL(outputReady(IRCOutput)));
}

IRCSession::~IRCSession() {
    /* We want this to get deleted automatically */
    m_channels.setAutoDelete(TRUE);
    m_people.setAutoDelete(TRUE);
        
    delete m_parser;
    delete m_connection;
}

void IRCSession::beginSession() {
    m_connection->doConnect();
}

void IRCSession::join(QString channelname) {
    m_connection->sendLine("JOIN "+channelname);
}

void IRCSession::quit(){
    m_connection->sendLine("QUIT :[OI] I'm too good to need a reason");
}

void IRCSession::quit(QString message){
    m_connection->sendLine("QUIT :" + message);
}

void IRCSession::topic(IRCChannel *channel, QString message){
    m_connection->sendLine("TOPIC :" + channel->channelname() + " " + message);
}

void IRCSession::mode(IRCChannel *channel, QString message){
    m_connection->sendLine("MODE " + channel->channelname() + " " + message);
}

void IRCSession::mode(IRCPerson *person, QString message){
    m_connection->sendLine("MODE " + person->nick() + " " + message);
}

void IRCSession::mode(QString message){
    m_connection->sendLine("MODE " + message);
}

void IRCSession::raw(QString message){
    m_connection->sendLine(message);
}

void IRCSession::kick(IRCChannel *channel, IRCPerson *person) {
    m_connection->sendLine("KICK "+ channel->channelname() + " " + person->nick() +" :0wn3d - no reason");
}

void IRCSession::op(IRCChannel *channel, IRCPerson *person) {
    m_connection->sendLine("MODE "+ channel->channelname() + " +ooo " + person->nick());
}

void IRCSession::kick(IRCChannel *channel, IRCPerson *person, QString message) {
    m_connection->sendLine("KICK "+ channel->channelname() + " " + person->nick() +" :" + message);
} 

void IRCSession::sendMessage(IRCPerson *person, QString message) {
    m_connection->sendLine("PRIVMSG " + person->nick() + " :" + message);
}

void IRCSession::sendMessage(IRCChannel *channel, QString message) {
    m_connection->sendLine("PRIVMSG " + channel->channelname() + " :" + message);
}

void IRCSession::sendAction(IRCChannel *channel, QString message) {
    m_connection->sendLine("PRIVMSG " + channel->channelname() + " :\001ACTION " + message + "\001");
}

void IRCSession::sendAction(IRCPerson *person, QString message) {
    m_connection->sendLine("PRIVMSG " + person->nick() + " :\001ACTION " + message + "\001");
}

bool IRCSession::isSessionActive() {
    return m_connection->isConnected(); 
}

void IRCSession::endSession() {
    if (m_connection->isLoggedIn())
        m_connection->sendLine("QUIT :" APP_VERSION);
    else
        m_connection->close();
}

void IRCSession::part(IRCChannel *channel) {
    m_connection->sendLine("PART " + channel->channelname() + " :" + APP_VERSION);
}


IRCChannel *IRCSession::getChannel(QString channelname) {
    QListIterator<IRCChannel> it(m_channels);
    for (; it.current(); ++it) {
        if (it.current()->channelname() == channelname) {
            return it.current();
        }
    }
    return 0;
}

IRCPerson *IRCSession::getPerson(QString nickname) {
    QListIterator<IRCPerson> it(m_people);
    for (; it.current(); ++it) {
        if (it.current()->nick() == nickname) {
            return it.current();
        }
    }
    return 0;
}

void IRCSession::getChannelsByPerson(IRCPerson *person, QList<IRCChannel> &channels) {
    QListIterator<IRCChannel> it(m_channels);
    for (; it.current(); ++it) {
        if (it.current()->getPerson(person->nick()) != 0) {
            channels.append(it.current());
        }
    }
}

void IRCSession::addPerson(IRCPerson *person) {
    m_people.append(person);
}

void IRCSession::addChannel(IRCChannel *channel) {
    m_channels.append(channel);
}

void IRCSession::removeChannel(IRCChannel *channel) {
    m_channels.remove(channel);
}

void IRCSession::removePerson(IRCPerson *person) {
    m_people.remove(person);
}

void IRCSession::handleMessage(IRCMessage *message) {
    m_parser->parse(message);
}
