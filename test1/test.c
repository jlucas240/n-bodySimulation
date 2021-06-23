#include <stdio.h>
#include <stdlib.h>

#include <stdbool.h>
#include <math.h>

#include <sys/stat.h>
#include <string.h>

// valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./test

long minSize;
int i;
struct octaTreeNode* root;
struct list* vals;
double theta;
double G;

// linked list 
// used for bucket in octatree nodes

struct list{
    struct node* body;
    struct list* next;
};

struct node{
    double x;
    double y;
    double z;

    double mass;
    double accX;
    double accY;
    double accZ;
    double velocityX;
    double velocityY;
    double velocityZ;

    long int id;
};



void insertlist(struct list** l, struct node** n){
    // new node to insert
    struct list* newN = (struct list*)malloc(sizeof(struct list));
    newN->body = *n;
    newN->next = NULL;
        // when the list is not initualized yet 
    // intiulize it with the new node.
    if(*l == NULL){
        // add node to list
        *l = newN;
        return;
    }
    
    // use a place holder to itorate thorugh the list untill the next node is NULL
    /*struct list* hold = *l;
    while (hold->next != NULL){
        hold = hold->next;
        
    }
    // then add to end of list
    hold->next = newN;*/
    newN->next = *l;
    *l = newN;

    return;
    
};

void deleteList(struct list** l){
    // if list is equal to null free it
    // other wise itorate through list freeing each node
    
    struct list* hold = *l;
    struct list* holdN;
    printf(" %p,\n %p\n", hold, hold->body);

    do{ //while(hold != NULL && hold->body != NULL); { 
        
        holdN = hold->next;
        /*hold->body = NULL;
        hold = NULL;*/
        free(hold->body);
        free(hold);
        hold = holdN;
       
        
    } while(hold != NULL);
    hold = NULL;
    *l = NULL;
    //printf("done!!!\n");

    
    return;
};

//*******************************************************************************

struct octaTreeNode {
    double xmin;
    double xmax;
    double ymin;
    double ymax;
    double zmin;
    double zmax;

    double size;

    bool leaf;

    struct octaTreeNode* c0;
    struct octaTreeNode* c1;
    struct octaTreeNode* c2;
    struct octaTreeNode* c3;
    struct octaTreeNode* c4;
    struct octaTreeNode* c5;
    struct octaTreeNode* c6;
    struct octaTreeNode* c7;
    
    struct list* nodes;

    double cmx;
    double cmxHold;
    double cmy;
    double cmyHold;
    double cmz;
    double cmzHold;
    double mass;
};


