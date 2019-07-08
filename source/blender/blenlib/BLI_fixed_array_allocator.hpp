#pragma once

/**
 * This allocator should be used, when arrays of the same length are often allocated and
 * deallocated. Knowing that all arrays have the same length makes it possible to just store the
 * size of a single element to identify the buffer length, which is a small number usually.
 */

#include "BLI_small_stack.hpp"

namespace BLI {

class FixedArrayAllocator {
 private:
  SmallVector<void *, 16> m_all_pointers;
  SmallVector<SmallStack<void *>, 16> m_pointer_stacks;
  uint m_array_length;

 public:
  FixedArrayAllocator(uint array_length) : m_array_length(array_length)
  {
  }

  FixedArrayAllocator(FixedArrayAllocator &other) = delete;

  ~FixedArrayAllocator()
  {
    for (void *ptr : m_all_pointers) {
      MEM_freeN(ptr);
    }
  }

  uint array_size() const
  {
    return m_array_length;
  }

  void *allocate_array(uint element_size)
  {
    SmallStack<void *> &stack = this->stack_for_element_size(element_size);
    if (stack.size() > 0) {
      return stack.pop();
    }
    void *ptr = MEM_mallocN_aligned(m_array_length * element_size, 64, __func__);
    m_all_pointers.append(ptr);
    return ptr;
  }

  void deallocate_array(void *ptr, uint element_size)
  {
    SmallStack<void *> &stack = this->stack_for_element_size(element_size);
    stack.push(ptr);
  }

  template<typename T> T *allocate_array()
  {
    return (T *)this->allocate_array(sizeof(T));
  }

  template<typename T> void deallocate_array(T *ptr)
  {
    return this->deallocate_array(ptr, sizeof(T));
  }

  template<typename T> class ScopedAllocation {
   private:
    FixedArrayAllocator &m_allocator;
    void *m_ptr;
    uint m_element_size;

   public:
    ScopedAllocation(FixedArrayAllocator &allocator, T *ptr, uint element_size)
        : m_allocator(allocator), m_ptr(ptr), m_element_size(element_size)
    {
    }

    ScopedAllocation(ScopedAllocation &other) = delete;
    ScopedAllocation(ScopedAllocation &&other)
        : m_allocator(other.m_allocator), m_ptr(other.m_ptr), m_element_size(other.m_element_size)
    {
      other.m_ptr = nullptr;
    }

    ScopedAllocation &operator=(ScopedAllocation &other) = delete;
    ScopedAllocation &operator=(ScopedAllocation &&other)
    {
      this->~ScopedAllocation();
      new (this) ScopedAllocation(std::move(other));
      return *this;
    }

    ~ScopedAllocation()
    {
      if (m_ptr != nullptr) {
        m_allocator.deallocate_array(m_ptr, m_element_size);
      }
    }

    operator T *() const
    {
      return (T *)m_ptr;
    }
  };

  ScopedAllocation<void> allocate_array_scoped(uint element_size)
  {
    return ScopedAllocation<void>(*this, this->allocate_array(element_size), element_size);
  }

  template<typename T> ScopedAllocation<T> allocate_array_scoped()
  {
    return ScopedAllocation<T>(*this, this->allocate_array<T>(), sizeof(T));
  }

 private:
  SmallStack<void *> &stack_for_element_size(uint element_size)
  {
    BLI_assert(element_size > 0);
    uint index = element_size - 1;
    if (index >= m_pointer_stacks.size()) {
      m_pointer_stacks.append_n_times({}, index - m_pointer_stacks.size() + 1);
    }
    return m_pointer_stacks[index];
  }
};

};  // namespace BLI
