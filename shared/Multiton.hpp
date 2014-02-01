/* 
 * File:   Multiton.hpp
 * Author: michal
 *
 * Created on February 1, 2014, 12:51 AM
 */

#ifndef MULTITON_HPP
#define	MULTITON_HPP

#include <map>
#include <string>
#include <memory>

namespace xcs {

/*!
 * Based on implementation on Wikipedia
 * \see http://en.wikipedia.org/wiki/Multiton_pattern#C.2B.2B
 * \todo Thread safety.
 */
template <typename T, typename Key = std::string>
class Multiton {
public:

    static T& getInstance(const Key& key) {
        return *getPtr(key);
    }

protected:

    Multiton() {
    }

    virtual ~Multiton() {
    }

private:
    /*!
     * We store it in unique_ptr to have proper deallocation in global destructor.
     */
    typedef std::map<Key, std::unique_ptr<T> > InstancesMap;

    static T* getPtr(const Key& key) {
        auto it = instances.find(key);

        if (it == instances.end()) {
            instances.insert(typename InstancesMap::value_type(key, std::unique_ptr<T>(new T(key))));
        }
        return instances[key].get();
    }

    Multiton(const Multiton&) {
    }

    Multiton& operator=(const Multiton&) {
        return *this;
    }

    static InstancesMap instances;
};

template <typename T, typename Key>
typename Multiton<T, Key>::InstancesMap Multiton<T, Key>::instances;

}

#endif	/* MULTITON_HPP */

