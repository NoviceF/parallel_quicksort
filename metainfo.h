#ifndef METAINFO_H
#define METAINFO_H

#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <typeindex>

#include "taskbase.h"

class TaskMetaTypeBase
{
    virtual ITask* createInstance()
    {
        throw std::runtime_error("Not implemented!");
    }
};

template <typename T>
class TaskMetaType : public TaskMetaTypeBase
{
public:
    TaskMetaType()
    {

    }

    ITask* createInstance()
    {

    }
};

class TaskMetaManager
{
public:
    enum Categories
    {
        sorts = 0,
        structs,
        totalCount
    };

public:
    template <typename T>
    void registerTaskMeta(Categories category)
    {
        static_assert(totalCount == 2, "Need to sync categories list!");

        m_typesInfo[category][std::type_index(typeid(T))]
                = std::make_shared(TaskMetaType<T>());
    }

private:
    typedef std::map<std::type_index, std::shared_ptr<TaskMetaTypeBase> > IndexToMetaMap;
    typedef std::map<Categories, IndexToMetaMap> CategoryMap;

    CategoryMap m_typesInfo;
};

#endif // METAINFO_H

