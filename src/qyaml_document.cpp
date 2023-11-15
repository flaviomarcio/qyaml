#include "qyaml_document.h"
#include <QFile>
#include <QJsonDocument>
#include <QVariantMap>
#include <yaml-cpp/yaml.h>
//Ref
//  https://github.com/jbeder/yaml-cpp/wiki/Tutorial

class QYamlDocumentPvt:public QObject{
public:
    const QString shell_separator=".";
    const QString yaml_separator=".";
    const QString yaml_separator_double="..";
    const QString spacer=" ";
    QYamlDocument *parent=nullptr;
    QVariant values;
    int indent=2;
    explicit QYamlDocumentPvt(QYamlDocument *parent):QObject{parent},parent{parent}{
        this->values=loadFrom(values);
    }
    explicit QYamlDocumentPvt(QYamlDocument *parent, const QVariant &values):QObject{parent},parent{parent}{
        this->values=loadFrom(values);
    }
    explicit QYamlDocumentPvt(QYamlDocument *parent, const VariantPair &values):QObject{parent},parent{parent}{
        QVariantMap v={{values.first.toString().trimmed(), values.second}};
        this->values=loadFrom(v);
    }

    QString envParseName(const QString &envName)
    {
        auto out=envName.trimmed().replace(spacer,"");
        while(out.contains(yaml_separator+yaml_separator))
            out=out.replace(yaml_separator,yaml_separator);

        while(out.startsWith(yaml_separator))
            out=out.mid(1,out.length());

        while(out.endsWith(yaml_separator))
            out=out.mid(0,out.length()-1);

        auto envNameList=out.split(yaml_separator);
        if(envNameList.last().isEmpty())
            envNameList.removeLast();

        return envNameList.join(yaml_separator);
    }

    void insert(const QString &envName, const QVariant &value)
    {
        auto envNameList=this->envParseNames(envName);
        if(envNameList.isEmpty())
            return;
        QString lastEnv=yaml_separator;
        QVariantMap vMap={{lastEnv,value}};
        while(!envNameList.isEmpty()){
            auto envName=envNameList.takeLast();
            vMap={{envName, vMap.value(lastEnv)}};
            lastEnv=envName;
        }
        vMap=this->values.toMap();
        vMap.insert(lastEnv,vMap);
        this->values=vMap;
        return;
    }

    VariantPair getPair(const QString &envName)
    {
        auto envNameList=this->envParseNames(envName);
        if(envNameList.isEmpty())
            return {};

        auto vMap=this->values.toMap();
        while(!envNameList.isEmpty()){
            auto envName=envNameList.takeFirst();
            if(!vMap.contains(envName))
                return {};
            if(envNameList.isEmpty()){
                auto eName=this->envParseName(envName);
                return {eName,vMap.value(envName)};
            }
            vMap=vMap.value(envName).toMap();
        }
        return {};
    }


    QStringList envParseNames(const QString &envName)
    {
        auto eName=envParseName(envName);
        if(eName.isEmpty())
            return {};
        if(eName.contains(yaml_separator))
            return {eName};
        return eName.split(yaml_separator);
    }

    static bool isObj(const QVariant &v)
    {
        switch (v.typeId()) {
        case QMetaType::QVariantHash:
        case QMetaType::QVariantMap:
        case QMetaType::QVariantList:
            return true;
        default:
            break;
        }
        return false;
    }

    static bool containsObj(const QVariant &v)
    {
        QVariantList vList;
        switch (v.typeId()) {
        case QMetaType::QVariantHash:
        case QMetaType::QVariantMap:
        case QMetaType::QVariantPair:
            vList=v.toMap().values();
            break;
        case QMetaType::QVariantList:
            vList=v.toList();
            break;
        default:
            break;
        }

        for (auto &v:vList)
            if(isObj(v))
                return true;

        return false;
    };

