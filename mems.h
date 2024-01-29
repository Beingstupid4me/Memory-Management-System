/*
All the main functions with respect to the MeMS are inplemented here
read the function discription for more details

NOTE: DO NOT CHANGE THE NAME OR SIGNATURE OF FUNCTIONS ALREADY PROVIDED
you are only allowed to implement the functions 
you can also make additional helper functions a you wish

REFER DOCUMENTATION FOR MORE DETAILS ON FUNSTIONS AND THEIR FUNCTIONALITY
*/
// add other headers as required
#include<stdio.h>
#include<stdlib.h>
#include<sys/mman.h>


/*
Use this macro where ever you need PAGE_SIZE.
As PAGESIZE can differ system to system we should have flexibility to modify this 
macro to make the output of all system same and conduct a fair evaluation. 
*/
#define PAGE_SIZE 4096


struct main_list_block
{
int page_number;
size_t size;
int *Start_physical; // the start of the address of page
int *End_physical;   // the end of the address of page
int Start_virtual; // the start of the address per block
int End_virtual; // the end of the address per block
struct side_list_block *side_head;
struct main_list_block *next;
};

struct side_list_block
{
size_t size_for_chunk;
int *Start_physical; // the start of the address per block
int *End_physical; // the end of the address per block
int Start_virtual; // the start of the address per block
int End_virtual; // the end of the address per block
int used;
struct side_list_block *next;
};


struct main_list_block *List_head=NULL;

int *node_page_start = NULL;
int *node_page_end = NULL;

/*
Initializes all the required parameters for the MeMS system. The main parameters to be initialized are:
1. the head of the free list i.e. the pointer that points to the head of the free list
2. the starting MeMS virtual address from which the heap in our MeMS virtual address space will start.
3. any other global variable that you want for the MeMS implementation can be initialized here.
Input Parameter: Nothing
Returns: Nothing
*/

int null_check(){
    if (List_head->Start_physical==NULL)
    {
        return 2;
    }
    return 0;
}


