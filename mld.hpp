#ifndef mld_hpp
#define mld_hpp

#include <iostream>
#include <cctype>
#include <cstring>
#include <cassert>
#include <memory.h>
#include <stdlib.h>

#define MAX_CLASS_NAME_SIZE 128
#define MAX_FIELD_NAME_SIZE 128


enum data_type_t {
    UINT8,
    UINT16,
    UINT32,
    CHAR,
    OBJ_PTR,
    FLOAT,
    DOUBLE,
    OBJ_CLASS
};

#define GET_FIELD_OFFSET(class_name, field_name) \
    reinterpret_cast<size_t>(&(((class_name *)0)->field_name))

#define GET_FIELD_SIZE(class_name, field_name) \
    sizeof(((class_name *)0)->field_name)

struct field_info_t
{
    char fname[MAX_FIELD_NAME_SIZE];
    data_type_t dtype;
    const size_t size;
    const size_t offset;
    char nested_class_name[MAX_CLASS_NAME_SIZE];
};


class class_db_rec_t
{
public:
    class_db_rec_t *next;
    char class_name[MAX_CLASS_NAME_SIZE];
    size_t ds_size ;
    size_t n_fields;
    const field_info_t *fields;
    
    class_db_rec_t();
    void print_class_rec() const;
};


class class_db_t
{
private:
    class_db_rec_t *head;
    size_t count;
public:
    class_db_t();
    
    ~class_db_t();
    
    void print_class_db () const;
    
    bool add_class_to_class_db(class_db_rec_t *class_rec);
    
    void mld_init_primitive_data_type_support();
    
    void REG_CLASS(const char *cl_name, const field_info_t *fields_arr,
                   const size_t no_of_fields, const size_t class_size);
    
    class_db_rec_t* class_db_look_up(const char *class_name);
    
};



#define FIELD_INFO(class_name, fld_name, dtype, nested_class_name)    \
   {#fld_name, dtype, GET_FIELD_SIZE(class_name, fld_name),           \
GET_FIELD_OFFSET(class_name, fld_name), #nested_class_name}


struct object_db_rec_t
{
    object_db_rec_t *next;
    void *ptr;
    size_t units;
    class_db_rec_t *class_rec;
    bool is_visited;
    bool is_root;
    
    void print_obj_rec(size_t i) const;
    void mld_dump_object_rec_detail() const;
};

class object_db_t
{
private:
    object_db_rec_t *head;
    size_t count;
    class_db_t *class_db;
public:
    
    object_db_t(class_db_t *class_db);
    
    ~ object_db_t();

    void print_obj_db() const;

    void mld_register_root_object (void *const obj_ptr, char *struct_name, const size_t units);

    void mld_set_dynamic_object_as_root(const void *object_ptr);

    void *xnew(const char *class_name, const size_t units);
    
    void xfree(void *ptr);
    
    void remove_obj_rec_from_db(object_db_rec_t *obj_rec);

    void run_mld_algorithm();
    
    void init_mld_algorithm();

    void report_leaked_objects() const;
    
    void add_object_to_object_db(void *const ptr, const size_t units,
                                 class_db_rec_t * const class_rec, bool is_root = false);
    
    object_db_rec_t *get_next_root_object(const object_db_rec_t *starting_from_here) const;
    
    void DFS(object_db_rec_t *parent_obj_rec) const;
    
    object_db_rec_t* object_db_look_up(const void *ptr) const;
};




#endif /* mld_hpp */