    static QMetaType::Type firstType(const QVariant &v)
    {
        switch (v.typeId()) {
        case QMetaType::QVariantHash:
        case QMetaType::QVariantMap:
        case QMetaType::QVariantPair:
            return QMetaType::QVariantMap;
        case QMetaType::QStringList:
            return QMetaType::QStringList;
        case QMetaType::QVariantList:
        {
            if(containsObj(v))
                return QMetaType::QVariantList;
            auto vList=v.toList();
            if(vList.isEmpty())
                return QMetaType::QString;
            for(auto&v:vList){
                switch (v.typeId()) {
                case QMetaType::Int:
                case QMetaType::UInt:
                case QMetaType::Long:
                case QMetaType::ULong:
                case QMetaType::LongLong:
                case QMetaType::ULongLong:
                    return QMetaType::ULongLong;
                case QMetaType::Double:
                    return QMetaType::Double;
                case QMetaType::QString:
                case QMetaType::QByteArray:
                case QMetaType::QChar:
                case QMetaType::QUuid:
                case QMetaType::QUrl:
                    return QMetaType::QString;
                default:
                    break;
                }
            }
            break;
        }
        default:
            break;
        }
        return QMetaType::UnknownType;
    }


    QVariant readEnvs(YAML::Node lineup)
    {
        if(lineup.IsNull()){
            return {};
        }
        else if(lineup.IsScalar()){
            return QString::fromStdString(lineup.as<std::string>());
        }
        else if(lineup.IsSequence()){
            QVariantList __return;
            for (std::size_t i = 0; i<lineup.size(); ++i) {
                auto nKey=lineup[i];
                QVariant putValue=nKey.IsNull()
                                        ?QVariant{}
                                        :readEnvs(nKey);
                __return.append(putValue);
            }
            return __return;
        }
        else if(lineup.IsMap()){
            QVariantMap __return;
            for(YAML::const_iterator it=lineup.begin();it!=lineup.end();++it) {
                auto nKey=it->first;
                auto nValue=it->second;
                auto putKey=QString::fromStdString(nKey.as<std::string>());
                QVariant putValue=nValue.IsNull()?QVariant{}:readEnvs(nValue);
                __return.insert(putKey,putValue);
            }
            return __return;
        }
        return {};
    }

    void writeEnvs(const QVariant &v, YAML::Emitter &out)
    {
        if(v.isNull() || !v.isValid())
            return;

        out << YAML::Block;
        switch (v.typeId()) {
        case QMetaType::QVariantPair:
        {
            auto vMap=v.value<VariantPair>();
            auto key=vMap.first.toString().toStdString();
            auto value=vMap.second;
            if(!isObj(value)){
                out << YAML::Key << key <<vToString(value).toStdString();
            }
            else{
                out << YAML::Key << key;
                writeEnvs(value,out);
            }
            break;
        }
        case QMetaType::QVariantHash:
        case QMetaType::QVariantMap:
        {
            out<<YAML::BeginMap;
            auto inMap=v.toMap();
            QMapIterator<QString,QVariant> i(inMap);
            while(i.hasNext()){
                i.next();
                auto key=i.key().toStdString();
                auto value=i.value();
                if(!isObj(value)){
                    out << YAML::Key << key <<vToString(value).toStdString();
                }
                else{
                    out << YAML::Key << key;
                    writeEnvs(value,out);
                }
            }
            out<<YAML::EndMap;
            break;
        }
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
        {
            out<<YAML::BeginSeq;
            if(containsObj(v)){
                auto vList=v.toList();
                for (auto &v: vList)
                    writeEnvs(v,out);
                break;
            }
            else{
                auto vList=v.toList();
                for (auto &v: vList)
                    out << YAML::Value << vToString(v).toStdString();
                break;
            }
            out<<YAML::EndSeq;
            break;
        }
        default:
            out << vToString(v).toStdString();
            break;
        }
    }

    QString toYaml(const QVariant &env)
    {
        YAML::Emitter out;
        out.SetIndent(2);
        out.SetMapFormat(YAML::Flow);
        out << YAML::BeginDoc;
        writeEnvs(env, out);
        out << YAML::EndDoc;
        return QString::fromStdString(out.c_str());
    }

