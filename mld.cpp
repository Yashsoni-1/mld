#include "mld.hpp"
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


const char *DATA_TYPE[] = {"UINT8", "UINT16", "UINT32"
    "CHAR", "OBJ_PTR", "FLOAT",
    "DOUBLE", "OBJ_CLASS"};

class_db_t::class_db_t() : count(0), head(nullptr)
{
    mld_init_primitive_data_type_support();
}

class_db_t::~class_db_t()
{
    class_db_rec_t *temp_head = head;
    
    for( ;head; )
    {
        temp_head = head->next;
        delete head;
        head = temp_head;
    }
}

class_db_rec_t::class_db_rec_t() {}

object_db_t::object_db_t(class_db_t *class_db) : class_db(class_db)
{ }

object_db_t::~object_db_t()
{
    object_db_rec_t *temp_head = head;
    
    for( ;head; )
    {
        temp_head = head->next;
        delete head;
        head = temp_head;
    }
}

void class_db_rec_t::print_class_rec() const
{
    int j = 0;
    const field_info_t *field = nullptr;
    std::cout << '|' <<  std::setfill('-') << std::setw(55) << "|\n";

    std::cout << '|' << std::setfill(' ') << std::setw(13) << std::left << class_name << '|'
    << "Size = " <<  std::setw(10) << ds_size << '|' << std::right
    << " #flds = " << std::setw(12) << n_fields << "|\n";
    
    std::cout << '|' <<  std::setfill('-') << std::setw(54) << "|";
    std::cout << std::setfill('-') << std::setw(66) << "|\n";
    for(; j<n_fields; ++j)
    {
        field = &fields[j];
        std::cout << std::setfill(' ')<<  std::setw(15) << "|";
        std::cout << std::setw(3) << j << std::setw(15) << field->fname
        << "| dtype = " << std::setw(15) << DATA_TYPE[field->dtype]
        << "| size = " << std::setw(5) << field->size
        << "| Offset = " << std::setw(6) << field->offset
        << "| nclassname = " << std::left << std::setw(15) << field->nested_class_name << "|\n";
        
        std::cout << std::setw(14) << "" << "|";
        std::cout << std::right << std::setfill('-') << std::setw(105) << "|" << "\n";
        
        
    }
}

void class_db_t::print_class_db() const
{
    std::cout << "Printing Class DB\n";
    int i = 0;
    class_db_rec_t *class_rec = head;
    std::cout << "No. of Class Registered = " << count << "\n";
    while(class_rec)
    {
        std::cout << "Class Number: " << i++ << ' ' << class_rec << '\n';
        class_rec->print_class_rec();
        class_rec = class_rec->next;
    }
}

void class_db_t::REG_CLASS(const char *cl_name, const field_info_t *fields_arr,
                           const size_t no_of_fields, const size_t class_size)
{
    auto rec = new class_db_rec_t;
    strncpy(rec->class_name, cl_name, MAX_CLASS_NAME_SIZE);
    rec->ds_size = class_size;
    rec->n_fields = no_of_fields;
    rec->fields = fields_arr;
    if(add_class_to_class_db(rec)){
        return;
    }
}

bool class_db_t::add_class_to_class_db(class_db_rec_t *class_rec)
{
    class_db_rec_t *temp_head = head;
    
    if(!temp_head)
    {
        head = class_rec;
        class_rec->next = nullptr;
        (count)++;
        return true;
    }
    
    class_rec->next = head;
    head = class_rec;
    ++count;
    return true;
}


class_db_rec_t* class_db_t::class_db_look_up(const char *class_name)
{
    class_db_rec_t *temp_head = head;
    if(!temp_head) return nullptr;
    
    for(; temp_head; temp_head = temp_head->next)
        if(strncmp(temp_head->class_name, class_name, MAX_CLASS_NAME_SIZE) == 0)
            return temp_head;
    
    return nullptr;
}

object_db_rec_t* object_db_t::object_db_look_up(const void *ptr) const
{
    object_db_rec_t *temp_head = head;
    if(!temp_head) return nullptr;
    for(; temp_head; temp_head = temp_head->next)
        if(temp_head->ptr == ptr)
            return temp_head;
    
    return nullptr;
}

