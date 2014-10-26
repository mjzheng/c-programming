/*File:    prog2.c
 *
 *Author:  Musica Zheng
 *
 *Purpose: Implement a sorted doubly linked list with ops insert,
 *         print, member, delete, free_list.
 *
 *Input:   Single character lower case letters to indicate operators, 
 *         followed by arguments needed by operators.
 *
 *Output:  Results of operations.
 *
 *Compile: gcc -g -Wall -o prog2 prog2.c
 *Run:     ./prog2
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct list_node_s {
   char*  data;
   struct list_node_s* pred_p;
   struct list_node_s* next_p;
};

struct list_s{
   struct list_node_s* h_p;
   struct list_node_s* t_p;
};

int Member(struct list_s* list, char string[]);
void Insert(struct list_s* list, char string[]);
void Delete(struct list_s* list, char string[]);
void Print(struct list_s* list);
void Free_list(struct list_s* list); 
char Get_command(void);
void Get_string(char string[]);
struct list_node_s* Allocate_node(int size);
void Free_node(struct list_node_s* node_p);

/*-----------------------------------------------------------------*/
int main(void) {
   char command;
   char word[100];

   struct list_s list;
   list.h_p= NULL;   
   list.t_p= NULL;
   /* start with empty list */

   command = Get_command();
   while (command != 'q' && command != 'Q') {
      switch (command) {
         case 'i': 
         case 'I': 
            Get_string(word);
            Insert(&list, word);
            break;
         case 'p':
         case 'P':
            Print(&list);
            break;
         case 'm': 
         case 'M':
            Get_string(word);
            if (Member(&list, word))
               printf("%s is in the list\n", word);
            else
               printf("%s is not in the list\n", word);
            break;
         case 'd':
         case 'D':
            Get_string(word);
            Delete(&list, word);
            break;
         case 'f':
         case 'F':
            Free_list(&list);
            break;
         default:
            printf("There is no %c command\n", command);
            printf("Please try again\n");
      }
      command = Get_command();
   }

   Free_list(&list);

   return 0;
}  /* main */


/*-----------------------------------------------------------------
 * Function:    Member
 * Purpose:     search list for string
 * Input args:  list: pointer to a struct that contains pointers
 *                    to head and tail of list
 *              string: string to search for
 * Return val:  1 if string is in list, 0 otherwise
 */
int Member(struct list_s* list, char string[]) {
   struct list_node_s* curr_p = list->h_p;

   while (curr_p != NULL) {
      if (strcmp(string,curr_p->data) == 0)
         return 1;
      else
         curr_p = curr_p->next_p;
   }
   return 0;
}  /* Member */


/*-----------------------------------------------------------------
 * Function:   Delete
 * Purpose:    Delete the string from list
 * Input args: list: pointer to a struct that contains pointers
 *                    to head and tail of list
 *             string: string to be deleted
 */
void Delete(struct list_s* list, char string[]) {

   struct list_node_s* curr_p = list->h_p;
   /* Find node containing val and predecessor of this node */
   while (curr_p != NULL){
      if (strcmp(string,curr_p->data) == 0){
         break;
      }
      else { // curr_p->data != string 
         curr_p = curr_p->next_p;
      }
   }

   if (curr_p == NULL){ //if empty string or if not in string
      printf("%s is not in the list\n", string);
   }
   else if(curr_p->next_p==NULL&&curr_p->pred_p==NULL){ //1 node in list
        Free_node(curr_p);
	list->h_p = NULL;
	list->t_p = NULL;
   }
   else if (curr_p->pred_p == NULL) { //delete head
      curr_p = curr_p->next_p;
      Free_node(curr_p->pred_p);
      curr_p->pred_p = NULL;
      list->h_p = curr_p;

   } 
   else if(curr_p->next_p == NULL){ //delete tail
      curr_p=curr_p->pred_p;
      Free_node(curr_p->next_p);
      curr_p->next_p = NULL;
      list->t_p = curr_p;
   }
   else { //general case
      curr_p->pred_p->next_p = curr_p->next_p;
      curr_p->next_p->pred_p = curr_p->pred_p;
      curr_p->next_p = NULL;
      curr_p->pred_p = NULL;
      Free_node(curr_p);
   }
}  /* Delete */