void insertOctaTree(struct octaTreeNode** nt, struct node* n){

    // variable to hold tree
    struct octaTreeNode* t = *nt;
    // variable to hold new insertion
    struct node* newNode = n;


    
    // there are no nodes in this sector/zone
    if(t->leaf){

        //change laf to negative
        t->leaf = false;
        
        // check if a node is initiated
        // create new body node then
        // if not initiate a list
        if(t->nodes == NULL){
            t->nodes = (struct list*)malloc(sizeof(struct list));
            struct node* hold2 = (struct node*)malloc(sizeof(struct node));
            hold2->mass = n->mass;
            hold2->x = n->x;
            hold2->y = n->y;
            hold2->z = n->z;
            t->nodes->body = hold2;
            t->nodes->next = NULL;
            printf("address oflist: %p\n", t->nodes);
            return;
        }
    }

    if(t->nodes != NULL){
        if(t->nodes->body->x == n->x && 
            t->nodes->body->y == n->y && 
            t->nodes->body->z == n->z){
            struct node* hold2 = (struct node*)malloc(sizeof(struct node));
            hold2->mass = n->mass;
            hold2->x = n->x;
            hold2->y = n->y;
            hold2->z = n->z;
            hold2->velocityX = n->velocityX;
            hold2->velocityY = n->velocityY;
            hold2->velocityZ = n->velocityZ;
            hold2->accX = n->accX;
            hold2->accY = n->accY;
            hold2->accZ = n->accZ;
            hold2->id = n->id;
            insertlist(&t->nodes, &hold2);
            return ;
        }
    }

    // determin which zone the node will go in
    // determin the coerecr zone for x first
    // if x is in first half
    if (newNode->x <= ((t->xmax+t->xmin)/2) ){
        //determin which y catigore the body goes in
        // if y is in forst half
        if (newNode->y <= ((t->ymax+t->ymin)/2)){
            // if z is in first half
            if (newNode->z <= ((t->zmax+t->zmin)/2) ){
                //mass caculations
                //total mass
                t->mass += n->mass;
                // center of mass calculations for x
                t->cmxHold += (n->x*n->mass);
                t->cmx = t->cmxHold/t->mass;
                // center of mass calculations for y
                t->cmyHold += (n->y*n->mass);
                t->cmy = t->cmyHold/t->mass;
                // center of mass calculations for z
                t->cmzHold += (n->z*n->mass);
                t->cmz = t->cmzHold/t->mass;
                
            
                // if the next node in tree exists call insert function again (recursive)
                if (t->c0 != NULL){
                    insertOctaTree(&t->c0, n);
                    return ;
                }
                // if no node exsts create it 
                // the x,y,z 
                // min and max will be set based on the values that got node to this point
                else{
                    struct octaTreeNode* hold = (struct octaTreeNode*)malloc(sizeof(struct octaTreeNode));
                    hold->xmin = t->xmin;
                    hold->xmax = (((t->xmax+t->xmin))/2);
                    hold->ymin = t->ymin;
                    hold->ymax = (((t->ymax+t->ymin))/2) ;
                    hold->zmin = t->zmin;
                    hold->zmax = ((t->zmax+t->zmin)/2) ;
                    hold->size = (t->xmax-t->xmin+1)/2 *
                        (t->ymax-t->ymin+1)/2 *
                        (t->zmax-t->zmin+1)/2;
                    hold->c0 = NULL;
                    hold->c1 = NULL;
                    hold->c2 = NULL;
                    hold->c3 = NULL;
                    hold->c4 = NULL;
                    hold->c5 = NULL;
                    hold->c6 = NULL;
                    hold->c7 = NULL;
                    hold->nodes = NULL;
                    hold->leaf = true;
                    
                    t->c0 = hold;
                    //printf("%f\n",newNode->mass);
                    insertOctaTree(&t->c0, n);///********************
                    // check if there is a node in the list 
                    if(t->nodes != NULL){
                        struct list* holdl = t->nodes; 
                        struct list* holdn;
                        //itorate trough list tom make sure it is empty
                        do{
                            // make place holder node == to the node
                            struct node temp = {t->nodes->body->x,
                                holdl->body->y,
                                holdl->body->z,
                                holdl->body->mass,
                                holdl->body->accX,
                                holdl->body->accY,
                                holdl->body->accZ,
                                holdl->body->velocityX,
                                holdl->body->velocityY,
                                holdl->body->velocityZ,
                                holdl->body->id};
                            holdn = holdl->next;
                            // set the tree list of nodes to null
                            free(holdl->body);
                            free(holdl);

                            holdl = holdn;
                            // insert tplace holder into tree
                            t->nodes = NULL;
                            insertOctaTree(&t, &temp);
                        }while(holdl != NULL);
                        //free(t->nodes);
                        t->nodes = NULL;
                    }
                    return;
                }
            }
            //if z is in second half
            else{ 
                //mass caculations
                //total mass
                t->mass += n->mass;
                // center of mass calculations for x
                t->cmxHold += (n->x*n->mass);
                t->cmx = t->cmxHold/t->mass;
                // center of mass calculations for y
                t->cmyHold += (n->y*n->mass);
                t->cmy = t->cmyHold/t->mass;
                // center of mass calculations for z
                t->cmzHold += (n->z*n->mass);
                t->cmz = t->cmzHold/t->mass;
                // if the next node in tree exists call insert function again (recursive)
                if (t->c1 != NULL){
                    insertOctaTree(&t->c1, n);
                    return ;
                }
                // if no node exsts create it 
                // the x,y,z 
                // min and max will be set based on the values that got node to this point
                else{
                    struct octaTreeNode* hold = (struct octaTreeNode*)malloc(sizeof(struct octaTreeNode));
                    hold->xmin = t->xmin;
                    hold->xmax = ((t->xmax+t->xmin)/2);
                    hold->ymin = t->ymin;
                    hold->ymax = ((t->ymax+t->ymin)/2);
                    hold->zmin = ((t->zmax+t->zmin)/2);
                    hold->zmax = t->zmax;
                    hold->size = (t->xmax-t->xmin+1)/2 *
                        (t->ymax-t->ymin+1)/2 *
                        (t->zmax-t->zmin+1)/2;
                    hold->c0 = NULL;
                    hold->c1 = NULL;
                    hold->c2 = NULL;
                    hold->c3 = NULL;
                    hold->c4 = NULL;
                    hold->c5 = NULL;
                    hold->c6 = NULL;
                    hold->c7 = NULL;
                    hold->nodes = NULL;
                    hold->leaf = true;
                    t->c1 = hold;
                    insertOctaTree(&t->c1, n);
                    // check if there is a node in the list 
                    if(t->nodes != NULL){
                        struct list* holdl = t->nodes; 
                        struct list* holdn;
                        //itorate trough list tom make sure it is empty
                        do{
                            // make place holder node == to the node
                            struct node temp = {t->nodes->body->x,
                                holdl->body->y,
                                holdl->body->z,
                                holdl->body->mass,
                                holdl->body->accX,
                                holdl->body->accY,
                                holdl->body->accZ,
                                holdl->body->velocityX,
                                holdl->body->velocityY,
                                holdl->body->velocityZ,
                                holdl->body->id};
                            holdn = holdl->next;
                            // set the tree list of nodes to null
                            free(holdl->body);
                            free(holdl);

                            holdl = holdn;
                            // insert tplace holder into tree
                            t->nodes = NULL;
                            insertOctaTree(&t, &temp);
                        }while(holdl != NULL);
                        //free(t->nodes);
                    }
                    return ;
                }
            }
        }
        // if y is in second half
        else{
            // if z is in first half
            if (newNode->z <= (t->zmax+t->zmin)/2){
                //mass caculations
                //total mass
                t->mass += n->mass;
                // center of mass calculations for x
                t->cmxHold += (n->x*n->mass);
                t->cmx = t->cmxHold/t->mass;
                // center of mass calculations for y
                t->cmyHold += (n->y*n->mass);
                t->cmy = t->cmyHold/t->mass;
                // center of mass calculations for z
                t->cmzHold += (n->z*n->mass);
                t->cmz = t->cmzHold/t->mass;
                // if the next node in tree exists call insert function again (recursive)
                if (t->c2 != NULL){
                    insertOctaTree(&t->c2, n);
                    return ;
                }
                // if no node exsts create it 
                // the x,y,z 
                // min and max will be set based on the values that got node to this point
                else{   
                    struct octaTreeNode* hold = (struct octaTreeNode*)malloc(sizeof(struct octaTreeNode));
                    hold->xmin = t->xmin;
                    hold->xmax =(t->xmax+t->xmin)/2;
                    hold->ymin =(t->ymax+t->ymin)/2;
                    hold->ymax = t->ymax;
                    hold->zmin = t->zmin;
                    hold->zmax =(t->zmax+t->zmin)/2;
                    hold->size = (t->xmax-t->xmin+1)/2 *
                        (t->ymax-t->ymin+1)/2 *
                        (t->zmax-t->zmin+1)/2;
                    hold->c0 = NULL;
                    hold->c1 = NULL;
                    hold->c2 = NULL;
                    hold->c3 = NULL;
                    hold->c4 = NULL;
                    hold->c5 = NULL;
                    hold->c6 = NULL;
                    hold->c7 = NULL;
                    hold->nodes = NULL;
                    hold->leaf = true;
                    t->c2 = hold;
                    insertOctaTree(&t->c2, n);
                    // check if there is a node in the list 
                    if(t->nodes != NULL){
                        struct list* holdl = t->nodes; 
                        struct list* holdn;
                        //itorate trough list tom make sure it is empty
                        do{
                            // make place holder node == to the node
                            struct node temp = {t->nodes->body->x,
                                holdl->body->y,
                                holdl->body->z,
                                holdl->body->mass,
                                holdl->body->accX,
                                holdl->body->accY,
                                holdl->body->accZ,
                                holdl->body->velocityX,
                                holdl->body->velocityY,
                                holdl->body->velocityZ,
                                holdl->body->id};
                            holdn = holdl->next;
                            // set the tree list of nodes to null
                            free(holdl->body);
                            free(holdl);

                            holdl = holdn;
                            // insert tplace holder into tree
                            t->nodes = NULL;
                            insertOctaTree(&t, &temp);
                        }while(holdl != NULL);
                        //free(t->nodes);
                    }
                    return ;
                }
            }
            //if z is in second half
            else{
                //mass caculations
                //total mass
                t->mass += n->mass;
                // center of mass calculations for x
                t->cmxHold += (n->x*n->mass);
                t->cmx = t->cmxHold/t->mass;
                // center of mass calculations for y
                t->cmyHold += (n->y*n->mass);
                t->cmy = t->cmyHold/t->mass;
                // center of mass calculations for z
                t->cmzHold += (n->z*n->mass);
                t->cmz = t->cmzHold/t->mass;
                // if the next node in tree exists call insert function again (recursive)
                if (t->c3 != NULL){
                    insertOctaTree(&t->c3, n);
                    return ;
                }
                // if no node exsts create it 
                // the x,y,z 
                // min and max will be set based on the values that got node to this point
                else{
                    struct octaTreeNode* hold = (struct octaTreeNode*)malloc(sizeof(struct octaTreeNode));
                    hold->xmin = t->xmin;
                    hold->xmax = (t->xmax+t->xmin)/2;
                    hold->ymin = (t->ymax+t->ymin)/2;
                    hold->ymax = t->ymax;
                    hold->zmin = (t->zmax+t->zmin)/2;
                    hold->zmax = t->zmax;
                    hold->size = (t->xmax-t->xmin+1)/2 *
                        (t->ymax-t->ymin+1)/2 *
                        (t->zmax-t->zmin+1)/2;
                    hold->c0 = NULL;
                    hold->c1 = NULL;
                    hold->c2 = NULL;
                    hold->c3 = NULL;
                    hold->c4 = NULL;
                    hold->c5 = NULL;
                    hold->c6 = NULL;
                    hold->c7 = NULL;
                    hold->nodes = NULL;
                    hold->leaf = true;
                    t->c3 = hold;
                    insertOctaTree(&t->c3, n);
                    // check if there is a node in the list 
                    if(t->nodes != NULL){
                        struct list* holdl = t->nodes; 
                        struct list* holdn;
                        //itorate trough list tom make sure it is empty
                        do{
                            // make place holder node == to the node
                            struct node temp = {t->nodes->body->x,
                                holdl->body->y,
                                holdl->body->z,
                                holdl->body->mass,
                                holdl->body->accX,
                                holdl->body->accY,
                                holdl->body->accZ,
                                holdl->body->velocityX,
                                holdl->body->velocityY,
                                holdl->body->velocityZ,
                                holdl->body->id};
                            holdn = holdl->next;
                            // set the tree list of nodes to null
                            free(holdl->body);
                            free(holdl);

                            holdl = holdn;
                            // insert tplace holder into tree
                            t->nodes = NULL;
                            insertOctaTree(&t, &temp);
                        }while(holdl != NULL);
                        //free(t->nodes);
                    }
                    return ;
                }
            }
        }

    }
    // if x is in second half
    else{
        //determin which y catigore the body goes in
        // if y is in forst half
        if (newNode->y <= (t->ymax+t->ymin)/2){
            // if z is in first half
            if (newNode->z <= (t->zmax+t->zmin)/2){
                //mass caculations
                //total mass
                t->mass += n->mass;
                // center of mass calculations for x
                t->cmxHold += (n->x*n->mass);
                t->cmx = t->cmxHold/t->mass;
                // center of mass calculations for y
                t->cmyHold += (n->y*n->mass);
                t->cmy = t->cmyHold/t->mass;
                // center of mass calculations for z
                t->cmzHold += (n->z*n->mass);
                t->cmz = t->cmzHold/t->mass;
                // if the next node in tree exists call insert function again (recursive)
                if (t->c4 != NULL){
                    insertOctaTree(&t->c4, n);
                    return ;
                }
                // if no node exsts create it 
                // the x,y,z 
                // min and max will be set based on the values that got node to this point
                else{
                    struct octaTreeNode* hold = (struct octaTreeNode*)malloc(sizeof(struct octaTreeNode));
                    hold->xmin = (t->xmax+t->xmin)/2;
                    hold->xmax = t->xmax;
                    hold->ymin = t->ymin;
                    hold->ymax = (t->ymax+t->ymin)/2;
                    hold->zmin = t->zmin;
                    hold->zmax = (t->zmax+t->zmin)/2;
                    hold->size = (t->xmax-t->xmin+1)/2 *
                        (t->ymax-t->ymin+1)/2 *
                        (t->zmax-t->zmin+1)/2;
                    hold->c0 = NULL;
                    hold->c1 = NULL;
                    hold->c2 = NULL;
                    hold->c3 = NULL;
                    hold->c4 = NULL;
                    hold->c5 = NULL;
                    hold->c6 = NULL;
                    hold->c7 = NULL;
                    hold->nodes = NULL;
                    hold->leaf = true;
                    t->c4 = hold;
                    insertOctaTree(&t->c4, n);
                    // check if there is a node in the list 
                    if(t->nodes != NULL){
                        struct list* holdl = t->nodes; 
                        struct list* holdn;
                        //itorate trough list tom make sure it is empty
                        do{
                            // make place holder node == to the node
                            struct node temp = {t->nodes->body->x,
                                holdl->body->y,
                                holdl->body->z,
                                holdl->body->mass,
                                holdl->body->accX,
                                holdl->body->accY,
                                holdl->body->accZ,
                                holdl->body->velocityX,
                                holdl->body->velocityY,
                                holdl->body->velocityZ,
                                holdl->body->id};
                            holdn = holdl->next;
                            // set the tree list of nodes to null
                            free(holdl->body);
                            free(holdl);

                            holdl = holdn;
                            // insert tplace holder into tree
                            t->nodes = NULL;
                            insertOctaTree(&t, &temp);
                        }while(holdl != NULL);
                        //free(t->nodes);
                        t->nodes = NULL;
                    }
                    return ;
                }
            }
            //if z is in second half
            else{
                //mass caculations
                //total mass
                t->mass += n->mass;
                // center of mass calculations for x
                t->cmxHold += (n->x*n->mass);
                t->cmx = t->cmxHold/t->mass;
                // center of mass calculations for y
                t->cmyHold += (n->y*n->mass);
                t->cmy = t->cmyHold/t->mass;
                // center of mass calculations for z
                t->cmzHold += (n->z*n->mass);
                t->cmz = t->cmzHold/t->mass;
                // if the next node in tree exists call insert function again (recursive)
                if (t->c5 != NULL){
                    insertOctaTree(&t->c5, n);
                    return ;
                }
                // if no node exsts create it 
                // the x,y,z 
                // min and max will be set based on the values that got node to this point
                else{
                    struct octaTreeNode* hold = (struct octaTreeNode*)malloc(sizeof(struct octaTreeNode));
                    hold->xmin = (t->xmax+t->xmin)/2;
                    hold->xmax = t->xmax;
                    hold->ymin = t->ymin;
                    hold->ymax = (t->ymax+t->ymin)/2;
                    hold->zmin = (t->zmax+t->zmin)/2;
                    hold->zmax = t->zmax;
                    hold->size = (t->xmax-t->xmin+1)/2 *
                        (t->ymax-t->ymin+1)/2 *
                        (t->zmax-t->zmin+1)/2;
                    hold->c0 = NULL;
                    hold->c1 = NULL;
                    hold->c2 = NULL;
                    hold->c3 = NULL;
                    hold->c4 = NULL;
                    hold->c5 = NULL;
                    hold->c6 = NULL;
                    hold->c7 = NULL;
                    hold->nodes = NULL;
                    hold->leaf = true;
                    t->c5 = hold;
                    insertOctaTree(&t->c5, n);
                    // check if there is a node in the list 
                    if(t->nodes != NULL){
                        struct list* holdl = t->nodes; 
                        struct list* holdn;
                        //itorate trough list tom make sure it is empty
                        do{
                            // make place holder node == to the node
                            struct node temp = {t->nodes->body->x,
                                holdl->body->y,
                                holdl->body->z,
                                holdl->body->mass,
                                holdl->body->accX,
                                holdl->body->accY,
                                holdl->body->accZ,
                                holdl->body->velocityX,
                                holdl->body->velocityY,
                                holdl->body->velocityZ,
                                holdl->body->id};
                            holdn = holdl->next;
                            // set the tree list of nodes to null
                            free(holdl->body);
                            free(holdl);

                            holdl = holdn;
                            // insert tplace holder into tree
                            t->nodes = NULL;
                            insertOctaTree(&t, &temp);
                        }while(holdl != NULL);
                        //free(t->nodes);
                        t->nodes = NULL;
                    }
                    return ;
                }
            }
        }
        // if y is in second half
        else{
            // if z is in first half
            if (newNode->z <= (t->zmax+t->zmin)/2 ){
                //mass caculations
                //total mass
                t->mass += n->mass;
                // center of mass calculations for x
                t->cmxHold += (n->x*n->mass);
                t->cmx = t->cmxHold/t->mass;
                // center of mass calculations for y
                t->cmyHold += (n->y*n->mass);
                t->cmy = t->cmyHold/t->mass;
                // center of mass calculations for z
                t->cmzHold += (n->z*n->mass);
                t->cmz = t->cmzHold/t->mass;
                // if the next node in tree exists call insert function again (recursive)
                if (t->c6 != NULL){
                    insertOctaTree(&t->c6, n);
                    return ;
                }
                // if no node exsts create it 
                // the x,y,z 
                // min and max will be set based on the values that got node to this point
                else{     
                    struct octaTreeNode* hold = (struct octaTreeNode*)malloc(sizeof(struct octaTreeNode));
                    hold->xmin = (t->xmax+t->xmin)/2;
                    hold->xmax = t->xmax;
                    hold->ymin = (t->ymax+t->ymin)/2;
                    hold->ymax = t->ymax;
                    hold->zmin = t->zmin;
                    hold->zmax = (t->zmax+t->zmin)/2;
                    hold->size = (t->xmax-t->xmin+1)/2 *
                        (t->ymax-t->ymin+1)/2 *
                        (t->zmax-t->zmin+1)/2;
                    hold->c0 = NULL;
                    hold->c1 = NULL;
                    hold->c2 = NULL;
                    hold->c3 = NULL;
                    hold->c4 = NULL;
                    hold->c5 = NULL;
                    hold->c6 = NULL;
                    hold->c7 = NULL;
                    hold->nodes = NULL;
                    hold->leaf = true;
                    t->c6 = hold;
                    insertOctaTree(&t->c6, n);
                    // check if there is a node in the list 
                    if(t->nodes != NULL){
                        struct list* holdl = t->nodes; 
                        struct list* holdn;
                        //itorate trough list tom make sure it is empty
                        do{
                            // make place holder node == to the node
                            struct node temp = {t->nodes->body->x,
                                holdl->body->y,
                                holdl->body->z,
                                holdl->body->mass,
                                holdl->body->accX,
                                holdl->body->accY,
                                holdl->body->accZ,
                                holdl->body->velocityX,
                                holdl->body->velocityY,
                                holdl->body->velocityZ,
                                holdl->body->id};
                            holdn = holdl->next;
                            // set the tree list of nodes to null
                            free(holdl->body);
                            free(holdl);

                            holdl = holdn;
                            // insert tplace holder into tree
                            t->nodes = NULL;
                            insertOctaTree(&t, &temp);
                        }while(holdl != NULL);
                        //free(t->nodes);
                        t->nodes = NULL;
                    }
                    return ;
                }
            }
            //if z is in second half
            else{
                //mass caculations
                //total mass
                t->mass += n->mass;
                // center of mass calculations for x
                t->cmxHold += (n->x*n->mass);
                t->cmx = t->cmxHold/t->mass;
                // center of mass calculations for y
                t->cmyHold += (n->y*n->mass);
                t->cmy = t->cmyHold/t->mass;
                // center of mass calculations for z
                t->cmzHold += (n->z*n->mass);
                t->cmz = t->cmzHold/t->mass;
                // if the next node in tree exists call insert function again (recursive)
                if (t->c7 != NULL){
                    insertOctaTree(&t->c7, n);
                    return ;
                }
                // if no node exsts create it 
                // the x,y,z 
                // min and max will be set based on the values that got node to this point
                else{
                    struct octaTreeNode* hold = (struct octaTreeNode*)malloc(sizeof(struct octaTreeNode));
                    hold->xmin = (t->xmax+t->xmin)/2;
                    hold->xmax = t->xmax;
                    hold->ymin = (t->ymax+t->ymin)/2;
                    hold->ymax = t->ymax;
                    hold->zmin = (t->zmax+t->zmin)/2;
                    hold->zmax = t->zmax;
                    hold->size = (t->xmax-t->xmin+1)/2 *
                        (t->ymax-t->ymin+1)/2 *
                        (t->zmax-t->zmin+1)/2;
                    hold->c0 = NULL;
                    hold->c1 = NULL;
                    hold->c2 = NULL;
                    hold->c3 = NULL;
                    hold->c4 = NULL;
                    hold->c5 = NULL;
                    hold->c6 = NULL;
                    hold->c7 = NULL;
                    hold->nodes = NULL;
                    hold->leaf = true;
                    t->c7 = hold;
                    insertOctaTree(&t->c7, n);
                    // check if there is a node in the list 
                    if(t->nodes != NULL){
                        struct list* holdl = t->nodes; 
                        struct list* holdn;
                        //itorate trough list tom make sure it is empty
                        do{
                            // make place holder node == to the node
                            struct node temp = {t->nodes->body->x,
                                holdl->body->y,
                                holdl->body->z,
                                holdl->body->mass,
                                holdl->body->accX,
                                holdl->body->accY,
                                holdl->body->accZ,
                                holdl->body->velocityX,
                                holdl->body->velocityY,
                                holdl->body->velocityZ,
                                holdl->body->id};
                            holdn = holdl->next;
                            // set the tree list of nodes to null
                            free(holdl->body);
                            free(holdl);

                            holdl = holdn;
                            // insert tplace holder into tree
                            t->nodes = NULL;
                            insertOctaTree(&t, &temp);
                        }while(holdl != NULL);
                        //free(t->nodes);
                        t->nodes = NULL;
                    }
                    return ;
                }
            }
        }
    }

};