void mems_init(){
node_page_start = (int*)mmap(NULL,PAGE_SIZE , PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
int *debug = node_page_start;
node_page_end=node_page_start+4095;
struct main_list_block *ptr=(struct main_list_block*)(node_page_start);
node_page_start=node_page_start+sizeof(struct main_list_block);
List_head=ptr;
List_head->page_number=0;
//List_head->Start_physical = (int*)mmap(NULL,PAGE_SIZE , PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
//List_head->End_physical = List_head->Start_physical+PAGE_SIZE-1;
List_head->Start_virtual=1000;
List_head->End_virtual=List_head->Start_virtual+PAGE_SIZE-1;
// Adding a side list block


}



/*
This function will be called at the end of the MeMS system and its main job is to unmap the 
allocated memory using the munmap system call.
Input Parameter: Nothing
Returns: Nothing
*/

void mems_finish(){
    struct main_list_block *tempo =List_head;
    while (tempo->next!=NULL)
    {
        munmap(tempo->Start_physical,NULL);
        tempo=tempo->next;
    }
}


/*
Allocates memory of the specified size by reusing a segment from the free list if 
a sufficiently large segment is available. 

Else, uses the mmap system call to allocate more memory on the heap and updates 
the free list accordingly.

Note that while mapping using mmap do not forget to reuse the unused space from mapping
by adding it to the free list.
Parameter: The size of the memory the user program wants
Returns: MeMS Virtual address (that is created by MeMS)
*/ 

// BREAD

void* mems_malloc(size_t size){
struct main_list_block *terp = List_head;
struct side_list_block *burpy = List_head->side_head;
struct main_list_block *pepa;
if (null_check()==0)
{

// Checking the currently maintained data structure
while (terp!=NULL)
{
    while (burpy!=NULL)
    {
        if (burpy->size_for_chunk>size && burpy->used==0)
        {
            // Debug part
            //printf("Checking greater, Burpy.size = %d and size given is %d\n",burpy->size_for_chunk,size);
            struct side_list_block *tempura=(struct side_list_block*)(node_page_start);
            node_page_start=node_page_start+sizeof(struct side_list_block);
            // modyfing the already existing node
            burpy->used=1;
            tempura->End_physical=burpy->End_physical;
            tempura->End_virtual=burpy->End_virtual;
            tempura->next=burpy->next;
            burpy->next=tempura;
            burpy->End_physical=burpy->Start_physical+size-1;
            burpy->End_virtual=burpy->Start_virtual+size-1;
            // now doing the new node
            tempura->Start_physical=burpy->End_physical+1;
            tempura->used=0;
            tempura->Start_virtual=burpy->End_virtual+1;    
            tempura->size_for_chunk=burpy->size_for_chunk-size;
            burpy->size_for_chunk=size;
            return burpy->Start_virtual;
        }
        else if (burpy->size_for_chunk==size && burpy->used==0)
        {
            burpy->used=1;
            return burpy->Start_virtual;
        }
        burpy=burpy->next;
    }
    terp=terp->next;
    if (terp==NULL)
    {
        break;
    }
    burpy=terp->side_head;
}
// Now creating a new page


// checking if space for node is available
if ((node_page_end-node_page_start)<sizeof(struct main_list_block))
{
    node_page_start=(int*)mmap(NULL,PAGE_SIZE , PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    node_page_end=node_page_start+PAGE_SIZE-1;
}

pepa=(struct main_list_block*)(node_page_start);
node_page_start=node_page_start+sizeof(struct main_list_block);
struct main_list_block *trippy = List_head;
while (trippy->next!=NULL)
{
    trippy=trippy->next;
}
trippy->next=pepa;

// Configuring next virtual start 

pepa->Start_virtual=trippy->End_virtual+1;   
pepa->page_number=trippy->page_number+1;
}

else if (null_check()==2)
{
    pepa = List_head;
}

//checking size for clarification
double factor = 0;
if (size>PAGE_SIZE)
{
    int quo = (size/PAGE_SIZE);
    double qwe = (size/PAGE_SIZE);
    if (qwe-quo==0)
    {
        factor = quo;
    }
    else
    {
        factor = quo + 1;
    }
}
else
{
    factor = 1;
}

// Calling mmap with calculated factor * Page size

pepa->Start_physical=(int*)mmap(NULL,PAGE_SIZE*factor , PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
pepa->End_physical=pepa->Start_physical+(int)(PAGE_SIZE*factor);
pepa->End_virtual=pepa->Start_virtual+(int)(PAGE_SIZE*factor)-1;
size_t rasd = PAGE_SIZE*factor;
pepa->size=(int)(PAGE_SIZE*factor);
// main list block has been added successfully;
if (size==(PAGE_SIZE*factor))
{
    if ((node_page_end-node_page_start)<sizeof(struct side_list_block))
    {
        node_page_start=(int*)mmap(NULL,PAGE_SIZE , PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        node_page_end=node_page_start+PAGE_SIZE-1;
    }
    struct side_list_block *tempa=(struct side_list_block*)(node_page_start);
    node_page_start=node_page_start+sizeof(struct side_list_block);
    tempa->size_for_chunk=PAGE_SIZE*factor;
    tempa->Start_physical=pepa->Start_physical;
    tempa->End_physical=pepa->End_physical;
    tempa->Start_virtual=pepa->Start_virtual;
    tempa->End_virtual=pepa->End_virtual;
    pepa->side_head=tempa;
    tempa->used=1;
}
else
{
    if ((node_page_end-node_page_start)<2*sizeof(struct side_list_block))
    {
        node_page_start=(int*)mmap(NULL,PAGE_SIZE , PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        node_page_end=node_page_start+PAGE_SIZE-1;
    }
    struct side_list_block *tempa=(struct side_list_block*)(node_page_start);
    node_page_start=node_page_start+sizeof(struct side_list_block);
    tempa->size_for_chunk=size;
    tempa->Start_physical=pepa->Start_physical;
    tempa->End_physical=pepa->Start_physical+size;
    tempa->Start_virtual=pepa->Start_virtual;
    tempa->End_virtual=pepa->Start_virtual+size;
    pepa->side_head=tempa;
    tempa->used=1;
    struct side_list_block *pamper=(struct side_list_block*)(node_page_start);
    node_page_start=node_page_start+sizeof(struct side_list_block);
    // now doing the new node for remaining as hole
    pamper->End_physical=pepa->End_physical;
    pamper->End_virtual=pepa->End_virtual;
    tempa->next=pamper;
    pamper->Start_physical=tempa->End_physical+1;
    pamper->used=0;
    pamper->Start_virtual=tempa->End_virtual;
    pamper->size_for_chunk=pepa->size-tempa->size_for_chunk;
}
return pepa->Start_virtual;
}


void print_random(){
    struct main_list_block *trippy=List_head;
    struct side_list_block *preppy=List_head->side_head;
    while (trippy!=NULL)
    {
        printf("Main list block with start-%d and end-%d with size-%d\n",trippy->Start_virtual,trippy->End_virtual,trippy->size);
        while (preppy!=NULL)
        {
            printf("\t Side_list_block with start-%d and end-%d with size-%d",preppy->Start_virtual,preppy->End_virtual,preppy->size_for_chunk);
            if (preppy->used==1)
            {
                printf(" and it is allocated to process.\n");
            }
            else
            {
                printf(" and this a hole \n");
            }
            
            preppy=preppy->next;
        }
        trippy=trippy->next;
        if (trippy==NULL)
        {
            break;
        }
        preppy=trippy->side_head;
    }
    
}

/*
this function print the stats of the MeMS system like
1. How many pages are utilised by using the mems_malloc
2. how much memory is unused i.e. the memory that is in freelist and is not used.
3. It also prints details about each node in the main chain and each segment (PROCESS or HOLE) in the sub-chain.
Parameter: Nothing
Returns: Nothing but should print the necessary information on STDOUT
*/
void mems_print_stats(){
//print_random();
printf("\n----------Printing Mems Stats---------------\n");
    struct main_list_block *trippy=List_head;
    struct side_list_block *preppy=List_head->side_head;
    int main_ct=0;
    int sdi[20];
    int f=0;
    int spc_us=0;
    int pg_us=0;
    while (trippy!=NULL)
    {
        main_ct++;
        printf("MAIN[%d:%d] -> ",trippy->Start_virtual,trippy->End_virtual);
        int m=0;
        pg_us=pg_us+((trippy->size)/PAGE_SIZE);
        while (preppy!=NULL)
        {
            m++;
            if (preppy->used==1)
            {
                printf(" P_[%d:%d] ->",preppy->Start_virtual,preppy->End_virtual);
                spc_us=spc_us+(preppy->End_virtual-preppy->Start_virtual)+1;
            }
            else
            {
                printf(" H_[%d:%d] ->",preppy->Start_virtual,preppy->End_virtual);
            }
            
            preppy=preppy->next;
        }
        sdi[f]=m;
        //printf("\nThe m is %d\n",m);
        f++;
        printf(" NULL\n");
        trippy=trippy->next;
        if (trippy==NULL)
        {
            break;
        }
        preppy=trippy->side_head;
    }
    printf("Main list length is - %d\n",main_ct);
    printf("The side chain length [");
    for(int g=0;g<f;g++){
        printf(" %d, ",sdi[g]);
    }
    printf("]\n");
    printf("Space used is %d\n",spc_us);
    printf("Page used  are %d\n",pg_us);
    printf("-------------Done-------------\n");
}
// MAIN[starting_mems_vitual_address:ending_mems_vitual_address] -> <HOLE or PROCESS>[starting_mems_vitual_address:ending_mems_vitual_address] <-> ..... <-> NULL

/*
Returns the MeMS physical address mapped to ptr ( ptr is MeMS virtual address).
Parameter: MeMS Virtual address (that is created by MeMS)
Returns: MeMS physical address mapped to the passed ptr (MeMS virtual address).
*/
void *mems_get(void*v_ptr){
    //printf("The starting address of List_head is %p\n",List_head->Start_physical);
    int trent = (int*) v_ptr;
    //printf("The value of trent is %d\n",trent);
    //printf("The address required is %d",trent);
    struct main_list_block *mentryy = List_head;
    struct side_list_block *sentryy = List_head->side_head;
    while (mentryy!=NULL)
    {
        if (trent>=mentryy->Start_virtual && trent<=mentryy->End_virtual)
        {
            sentryy=mentryy->side_head;
            while (sentryy!=NULL)
            {
                if (trent>=sentryy->Start_virtual && trent<=sentryy->End_virtual)
                {
                    if (sentryy->used==1)
                    {
                        int add_on = trent-(sentryy->Start_virtual);



                        int *ptr = (int*)sentryy->Start_physical;
                        int *trash=add_on;
                        //printf("The ptr of start is %d",ptr);
                        //ptr=(trent)-(sentryy->Start_virtual)+(sentryy->Start_physical);
                        for (int z = 0; z < (add_on/4); z++)
                        {
                            ptr=ptr+1;
                        }
                        //printf(" and the returning adress is %d\n",ptr);
                        return ptr;
                    }
                    else
                    {
                        printf("This address is not alloted to any process\n");
                    }
                }
                sentryy=sentryy->next;
            }
        
        }
        mentryy=mentryy->next;
    }
    printf("The address is out of bound\n");
}


/*
this function free up the memory pointed by our virtual_address and add it to the free list
Parameter: MeMS Virtual address (that is created by MeMS) 
Returns: nothing
*/

void freeinga(void *v_ptr){
    int pempa = (int*) v_ptr; 
    //printf("The address to free is %d\n",pempa);
    struct main_list_block *rentyy = List_head;
    struct side_list_block *shenty = List_head->side_head;
    while (rentyy!=NULL)
    {
        if ((rentyy->Start_virtual)<=pempa && (rentyy->End_virtual)>=pempa)
        {
            //printf("The main_block is start- %d, end- %d\n",rentyy->Start_virtual,rentyy->End_virtual);
            while (shenty!=NULL)
            {
                //printf("\t Block is start- %d, end- %d with use- %d\n",shenty->Start_virtual,shenty->End_virtual,shenty->used);
                if (shenty->Start_virtual<=pempa && shenty->End_virtual>=pempa)
                {
                    shenty->used=0;
                    //printf("After setting, we get,\nThe block is start- %d, end- %d with use- %d\n",shenty->Start_virtual,shenty->End_virtual,shenty->used);
                    return;
                }
                shenty=shenty->next;
            }
        }
        rentyy=rentyy->next;
        if (rentyy==NULL)
        {
            break;
        }
        
        shenty=rentyy->side_head;
    }
}

void merge_holes(){
    struct main_list_block *rentyy = List_head;
    struct side_list_block *shenty = List_head->side_head;
    struct side_list_block *tempy;
    struct side_list_block *prevy;
    while (rentyy!=NULL)
    {
        while (shenty!=NULL){
            
            if (shenty->used==0)
            {
                tempy=shenty;
                prevy=shenty;
                while (tempy->used==0)
                {
                    prevy=tempy;
                    tempy=tempy->next;
                    if (tempy==NULL)
                    {
                        break;
                    }
                }
                if (tempy!=shenty)
                {
                    shenty->End_physical=prevy->End_physical;
                    shenty->End_virtual=prevy->End_virtual;
                    shenty->size_for_chunk=shenty->End_virtual-shenty->Start_virtual+1;
                    shenty->next=prevy->next;
                }
            }
            shenty=shenty->next;
        }
        rentyy=rentyy->next;
    }

}

void mems_free(void *v_ptr){
freeinga(v_ptr);
merge_holes();
}