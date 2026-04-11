#pragma once
struct TypeInfo
{
    TypeInfo(const TypeInfo* parents) : _id(GetUniqueID()), _parents(parents) {}
    int _id;
    const TypeInfo* _parents = nullptr;

    static int GetUniqueID()
    {
        static int id = 0;
        return id++;
    }
};

#define DECLARE_RTTI(Base) \
public: \
    using Super = Base; \
    static const TypeInfo* GetStaticTypeInfo() { return GetTypeInfo(); } \
    virtual const TypeInfo* GetInstanceTypeInfo() const { return GetTypeInfo(); } \
private:  \
    static const TypeInfo* GetTypeInfo() \
    { \
        static TypeInfo _staticInfo = []() { \
                if constexpr (std::is_same_v<Super, void>) \
                { \
                    return TypeInfo(nullptr); \
                } \
                else \
                { \
                    return TypeInfo(Super::GetStaticTypeInfo()); \
                } \
            }(); \
        return &_staticInfo; \
    }

template<typename T, typename U>
static bool IsA(U* instance)
{
    auto info = instance->GetInstanceTypeInfo();
    if (info->_id == T::GetStaticTypeInfo()->_id)
        return true;
    else
    {
        auto parents = info->_parents;
        while (parents != nullptr)
        {
            if (parents->_id == T::GetStaticTypeInfo()->_id)
                return true;
            parents = parents->_parents;
        }
    }

    return false;
}

template<typename T, typename U>
T* Cast(U* instance)
{
    if (IsA<T, U>(instance))
        return static_cast<T*>(instance);

    return nullptr;
}