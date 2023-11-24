#include "mld.hpp"
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


const char *DATA_TYPE[] = {"UINT8", "UINT16", "UINT32"
    "CHAR", "OBJ_PTR", "FLOAT",
    "DOUBLE", "OBJ_CLASS"};

void print_class_rec(class_db_rec_t *class_rec)
{
    if(!class_rec) return;
    int j = 0;
    field_info_t *field = NULL;
    std::cout << '|' <<  std::setfill('-') << std::setw(55) << "|\n";

    std::cout << '|' << std::setfill(' ') << std::setw(15) << std::left << class_rec->class_name << '|'
    << "Size= " <<  std::setw(9) << class_rec->ds_size << '|' << std::right
    << " #flds= " << std::setw(13) << class_rec->n_fields << "|\n";
    
    std::cout << '|' <<  std::setfill('-') << std::setw(54) << "|";
    std::cout << std::setfill('-') << std::setw(66) << "|\n";
    for(; j<class_rec->n_fields; ++j)
    {
        field = &class_rec->fields[j];
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

void print_class_db(class_db_t *class_db)
{
    if(!class_db) return;
    std::cout << "Printing Class DB\n";
    int i = 0;
    class_db_rec_t *class_rec = class_db->head;
    std::cout << "No. of Class Registered = " << class_db->count << "\n";
    while(class_rec)
    {
        std::cout << "Class Number: " << i++ << ' ' << class_rec << '\n';
        print_class_rec(class_rec);
        class_rec = class_rec->next;
    }
}

bool add_class_to_class_db(class_db_t *class_db,
                           class_db_rec_t *class_rec)
{
    class_db_rec_t *head = class_db->head;
    
    if(!head)
    {
        class_db->head = class_rec;
        class_rec->next = NULL;
        (class_db->count)++;
        return true;
    }
    
    class_rec->next = head;
    class_db->head = class_rec;
    (class_db->count)++;
    return true;
}


static class_db_rec_t* class_db_look_up(class_db_t *class_db, const char *class_name)
{
    class_db_rec_t *head = class_db->head;
    if(!head) return nullptr;
    
    for(; head; head = head->next)
        if(strncmp(head->class_name, class_name, MAX_CLASS_NAME_SIZE) == 0)
            return head;
    
    return nullptr;
}

static object_db_rec_t* object_db_look_up(object_db_t *obj_db, void *ptr)
{
    object_db_rec_t *head = obj_db->head;
    if(!head) return nullptr;
    for(; head; head = head->next)
        if(head->ptr == ptr)
            return head;
    
    return nullptr;
}

static void add_object_to_object_db(object_db_t *obj_db, void *ptr, int units,
                                    class_db_rec_t *class_rec, bool is_root = false)
{
    object_db_rec_t *obj_rec = object_db_look_up(obj_db, ptr);
    assert(!obj_rec);
    obj_rec = (object_db_rec_t*)calloc(1, sizeof(object_db_rec_t));
    obj_rec->next = nullptr;
    obj_rec->ptr = ptr;
    obj_rec->units = units;
    obj_rec->class_rec = class_rec;
    obj_rec->is_root = is_root;
    object_db_rec_t *head = obj_db->head;
    
    if(!head)
    {
        obj_db->head = obj_rec;
        obj_rec->next = nullptr;
        (obj_db->count)++;
        return;
    }
    
    obj_rec->next = head;
    obj_db->head = obj_rec;
    (obj_db->count)++;
}

void *xnew(object_db_t *object_db, const char *class_name, int units)
{
    class_db_rec_t *class_rec = class_db_look_up(object_db->class_db, class_name);
    assert(class_rec);
    void *ptr = calloc(units, class_rec->ds_size);
    add_object_to_object_db(object_db, ptr, units, class_rec);
    return ptr;
    
}



void mld_register_root_object (object_db_t* obj_db, void *obj_ptr, char *class_name, unsigned int units)
{
    class_db_rec_t *class_rec = class_db_look_up(obj_db->class_db, class_name);
    assert(class_rec);
    add_object_to_object_db(obj_db, obj_ptr, units, class_rec, true);
}

void mld_set_dynamic_object_as_root(object_db_t *obj_db, void *object_ptr)
{
    object_db_rec_t *obj_rec = object_db_look_up(obj_db, object_ptr);
    assert(obj_rec);
    obj_rec->is_root = true;
}


void print_obj_rec(object_db_rec_t *obj_rec, int i)
{
    if(!obj_rec) return;
    
    std::cout << std::setfill('-') << std::setw(102) << "|\n";
    std::cout << std::setfill(' ') << std::left << std::setw(3) << i
    << "ptr = " << std::setw(10) << obj_rec->ptr
    << " | next = " << std::setw(12) << obj_rec->next
    << " | units = " << std::setw(4) << obj_rec->units
    << " | class_name = " << std::setw(9) << obj_rec->class_rec->class_name
    << " | is_root = " << (obj_rec->is_root ? "TRUE" : "FALSE") << "|\n";
    std::cout << std::setfill('-') << std::setw(102)<< std::right <<  "|\n";
}

void print_obj_db(object_db_t *obj_db)
{
    object_db_rec_t *obj_rec = obj_db->head;
    unsigned int i = 0;
    std::cout << "\nPrinting OBJECT DATABASE\n";
    for(; obj_rec; obj_rec = obj_rec->next)
        print_obj_rec(obj_rec, i++);
}


static void init_mld_algorithm(object_db_t *obj_db)
{
    object_db_rec_t *obj_rec = obj_db->head;
    for (;obj_rec; obj_rec = obj_rec->next) {
        obj_rec->is_visited = false;
    }
}

object_db_rec_t *get_next_root_object(object_db_t *obj_db, object_db_rec_t *starting_from_here)
{
    object_db_rec_t *first = starting_from_here ? starting_from_here->next : obj_db->head;
    
    for(; first; first = first->next)
    {
        if(first->is_root)
            return first;
    }
    return nullptr;
}

void DFS(object_db_t *object_db, object_db_rec_t *parent_obj_rec)
{
    if(parent_obj_rec->is_visited)
        return;
    
    parent_obj_rec->is_visited = true;
    
    unsigned int i, n_fields;
    char *parent_obj_ptr = nullptr,
    *child_obj_offset = nullptr;
    void *child_object_address = nullptr;
    field_info_t *field_info = nullptr;
    
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
                    
                    child_obj_rec = object_db_look_up(object_db, child_object_address);
                    assert(child_obj_rec);
                    
                    if(!child_obj_rec->is_visited)
                        DFS(object_db, child_obj_rec);
            }
        }
    }
    
}

