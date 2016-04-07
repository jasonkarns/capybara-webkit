#include <QObject>
#include <QVariantList>

class JsonSerializer : public QObject {
  Q_OBJECT

  public:
    JsonSerializer(QObject *parent = 0);
    QByteArray serialize(const QVariant &object);

  private:
    void addVariant(const QVariant &object);
    void addString(const QString &string);
    void addArray(const QVariantList &list);
    void addLiteral(const QString &string);
    void addMap(const QVariantMap &map);
    void addNull();
    QByteArray sanitizeString(QString string);

    QByteArray m_buffer;
};