void object_db_t::add_object_to_object_db(void *const ptr, const size_t units,
                                        class_db_rec_t * const class_rec, bool is_root)
{
    object_db_rec_t *obj_rec = object_db_look_up(ptr);
    assert(!obj_rec);
    obj_rec = new object_db_rec_t;
    obj_rec->next = nullptr;
    obj_rec->ptr = ptr;
    obj_rec->units = units;
    obj_rec->class_rec = class_rec;
    obj_rec->is_root = is_root;
    
    
    object_db_rec_t *temp_head = head;
    
    if(!temp_head)
    {
        head = obj_rec;
        obj_rec->next = nullptr;
        (count)++;
        return;
    }
    
    obj_rec->next = head;
    head = obj_rec;
    (count)++;
}

void object_db_t::remove_obj_rec_from_db(object_db_rec_t *obj_rec)
{
    assert(obj_rec);
    
    object_db_rec_t *temp_head = head;
    object_db_rec_t *prev{temp_head};
    
    for(; temp_head; temp_head = temp_head->next)
    {
        if(temp_head == obj_rec)
        {
            prev->next = temp_head->next;
            break;
        }
        
        prev = temp_head;
    }
        
}

void *object_db_t::xnew(const char * class_name, size_t units)
{
    class_db_rec_t *class_rec = class_db->class_db_look_up(class_name);
    assert(class_rec);
    void *ptr = calloc(units, class_rec->ds_size);
    add_object_to_object_db(ptr, units, class_rec);
    return ptr;
    
}

void object_db_t::xfree(void *ptr)
{
    object_db_rec_t * obj_rec = object_db_look_up(ptr);
    assert(obj_rec);
    remove_obj_rec_from_db(obj_rec);
    
    free(obj_rec->ptr);
    delete obj_rec;
}



void object_db_t::mld_register_root_object (void * const obj_ptr, char * class_name, const size_t units)
{
    class_db_rec_t *class_rec = class_db->class_db_look_up(class_name);
    assert(class_rec);
    add_object_to_object_db(obj_ptr, units, class_rec, true);
}

void object_db_t::mld_set_dynamic_object_as_root(const void *object_ptr)
{
    object_db_rec_t *obj_rec = object_db_look_up(object_ptr);
    assert(obj_rec);
    obj_rec->is_root = true;
}


void object_db_rec_t::print_obj_rec(size_t i) const
{
    std::cout << std::setfill('-') << std::setw(110) << "|\n";
    std::cout << std::setfill(' ') << std::left << std::setw(3) << i
    << "ptr = " << std::setw(10) << ptr
    << " | next = " << std::setw(15) << next
    << " | units = " << std::setw(4) << units
    << " | class_name = " << std::setw(10) << class_rec->class_name
    << " | is_root = " << std::setw(6) << (is_root ? "TRUE" : "FALSE") << "|\n";
    std::cout << std::setfill('-') << std::setw(110)<< std::right <<  "|\n";
}

void object_db_t::print_obj_db() const
{
    object_db_rec_t *obj_rec = head;
    size_t i = 0;
    std::cout << "\nPrinting OBJECT DATABASE\n";
    for(; obj_rec; obj_rec = obj_rec->next)
        obj_rec->print_obj_rec(i++);
}


void object_db_t::init_mld_algorithm()
{
    object_db_rec_t *obj_rec = head;
    for (;obj_rec; obj_rec = obj_rec->next) {
        obj_rec->is_visited = false;
    }
}

object_db_rec_t *object_db_t::get_next_root_object(const object_db_rec_t *starting_from_here) const
{
    object_db_rec_t *first = starting_from_here ? starting_from_here->next : head;
    
    for(; first; first = first->next)
    {
        if(first->is_root)
            return first;
    }
    return nullptr;
}

