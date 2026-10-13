#ifndef REGISTRY_H
#define REGISTRY_H

#include <cassert>
#include <utility>
#include <cstddef>
#include <vector>

template <typename T>
class Registry {
    public:
        size_t store(const T& val) {
            if (recycled.empty()) {
                bank.push_back(val);
                validity.push_back(true);
                active += 1;
                return bank.size() - 1;
            } else {
                size_t i = recycled.back();
                recycled.pop_back();
                bank[i] = val;
                validity[i] = true;
                active += 1;
                return i;
            }
        }

        size_t store(T&& val) {
            if (recycled.empty()) {
                bank.push_back(std::move(val));
                validity.push_back(true);
                active += 1;
                return bank.size() - 1;
            } else {
                size_t i = recycled.back();
                recycled.pop_back();
                bank[i] = std::move(val);
                validity[i] = true;
                active += 1;
                return i;
            }
        }

        void remove(size_t i) {
            assert(valid(i));
            recycled.push_back(i);
            validity[i] = false;
            bank[i] = T{};
            active -= 1;
        }

        T& operator[](size_t i) {
            assert(valid(i));
            return bank[i];
        }

        const T& operator[](size_t i) const {
            assert(valid(i));
            return bank[i];
        }

        bool valid(size_t i) const {
            return i < bank.size() && validity[i] == true;
        }

        size_t size() const {
            return active;
        }

    private:
        std::vector<T> bank;
        std::vector<bool> validity;
        std::vector<size_t> recycled;
        size_t active = 0;

};

#endif  // REGISTRY_H
