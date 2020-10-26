#include "appconsts.h"

ServerURLsModel::ServerURLsModel(const QVector<QString> &data_)
{
    m_roleNames[TextRole] = "text";

    setURLs(data_);
}

void ServerURLsModel::setURLs(const QVector<QString> &data_)
{
    m_data = data_;
}

int ServerURLsModel::rowCount(const QModelIndex &parent_ /*= QModelIndex()*/) const
{
    Q_UNUSED(parent_);
    return m_data.size();
}

QVariant ServerURLsModel::data(const QModelIndex &index_, int role_ /*= Qt::DisplayRole*/) const
{
    Q_UNUSED(role_);

    const int row = index_.row();
    if(row < 0 || row >= m_data.size())
    {
        return QVariant();
    }
    return QVariant(m_data[row]);
}

QHash<int, QByteArray> ServerURLsModel::roleNames() const
{
    return m_roleNames;
}


AppConsts::AppConsts()
{
    //m_serverUrls.push_back("http://localhost:8000/");
}

AppConsts::~AppConsts()
{
    delete m_serverURLsModel;
    m_serverURLsModel = nullptr;
}

void AppConsts::addServerURL(const QString &serverURL_)
{
    auto fit = std::find_if(std::begin(m_serverUrls), std::end(m_serverUrls),
                            [serverURL_](const QString &url_)->bool
    {
       return url_ == serverURL_;
    });
    if(std::end(m_serverUrls) != fit) { return; }
    m_serverUrls.push_back(serverURL_);
}

ServerURLsModel *AppConsts::getServerURLsModel()
{
    if(nullptr == m_serverURLsModel)
    {
        m_serverURLsModel = new ServerURLsModel(m_serverUrls);
    }
    else
    {
        m_serverURLsModel->setURLs(m_serverUrls);
    }
    return m_serverURLsModel;
}

void AppConsts::load()
{
    QFile textFile(".consts");
    textFile.open(QIODevice::ReadOnly);
    if(textFile.isOpen()) {
        QTextStream textStream(&textFile);
        while(true)
        {
            QString line = textStream.readLine();
            if (line.isNull())
                break;
            else
                m_serverUrls.append(line);
        }
    }
}

void AppConsts::save() const
{
    QFile textFile(".consts");
    textFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream textStream(&textFile);
    for(const auto &l : m_serverUrls)
    {
        textStream << l << "\n";
    }
}

static AppConsts *g_instance = nullptr;

AppConsts *AppConsts::getInstance()
{
    if(nullptr == g_instance)
    {
        g_instance = new AppConsts();
        g_instance->load();
    }
    return g_instance;
}

void AppConsts::freeInstance()
{
    if(nullptr != g_instance)
    {
        g_instance->save();
    }
    delete g_instance;
    g_instance = nullptr;
}
