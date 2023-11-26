


#include <iostream>
#include <cctype>
#include <cassert>
#include "mld.hpp"



struct linked_list_node
{
    int i;
    linked_list_node *next;
    
    linked_list_node() : i(0), next(nullptr) { }
};

struct linked_list
{
    linked_list_node *head;
    linked_list() : head(nullptr) { }
};

class emp_t
{
public:
    char emp_name[30];
    unsigned int emp_id;
    unsigned int age;
    emp_t *mgr;
    float salary;
    int *p;
};

class student_t
{
public:
    char stud_name[30];
    unsigned int rollno;
    unsigned int age;
    float aggregate;
    student_t *bestfriend;
};



int main(int argc, const char * argv[]) {
    
    /*Step 1 : Initialize a new structure database */
    class_db_t *class_db = new class_db_t;

        /*Step 2 : Create structure record for structure emp_t*/
        static field_info_t emp_fields[] = {
            FIELD_INFO(emp_t, emp_name, CHAR,    0),
            FIELD_INFO(emp_t, emp_id,   UINT32,  0),
            FIELD_INFO(emp_t, age,      UINT32,  0),
            FIELD_INFO(emp_t, mgr,      OBJ_PTR, emp_t),
            FIELD_INFO(emp_t, salary,   FLOAT, 0),
            FIELD_INFO(emp_t, p, OBJ_PTR, 0)
        };
    
    /*Step 3 : Register structure record for structure emp_t*/
    class_db->REG_CLASS("emp_t", emp_fields, 6, sizeof(emp_t));

        static field_info_t stud_fields[] = {
            FIELD_INFO(student_t, stud_name, CHAR, 0),
            FIELD_INFO(student_t, rollno,    UINT32, 0),
            FIELD_INFO(student_t, age,       UINT32, 0),
            FIELD_INFO(student_t, aggregate, FLOAT, 0),
            FIELD_INFO(student_t, bestfriend, OBJ_PTR, student_t)
        };
    class_db->REG_CLASS("student_t", stud_fields, 5, sizeof(student_t));

    class_db->print_class_db();
    
    /*Step 4 : Initialize object_db_t with class db created above*/
    object_db_t *obj_db = new object_db_t(class_db);
    
    student_t *tim = (student_t *)obj_db->xnew("student_t", 1);
    /*Step 5 : Set dynamic object as root*/
    obj_db->mld_set_dynamic_object_as_root(tim);
    
    /*--- Leak 1 ---*/
    student_t *jim = (student_t *)obj_db->xnew("student_t", 1);
    strncpy(jim->stud_name, "jim", strlen("jim"));
    emp_t *micheal = (emp_t *)obj_db->xnew("emp_t", 2);
    
    
    micheal->p = (int *)obj_db->xnew("int", 1);
    /*--- Leak 2 ---*/;
    micheal->p = nullptr;
    
    obj_db->mld_set_dynamic_object_as_root(micheal);
    obj_db->print_obj_db();
    
    [[maybe_unused]]student_t *kim = new student_t;
    std::cout << "address of tim = " << tim << '\n';
    obj_db->xfree((void *)tim);
    obj_db->print_obj_db();
    
    
    static field_info_t linked_list_node_fields[] =
    {
        FIELD_INFO(linked_list_node, i, UINT32, 0),
        FIELD_INFO(linked_list_node, next, OBJ_PTR, linked_list_node),
    };
    
    class_db->REG_CLASS("linked_list_node", linked_list_node_fields, 2, sizeof(linked_list_node));
    
    static field_info_t linked_list_fields[] =
    {
        FIELD_INFO(linked_list, head, OBJ_PTR, linked_list_node)
    };
    
    class_db->REG_CLASS("linked_list", linked_list_fields, 1, sizeof(linked_list));
    
    linked_list ls;
    ls.head = (linked_list_node *)obj_db->xnew("linked_list_node", 1);
    auto temp_head = ls.head;
    for(size_t j=0; j<10; ++j)
    {
        temp_head->next = (linked_list_node *)obj_db->xnew("linked_list_node", 1);
        temp_head = temp_head->next;
        
    }
    
    obj_db->mld_set_dynamic_object_as_root(ls.head);
    
    obj_db->print_obj_db();
    
    /*--- Leak 3 ---*/
    ls.head->next = nullptr;
    
    obj_db->run_mld_algorithm();
    std::cout << "Leaked Objects: \n";
    obj_db->report_leaked_objects();
    
    delete obj_db;
    delete class_db;
    
        return 0;
}
