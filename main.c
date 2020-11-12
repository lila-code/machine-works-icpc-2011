/******************************************************************************

    My solution is based on generating possible profits and selecting the best one
    I used a linked list to store the different possibilites 
    With each new available machine, I generate at most two decisions
    The first one is to not buy the available machine, 
      if there is a machine at the company I resale it, and I calculate the profit that it generated from the last day I did
      if not I do nothing
    The second one is to buy the available machine
      only if the company has enough money after saling the current one
      and if it has not the same day of availibity of the current machine 
      
    It took me about one hour to figure out the general solution 
    Half an hour to decide which suitable structures to use
    Then about six hours to program it and debug it
    

*******************************************************************************/
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

/********************************************************************************************************************************/
/******************************************************* Structures ************************************************************/
/******************************************************************************************************************************/
// Machine describes a machine 
struct Machine
{
    unsigned long long di;                                                                    // day of availibity
    unsigned long long pi;                                                                    // price
    unsigned long long ri;                                                                    // resale price 
    unsigned long long gi;                                                                    // daily generated profit
};

// StatusNode describes the state of the company after making a set of decisions
struct StatusNode
{

    unsigned long long currentMachine;                                                        // the current machine at the company
    unsigned long long currentProfit;                                                         // current profit of the company 
    struct StatusNode* next;                                                                 // another status based on other decisions
};


// Period describes a period of retructing 
struct Period
{
    unsigned long long n;                                                                     // number of available machines
    unsigned long long c;                                                                     // starting profit of the company
    unsigned long long d;                                                                    // duration of the restructring period
    struct Machine availableMachines[];
};



/********************************************************************************************************************************/
/***************************************************** Functions ***************************************************************/
/******************************************************************************************************************************/

//initialises its parameter mi with di, pi, ri and gi
void init_machine(struct Machine *mi, unsigned long long di, unsigned long  long pi, unsigned long long ri, unsigned long long gi)
{
    mi->di = di;
    mi->pi = pi;
    mi->ri = ri;
    mi->gi = gi;
}


//initialises its parameter sn with currentMachine and currentProfit
void init_status_node(struct StatusNode *sn, unsigned long  long currentMachine, unsigned long long currentProfit)
{
    sn->currentMachine = currentMachine;
    sn->currentProfit = currentProfit;
    sn->next = (struct StatusNode*)0;
}


//initialises its parameter p with n, c and d
void init_period(struct Period *p, unsigned long long n, unsigned long long c, unsigned long long d)
{
    p->n = n;
    p->c = c;
    p->d = d;
}



//sorts a row of machines by their availibity date using the selection sort
void sort_row(unsigned long long n, struct Machine mac_row[]) 
{
    unsigned long long i, j, mem;
    struct Machine temp;
    for(i = 0; i < n-1; i++)
    {
        mem = i; 
        for(j = i+1; j < n; j++)
        {
            if(mac_row[j].di < mac_row[mem].di)
                mem = j;
        }
        temp = mac_row[mem];
        mac_row[mem] = mac_row[i];
        mac_row[i] = temp;
    }
}


