// writen by Joshua Lucas
// data used for this simulation is the NASA Jet Propulsion Laboratory

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <sys/stat.h>
#include <string.h>




/*
liat works
*********************************NOTES:*****************************

1. add insert into tree to reed in 

2. add itorator for tree/ get function

3. find why one block is not getting deleted in the tree

4. start calculation programming

5. change insert fuction so that it does not make a new zone unless ther is 
overlap into the same zone. or if the size is to small add to node list and 
compute center of mass and center of mass locations

6. 

*/


//n-body simulation using the Barnes-hunt algorithm.
/* Description of project */


// helper strcuts and functions
// ____________________________________________________________________________

// linked list 
// used for bucket in octatree nodes

struct list{
    struct node* body;
    struct list* next;
};

// insert into linked list

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

// clear the list when moving to next round of simulation

void deleteList(struct list** l){
    // if list is equal to null free it
    // other wise itorate through list freeing each node
    
    struct list* hold = *l;
    struct list* holdN;

    do { 
        
        holdN = hold->next;
        /*hold->body = NULL;
        hold = NULL;*/
        free(hold->body);
        free(hold);
        hold = holdN;
        printf("powww\n");
        
    } while(hold != NULL);
    *l = NULL;
    printf("done!!!\n");

    
    return;
};


// Octa-tree
// ___________________________________________________________________________________________

// node struct 
// represents a body in the syestem being simulated
// contatins nodes x y and  x cordinates, and mass

struct node{
    long x;
    long y;
    long z;

    double mass;
};


// octatree node struct 
// contains the x, y and, z range for zone
// contains pointers to inbeded zones
// contains a bucket for nodes contained

struct octaTreeNode {
    long xmin;
    long xmax;
    long ymin;
    long ymax;
    long zmin;
    long zmax;

    long size;

    struct octaTreeNode* c0;
    struct octaTreeNode* c1;
    struct octaTreeNode* c2;
    struct octaTreeNode* c3;
    struct octaTreeNode* c4;
    struct octaTreeNode* c5;
    struct octaTreeNode* c6;
    struct octaTreeNode* c7;
    
    struct list* nodes;
};

/* when inserting into a octa tree the root will decied where the node will go by 
determining the which zone the node goes into creating a zone with the proper 
ranges then repeatong untill the area is the desred size. when at the desided size
the node is added to a bucket*/

// octa tree

void insertOctaTree(struct octaTreeNode** nt, struct node* n){

    // variable to hold tree
    struct octaTreeNode* t = *nt;
    // variable to hold new insertion
    struct node* newNode = n;

    printf("mass: %lf, x: %lf, y: %lf, z: %lf,\n", 
    n->mass, newNode->x, newNode->y, newNode->z);

    printf("x : %lf - %lf, y : %lf - %lf, y : %lf - %lf, size : %lf\n", 
    t->xmin, t->xmax, t->ymin, t->ymax, t->zmin, t->zmax, t->size);


    // if the size is too small and needs put in a bucket aka at leaf node
    if(t->size <= minSize){
        
        // check if a node is initiated
        // create new body node then
        // if not initiate a list
        if(t->nodes == NULL){
            struct list* holdl = (struct list*)malloc(sizeof(struct list));
            struct node* hold2 = (struct node*)malloc(sizeof(struct node));
            hold2->mass = n->mass;
            hold2->x = n->x;
            hold2->y = n->y;
            hold2->z = n->z;
            holdl->body = hold2;
            holdl->next = NULL;
            t->nodes = holdl; 
            return;
        }
        // else insert into list                
        else{
            
            struct node* hold2 = (struct node*)malloc(sizeof(struct node));
            hold2->mass = n->mass;
            hold2->x = n->x;
            hold2->y = n->y;
            hold2->z = n->z;
            insertlist(&t->nodes, &hold2);
            //if( i ==3){printf("popop %lf\n", t->nodes->next->next->body->mass);}
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
                    
                    t->c0 = hold;
                    //printf("%f\n",newNode->mass);
                    insertOctaTree(&t->c0, n);///********************
                    return ;
                }
            }
            //if z is in second half
            else{ 
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
                    t->c1 = hold;
                    insertOctaTree(&t->c1, n);
                    return ;
                }
            }
        }
        // if y is in second half
        else{
            // if z is in first half
            if (newNode->z <= (t->zmax+t->zmin)/2){
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
                    t->c2 = hold;
                    insertOctaTree(&t->c2, n);
                    return ;
                }
            }
            //if z is in second half
            else{
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
                    t->c3 = hold;
                    insertOctaTree(&t->c3, n);
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
                    t->c4 = hold;
                    insertOctaTree(&t->c4, n);
                    return ;
                }
            }
            //if z is in second half
            else{
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


                    t->c5 = hold;
                    insertOctaTree(&t->c5, n);
                    return ;
                }
            }
        }
        // if y is in second half
        else{
            // if z is in first half
            if (newNode->z <= (t->zmax+t->zmin)/2 ){
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
                    t->c6 = hold;
                    insertOctaTree(&t->c6, n);
                    return ;
                }
            }
            //if z is in second half
            else{
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
                    t->c7 = hold;
                    insertOctaTree(&t->c7, n);
                    return ;
                }
            }
        }
    }

};

