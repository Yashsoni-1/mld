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

class field_info_t
{
public:
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
    char class_name [MAX_CLASS_NAME_SIZE];
    unsigned int ds_size ;
    unsigned int n_fields;
    field_info_t *fields;
};


class class_db_t
{
public:
    class_db_rec_t *head;
    size_t count;
};



void print_class_rec (class_db_rec_t *class_rec);

void print_class_db (class_db_t *class_db);


bool add_class_to_class_db(class_db_t *class_db, class_db_rec_t *class_rec);



#define FIELD_INFO(class_name, fld_name, dtype, nested_class_name)    \
   {#fld_name, dtype, GET_FIELD_SIZE(class_name, fld_name),           \
GET_FIELD_OFFSET(class_name, fld_name), #nested_class_name}

static void REG_CLASS(class_db_t *class_db, const char *cl_name, field_info_t *fields_arr,
                      int no_of_fields, unsigned int class_size)
{
    auto rec = new class_db_rec_t;
    strncpy(rec->class_name, cl_name, MAX_CLASS_NAME_SIZE);
    rec->ds_size = class_size;
    rec->n_fields = no_of_fields;
    rec->fields = fields_arr;
    if(add_class_to_class_db(class_db, rec)){
        return;
    }
}
    


class object_db_rec_t
{
public:
    object_db_rec_t *next;
    void *ptr;
    unsigned int units;
    class_db_rec_t *class_rec;
    bool is_visited;
    bool is_root;
};

class object_db_t
{
public:
    class_db_t *class_db;
    object_db_rec_t *head;
    unsigned int count;
};

void print_obj_rec(object_db_rec_t *obj_rec, int i);

void print_obj_db(object_db_t *obj_db);


void mld_register_root_object (object_db_t* obj_db, void *obj_ptr, char *struct_name, unsigned int units);

void mld_set_dynamic_object_as_root(object_db_t *obj_db, void *object_ptr);

void *xnew(object_db_t *object_db, const char *class_name, int units);

void run_mld_algorithm(object_db_t *obj_db);

void report_leaked_objects(object_db_t *obj_db);

void mld_init_primitive_data_type_support(class_db_t *class_db);

#endif /* mld_hpp */
