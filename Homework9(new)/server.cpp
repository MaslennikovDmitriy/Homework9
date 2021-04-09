#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/container/scoped_allocator.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/string.hpp>

using namespace boost::interprocess;

typedef managed_shared_memory::segment_manager segment_manager_t;
typedef boost::container::scoped_allocator_adaptor<allocator<void, segment_manager_t> > void_allocator;
typedef void_allocator::rebind<int>::other              int_allocator;
typedef void_allocator::rebind<std::string>::other      string_allocator;
typedef vector<std::string, string_allocator>       string_vector;
typedef vector<int, int_allocator>       int_vector;


class complex_data
{
public:
    string_vector string_vector;
    int_vector ID_vector;
    size_t indicator;
    size_t number_of_users;
    size_t vec_size;

    typedef void_allocator allocator_type;

    complex_data(complex_data const& other, const allocator_type& void_alloc)
        : vec_size(other.vec_size), indicator(other.indicator), ID_vector(other.ID_vector), number_of_users(other.number_of_users), string_vector(other.string_vector, void_alloc)
    {}
    complex_data(const allocator_type& void_alloc)
        : vec_size(0), indicator(0), ID_vector(void_alloc), number_of_users(0), string_vector(void_alloc)
    {}
};

typedef void_allocator::rebind<complex_data>::other    complex_data_allocator;
typedef vector<complex_data, complex_data_allocator>   complex_data_vector;

int main()
{
    struct shared_memory_remove
    {
        shared_memory_remove() { shared_memory_object::remove("MySharedMemory"); }
        ~shared_memory_remove() { shared_memory_object::remove("MySharedMemory"); }
    } remover;

    managed_shared_memory segment(open_or_create, "MySharedMemory", 65536);
    void_allocator alloc_inst(segment.get_segment_manager());
    complex_data* complex_data0_ = segment.construct<complex_data>("MyComplexData")(alloc_inst);

    while (complex_data0_->indicator != 2)
    {
        if (complex_data0_->indicator == 1)
        {
            std::cout << "Person #" << complex_data0_->ID_vector.back() << ": " << complex_data0_->string_vector.back() << std::endl;
            complex_data0_->indicator = 0;
        }
    }
    shared_memory_remove();
    system("pause");
    return EXIT_SUCCESS;
}