    QVariantMap loadFrom(const QVariant &values)
    {
        switch (values.typeId()) {
        case QMetaType::QVariantHash:
        case QMetaType::QVariantMap:
        case QMetaType::QVariantPair:
            return values.toMap();
        case QMetaType::QByteArray:
        case QMetaType::QString:
        {
            QByteArray bytes=values.toByteArray();
            if(bytes.isEmpty())
                return {};

            if(QFile::exists(bytes)){
                QFile file(bytes);
                if(!file.open(QFile::ReadOnly)){
                    qCritical()<<file.errorString();
                    return {};
                }
                auto values=file.readAll();
                file.close();
                YAML::Node lineup = YAML::Load(values);
                auto v=readEnvs(lineup);
                return v.toMap();
            }

            return QJsonDocument::fromJson(bytes).toVariant().toMap();
        }
        default:
            return {};
        }
    }

    void clear()
    {
        this->indent=2;
        this->values.clear();
    }

    bool read(const QVariant &values)
    {
        this->values=loadFrom(values);
        return true;
    }

    static QString vToString(const QVariant &v)
    {
        switch (v.typeId()) {
        case QMetaType::QVariantHash:
        case QMetaType::QVariantMap:
        case QMetaType::QVariantPair:
        {
            auto vHash=v.toHash();
            if(vHash.isEmpty())
                return {};
            return QJsonDocument::fromVariant(vHash).toJson(QJsonDocument::Compact);
        }
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
        {
            auto vList=v.toList();
            if(vList.isEmpty())
                return {};
            QStringList sList;
            for(auto &v: vList)
                sList.append(vToString(v));
            return sList.join(',');
        }
        case QMetaType::QUuid:
            return v.toUuid().toString();
        case QMetaType::QUrl:
            return v.toUrl().toString();
        case QMetaType::Bool:
            return v.toString();
        default:
            return v.toString();
        }
    }

    static QVariantMap expandHash(const QVariantMap &inMap)
    {
        QVariantMap __return;
        QMapIterator<QString,QVariant> i(inMap);
        while(i.hasNext()){
            i.next();
            const QVariant &value = i.value();
            switch (value.typeId()) {
            case QMetaType::QVariantHash:
            case QMetaType::QVariantMap:
            case QMetaType::QVariantPair:
            {
                auto keyName=i.key()+".";
                auto vHash=i.value().toMap();
                QMapIterator<QString,QVariant> i(vHash);
                while(i.hasNext()){
                    i.next();
                    __return.insert(keyName+i.key(),i.value());
                }
                break;
            }
            case QMetaType::QVariantList:
            {
                if(containsObj(i.value())){
                    auto vList=i.value().toList();
                    for(auto&v:vList){
                        if(isObj(v))
                            __return.insert(i.key(),v);
                        else
                            __return.insert(i.key(),vToString(v));
                    }
                }
                else{
                    __return.insert(i.key(),vToString(i.value()));
                }
                break;
            }
            default:
                __return.insert(i.key(),vToString(i.value()));
                break;
            }
        }
        return __return;
    }


    QVariant toEnvs() const
    {
        const auto &v=this->values;
        switch (v.typeId()) {
        case QMetaType::QVariantHash:
        case QMetaType::QVariantMap:
        case QMetaType::QVariantPair:
        {
            auto out=v.toMap();
            while(containsObj(out))
                out=expandHash(out);

            return out;
        }
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
        {
            QStringList out;
            auto vList=v.toList();
            for(auto&v:vList)
                out.append(vToString(v));
            return out;
        }
        default:
            return {};
        }
    }

    QString toString(QYamlDocument::SourceFormat sourceFormat)
    {
        switch (sourceFormat) {
        case QYamlDocument::Yalm:
            return this->toYaml(this->values);
        case QYamlDocument::Json:{
            return QString(QJsonDocument::fromVariant(this->values).toJson(QJsonDocument::Indented));
        }
        case QYamlDocument::Properties:
        case QYamlDocument::Shell:
        {
            static const auto __format=QString("%1=%2");
            QStringList __return;
            auto envs=this->toEnvs().toMap();
            QMapIterator<QString,QVariant> i(envs);
            while(i.hasNext()){
                i.next();
                QString keyName=i.key();
                if(sourceFormat==QYamlDocument::Shell)
                    keyName=keyName.replace(yaml_separator,shell_separator);
                __return.append(__format.arg(keyName,i.value().toString()));
            }
            __return.sort();
            return __return.join('\n');
        }
        case QYamlDocument::TOML:
        {
            static const auto __format_1=QString("%1=%2");
            static const auto __format_2=QString("  %1=%2");
            QStringList __return;
            auto envs=this->toEnvs().toMap();
            QMapIterator<QString,QVariant> i(envs);
            while(i.hasNext()){
                i.next();
                auto tomlKey=i.key().split('.');
                if(tomlKey.size()<2)
                    __return.append(__format_1.arg(i.key(),i.value().toString()));
                else{
                    auto keyName=tomlKey.takeLast();
                    auto tomlGroup=QString("[%1]").arg(tomlKey.join('.'));
                    if(!__return.contains(tomlGroup)){
                        __return.append("");
                        __return.append(tomlGroup);
                    }
                    __return.append(__format_2.arg(keyName,i.value().toString()));
                }
            }
            return __return.join('\n');
        }
        default:
            return {};
        }
    }
};

