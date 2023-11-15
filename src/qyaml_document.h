#pragma once

#include <QObject>
#include <QVariant>
#include <QVariantMap>
#include <QVariantHash>
#include <QPair>
#include "./qyaml_global.h"

class QYamlDocumentPvt;

typedef QPair<QVariant, QVariant> VariantPair;
//!
//! \brief The QYamlDocument class
//!
class Q_REFORCE_QYAML_EXPORT QYamlDocument : public QObject
{
    Q_OBJECT
public:
    //!
    //! \brief The SourceFormat enum
    //!
    enum SourceFormat{
        Yalm, Json, Properties, TOML, Shell
    };
    Q_ENUM(SourceFormat)

    //!
    //! \brief QYamlDocument
    //! \param parent
    //!
    Q_INVOKABLE explicit QYamlDocument(QObject *parent=nullptr);

    //!
    //! \brief QYamlDocument
    //! \param values
    //! \param parent
    //!
    explicit QYamlDocument(const QVariant &values, QObject *parent=nullptr);

    //!
    //! \brief QYamlDocument
    //! \param values
    //! \param parent
    //!
    explicit QYamlDocument(const QVariantMap &values, QObject *parent=nullptr);

    //!
    //! \brief QYamlDocument
    //! \param values
    //! \param parent
    //!
    explicit QYamlDocument(const QVariantHash &values, QObject *parent=nullptr);

    //!
    //! \brief QYamlDocument
    //! \param values
    //! \param parent
    //!
    explicit QYamlDocument(const VariantPair &values, QObject *parent=nullptr);

    //!
    //! \brief of
    //! \param values
    //! \param parent
    //! \return
    //!
    static QYamlDocument *of(const QVariant &values, QObject *parent=nullptr);

    //!
    //! \brief of
    //! \param values
    //! \param parent
    //! \return
    //!
    static QYamlDocument *of(const QVariantMap &values, QObject *parent=nullptr);

    //!
    //! \brief of
    //! \param values
    //! \param parent
    //! \return
    //!
    static QYamlDocument *of(const QVariantHash &values, QObject *parent=nullptr);

    //!
    //! \brief of
    //! \param values
    //! \param parent
    //! \return
    //!
    static QYamlDocument *of(const VariantPair &values, QObject *parent=nullptr);

    //!
    //! \brief indent
    //! \return
    //!
    int indent();
    QYamlDocument &setIndent(int newIndent);

    //!
    //! \brief contains
    //! \param /**envName
    //! \return
    //!
    QVariant contains(const QString &envName);

    //!
    //! \brief get
    //! \param envName
    //! \return
    //!
    QVariant get(const QString &envName);

    //!
    //! \brief insert
    //! \param envName
    //! \param value
    //! \return
    //!
    QYamlDocument &insert(const QString &envName, const QVariant &value);

    //!
    //! \brief toString
    //! \return
    //!
    QString toString()const;

    //!
    //! \brief toString
    //! \param sourceFormat
    //! \return
    //!
    QString toString(SourceFormat sourceFormat)const;

    //!
    //! \brief isEmpty
    //! \return
    //!
    bool isEmpty();

    //!
    //! \brief isObject
    //! \return
    //!
    bool isObject();

    //!
    //! \brief isArray
    //! \return
    //!
    bool isArray();

    //!
    //! \brief isValue
    //! \return
    //!
    bool isValue();

    //!
    //! \brief clear
    //! \return
    //!
    QYamlDocument &clear();

    //!
    //! \brief setValues
    //! \param values
    //! \return
    //!
    QYamlDocument &setValues(const QVariant &values);

    //!
    //! \brief toVariant
    //! \return
    //!
    QVariant toVariant()const;

    //!
    //! \brief toVariantMap
    //! \return
    //!
    QVariantMap toVariantMap()const;

    //!
    //! \brief toVariantList
    //! \return
    //!
    QVariantList toVariantList()const;

private:
    QYamlDocumentPvt *p=nullptr;
};

