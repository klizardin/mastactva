/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef CONCENTS_H
#define CONCENTS_H

#include <QObject>

class DataInterface
{
public:
    virtual ~DataInterface() = default;
};

class DataType: public DataInterface
{
public:
    int m_intValue = 0;
    QString m_strValue;
};

class DataObject : public QObject, public DataType
{
    Q_OBJECT
public:
    explicit DataObject(QObject *parent = nullptr);

    Q_PROPERTY(int intValue READ intValue WRITE setIntValue NOTIFY intValueChanged)
    Q_PROPERTY(QString  strValue READ strValue WRITE setStrValue NOTIFY strValueChanged)

    int intValue() const;
    void setIntValue(int v_);
    QString strValue() const;
    void setStrValue(const QString &v_);

signals:
    void intValueChanged();
    void strValueChanged();

public:
    int m_id = -1;
};

template<class DataType_, class DataObject_>
class ModelConcept
{
public:
    DataObject_ *create()
    {
        DataObject_ *obj = new DataObject_();
        obj->m_id = m_nextId++;
        return obj;
    }

    DataObject_ *find(int id_)
    {
        auto fit = std::find_if(
                    std::begin(m_data),
                    std::end(m_data),
                    [id_](DataObject_ *data_)->bool
        {
            if(!data_)
            {
                return false;
            }
            return data_->m_id == id_;
        });
        if(std::end(m_data) == fit)
        {
            return nullptr;
        }
        else
        {
            return *fit;
        }
    }

    void add(DataObject_ *obj_)
    {
        const int currentId = obj_->m_id;
        m_data.push_back(obj_);
        DataObject_ *obj = find(currentId);
        Q_ASSERT(obj && currentId == obj->m_id);
    }

private:
    QVector<DataObject_ *> m_data;
    int m_nextId = 0;
};

void testCreatePush();

#endif // CONCENTS_H
