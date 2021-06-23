#ifndef LUAAPI_H
#define LUAAPI_H


#include <map>
#include <QString>
#include <QVector>


class LuaAPI
{
public:
    bool run(const QString &script_) const;
    bool call(
            const QString &script_,
            const QString &functionName_,
            std::map<QString, QVector<double>> &result_
            ) const;
};


#endif // LUAAPI_H