void deleteOctaTree(struct octaTreeNode** t){
    // if list is equal to null free it
    // other wise itorate through list freeing each node
    
    struct octaTreeNode* hold = *t;

    // if any of the children nodes are not NULL call delete octatree function
    if(hold->c0 != NULL){deleteOctaTree(&hold->c0);free(hold->c0);hold->c0 =NULL;}
    if(hold->c1 != NULL){deleteOctaTree(&hold->c1);free(hold->c1);hold->c1 =NULL;}
    if(hold->c2 != NULL){deleteOctaTree(&hold->c2);free(hold->c2);hold->c2 =NULL;}
    if(hold->c3 != NULL){deleteOctaTree(&hold->c3);free(hold->c3);hold->c3 =NULL;}
    if(hold->c4 != NULL){deleteOctaTree(&hold->c4);free(hold->c4);hold->c4 =NULL;}
    if(hold->c5 != NULL){deleteOctaTree(&hold->c5);free(hold->c5);hold->c5 =NULL;}
    if(hold->c6 != NULL){deleteOctaTree(&hold->c6);free(hold->c6);hold->c6 =NULL;}
    if(hold->c7 != NULL){deleteOctaTree(&hold->c7);free(hold->c7);hold->c7 =NULL;}
    // next check if ocatree.nodes is null, if not null call delete list
    if(hold->nodes != NULL){printf("mass: %lf\n", hold->nodes->body->mass); deleteList(&hold->nodes);free(hold->nodes);}
    // next free current node
    free(hold);
    hold = NULL;
    *t = NULL;
    //printf("hold: %p, hold.nodes: %p\n",hold, hold->nodes);
    //printf("hold: %lf, \n",hold->xmax);
    printf("done!!!\n");
    return;
};


