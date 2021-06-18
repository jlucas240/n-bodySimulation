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
    double acc;
    double velocity;

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
    struct list* hold = *l;
    while (hold->next != NULL){
        hold = hold->next;
        
    }
    // then add to end of list
    hold->next = newN;
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

    printf("mass: %lf, x: %lf, y: %lf, z: %lf,\n", 
    n->mass, newNode->x, newNode->y, newNode->z);

    printf("x : %lf - %lf, y : %lf - %lf, y : %lf - %lf, size : %lf\n", 
    t->xmin, t->xmax, t->ymin, t->ymax, t->zmin, t->zmax, t->size);


    
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
            hold2->velocity = n->velocity;
            hold2->acc = n->acc;
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
                                holdl->body->acc,
                                holdl->body->velocity,
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
                                holdl->body->acc,
                                holdl->body->velocity,
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
                                holdl->body->acc,
                                holdl->body->velocity,
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
                                holdl->body->acc,
                                holdl->body->velocity,
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
                                holdl->body->acc,
                                holdl->body->velocity,
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
                                holdl->body->acc,
                                holdl->body->velocity,
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
                                holdl->body->acc,
                                holdl->body->velocity,
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
                                holdl->body->acc,
                                holdl->body->velocity,
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
        //n->mass = gm; // fix this to just mass
         // add insert to tree
         insertOctaTree(&root, &n);
        
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

double theta;
double G;

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


void main(){
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
    t->xmin = 1;
    t->xmax = 30;
    t->ymin = 1;
    t->ymax = 30;
    t->zmin = 1;
    t->zmax = 30;
    t->size = 30*30*30;
    t->leaf= true;

    

    struct node n1 = {1,1,1,10};
    struct node n2 = {1,5,1,12};
    struct node n3 = {12,1,7,13};
    struct node n4 = {1,18,7,14};
    struct node n5 = {17,1,7,15};
    struct node n6 = {1,1,7,16};
    struct node n7 = {5,1,10,17};
    struct node n8 = {1,1,7,18};
    struct node n9 = {1,1,7,19};
    struct node n10 = {1,12,7,20};
    struct node n13 = {1,16,7,23};
    struct node n14 = {1,15,7,24};
    struct node n15 = {1,14,7,25};
    struct node n16 = {1,13,7,26};
    struct node n11 = {14,1,7,21};
    struct node n12 = {1,1,16,22};
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
    deleteOctaTree(&t);
    free(t);

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