// delets an entire tree based off of a root node
void deleteOctaTree(struct octaTreeNode** t){
    // if list is equal to null free it
    // other wise itorate through list freeing each node
    
    struct octaTreeNode* hold = *t;
    struct octaTreeNode* holdN;

    // if any of the children nodes are not NULL call delete octatree function
    if(hold->c0 != NULL){deleteOctaTree(&hold->c0); free(hold->c0);}
    if(hold->c1 != NULL){deleteOctaTree(&hold->c1);free(hold->c1);}
    if(hold->c2 != NULL){deleteOctaTree(&hold->c2);free(hold->c2);}
    if(hold->c3 != NULL){deleteOctaTree(&hold->c3);free(hold->c3);}
    if(hold->c4 != NULL){deleteOctaTree(&hold->c4);free(hold->c4);}
    if(hold->c5 != NULL){deleteOctaTree(&hold->c5);free(hold->c5);}
    if(hold->c6 != NULL){deleteOctaTree(&hold->c6);free(hold->c6);}
    if(hold->c7 != NULL){deleteOctaTree(&hold->c7);free(hold->c7);}
    // next check if ocatree.nodes is null, if not null call delete list
    if(hold->nodes != NULL){printf("mass: %lf\n", hold->nodes->body->mass); deleteList(&hold->nodes);free(hold->nodes);}
    // next free current node
    free(hold);
    *t = NULL;
    printf("done!!!\n");
    return;
};


long minSize;//*******************************************************
struct octaTreeNode root;

// function to read in data from a file data format is
// body GM (mass * gravitational constant), q (perinelion distence), 
// i (incline angle),w (peri angle), l (longitude od accnding node)
// x = q(sin(w))
// y = x/tan(l)
// z = q(sin(i))
// mass = GM/(6.674810^-20)

void readIn(){
    double gm;
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
        x = q*sin(w * (M_PI / 180)); // fix functions not found
        y = x/tan(l * (M_PI / 180));
        z = q*sin(i * (M_PI / 180));

        struct node n ={gm, x, y, z};
        //n->mass = gm; // fix this to just mass
         // add insert to tree
         insertOctaTree(&root, &n);
    }

    fclose(in_file);
    exit(EXIT_SUCCESS);
    
}

//************************ calculations ****************************************

// structure used to hold acceleration and velocity
struct calcVal{
    double accX;
    double accY;
    double accZ;

    double velX;
    double velY;
    double velZ;

};

/* group fucnion finds leafs in a tree,
 then calculate the center of mass and
 the combined mass of all boy,
 return info on node*/

/********calculations******************

1. itorate through the tree

2. For each node make a list of nodes for force calculations

3. use the function group to combined groups for the algo

4. calculate the forces

 - for each node in the list  use (G*mn*mi)/abs(disn - dism)^2

 - acceleration: summation from i to n where i != j; (G * massBodyi(xj - xi))/(abs(xj-xi)^3)
   where j is the value body in question

 - change in x = integer(ineger(sum(Gmi*((xj-xi)/abs(xj-xi)^3))))

*/

// itegrationooPh

    // itoreate trough tree (selecting one node at a time)
        // call function to add up all forces, velocity, acceleration, acting on element
        // use informatin returned to calculate opsition sum from i to n G*mi*((xj-xi)/pow(abs(xj-xi),3))
        // add updated body to new tree
    //delete old tree
    // set root equal to new tree

//function to traverse tree and calculate

    //traverse down to a leaf node
        // if node is not in leaf use center of mass to calculate faoce
        // else add up individual forces
        // return values in a node
    // navaget to children nodes if not null

