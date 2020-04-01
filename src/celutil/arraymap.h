
#pragma once

#include <cstddef>
#include <stdexcept>
#include <iostream>

template<typename K, typename V, size_t ArrayKeyLen>
class ArrayMap
{
public:
    static V invalidValue();
    static constexpr size_t ArraySize = 1 << ArrayKeyLen;
    static size_t arrayKey(K k)
    {
        k = k << (sizeof(k) * 8 - ArrayKeyLen);
        return k >> (sizeof(k) * 8 - ArrayKeyLen);
    }
    size_t size() const { return ArraySize; }
protected:
    V m_array[ArraySize];
    size_t m_used { 0 };
public:
    ArrayMap()
    {
        for(size_t i = 0; i < ArraySize; i++)
            m_array[i] = invalidValue();
    }
    size_t used() const { return m_used; }
    size_t totalUsed() const { return m_used; }
    bool has(K k) const { return m_array[k] != invalidValue(); }
    const V &getRef(K k) const
    {
        auto ak = arrayKey(k);
        if (m_array[ak] == invalidValue())
            throw std::invalid_argument("Invalid element in array!");
        return m_array[ak];
    }
    V &getRef(K k)
    {
        auto ak = arrayKey(k);
        if (m_array[ak] == invalidValue())
            throw std::invalid_argument("Invalid element in array!");
        return m_array[ak];
    }
    V getValue(K k) const
    {
        auto ak = arrayKey(k);
        return m_array[ak];
    }
    V *getPtr(K k)
    {
        auto ak = arrayKey(k);
        if (m_array[ak] == invalidValue())
            return nullptr;
        return &m_array[ak];
    }
    const V *getPtr(K k) const
    {
        auto ak = arrayKey(k);
        if (m_array[ak] == invalidValue())
            return nullptr;
        return &m_array[ak];
    }
    bool insert(K k, const V &v)
    {
        auto ak = arrayKey(k);
        bool ret = m_array[ak] == invalidValue();
        if (ret)
            m_used++;
        m_array[ak] = v;
        return ret;
    }
    bool erase(K k)
    {
        auto ak = arrayKey(k);
        bool ret = m_array[ak] != invalidValue();
        if (ret)
            m_used--;
        m_array[ak] = invalidValue();
        return ret;
    }
};

template<typename K, typename V, typename C, size_t ArrayKeyLen, size_t KeyLen>
class MultilevelArrayMap
{
public:
    static V invalidValue() { return C::invalidValue(); }
    static constexpr size_t ArraySize = 1 << ArrayKeyLen;
    static size_t arrayKey(K k)
    {
        k = k << (sizeof(k) * 8 - KeyLen);
        return k >> (KeyLen - ArrayKeyLen);
    }
    size_t size() const { return ArraySize; }
protected:
    C *m_array[ArraySize];
    size_t m_used { 0 };
    size_t m_totUsed { 0 };
public:
    MultilevelArrayMap()
    {
        for(size_t i = 0; i < ArraySize; i++)
            m_array[i] = nullptr;
    }
    ~MultilevelArrayMap()
    {
        for(size_t i = 0; i < ArraySize; i++)
            delete m_array[i];
    }
    size_t used() const { return m_used; }
    size_t totalUsed() const { return m_totUsed; }
    C * const *data() const { return m_array; }
    const C *container(K k) const { return m_array[arrayKey(k)]; }
    bool has(K k) const
    {
        size_t ak = arrayKey(k);
        if (m_array[ak] == nullptr)
            return false;
        return m_array[ak]->has(k);
    }
    const V &getRef(K k) const
    {
        size_t ak = arrayKey(k);
//         std::cout << "getRef const key: " << ak << std::endl;
        if (m_array[ak] == nullptr)
            throw std::invalid_argument("Invalid element in array!");
        return m_array[ak]->getRef(k);
    }
    V &getRef(K k)
    {
        size_t ak = arrayKey(k);
//         std::cout << "getRef key: " << ak << std::endl;
        if (m_array[ak] == nullptr)
            throw std::invalid_argument("Invalid element in array!");
        return m_array[ak]->getRef(k);
    }
    const V *getPtr(K k) const
    {
        size_t ak = arrayKey(k);
        if (m_array[ak] == nullptr)
            return nullptr;
        return m_array[ak]->getPtr(k);
    }
    V *getPtr(K k)
    {
        size_t ak = arrayKey(k);
        if (m_array[ak] == nullptr)
            return nullptr;
        return m_array[ak]->getPtr(k);
    }
    V getValue(K k) const
    {
        size_t ak = arrayKey(k);
        if (m_array[ak] == nullptr)
            return invalidValue();
        return m_array[ak]->getValue(k);
    }
    bool insert(K k, const V &v)
    {
        size_t ak = arrayKey(k);
        if (m_array[ak] == nullptr)
        {
            m_array[ak] = new C;
            m_used++;
        }
        if (m_array[ak]->insert(k, v))
        {
            m_totUsed++;
            return true;
        }
        return false;
    }
    bool erase(K k)
    {
        size_t ak = arrayKey(k);
        if (m_array[ak] == nullptr)
            return false;
        if (m_array[ak]->erase(k))
        {
            if (m_array[ak]->used() == 0)
            {
                delete m_array[ak];
                m_array[ak] = nullptr;
                m_used--;
            }
            m_totUsed--;
            return true;
        }
        return false;
    }
};
