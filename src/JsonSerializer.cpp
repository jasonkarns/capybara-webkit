#include "JsonSerializer.h"

JsonSerializer::JsonSerializer(QObject *parent) : QObject(parent) {
}

QByteArray JsonSerializer::serialize(const QVariant &object) {
  addVariant(object);
  return m_buffer;
}

void JsonSerializer::addVariant(const QVariant &object) {
  if (object.isValid()) {
    switch(object.type()) {
      case QMetaType::Bool:
      case QMetaType::Int:
        addLiteral(object.toString());
        break;
      case QMetaType::QDateTime:
        addString(object.toDateTime().toString(Qt::ISODate));
        break;
      case QMetaType::Double:
        if (qIsInf(object.toDouble()))
          addNull();
        else
          addLiteral(object.toString());
        break;
      case QMetaType::QString:
        addString(object.toString());
        break;
      case QMetaType::QVariantList:
        addArray(object.toList());
        break;
      case QMetaType::QVariantMap:
        addMap(object.toMap());
        break;
      default:
        addNull();
    }
  } else {
    addNull();
  }
}

void JsonSerializer::addString(const QString &string) {
  m_buffer.append("\"");
  m_buffer.append(sanitizeString(string));
  m_buffer.append("\"");
}

void JsonSerializer::addArray(const QVariantList &list) {
  m_buffer.append("[");
  for (int i = 0; i < list.length(); i++) {
    if (i > 0)
      m_buffer.append(",");
    addVariant(list[i]);
  }
  m_buffer.append("]");
}

void JsonSerializer::addLiteral(const QString &string) {
  m_buffer.append(string);
}

void JsonSerializer::addMap(const QVariantMap &map) {
  m_buffer.append("{");
  QMapIterator<QString, QVariant> iterator(map);
  while (iterator.hasNext()) {
    iterator.next();
    QString key = iterator.key();
    QVariant value = iterator.value();
    addString(key);
    m_buffer.append(":");
    addVariant(value);
    if (iterator.hasNext())
      m_buffer.append(",");
  }
  m_buffer.append("}");
}

void JsonSerializer::addNull() {
  m_buffer.append("null");
}

QByteArray JsonSerializer::sanitizeString(QString str) {
  str.replace("\\", "\\\\");
  str.replace("\"", "\\\"");
  str.replace("\b", "\\b");
  str.replace("\f", "\\f");
  str.replace("\n", "\\n");
  str.replace("\r", "\\r");
  str.replace("\t", "\\t");

  QByteArray result;
  const ushort* unicode = str.utf16();
  unsigned int i = 0;

  while (unicode[i]) {
    if (unicode[i] > 31 && unicode[i] < 128) {
      result.append(unicode[i]);
    }
    else {
      QString hexCode = QString::number(unicode[i], 16).rightJustified(4, '0');

      result.append("\\u").append(hexCode);
    }
    ++i;
  }

  return result;
}

