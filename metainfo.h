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
//typedef std::function<void*()> TaskCreator;

class TaskMetaType
{
public:
    TaskMetaType(TaskCreator creator)
        : m_creator(creator)
    {}

    ITask* createInstance()
    {
        return m_creator();
    }

//    std::unique_ptr<ITask> createInstance()
//    {
//        return std::unique_ptr<ITask>(m_creator());
//    }

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

        const auto pairToInsert
                = std::make_pair(std::type_index(typeid(T)),
                                 TaskMetaType(typeCreator));

        if (m_typesInfo[category].insert(pairToInsert).second == false)
        {
            throw std::runtime_error("TaskMetaManager::registerTaskMeta: "
                                     "Try add element, that already exists.");
        }
    }

private:
    typedef std::map<std::type_index, TaskMetaType> IndexToMetaMap;
    typedef std::map<Categories, IndexToMetaMap> CategoryMap;

    static CategoryMap m_typesInfo;
};

TaskMetaManager::CategoryMap TaskMetaManager::m_typesInfo;

template <typename T>
TaskCreator make_simple_creator()
{
    // if T has one base type set one argument to lambda:
    // [](arg1, arg2..)->ITask* { return new T(arg1, arg2...); })
    return TaskCreator([]()->ITask* { return new T; });
//    return TaskCreator();
}

template <typename T>
class MetaRegistrator
{
public:
    MetaRegistrator(TaskMetaManager::Categories category)
    {
        TaskMetaManager::registerTaskMeta<T>(
            category, make_simple_creator<T>());
    }
};

//static MetaRegistrator<int> s_reg(TaskMetaManager::sorts);


#endif // METAINFO_H