void readIn(){
    double gm;
    double q;
    double i;
    double w;
    double l;

    double x;
    double y; 
    double z;

    long int id = 0;

    FILE *in_file = fopen("/home/jim/Desktop/test1/20body.txt", "r");

    struct stat sb;
    stat("/home/jim/Desktop/test1/20body.txt", &sb);

    char *file_contents = malloc(sb.st_size);

    while (fscanf(in_file, "%[^\n] ", file_contents) != EOF) {
        int j = 0;
        char * token = strtok(file_contents, ",");
        char *eptr;
        while( token != NULL ) {
            switch (j){
                case 0 :
                    gm = strtod(token, &eptr);
                    break;
                case 1 :
                    q = strtod(token, &eptr);
                    break;
                case 2 :
                    i= strtod(token, &eptr);
                    break;
                case 3 :
                    l= strtod(token, &eptr);
                    break;
                case 4 :
                    w= strtod(token, &eptr);
                    break;
            }
            token = strtok(NULL, ",");
            ++j;
        }
        printf("%lf, %lf, %lf, %lf, %lf\n", gm, q, i ,l, w);
        x = q*sin(w * (M_PI / 180)); // fix functions not found
        y = x/tan(l * (M_PI / 180));
        z = q*sin(i * (M_PI / 180));

        struct node n ={gm, x, y, z, 0, 0, id};
        struct node* n1 = (struct node*)malloc(sizeof(struct node));
        n1->mass = gm;
        n1->x = x;
        n1->y = y;
        n1->z = z;
        n1->id = id;
        n1->accX = 0;
        n1->accY = 0;
        n1->accZ = 0;
        n1->velocityX = 0;
        n1->velocityY = 0;
        n1->velocityZ = 0;
        //n->mass = gm; // fix this to just mass
         // add insert to tree
        insertOctaTree(&root, &n);

        insertlist(&vals, &n1);
        
        ++id;
    }

    fclose(in_file);
    exit(EXIT_SUCCESS);
    
}