/*-----------------------------------------------------------------
 * Function:   Insert
 * Purpose:    Insert string so it's sorted in list
 * Input args: list: pointer to a struct that contains pointers
 *                    to head and tail of list
 *             string:  new string to be inserted
 */
void Insert(struct list_s* list, char string[]) {
   struct list_node_s* curr_p = list->h_p;
   struct list_node_s* temp_p;

   while(curr_p!=NULL){
	if(strcmp(string,curr_p->data) == 0){
	    printf("%s is already in list\n",string);
	    return;
        }else if(strcmp(string,curr_p->data) < 0){
	    break;
	}else{
	    curr_p=curr_p->next_p;
	}
   }

   //create new node
   temp_p = Allocate_node(strlen(string));
   strcpy(temp_p->data, string);

   //case 1: Empty String
   if(list->h_p==NULL){
	temp_p->pred_p = NULL;
	temp_p->next_p = NULL;
	list->h_p = temp_p;
	list->t_p = temp_p;
   }
   //case 2: Insert at beginning
   else if(curr_p==list->h_p){
	temp_p->next_p = curr_p;
	temp_p->pred_p = NULL;
	curr_p->pred_p = temp_p;
        list->h_p = temp_p;
   }
   //case 3: Insert at end
   else if(curr_p==NULL){
	curr_p = list->t_p;
	curr_p->next_p = temp_p;
	temp_p->pred_p = curr_p;
	temp_p->next_p = NULL;
	list->t_p = temp_p;
   }
   //case 4: General case
   else{
	temp_p->pred_p = curr_p->pred_p;
	temp_p->next_p = curr_p;
	curr_p->pred_p->next_p = temp_p;
	curr_p->pred_p = temp_p;
   }
}   /* Insert */

/*-----------------------------------------------------------------
 * Function: Allocate_node
 * Purpose:  To make storage for new nodes and data
 * Input arg: size of string
 * Return Value: temp_p
*/
struct list_node_s* Allocate_node(int size){
   struct list_node_s* temp_p;
   temp_p = malloc(sizeof(struct list_node_s));
   temp_p-> data = malloc((size+1)*sizeof(char));
   return temp_p;
} /*Allocate_node*/

/*-----------------------------------------------------------------
 * Function:   Print
 * Purpose:    Print list on a single line of stdout
 * Input arg:  list
 */
void Print(struct list_s* list) {
   struct list_node_s* curr_p = list->h_p;

   printf("list = ");
   while (curr_p != NULL) {
      printf("%s ", curr_p->data);
      curr_p = curr_p->next_p;
   }  
   printf("\n");
}  /* Print */


/*-----------------------------------------------------------------
 * Function:    Free_list
 * Purpose:     free each node in the list
 * Input arg:   list
 * Note:        list->h_p and list->t_p is set to NULL on 
 *              completion, indicating
 *              list is empty.
 */
void Free_list(struct list_s* list) {
   struct list_node_s* curr_p;
   struct list_node_s* temp_p;
   
   curr_p = list->h_p;

   while (curr_p != NULL) {
      temp_p = curr_p;
      Free_node(temp_p); 
      curr_p = curr_p->next_p;
   }

   list->h_p = NULL;
   list->t_p = NULL;
   
}  /* Free_list */

/*-----------------------------------------------------------------
 * Function:   Free_node
 * Purpose:    empty storage of node and it's data
 * Input arg:  node_p
 */
void Free_node(struct list_node_s* node_p){
   free(node_p -> data);
   free(node_p);
} /*Free_node*/

/*-----------------------------------------------------------------
 * Function:      Get_command
 * Purpose:       Get a single character command from stdin
 * Return value:  the first non-whitespace character from stdin
 */
char Get_command(void) {
   char c;

   printf("Please enter a command (i, p, m, d, f, q):  ");
   /* Put the space before the %c so scanf will skip white space */
   scanf(" %c", &c);
   return c;
}  /* Get_command */

/*-----------------------------------------------------------------
 * Function:   Get_string
 * Purpose:    Get a string from stdin
 * Input:      variable to store user's input
 */
void Get_string(char string[]) {
   printf("Please enter a string:  ");
   scanf("%s", string);
}  /* Get_value */

