/*
A1
dziorli
1016491
*/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
 
#include "LinkedListAPI.h"

List* initializeList(char* (*printFunction)(void* toBePrinted),void (*deleteFunction)(void* toBeDeleted),int (*compareFunction)(const void* first,const void* second)){

    if (printFunction == NULL || deleteFunction == NULL || compareFunction == NULL){
        return NULL;
    }
    List * temp = (List*)malloc(sizeof(List));
    temp->head = NULL;
    temp->tail = NULL;
    temp->length = 0;
    temp->deleteData = deleteFunction;
    temp->compare = compareFunction;
    temp->printData = printFunction;
    return temp;
}



/**Function for creating a node for the linked list. 
* This node contains abstracted (void *) data as well as previous and next
* pointers to connect to other nodes in the list
*@pre data should be of same size of void pointer on the users machine to avoid size conflicts. data must be valid.
*data must be cast to void pointer before being added.
*@post data is valid to be added to a linked list
*@return On success returns a node that can be added to a linked list. On failure, returns NULL.
*@param data - a void * pointer to any data type.  Data must be allocated on the heap.
**/
Node* initializeNode(void* data){

    if (data == NULL){
        return NULL;
    }
    Node * temp = malloc(sizeof(Node));
    temp->data = data;
    temp->previous = NULL;
    temp->next = NULL;
    return temp;
}



/**Inserts a Node at the front of a linked list.  List metadata is updated
* so that head and tail pointers are correct.
*@pre 'List' type must exist and be used in order to keep track of the linked list.
*@param list pointer to the List struct
*@param toBeAdded - a pointer to data that is to be added to the linked list
**/
void insertFront(List* list, void* toBeAdded){

    if (list == NULL || toBeAdded == NULL){
        return;
    }
    Node * toAdd = initializeNode(toBeAdded);
    if (list->head == NULL && list->tail == NULL){
        list->head = toAdd;
        list->tail = toAdd;
        list->length += 1;
        return;
    }
    list->head->previous = toAdd;
    toAdd->next = list->head;
    list->head = toAdd;
    list->length += 1;

    return;
}



/**Inserts a Node at the back of a linked list. 
*List metadata is updated so that head and tail pointers are correct.
*@pre 'List' type must exist and be used in order to keep track of the linked list.
*@param list pointer to the List struct
*@param toBeAdded - a pointer to data that is to be added to the linked list
**/
void insertBack(List* list, void* toBeAdded){

    if (list == NULL || toBeAdded == NULL){
        return;
    }
    Node * toAdd = initializeNode(toBeAdded);
    if (list->head == NULL && list->tail == NULL){
        list->head = toAdd;
        list->tail = toAdd;
        list->length += 1;
        return;
    }
    list->tail->next = toAdd;
    toAdd->previous = list->tail;
    list->tail = toAdd;
    list->length += 1;

    return;
}



/** Deletes the entire linked list, freeing all memory asssociated with the list, including the list struct itself.
* Uses the supplied function pointer to release allocated memory for the data.
* @pre 'List' type must exist and be used in order to keep track of the linked list.
* @param list pointer to the List struct
**/
void freeList(List* list){
    if (list == NULL){
        return;
    }
    ListIterator iterator = createIterator(list);
    while (iterator.current != NULL){
        Node * temp = nextElement(&iterator);
        list->deleteData(temp->data);
        free(temp);
    }
    free(list);
    return;
}

/** Clears the list: frees the contents of the list - Node structs and data stored in them - 
 * without deleting the List struct
 * uses the supplied function pointer to release allocated memory for the data
 * @pre 'List' struct must exist and be used in order to keep track of the linked list.
 * @post List struct still exists, list head = list tail = NULL, list length = 0
 * @param list pointer to the List-type dummy node
 * @return  on success: NULL, on failure: head of list
**/
void clearList(List* list){
    if (list == NULL){
        return;
    }
    ListIterator iterator = createIterator(list);
    while (iterator.current != NULL){
        Node * temp = nextElement(&iterator);
        list->deleteData(temp->data);
        free(temp);
    }
    list->head = NULL;
    list->tail = NULL;
    list->length = 0;
    return;
}


/** Uses the comparison function pointer to place the element in the 
* appropriate position in the list.
* should be used as the only insert function if a sorted list is required.  
*@pre List exists and has memory allocated to it. Node to be added is valid.
*@post The node to be added will be placed immediately before or after the first occurrence of a related node
*@param list - a pointer to the List struct
*@param toBeAdded - a pointer to data that is to be added to the linked list
**/
void insertSorted(List* list, void* toBeAdded){
    ListIterator iterator = createIterator(list);
    if (iterator.current == NULL || toBeAdded == NULL){
        insertFront(list, toBeAdded);
        return;
    }
    if (getLength(list) == 1){
        if (list->compare(toBeAdded, list->head->data) <= 0){
            insertFront(list, toBeAdded);
        }
        else{
            insertBack(list, toBeAdded);
        }
        return;
    }
    if (list->compare(toBeAdded, list->head->data) <= 0){
        insertFront(list, toBeAdded);
        return;
    }
    if (list->compare(toBeAdded, list->tail->data) >= 0){
        insertBack(list, toBeAdded);
        return;
    }

    Node * toAdd = initializeNode(toBeAdded);
    while (iterator.current != NULL){
        Node * currentNode = nextElement(&iterator);
        if (list->compare(toBeAdded, currentNode->data) <= 0){
            Node * prevNode = currentNode->previous;

            toAdd->next = currentNode;
            toAdd->previous = prevNode;
            if (prevNode != NULL){
                prevNode->next = toAdd;
            }
            currentNode->previous = toAdd;
            list->length += 1;
            return;
        }
    }

    return;
}