QYamlDocument::QYamlDocument(QObject *parent):QObject{parent}, p{new QYamlDocumentPvt{this}}
{
}

QYamlDocument::QYamlDocument(const QVariant &values, QObject *parent):QObject{parent}, p{new QYamlDocumentPvt{this,values}}
{
}

QYamlDocument::QYamlDocument(const QVariantMap &values, QObject *parent):QObject{parent}, p{new QYamlDocumentPvt{this,values}}
{
}

QYamlDocument::QYamlDocument(const QVariantHash &values, QObject *parent):QObject{parent}, p{new QYamlDocumentPvt{this,values}}
{
}

QYamlDocument::QYamlDocument(const VariantPair &values, QObject *parent):QObject{parent}, p{new QYamlDocumentPvt{this,values}}
{
}

QYamlDocument *QYamlDocument::of(const QVariant &values, QObject *parent)
{
    return new QYamlDocument(values, parent);
}

QYamlDocument *QYamlDocument::of(const QVariantMap &values, QObject *parent)
{
    return new QYamlDocument(values, parent);
}

QYamlDocument *QYamlDocument::of(const QVariantHash &values, QObject *parent)
{
    return new QYamlDocument(values, parent);
}

QYamlDocument *QYamlDocument::of(const VariantPair &values, QObject *parent)
{
    return new QYamlDocument(values, parent);
}

int QYamlDocument::indent()
{
    if(p->indent<=0)
        return 0;

    return p->indent;
}

QYamlDocument &QYamlDocument::setIndent(int newIndent)
{
    p->indent=newIndent;
    return *this;
}

QVariant QYamlDocument::contains(const QString &envName)
{
    return !p->getPair(envName).first.toString().trimmed().isEmpty();
}

QVariant QYamlDocument::get(const QString &envName)
{
    return p->getPair(envName).second;
}


QYamlDocument &QYamlDocument::insert(const QString &envName, const QVariant &value)
{
    p->insert(envName, value);
    return *this;
}

QString QYamlDocument::toString() const
{
    return p->toString(Yalm);
}

QString QYamlDocument::toString(SourceFormat sourceFormat) const
{
    return p->toString(sourceFormat);
}

bool QYamlDocument::isEmpty()
{
    return p->values.isNull() || !p->values.isValid();
}

bool QYamlDocument::isObject()
{
    switch (p->values.typeId()) {
    case QMetaType::QVariantMap:
    case QMetaType::QVariantHash:
    case QMetaType::QVariantPair:
        return true;
    default:
        return false;
    }
}

bool QYamlDocument::isArray()
{
    switch (p->values.typeId()) {
    case QMetaType::QVariantList:
    case QMetaType::QStringList:
        return true;
    default:
        return false;
    }
}

bool QYamlDocument::isValue()
{
    if(this->isArray() || this->isObject())
        return false;
    return true;
}

QYamlDocument &QYamlDocument::clear()
{
    p->values.clear();
    return *this;
}

QYamlDocument &QYamlDocument::setValues(const QVariant &values)
{
    p->read(values);
    return *this;
}

QVariant QYamlDocument::toVariant() const
{
    return p->values;
}

QVariantMap QYamlDocument::toVariantMap() const
{
    return p->values.toMap();
}

QVariantList QYamlDocument::toVariantList() const
{
    return p->values.toList();
}