struct calcVal{
    double accX;
    double accY;
    double accZ;
};



// calaulates acceleration

/*void traversAndCalc(struct octaTreeNode* node, struct node* val, struct calcVal * hold){
    
    double X = (node->xmax-node->xmin+1) /abs(val->x-node->cmx);
    double Y = (node->ymax-node->ymin+1) /abs(val->y-node->cmy);
    double Z = (node->zmax-node->zmin+1) /abs(val->z-node->cmz);
    
    if( X < theta && Y < theta && Z < theta){
        double holdaccx = G * node->mass / pow(abs(val->x-node->cmx), 3);
        hold->accX +=  (node->cmx-val->x)* holdaccx;

        double holdaccy = G * node->mass / pow(abs(val->y-node->cmy), 3);
        hold->accY +=  (node->cmy-val->y)* holdaccy;

        double holdaccz = G * node->mass / pow(abs(val->z-node->cmz), 3);
        hold->accZ +=  (node->cmz-val->z)* holdaccz;

        return;
    }

    if(node->nodes != NULL){
        // itorrate trough list adding accelration, unless node.id = val.id 
    }

    if(node->c0 != NULL){traversAndCalc(&node->c0, val, hold);}
    if(node->c1 != NULL){traversAndCalc(&node->c1, val, hold);}
    if(node->c2 != NULL){traversAndCalc(&node->c2, val, hold);}
    if(node->c3 != NULL){traversAndCalc(&node->c3, val, hold);}
    if(node->c4 != NULL){traversAndCalc(&node->c4, val, hold);}
    if(node->c5 != NULL){traversAndCalc(&node->c5, val, hold);}
    if(node->c6 != NULL){traversAndCalc(&node->c6, val, hold);}
    if(node->c7 != NULL){traversAndCalc(&node->c7, val, hold);}

    return;

    
}*/

/******************************calculations**************************/



