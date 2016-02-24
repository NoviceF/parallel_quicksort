#ifndef METAINFO_H
#define METAINFO_H

#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <typeindex>

#include "taskbase.h"

typedef std::function<ITask*()> TaskCreator;

template <typename T>
TaskCreator make_simple_creator()
{
    return [](){ return new T; };
}

class TaskMetaType
{
public:
    TaskMetaType(TaskCreator creator)
        : m_creator(creator)
    {}

    std::unique_ptr<ITask> createInstance()
    {
        return std::unique_ptr<ITask>(m_creator());
    }

private:
    TaskCreator m_creator;
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
    static void registerTaskMeta(Categories category,
                          TaskCreator typeCreator)
    {
        static_assert(totalCount == 2, "Need to sync categories list!");

        m_typesInfo[category][std::type_index(typeid(T))]
                = TaskMetaType(typeCreator);
    }

private:
    typedef std::map<std::type_index, std::shared_ptr<TaskMetaType> > IndexToMetaMap;
    typedef std::map<Categories, IndexToMetaMap> CategoryMap;

    static CategoryMap m_typesInfo;
};

template <template <typename> class T, typename U>
class MetaRegistrator
{
public:
    MetaRegistrator(TaskMetaManager::Categories category)
    {
        TaskMetaManager::registerTaskMeta<T<U> >(category,
                                                 make_simple_creator<T>);
    }
};


#endif // METAINFO_H

