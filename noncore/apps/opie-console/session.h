#ifndef OPIE_SESSION_H
#define OPIE_SESSION_H

#include <qwidget.h>

class IOLayer;
/**
 * This is a Session. A session contains
 * a QWidget pointer and a IOLayer
 * Imagine a session like a collection of what
 * is needed to show your widget in a tab ;)
 */
class Session {
public:
    /**
     * c'tor with widget and layer
     * ownership get's transfered
     */
    Session();
    Session( const QString&, QWidget* widget, IOLayer* );
    ~Session();

    /**
     * return the name of the session
     */
    QString name()const;

    /**
     * return the widget
     */
    QWidget* widget();

    /**
     * return the layer
     */
    IOLayer* layer();
    void setWidget( QWidget* widget );
    void setIOLayer( IOLayer*  );
    void setName( const QString& );

private:
    QString m_name;
    QWidget* m_widget;
    IOLayer* m_layer;

};

#endif
