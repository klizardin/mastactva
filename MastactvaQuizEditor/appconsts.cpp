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
    m_maxImagePoints = 64;
    initColors();
}

AppConsts::~AppConsts()
{
    delete m_serverURLsModel;
    m_serverURLsModel = nullptr;
}

void AppConsts::initColors()
{
    m_colors.resize(m_maxImagePoints);
    for(auto &c : m_colors)
    {
        c.setX((float)rand()/(float)RAND_MAX);
        c.setY((float)rand()/(float)RAND_MAX);
        c.setZ((float)rand()/(float)RAND_MAX);
    }
    const float m = 0.3333333/(float)m_colors.size();
    std::sort(std::begin(m_colors), std::end(m_colors),
              [](const QVector3D &c1_,const QVector3D &c2_) -> bool
    {
        return c1_.x() < c2_.x();
    });
    for(int i = 0; i < m_colors.size() - 1; i++)
    {
        if(m_colors[i+1].x() < m_colors[i].x() + m)
        {
            m_colors[i+1].setX(m_colors[i].x() + m);
        }
    }
    std::sort(std::begin(m_colors), std::end(m_colors),
              [](const QVector3D &c1_,const QVector3D &c2_) -> bool
    {
        return c1_.y() < c2_.y();
    });
    for(int i = 0; i < m_colors.size() - 1; i++)
    {
        if(m_colors[i+1].y() < m_colors[i].y() + m)
        {
            m_colors[i+1].setY(m_colors[i].y() + m);
        }
    }
    std::sort(std::begin(m_colors), std::end(m_colors),
              [](const QVector3D &c1_,const QVector3D &c2_) -> bool
    {
        return c1_.z() < c2_.z();
    });
    for(int i = 0; i < m_colors.size() - 1; i++)
    {
        if(m_colors[i+1].z() < m_colors[i].z() + m)
        {
            m_colors[i+1].setZ(m_colors[i].z() + m);
        }
    }
    std::random_shuffle(std::begin(m_colors), std::end(m_colors));
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

int AppConsts::getMaxImagePoints() const
{
    return m_maxImagePoints;
}

static const QString g_settingFileName = "consts";

void AppConsts::load()
{
    QVector<QVector3D> colorsOld = m_colors;
    m_colors.clear();
    QFile textFile(g_settingFileName);
    textFile.open(QIODevice::ReadOnly);
    if(textFile.isOpen()) {
        QTextStream textStream(&textFile);
        while(true)
        {
            QString line = textStream.readLine();
            if (line.isNull())
            {
                break;
            }
            else
             {
                set(line);
            }
        }
    }
    for(int i = m_colors.size(); i < m_maxImagePoints; i++)
    {
        m_colors.push_back(colorsOld[i]);
    }
    m_colors.resize(m_maxImagePoints);
}

static const QString g_hostURLPrefix = "HostURL:";
static const QString g_maxImagePointsPrefix = "MaxImagePoints:";
static const QString g_colorsPrefix = "Colors:";

void AppConsts::set(const QString &line_)
{
    if(line_.startsWith(g_hostURLPrefix))
    {
        m_serverUrls.append(line_.mid(g_hostURLPrefix.length()).trimmed());
    }
    else if(line_.startsWith(g_maxImagePointsPrefix))
    {
        bool ok = false;
        auto tmp = QVariant(line_.mid(g_maxImagePointsPrefix.length()).trimmed()).toInt(&ok);
        if(ok)
        {
            m_maxImagePoints = tmp;
        }
    }
    else if(line_.startsWith(g_colorsPrefix))
    {
        QString t = line_.mid(g_colorsPrefix.length()).trimmed();
        QTextStream ts(&t);
        qreal x = 0, y = 0, z = 0;
        char c = ',';
        ts >> x >> c >> y >> c >> z;
        m_colors.push_back(QVector3D(x,y,z));
    }
}

void AppConsts::save() const
{
    QFile textFile(g_settingFileName);
    textFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream textStream(&textFile);
    for(const auto &l : m_serverUrls)
    {
        textStream << g_hostURLPrefix << l << "\n";
    }
    textStream << g_maxImagePointsPrefix << m_maxImagePoints << "\n";
    for(const auto &c: m_colors)
    {
        textStream << g_colorsPrefix << c.x() << "," << c.y() << "," << c.z() << "\n";
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