void calculation(struct node* n, struct calcVal** c, struct octaTreeNode** t){
    // itorate through tree intill distence qualifier is met
    // to test the use the range of each node to varify n is not being
    // added to the calculations
    
    struct calcVal* calc = *c;
    struct octaTreeNode* tree = *t;
    
    // when a leaf node add values to c
    if(tree->nodes != NULL){
        struct list* l = tree->nodes;
        double mass = 0;
        while(l!= NULL){
            if(l->body->id != n->id){mass += l->body->mass;}
            l = l->next;
        }

        double holdx;
        double holdy;
        double holdz;
        
        if(n->x != tree->nodes->body->x){
            holdx = (G *mass)/pow(fabs(n->x - tree->nodes->body->x), 3);
        }else{
            holdx = 0;
        }
        if(n->y != tree->nodes->body->y){
            holdy = (G *mass)/pow(fabs(n->y - tree->nodes->body->y), 3);
        }else{
            holdy = 0;
        }
        if(n->z != tree->nodes->body->z){
            holdz = (G *mass)/pow(fabs(n->z - tree->nodes->body->z), 3);
        }else{
            holdz = 0;
        }

        calc->accX += (n->x - tree->nodes->body->x) * holdx;
        calc->accY += (n->y - tree->nodes->body->y) * holdy;
        calc->accZ += (n->z - tree->nodes->body->z) * holdz;
        return;
    }
    //if child != NULL, when ti is in zone go to child
    // otherwise if size of node/distace from center of mass is smaller 
    // than theta  compute aproximet accelerations esle go to zone
    if(tree->c0 != NULL){
        if(tree->xmin < n->x && tree->xmax > n->x &&
         tree->ymin < n->y && tree->ymax > n->y && 
         tree->zmin < n->z && tree->zmax > n->z){
            calculation(n, &calc, &tree->c0);
        }
        else if(tree->c0->size/pow(pow(tree->c0->cmx-n->x,2)+pow(tree->c0->cmy-n->y,2)+pow(tree->c0->cmz-n->z,2), 1/2) < theta){
            double holdx;
            double holdy;
            double holdz;
            
            if(n->x != tree->nodes->body->x){
                holdx = (G *tree->c0->mass)/pow(fabs(n->x - tree->nodes->body->x), 3);
            }else{
                holdx = 0;
            }
            if(n->y != tree->nodes->body->y){
                holdy = (G *tree->c0->mass)/pow(fabs(n->y - tree->nodes->body->y), 3);
            }else{
                holdy = 0;
            }
            if(n->z != tree->nodes->body->z){
                holdz = (G *tree->c0->mass)/pow(fabs(n->z - tree->nodes->body->z), 3);
            }else{
                holdz = 0;
            }
            calc->accX += (n->x - tree->c0->nodes->body->x) * holdx;
            calc->accY += (n->y - tree->c0->nodes->body->y) * holdy;
            calc->accZ += (n->z - tree->c0->nodes->body->z) * holdz;
        }
        else{calculation(n, &calc, &tree->c0);}
    }
    if(tree->c1 != NULL){
        if(tree->xmin < n->x && tree->xmax > n->x &&
         tree->ymin < n->y && tree->ymax > n->y && 
         tree->zmin < n->z && tree->zmax > n->z){
            calculation(n, &calc, &tree->c1);
        }
        else if(tree->c1->size/pow(pow(tree->c1->cmx-n->x,2)+pow(tree->c1->cmy-n->y,2)+pow(tree->c1->cmz-n->z,2), 1/2) < theta){
            double holdx;
            double holdy;
            double holdz;
            
            if(n->x != tree->nodes->body->x){
                holdx = (G *tree->c1->mass)/pow(fabs(n->x - tree->nodes->body->x), 3);
            }else{
                holdx = 0;
            }
            if(n->y != tree->nodes->body->y){
                holdy = (G *tree->c1->mass)/pow(fabs(n->y - tree->nodes->body->y), 3);
            }else{
                holdy = 0;
            }
            if(n->z != tree->nodes->body->z){
                holdz = (G *tree->c1->mass)/pow(fabs(n->z - tree->nodes->body->z), 3);
            }else{
                holdz = 0;
            }
            calc->accX += (n->x - tree->c1->nodes->body->x) * holdx;
            calc->accY += (n->y - tree->c1->nodes->body->y) * holdy;
            calc->accZ += (n->z - tree->c1->nodes->body->z) * holdz;
        }
        else{calculation(n, &calc, &tree->c1);}
    }
    if(tree->c2 != NULL){
        if(tree->xmin < n->x && tree->xmax > n->x &&
         tree->ymin < n->y && tree->ymax > n->y && 
         tree->zmin < n->z && tree->zmax > n->z){
            calculation(n, &calc, &tree->c2);
        }
        else if(tree->c2->size/pow(pow(tree->c2->cmx-n->x,2)+pow(tree->c2->cmy-n->y,2)+pow(tree->c2->cmz-n->z,2), 1/2) < theta){
            double holdx;
            double holdy;
            double holdz;
            
            if(n->x != tree->nodes->body->x){
                holdx = (G *tree->c2->mass)/pow(fabs(n->x - tree->nodes->body->x), 3);
            }else{
                holdx = 0;
            }
            if(n->y != tree->nodes->body->y){
                holdy = (G *tree->c2->mass)/pow(fabs(n->y - tree->nodes->body->y), 3);
            }else{
                holdy = 0;
            }
            if(n->z != tree->nodes->body->z){
                holdz = (G *tree->c2->mass)/pow(fabs(n->z - tree->nodes->body->z), 3);
            }else{
                holdz = 0;
            }
            calc->accX += (n->x - tree->c2->nodes->body->x) * holdx;
            calc->accY += (n->y - tree->c2->nodes->body->y) * holdy;
            calc->accZ += (n->z - tree->c2->nodes->body->z) * holdz;
        }
        else{calculation(n, &calc, &tree->c2);}
    }
    if(tree->c3 != NULL){
        if(tree->xmin < n->x && tree->xmax > n->x &&
         tree->ymin < n->y && tree->ymax > n->y && 
         tree->zmin < n->z && tree->zmax > n->z){
            calculation(n, &calc, &tree->c3);
        }
        else if(tree->c3->size/pow(pow(tree->c3->cmx-n->x,2)+pow(tree->c3->cmy-n->y,2)+pow(tree->c3->cmz-n->z,2), 1/2) < theta){
            double holdx;
            double holdy;
            double holdz;
            
            if(n->x != tree->nodes->body->x){
                holdx = (G *tree->c3->mass)/pow(fabs(n->x - tree->nodes->body->x), 3);
            }else{
                holdx = 0;
            }
            if(n->y != tree->nodes->body->y){
                holdy = (G *tree->c3->mass)/pow(fabs(n->y - tree->nodes->body->y), 3);
            }else{
                holdy = 0;
            }
            if(n->z != tree->nodes->body->z){
                holdz = (G *tree->c3->mass)/pow(fabs(n->z - tree->nodes->body->z), 3);
            }else{
                holdz = 0;
            }
            calc->accX += (n->x - tree->c3->nodes->body->x) * holdx;
            calc->accY += (n->y - tree->c3->nodes->body->y) * holdy;
            calc->accZ += (n->z - tree->c3->nodes->body->z) * holdz;
        }
        else{calculation(n, &calc, &tree->c3);}
    }
    if(tree->c4 != NULL){
        if(tree->xmin < n->x && tree->xmax > n->x &&
         tree->ymin < n->y && tree->ymax > n->y && 
         tree->zmin < n->z && tree->zmax > n->z){
            calculation(n, &calc, &tree->c4);
        }
        else if(tree->c4->size/pow(pow(tree->c4->cmx-n->x,2)+pow(tree->c4->cmy-n->y,2)+pow(tree->c4->cmz-n->z,2), 1/2) < theta){
            double holdx;
            double holdy;
            double holdz;
            
            if(n->x != tree->nodes->body->x){
                holdx = (G *tree->c4->mass)/pow(fabs(n->x - tree->nodes->body->x), 3);
            }else{
                holdx = 0;
            }
            if(n->y != tree->nodes->body->y){
                holdy = (G *tree->c4->mass)/pow(fabs(n->y - tree->nodes->body->y), 3);
            }else{
                holdy = 0;
            }
            if(n->z != tree->nodes->body->z){
                holdz = (G *tree->c4->mass)/pow(fabs(n->z - tree->nodes->body->z), 3);
            }else{
                holdz = 0;
            }
            calc->accX += (n->x - tree->c4->nodes->body->x) * holdx;
            calc->accY += (n->y - tree->c4->nodes->body->y) * holdy;
            calc->accZ += (n->z - tree->c4->nodes->body->z) * holdz;
        }
        else{calculation(n, &calc, &tree->c4);}
    }
    if(tree->c5 != NULL){
        if(tree->xmin < n->x && tree->xmax > n->x &&
         tree->ymin < n->y && tree->ymax > n->y && 
         tree->zmin < n->z && tree->zmax > n->z){
            calculation(n, &calc, &tree->c5);
        }
        else if(tree->c5->size/pow(pow(tree->c5->cmx-n->x,2)+pow(tree->c5->cmy-n->y,2)+pow(tree->c5->cmz-n->z,2), 1/2) < theta){
            double holdx;
            double holdy;
            double holdz;
            
            if(n->x != tree->nodes->body->x){
                holdx = (G *tree->c5->mass)/pow(fabs(n->x - tree->nodes->body->x), 3);
            }else{
                holdx = 0;
            }
            if(n->y != tree->nodes->body->y){
                holdy = (G *tree->c5->mass)/pow(fabs(n->y - tree->nodes->body->y), 3);
            }else{
                holdy = 0;
            }
            if(n->z != tree->nodes->body->z){
                holdz = (G *tree->c5->mass)/pow(fabs(n->z - tree->nodes->body->z), 3);
            }else{
                holdz = 0;
            }
            calc->accX += (n->x - tree->c5->nodes->body->x) * holdx;
            calc->accY += (n->y - tree->c5->nodes->body->y) * holdy;
            calc->accZ += (n->z - tree->c5->nodes->body->z) * holdz;
        }
        else{calculation(n, &calc, &tree->c5);}
    }
    if(tree->c6 != NULL){
        if(tree->xmin < n->x && tree->xmax > n->x &&
         tree->ymin < n->y && tree->ymax > n->y && 
         tree->zmin < n->z && tree->zmax > n->z){
            calculation(n, &calc, &tree->c6);
        }
        else if(tree->c6->size/pow(pow(tree->c6->cmx-n->x,2)+pow(tree->c6->cmy-n->y,2)+pow(tree->c6->cmz-n->z,2), 1/2) < theta){
            double holdx;
            double holdy;
            double holdz;
            
            if(n->x != tree->nodes->body->x){
                holdx = (G *tree->c6->mass)/pow(fabs(n->x - tree->nodes->body->x), 3);
            }else{
                holdx = 0;
            }
            if(n->y != tree->nodes->body->y){
                holdy = (G *tree->c6->mass)/pow(fabs(n->y - tree->nodes->body->y), 3);
            }else{
                holdy = 0;
            }
            if(n->z != tree->nodes->body->z){
                holdz = (G *tree->c6->mass)/pow(fabs(n->z - tree->nodes->body->z), 3);
            }else{
                holdz = 0;
            }
            calc->accX += (n->x - tree->c6->nodes->body->x) * holdx;
            calc->accY += (n->y - tree->c6->nodes->body->y) * holdy;
            calc->accZ += (n->z - tree->c6->nodes->body->z) * holdz;
        }
        else{calculation(n, &calc, &tree->c6);}
    }
    if(tree->c7 != NULL){
        if(tree->xmin < n->x && tree->xmax > n->x &&
         tree->ymin < n->y && tree->ymax > n->y && 
         tree->zmin < n->z && tree->zmax > n->z){
            calculation(n, &calc, &tree->c7);
        }
        else if(tree->c7->size/pow(pow(tree->c7->cmx-n->x,2)+pow(tree->c7->cmy-n->y,2)+pow(tree->c7->cmz-n->z,2), 1/2) < theta){
            double holdx;
            double holdy;
            double holdz;
            
            if(n->x != tree->nodes->body->x){
                holdx = (G *tree->c7->mass)/pow(fabs(n->x - tree->nodes->body->x), 3);
            }else{
                holdx = 0;
            }
            if(n->y != tree->nodes->body->y){
                holdy = (G *tree->c7->mass)/pow(fabs(n->y - tree->nodes->body->y), 3);
            }else{
                holdy = 0;
            }
            if(n->z != tree->nodes->body->z){
                holdz = (G *tree->c7->mass)/pow(fabs(n->z - tree->nodes->body->z), 3);
            }else{
                holdz = 0;
            }
            calc->accX += (n->x - tree->c7->nodes->body->x) * holdx;
            calc->accY += (n->y - tree->c7->nodes->body->y) * holdy;
            calc->accZ += (n->z - tree->c7->nodes->body->z) * holdz;
        }
        else{calculation(n, &calc, &tree->c7);}
    }
    return;
    
}