//generates the decisions of each possible state of the company during the restructing period p
//according to the available machine the i th day 
//decisions are saved in a linked list
// the head of this list is sn
void make_decisions(struct Period *p, struct StatusNode **sn, struct Machine mac_row[], unsigned long long i)
{
    struct StatusNode *saleAndBuyNode;
    struct StatusNode *notSaleOrBuyNode;
    struct StatusNode *currentNode;
    unsigned long long tempProfit;
    
    // first available machine 
    if( *sn == (struct StatusNode*)0)
    {
        // node for the non buying decision
        if((notSaleOrBuyNode = (struct StatusNode *)malloc(sizeof(struct StatusNode))) == NULL)
        {
            printf("malloc returned NULL");
            exit(3);
        }
        init_status_node(notSaleOrBuyNode, -1, p->c);
        printf("result of not buy the %llu machine : %llu\n", mac_row[i].di, p->c);
        // node for the buying decision if and only if the company has enough money 
        if(p->c >= mac_row[i].pi)
        {
            
            if((saleAndBuyNode = (struct StatusNode *)malloc(sizeof(struct StatusNode))) == NULL)
            {
                printf("malloc returned NULL");
                exit(4);
            }
            init_status_node(saleAndBuyNode, i, p->c - mac_row[i].pi);
            notSaleOrBuyNode-> next = saleAndBuyNode;
            printf("result of not buy the %llu machine : %llu\n", mac_row[i].di, p->c - mac_row[i].pi);
        }
        // saving the head of the linked list
        *sn = notSaleOrBuyNode;
    }
    else 
    {
       // browsing and making new decisions for each possible states
       currentNode = *sn;
       while(currentNode != (struct StatusNode *)0)
       {
           
           // a machine is already at the company according to this state
           tempProfit = currentNode->currentProfit;
           if(currentNode->currentMachine != -1) 
            {   
                // calculating the result of not selling the machine 
                currentNode->currentProfit = (mac_row[i].di - mac_row[i-1].di)*mac_row[currentNode->currentMachine].gi
                                                + currentNode->currentProfit;
                printf("result of selling the %llu machine : %llu\n", mac_row[currentNode->currentMachine].di, currentNode->currentProfit);  
                // calculating the result of selling the machine
                // if the current machine has the same day of availibity than the available one, don't consider the resale option
                // if not then calculate the result of the resale option
                if(mac_row[currentNode->currentMachine].di == mac_row[i].di)
                    tempProfit = -1;
                else 
                    tempProfit = (mac_row[i].di - mac_row[i-1].di-1)*mac_row[currentNode->currentMachine].gi 
                                   + mac_row[currentNode->currentMachine].ri
                                   + tempProfit;
                    printf("result of not selling the %llu machine : %llu\n", mac_row[currentNode->currentMachine].di, tempProfit);
            }
            // if enough money to buy a new machine then save the next state to process
            // create a new node to store this decision and link it to the list
            // else go on to the next state
            if(tempProfit >= mac_row[i].pi)
            {
                if((saleAndBuyNode = (struct StatusNode *)malloc(sizeof(struct StatusNode))) == NULL)
                {
                    printf("malloc returned NULL");
                    exit(5);
                }                  
                init_status_node(saleAndBuyNode, i, tempProfit -mac_row[i].pi);
                printf("result of selling the machine %llu and buy the %llu machine : %llu\n", mac_row[currentNode->currentMachine].di, mac_row[i].di, tempProfit -mac_row[i].pi);
                saleAndBuyNode->next = currentNode->next;
                currentNode->next = saleAndBuyNode;
                currentNode = saleAndBuyNode->next;
            }
            else currentNode = currentNode->next;    
       }
    }
   
}


// calculates the final profits at the end of the period of restructing and selects the best one
unsigned long long select_max_profit(struct StatusNode *sl, struct Machine mac_row[], struct Period *p)
{
    struct StatusNode *currentNode;
    unsigned long long maxProfit;
    unsigned long long lastProfit; 
    
    // browsing all the last possible states 
    maxProfit = p->c;
    currentNode = sl;
    while(currentNode != (struct StatusNode*)0)
    {
        if(currentNode->currentMachine != -1)
        {
           lastProfit = mac_row[currentNode->currentMachine].gi*(p->d - mac_row[p->n -1].di)+mac_row[currentNode->currentMachine].ri+currentNode->currentProfit;
           printf("profit of the %llu machine : %llu\n", mac_row[currentNode->currentMachine].di, lastProfit); 
           if (maxProfit < lastProfit)
           {
               maxProfit = lastProfit;
              
           }
        }
        
        currentNode = currentNode->next;
    }
    return maxProfit;
}

// generates all the possible marketing strategies that the company can make 
// and selects the maximum profit 
unsigned long long marketing_strategy(struct Period *p, struct Machine mac_row[])
{ 
    struct StatusNode *statusList, *lastStatus;
    unsigned long long max;
    int i;
    
    max = p->c;
    statusList = (struct StatusNode *)0;
    sort_row(p->n, mac_row);
    for(i = 0; i < p->n; i++)
    {
       make_decisions(p, &statusList, mac_row, i);
    }
    max = select_max_profit(statusList, mac_row, p);
    while (statusList != (struct StatusNode *) 0)
    {
        lastStatus = statusList;
     	statusList = statusList->next;
        free(lastStatus);
    }
    return max;
}

/******************************************************************************************************************************/
/****************************************************Main Function************************************************************/
/****************************************************************************************************************************/

int main()
{
    FILE *in, *out;
    struct Period p;
    unsigned long long n, c, d;
    unsigned long long di, pi, ri, gi;
    unsigned long long i,max; 
    int cpt;
     
    
    if ((in = fopen("input.txt", "r")) == NULL)
    {
        printf("cannot open the input file");
        exit(1);
    }
    if ((out = fopen("output.txt", "w")) == NULL)
    {
        printf("cannot open the output file");
        exit(2);
    }
    fscanf(in, "%llu %llu %llu", &n, &c, &d);
    cpt = 1;
    while(n || c || d)
    {
        struct Machine mac_row[n];
        init_period(&p, n, c, d);
        for(i = 0; i < p.n; i++)
        {
            fscanf(in, "%llu %llu %llu %llu", &di, &pi, &ri, &gi);
            init_machine(&mac_row[i], di, pi, ri, gi);
        }
	max = marketing_strategy(&p, mac_row);
        fprintf(out, "Case %d : %llu\n", cpt, max);
        fscanf(in, "%llu %llu %llu", &n, &c, &d);
        cpt += 1;
    }
    fclose(out);
    fclose(in);
    return 0;
}