/** Removes data from from the list, deletes the node and frees the memory,
 * changes pointer values of surrounding nodes to maintain list structure.
 * returns the data 
 * You can assume that the list contains no duplicates
 *@pre List must exist and have memory allocated to it
 *@post toBeDeleted will have its memory freed if it exists in the list.
 *@param list - a pointer to the List struct
 *@param toBeDeleted - a pointer to data that is to be removed from the list
 *@return on success: void * pointer to data  on failure: NULL
 **/
void* deleteDataFromList(List* list, void* toBeDeleted){
    ListIterator iterator = createIterator(list);
    if (iterator.current == NULL || toBeDeleted == NULL){
        return NULL;
    }
    if (list->compare(toBeDeleted, list->head->data) == 0){
        Node * nodeToDelete = list->head;
        void * reutrnData = nodeToDelete->data;

        list->head = nodeToDelete->next;
        list->deleteData(nodeToDelete->data);
        free(nodeToDelete);
        list->length -=1;
        return reutrnData;
    }

    while (iterator.current != NULL){
        Node * currentNode = nextElement(&iterator);
        if (list->compare(toBeDeleted, currentNode->data) == 0){
            void * returnData = currentNode->data;
            Node * prevNode = currentNode->previous;
            Node * nextNode = currentNode->next;

            if (prevNode != NULL){
                prevNode->next = nextNode;
            }
            if (nextNode != NULL){
                nextNode->previous = prevNode;
            }
            list->length -=1;
            free(currentNode);
            return returnData;
        }
    }
    return NULL;
}


/**Returns a pointer to the data at the front of the list. Does not alter list structure.
 *@pre The list exists and has memory allocated to it
 *@param list - a pointer to the List struct
 *@return pointer to the data located at the head of the list
 **/
void* getFromFront(List* list){
    if (list == NULL || list->head == NULL || list->head->data == NULL){
        return NULL;
    }
    return list->head->data;
}



/**Returns a pointer to the data at the back of the list. Does not alter list structure.
 *@pre The list exists and has memory allocated to it
 *@param list - a pointer to the List struct
 *@return pointer to the data located at the tail of the list
 **/
void* getFromBack(List* list){
    if (list == NULL || list->tail == NULL || list->tail->data == NULL){
        return NULL;
    }
    return list->tail->data;
}

/**Returns a string that contains a string representation of
the list traversed from  head to tail. Utilize the list's printData function pointer to create the string.
returned string must be freed by the calling function.
 *@pre List must exist, but does not have to have elements.
 *@param list - a pointer to the List struct
 *@return on success: char * to string representation of list (must be freed after use).  on failure: NULL
 **/
char* toString(List* list){
    if(list == NULL){
        return NULL;
    }
    ListIterator iterator = createIterator(list);
    char * out = NULL;
    while (iterator.current != NULL){
        Node * temp = nextElement(&iterator);
        char * tempString = list->printData(temp->data);
        if (out == NULL){
            out = malloc(sizeof(char) * strlen(tempString) + 1);
            strcpy(out, tempString);
        }
        else{
            out = realloc(out, (strlen(out) + strlen(tempString)) * sizeof(char) + 1);
            strcat(out, tempString);
        }
    }
    return out;
}

/** Function for creating an iterator for the linked list.  
 * Newly created iterator points to the head of the list.
 *@pre List exists and is valid
 *@post List remains unchanged.  The iterator has been allocated and points to the head of the list.
 *@return The newly created iterator object.
 *@param list - pointer to the List struct to iterate over.
**/
ListIterator createIterator(List* list){
    if (list != NULL && list->head != NULL){
        ListIterator temp;
        temp.current = list->head;
        return temp;
    }
    ListIterator temp;
    temp.current = NULL;
    return temp;
}


/** Function that returns the next element of the list through the iterator. 
* This function returns the data at head of the list the first time it is called after
* the iterator was created. 
* Every subsequent call returns the data associated with the next element.
* Returns NULL once the end of the iterator is reached.
*@pre List exists and is valid.  Iterator exists and is valid.
*@post List remains unchanged.  The iterator points to the next element on the list.
*@return The data associated with the list element that the iterator pointed to when the function was called.
*@param iter - a pointer to an iterator for a List struct.
**/
void* nextElement(ListIterator* iter){
    if (iter == NULL || iter->current == NULL){
        return NULL;
    }
    void * temp = iter->current;
    iter->current = iter->current->next;
    return temp;
}


/**Returns the number of elements in the list.
 *@pre List must exist, but does not have to have elements.
 *@param list - a pointer to the List struct.
 *@return on success: number of eleemnts in the list (0 or more).  on failure: -1 (e.g. list not initlized correctly)
 **/
int getLength(List* list){
    if(list == NULL){
        return 0;
    }
    return list->length;
}

/** Function that searches for an element in the list using a comparator function.
 * If an element is found, a pointer to the data of that element is returned
 * Returns NULL if the element is not found.
 *@pre List exists and is valid.  Comparator function has been provided.
 *@post List remains unchanged.
 *@return The data associated with the list element that matches the search criteria.  If element is not found, return NULL.
 *@param list - a pointer to the List sruct
 *@param customCompare - a pointer to comparator function for customizing the search
 *@param searchRecord - a pointer to search data, which contains seach criteria
 *Note: while the arguments of compare() and searchRecord are all void, it is assumed that records they point to are
 *      all of the same type - just like arguments to the compare() function in the List struct
 **/
void* findElement(List * list, bool (*customCompare)(const void* first,const void* second), const void* searchRecord){
    ListIterator iterator = createIterator(list);
    while (iterator.current != NULL){
        Node * currentNode = nextElement(&iterator);
        if (customCompare(searchRecord, currentNode->data)){
            return currentNode->data;
        }
    }
    return NULL;
}