void run_mld_algorithm(object_db_t *obj_db)
{
    init_mld_algorithm(obj_db);
    object_db_rec_t *root_obj = get_next_root_object(obj_db, nullptr);
    
    for(; root_obj; root_obj = get_next_root_object(obj_db, root_obj))
    {
        if(!(root_obj->is_visited))
        {
            DFS(obj_db, root_obj);
        }
    }
}

void mld_dump_object_rec_detail(object_db_rec_t *obj_rec)
{
    int n_fields = obj_rec->class_rec->n_fields;
    field_info_t *field = nullptr;
    
    int units = obj_rec->units, obj_index = 0,
    field_index = 0;
    
    for(; obj_index < units; ++obj_index)
    {
        char *current_object_ptr = (char *)(obj_rec->ptr) + \
        (obj_index * obj_rec->class_rec->ds_size);
        
        for(field_index = 0; field_index < n_fields; ++field_index)
        {
            field = &(obj_rec->class_rec->fields[field_index]);
            switch (field->dtype) {
                case UINT8:
                case UINT16:
                case UINT32:
                    std::cout << obj_rec->class_rec->class_name << "[" << obj_index <<"]->"
                    << field->fname << " = " << * (int *)(current_object_ptr + field->offset) << '\n';
                    break;
                case CHAR:
                    std::cout << obj_rec->class_rec->class_name << "[" << obj_index <<"]->"
                    << field->fname << " = " << * (char *)(current_object_ptr + field->offset) << '\n';
                    break;
                case FLOAT:
                    std::cout << obj_rec->class_rec->class_name << "[" << obj_index <<"]->"
                    << field->fname << " = " << * (float *)(current_object_ptr + field->offset) << '\n';
                    break;
                case DOUBLE:
                    std::cout << obj_rec->class_rec->class_name << "[" << obj_index <<"]->"
                    << field->fname << " = " << * (double *)(current_object_ptr + field->offset) << '\n';
                    break;
                case OBJ_PTR:
                    std::cout << obj_rec->class_rec->class_name << "[" << obj_index <<"]->"
                    << field->fname << " = " << (void *)*(int *)(current_object_ptr + field->offset) << '\n';
                    break;
                case OBJ_CLASS:
                default:
                    break;
            }
        }
    }
    
}


void report_leaked_objects(object_db_t *obj_db)
{
    int i = 0;
    object_db_rec_t *head = obj_db->head;
    std::cout << "\nPrinting Leaked Objects\n";
    
    for(; head; head = head->next)
    {
        if(!head->is_visited)
        {
            print_obj_rec(head, i++);
            mld_dump_object_rec_detail(head);
            std::cout << "\n\n";
        }
    }
}

void mld_init_primitive_data_type_support(class_db_t *class_db)
{
    REG_CLASS(class_db, "char", nullptr, 0, sizeof(char));
    REG_CLASS(class_db, "bool", nullptr, 0, sizeof(bool));
    REG_CLASS(class_db, "int", nullptr, 0, sizeof(int));
    REG_CLASS(class_db, "float", nullptr, 0, sizeof(float));
    REG_CLASS(class_db, "double", nullptr, 0, sizeof(double));
    
}