void object_db_t::DFS(object_db_rec_t *parent_obj_rec) const 
{
    if(parent_obj_rec->is_visited)
        return;
    
    parent_obj_rec->is_visited = true;
    
    unsigned int i, n_fields;
    char *parent_obj_ptr = nullptr,
    *child_obj_offset = nullptr;
    void *child_object_address = nullptr;
    const field_info_t *field_info = nullptr;
    
    object_db_rec_t *child_obj_rec = nullptr;
    class_db_rec_t *parent_class_rec = parent_obj_rec->class_rec;
    
    if(parent_class_rec->n_fields == 0)
        return;
    
    for (i = 0; i < parent_obj_rec->units; ++i)
    {
        parent_obj_ptr = (char *)(parent_obj_rec->ptr) + (i * parent_class_rec->ds_size);
        
        for (n_fields = 0; n_fields < parent_class_rec->n_fields; ++n_fields)
        {
            field_info = &parent_class_rec->fields[n_fields];
            
            switch (field_info->dtype) {
                case UINT8:
                case UINT16:
                case UINT32:
                case FLOAT:
                case CHAR:
                case DOUBLE:
                case OBJ_CLASS:
                    break;
                case OBJ_PTR:
                default:
                    ;
                    
                    child_obj_offset = parent_obj_ptr + field_info->offset;
                    memcpy(&child_object_address, child_obj_offset, sizeof(void *));
                    if(!child_object_address) continue;
                    
                    child_obj_rec = object_db_look_up(child_object_address);
                    assert(child_obj_rec);
                    
                    if(!child_obj_rec->is_visited)
                        DFS(child_obj_rec);
            }
        }
    }
}

void object_db_t::run_mld_algorithm()
{
    init_mld_algorithm();
    object_db_rec_t *root_obj = get_next_root_object(nullptr);
    
    for(; root_obj; root_obj = get_next_root_object(root_obj))
    {
        if(!(root_obj->is_visited))
        {
            DFS(root_obj);
        }
    }
}

void object_db_rec_t::mld_dump_object_rec_detail() const
{
    size_t n_fields = class_rec->n_fields;
    const field_info_t *field = nullptr;
    
    size_t temp_units = units, obj_index = 0,
    field_index = 0;
    
    for(; obj_index < temp_units; ++obj_index)
    {
        char *current_object_ptr = (char *)(ptr) + \
        (obj_index * class_rec->ds_size);
        
        for(field_index = 0; field_index < n_fields; ++field_index)
        {
            field = &(class_rec->fields[field_index]);
            switch (field->dtype) {
                case UINT8:
                case UINT16:
                case UINT32:
                    std::cout << class_rec->class_name << "[" << obj_index <<"]->"
                    << field->fname << " = " << * (int *)(current_object_ptr + field->offset) << '\n';
                    break;
                case CHAR:
                    std::cout << class_rec->class_name << "[" << obj_index <<"]->"
                    << field->fname << " = " << * (char *)(current_object_ptr + field->offset) << '\n';
                    break;
                case FLOAT:
                    std::cout << class_rec->class_name << "[" << obj_index <<"]->"
                    << field->fname << " = " << * (float *)(current_object_ptr + field->offset) << '\n';
                    break;
                case DOUBLE:
                    std::cout << class_rec->class_name << "[" << obj_index <<"]->"
                    << field->fname << " = " << * (double *)(current_object_ptr + field->offset) << '\n';
                    break;
                case OBJ_PTR:
                    std::cout << class_rec->class_name << "[" << obj_index <<"]->"
                    << field->fname << " = " << (void *)*(unsigned long *)(current_object_ptr + field->offset) << '\n';
                    break;
                case OBJ_CLASS:
                default:
                    break;
            }
        }
    }
}


void object_db_t::report_leaked_objects() const
{
    int i = 0;
    object_db_rec_t *temp_head = head;
    std::cout << "\nPrinting Leaked Objects\n";
    
    for(; temp_head; temp_head = temp_head->next)
    {
        if(!temp_head->is_visited)
        {
            temp_head->print_obj_rec(i++);
            temp_head->mld_dump_object_rec_detail();
        }
    }
}

void class_db_t::mld_init_primitive_data_type_support()
{
    REG_CLASS("char",  nullptr, 0, sizeof(char));
    REG_CLASS("bool",  nullptr, 0, sizeof(bool));
    REG_CLASS("int",   nullptr, 0, sizeof(int));
    REG_CLASS("float", nullptr, 0, sizeof(float));
    REG_CLASS("double",nullptr, 0, sizeof(double));
}