void movment(){
    // create new tree
    struct octaTreeNode* tempt = (struct octaTreeNode*)malloc(sizeof(struct octaTreeNode));
    tempt->c0 = NULL;
    tempt->c1 = NULL;
    tempt->c2 = NULL;
    tempt->c3 = NULL;
    tempt->c4 = NULL;
    tempt->c5 = NULL;
    tempt->c6 = NULL;
    tempt->c7 = NULL;
    tempt->nodes = NULL;
    tempt->xmin = -100;
    tempt->xmax = 100;
    tempt->ymin = -100;
    tempt->ymax = 100;
    tempt->zmin = -100;
    tempt->zmax = 100;
    tempt->leaf= true;
    //create new list
    struct list* templ = (struct list*)malloc(sizeof(struct list));
    templ->body = NULL;
    templ->next = NULL;
    // itorate trought vals
    struct list* hold = vals;
    while(hold != NULL){
        // create node as value and pointer;
        struct node newN = {hold->body->x, 
            hold->body->y,
            hold->body->z,
            hold->body->mass,
            hold->body->accX,
            hold->body->accY,
            hold->body->accZ,
            hold->body->velocityX,
            hold->body->velocityY,
            hold->body->velocityZ,
            hold->body->id};
        
        struct node* newN1 = (struct node*)malloc(sizeof(struct node));
        newN1->x = hold->body->x;
        newN1->y = hold->body->y;
        newN1->z = hold->body->z;
        newN1->mass = hold->body->mass;
        newN1->accX = hold->body->accX;
        newN1->accY = hold->body->accY;
        newN1->accZ = hold->body->accZ;
        newN1->velocityX = hold->body->velocityX;
        newN1->velocityY= hold->body->velocityY;
        newN1->velocityZ = hold->body->velocityZ;
        newN1->id = hold->body->id;
        printf("x: %lf, y: %lf, z: %lf, id: %ld\n", newN.x, newN.y, newN.z, newN.id);
        // at each node calculate the acceleration on the node
        struct calcVal* cal = (struct calcVal*)malloc(sizeof(struct calcVal));
        cal->accX = hold->body->accX;
        cal->accY = hold->body->accY;
        cal->accZ = hold->body->accZ;
        // call calculation fucntion here
        calculation(hold->body, &cal, &root);
        
        newN1->accX = cal->accX;
        newN1->accY = cal->accY;
        newN1->accZ = cal->accZ;
        // the change in velocity 
        newN1->velocityX = hold->body->velocityX + cal->accX;
        newN1->velocityY = hold->body->velocityY + cal->accY;
        newN1->velocityZ = hold->body->velocityZ + cal->accZ;
        // the change in posintion over 1 time unit
        newN1->x = hold->body->x + newN1->velocityX + .5*cal->accX;
        newN1->y = hold->body->y + newN1->velocityY + .5*cal->accY;
        newN1->z = hold->body->z + newN1->velocityZ + .5*cal->accZ;
        // set node for tree
        newN.accX = newN1->accX;
        newN.accY = newN1->accY;
        newN.accZ = newN1->accZ;
        newN.velocityX = newN1->velocityX;
        newN.velocityY = newN1->velocityY;
        newN.velocityZ = newN1->velocityZ;
        newN.x = newN1->x;
        newN.y = newN1->y;
        newN.z = newN1->z;
        printf("x: %lf, y: %lf, z: %lf, id: %ld\n", newN.x, newN.y, newN.z, newN.id);
        // add to new tree
        insertOctaTree(&tempt, &newN);
        insertlist(&templ, &newN1);
        // clear used memory
        free(cal);

        

        hold = hold->next;
    }
    // delete root and vals 
    deleteOctaTree(&root);
    deleteList(&vals);
    // set root equal to new tree and vals = to new list
    root = tempt;
    vals = templ;
    return;
}

