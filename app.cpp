


#include <iostream>
#include "mld.hpp"
#include <cctype>
#include <cassert>

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
    mld_init_primitive_data_type_support(class_db);

        /*Create structure record for structure emp_t*/
        static field_info_t emp_fields[] = {
            FIELD_INFO(emp_t, emp_name, CHAR,    0),
            FIELD_INFO(emp_t, emp_id,   UINT32,  0),
            FIELD_INFO(emp_t, age,      UINT32,  0),
            FIELD_INFO(emp_t, mgr,      OBJ_PTR, emp_t),
            FIELD_INFO(emp_t, salary,   FLOAT, 0),
            FIELD_INFO(emp_t, p, OBJ_PTR, 0)
        };
    
    REG_CLASS(class_db, "emp_t", emp_fields, 6, sizeof(emp_t));

        static field_info_t stud_fields[] = {
            FIELD_INFO(student_t, stud_name, CHAR, 0),
            FIELD_INFO(student_t, rollno,    UINT32, 0),
            FIELD_INFO(student_t, age,       UINT32, 0),
            FIELD_INFO(student_t, aggregate, FLOAT, 0),
            FIELD_INFO(student_t, bestfriend, OBJ_PTR, student_t)
        };
    REG_CLASS(class_db, "student_t", stud_fields, 5, sizeof(student_t));

        print_class_db(class_db);
    
    object_db_t *obj_db = new object_db_t;
    obj_db->class_db = class_db;
    
    
    student_t *tim = (student_t *)xnew(obj_db, "student_t", 1);
    student_t *jim = (student_t *)xnew(obj_db, "student_t", 1);
    strncpy(jim->stud_name, "jim", strlen("jim"));
    emp_t *micheal = (emp_t *)xnew(obj_db, "emp_t", 2);
    
    mld_set_dynamic_object_as_root(obj_db, tim);
    
    micheal->p = (int *)xnew(obj_db, "int", 1);
    micheal->p = nullptr;
    mld_set_dynamic_object_as_root(obj_db, micheal);
    print_obj_db(obj_db);
    
    run_mld_algorithm(obj_db);
    std::cout << "Leaked Objects: \n";
    report_leaked_objects(obj_db);
    
        return 0;
}
