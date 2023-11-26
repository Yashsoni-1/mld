
# C++ Memory Leak Detector
## Overview:
In this project I have created a _C++_ memory leak detector tool. This tool keeps track of all the heap objects that the program has created and also how they hold reference to one another. This tool will tell you all the leaked objects, if any. 
## But Why Do We Need MLD?
Since the advent of _C/C++_, memory management is one of the main responsibility that the programmer has to deal with. C/C++ programs often suffer from two memory related problems
which are _Memory Corruption_ and _Memory Leaks_. Unlike _Java_, _C/C++_ doesn't have the luxury of garbage collection. Since other programming languages does not allow programmer to access the memory directly, they do not suffer memory corruption.


## How does it works Internally?
It contains two databases first one is class_db_t, to store the info of all the structures the application is using and second one is object_db_t, to store all the dynamic objects created by the application. After initializing and registering all the structures/classes and dynamic objects we enter into the next phase. In last phase we run MLD algorithm, which is basically DFS(depth first search algorithm) on our object database with the help of structure databse in order to find out all the objects application is leaking. 
Actually finding all the leaked objects is the graph problem. Given graph of nodes and edges, find all the nodes which are not reachable from any other node(Island Problem). Here nodes are objects and edges are references. 

![Graph A img](https://github.com/Yashsoni-1/mld/blob/main/images/Screenshot%202023-11-26%20at%2012.17.17%20PM.png)  ![Graph B img](https://github.com/Yashsoni-1/mld/blob/main/images/Screenshot%202023-11-26%20at%2012.17.48%20PM.png) ![Legends img](https://github.com/Yashsoni-1/mld/blob/main/images/Screenshot%202023-11-26%20at%2011.57.04%20AM.png)


Therefore:
- MLD must know the all the structures being used by application.
- It is the responsibility of the application to tell the MLD library during initialization about all the structures it is using.
- MLD library will maintain the structure database(linked list) to store application structure info.
- Leaked objects are those objects which are not reachable from any other objects or actually root objects.
- Root objects are usually the global or static objects maintained by the application.
- Every other dynamic object must be reachable from at-least one root object.
- Dynamic objects which are not reachable are leaked objects.
- Global objects are referenced by the global variables in application, so, they never leak.
- MLD assumes that the dynamic root objects are never leaked by the application.
- And, if the dynamic root objects are somehow leaked, MLD cannot catch them since the MLD algorithm starts from the root objects.
- Small hack can be to make them const pointers. So, you are never leaking them.
- This works because data structures always take the form of disjoint set of graphs with root nodes.
- One more thing is that this tool works fine if we write in _java_ style. But, fails we write pure c++ style. For e.g. it cannot handle unions and embedded objects.
- This is the main reason why I guess c++ doesn't have any garbage collector.

## How do Get Started?
You can download the library from __gitbub__.

We can start with MLD in 4 simple steps

`Step 1 :` Initialize a new structure database.

![step 1 img](https://github.com/Yashsoni-1/mld/blob/main/images/Screenshot%202023-11-26%20at%2012.30.17%20PM.png)

`Step 2 :` Create structure record for structure.

![step 2 img](https://github.com/Yashsoni-1/mld/blob/main/images/Screenshot%202023-11-26%20at%2012.30.32%20PM.png)

`Step 3 :` Register structure record for structure.

![step 3 img](https://github.com/Yashsoni-1/mld/blob/main/images/Screenshot%202023-11-26%20at%2012.30.48%20PM.png)

`Step 4 :` Initialize object_db_t with class db created above.

![step 4 img](https://github.com/Yashsoni-1/mld/blob/main/images/Screenshot%202023-11-26%20at%2012.33.49%20PM.png)

## API
- `mld_set_dynamic_object_as_root` - To the dynamic object as root object.
- `mld_register_root_object` - To set other static and global variable as root.
- `xnew` - To dynamically create the object.
- `xdelete` - To dynamically create the object.
- `run_mld_algorithm` - To run the mld algorithm
- `report_leaked_objects` - To print all the leaked objects.
- `print_obj_db` - To print all the objects.
- `print_class_db` - To print all the class and structures registered.

  
## Reference
- Writing garbage collector [link](https://youtu.be/2JgEKEd3tw8?si=3p647qL57Fr8JiSy)
- Garbage collector [link](https://www.udemy.com/course/memory-leak-detector/)
- Java Memory Model [link](https://www.digitalocean.com/community/tutorials/java-jvm-memory-model-memory-management-in-java#java-memory-model-permanent-generation)
  