void main(){

    G = 1;
    theta = .5;

    //*************************************list tests********************

    /*struct list* l0 = NULL;

    struct node* n10 = (struct node*)malloc(sizeof(struct node));
    n10->mass = 10;

    struct node* n11 = (struct node*)malloc(sizeof(struct node));
    n11->mass = 11;

    struct node* n12 = (struct node*)malloc(sizeof(struct node));
    n12->mass = 12;

    struct node* n13 = (struct node*)malloc(sizeof(struct node));
    n13->mass = 13;

    insertlist(&l0, &n10);
    insertlist(&l0, &n11);
    insertlist(&l0, &n12);
    insertlist(&l0, &n13);

 
    

    deleteList(&l0);*/

    //*************************tree tests***************************************

    minSize = 1;

    struct octaTreeNode* t = (struct octaTreeNode*)malloc(sizeof(struct octaTreeNode));
    t->c0 = NULL;
    t->c1 = NULL;
    t->c2 = NULL;
    t->c3 = NULL;
    t->c4 = NULL;
    t->c5 = NULL;
    t->c6 = NULL;
    t->c7 = NULL;
    t->nodes = NULL;
    t->xmin = -100;
    t->xmax = 100;
    t->ymin = -100;
    t->ymax = 100;
    t->zmin = -100;
    t->zmax = 100;
    t->size = 100*100*100;
    t->leaf= true;

    

    struct node n1 = {1,1,1,10,0,0,0,0,0,0,0};
    struct node* n00 = (struct node*)malloc(sizeof(struct node));
    n00->x =1;
    n00->y =1;
    n00->z =1;
    n00->mass =10;
    n00->accX=0;
    n00->accY=0;
    n00->accZ=0;
    n00->velocityX=0;
    n00->velocityY=0;
    n00->velocityZ=0;
    n00->id=0;

    struct node n2 = {1,5,1,12,0,0,0,0,0,0,1};
    struct node* n01 = (struct node*)malloc(sizeof(struct node));
    n01->x =1;
    n01->y =5;
    n01->z =1;
    n01->mass =12;
    n01->accX=0;
    n01->accY=0;
    n01->accZ=0;
    n01->velocityX=0;
    n01->velocityY=0;
    n01->velocityZ=0;
    n01->id=1;
    struct node n3 = {12,1,7,13,0,0,0,0,0,0,2};
    struct node* n02 = (struct node*)malloc(sizeof(struct node));
    n02->x =12;
    n02->y =1;
    n02->z =7;
    n02->mass =13;
    n02->accX=0;
    n02->accY=0;
    n02->accZ=0;
    n02->velocityX=0;
    n02->velocityY=0;
    n02->velocityZ=0;
    n02->id=2;
    struct node n4 = {1,18,7,14,0,0,0,0,0,0,3};
    struct node* n03 = (struct node*)malloc(sizeof(struct node));
    n03->x =1;
    n03->y =18;
    n03->z =7;
    n03->mass =14;
    n03->accX=0;
    n03->accY=0;
    n03->accZ=0;
    n03->velocityX=0;
    n03->velocityY=0;
    n03->velocityZ=0;
    n03->id=3;
    struct node n5 = {17,1,7,15,0,0,0,0,0,0,4};
    struct node* n04 = (struct node*)malloc(sizeof(struct node));
    n04->x =17;
    n04->y =1;
    n04->z =7;
    n04->mass =15;
    n04->accX=0;
    n04->accY=0;
    n04->accZ=0;
    n04->velocityX=0;
    n04->velocityY=0;
    n04->velocityZ=0;
    n04->id=4;
    struct node n6 = {1,1,7,16,0,0,0,0,0,0,5};
    struct node* n05 = (struct node*)malloc(sizeof(struct node));
    n05->x =1;
    n05->y =1;
    n05->z =7;
    n05->mass =16;
    n05->accX=0;
    n05->accY=0;
    n05->accZ=0;
    n05->velocityX=0;
    n05->velocityY=0;
    n05->velocityZ=0;
    n05->id=5;
    struct node n7 = {5,1,10,17,0,0,0,0,0,0,6};
    struct node* n06 = (struct node*)malloc(sizeof(struct node));
    n06->x =5;
    n06->y =1;
    n06->z =10;
    n06->mass =17;
    n06->accX=0;
    n06->accY=0;
    n06->accZ=0;
    n06->velocityX=0;
    n06->velocityY=0;
    n06->velocityZ=0;
    n06->id=6;
    struct node n8 = {1,1,7,18,0,0,0,0,0,0,7};
    struct node* n07 = (struct node*)malloc(sizeof(struct node));
    n07->x =1;
    n07->y =1;
    n07->z =7;
    n07->mass =18;
    n07->accX=0;
    n07->accY=0;
    n07->accZ=0;
    n07->velocityX=0;
    n07->velocityY=0;
    n07->velocityZ=0;
    n07->id=7;
    struct node n9 = {1,1,7,19,0,0,0,0,0,0,8};
    struct node* n08 = (struct node*)malloc(sizeof(struct node));
    n08->x =1;
    n08->y =1;
    n08->z =7;
    n08->mass =19;
    n08->accX=0;
    n08->accY=0;
    n08->accZ=0;
    n08->velocityX=0;
    n08->velocityY=0;
    n08->velocityZ=0;
    n08->id=8;
    struct node n10 = {1,12,7,20,0,0,0,0,0,0,9};
    struct node* n09 = (struct node*)malloc(sizeof(struct node));
    n09->x =1;
    n09->y =12;
    n09->z =7;
    n09->mass =20;
    n09->accX=0;
    n09->accY=0;
    n09->accZ=0;
    n09->velocityX=0;
    n09->velocityY=0;
    n09->velocityZ=0;
    n09->id=9;
    struct node n13 = {1,16,7,23,0,0,0,0,0,0,10};
    struct node* n010 = (struct node*)malloc(sizeof(struct node));
    n010->x =1;
    n010->y =16;
    n010->z =7;
    n010->mass =23;
    n010->accX=0;
    n010->accY=0;
    n010->accZ=0;
    n010->velocityX=0;
    n010->velocityY=0;
    n010->velocityZ=0;
    n010->id=10;
    struct node n14 = {1,15,7,24,0,0,0,0,0,0,11};
    struct node* n011 = (struct node*)malloc(sizeof(struct node));
    n011->x =1;
    n011->y =15;
    n011->z =7;
    n011->mass =24;
    n011->accX=0;
    n011->accY=0;
    n011->accZ=0;
    n011->velocityX=0;
    n011->velocityY=0;
    n011->velocityZ=0;
    n011->id=11;
    struct node n15 = {1,14,7,25,0,0,0,0,0,0,12};
    struct node* n012 = (struct node*)malloc(sizeof(struct node));
    n012->x =1;
    n012->y =14;
    n012->z =7;
    n012->mass =25;
    n012->accX=0;
    n012->accY=0;
    n012->accZ=0;
    n012->velocityX=0;
    n012->velocityY=0;
    n012->velocityZ=0;
    n012->id=12;
    struct node n16 = {1,13,7,26,0,0,0,0,0,0,13};
    struct node* n013 = (struct node*)malloc(sizeof(struct node));
    n013->x =1;
    n013->y =13;
    n013->z =7;
    n013->mass =26;
    n013->accX=0;
    n013->accY=0;
    n013->accZ=0;
    n013->velocityX=0;
    n013->velocityY=0;
    n013->velocityZ=0;
    n013->id=13;
    struct node n11 = {14,1,7,21,0,0,0,0,0,0,14};
    struct node* n014 = (struct node*)malloc(sizeof(struct node));
    n014->x =14;
    n014->y =1;
    n014->z =7;
    n014->mass =21;
    n014->accX=0;
    n014->accY=0;
    n014->accZ=0;
    n014->velocityX=0;
    n014->velocityY=0;
    n014->velocityZ=0;
    n014->id=14;
    struct node n12 = {1,1,16,22,0,0,0,0,0,0,15};
    struct node* n015 = (struct node*)malloc(sizeof(struct node));
    n015->x =1;
    n015->y =1;
    n015->z =16;
    n015->mass =22;
    n015->accX=0;
    n015->accY=0;
    n015->accZ=0;
    n015->velocityX=0;
    n015->velocityY=0;
    n015->velocityZ=0;
    n015->id=15;
    i = 0;
    insertOctaTree(&t, &n1);
    insertOctaTree(&t, &n2);
    i =3;
    insertOctaTree(&t, &n3);
    insertOctaTree(&t, &n4);
    insertOctaTree(&t, &n5);
    insertOctaTree(&t, &n6);
    insertOctaTree(&t, &n7);
    insertOctaTree(&t, &n8);
    insertOctaTree(&t, &n9);
    insertOctaTree(&t, &n10);
    insertOctaTree(&t, &n13);
    insertOctaTree(&t, &n14);
    insertOctaTree(&t, &n15);
    insertOctaTree(&t, &n16);
    insertOctaTree(&t, &n11);
    insertOctaTree(&t, &n12);
    root = t;
    insertlist(&vals, &n00);
    insertlist(&vals, &n01);
    insertlist(&vals, &n02);
    insertlist(&vals, &n03);
    insertlist(&vals, &n04);
    insertlist(&vals, &n05);
    insertlist(&vals, &n06);
    insertlist(&vals, &n07);
    insertlist(&vals, &n08);
    insertlist(&vals, &n09);
    insertlist(&vals, &n010);
    insertlist(&vals, &n011);
    insertlist(&vals, &n012);
    insertlist(&vals, &n013);
    insertlist(&vals, &n014);
    insertlist(&vals, &n015);

    movment();
    /*movment();
    movment();
    movment();
    movment();
    movment();*/

    deleteOctaTree(&root);
    deleteList(&vals);
    //free(t);

    //****************** read test*******************
    //root =(struct octaTreeNode*) malloc(sizeof(struct octaTreeNode));

    //readIn();
    /*double gm;
    double q;
    double i;
    double w;
    double l;

    double x;
    double y; 
    double z;
    FILE *in_file = fopen("/home/jim/Desktop/test1/20body.txt", "r");

    struct stat sb;
    stat("/home/jim/Desktop/test1/20body.txt", &sb);

    char *file_contents = malloc(sb.st_size);

    while (fscanf(in_file, "%[^\n] ", file_contents) != EOF) {
        int j = 0;
        char * token = strtok(file_contents, ",");
        char *eptr;
        while( token != NULL ) {
            switch (j){
                case 0 :
                    gm = strtod(token, &eptr);
                    break;
                case 1 :
                    q = strtod(token, &eptr);
                    break;
                case 2 :
                    i= strtod(token, &eptr);
                    break;
                case 3 :
                    l= strtod(token, &eptr);
                    break;
                case 4 :
                    w= strtod(token, &eptr);
                    break;
            }
            token = strtok(NULL, ",");
            ++j;
        }
        printf("%lf, %lf, %lf, %lf, %lf\n", gm, q, i ,l, w);
        x = q*sin(w);
        y = x/tan(l);
        z = q*sin(i);

        struct node* n = (struct node*)malloc(sizeof(struct node));
        n->mass = gm;
        n->x = x;
        n->y = y;
        n->z = z;
    }

    fclose(in_file);
    exit(EXIT_SUCCESS);*/

}

