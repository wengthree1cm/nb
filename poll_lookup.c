/*
 * CSE30 Summer Session 1 '22 HW3
 * CSE30 username: cs30su122xxx (TODO: Fill in)
 */

#include "poll_lookup.h"

/*
 * !!! DO NOT EDIT THIS FUNCTION !!!
 * main
 *
 * Arguments: argc, argv
 *
 * Operation: Main driver for the program, calls other funttions to:
 *            parse the options, allocate the hash table, load the table, print
 *out the table stats
 *            and make print population stats of the desired city/state
 * Returns:   EXIT_SUCCESS if all ok, EXIT_FAILURE otherwise
 * !!! DO NOT EDIT THIS FUNCTION !!!
 */
int main(int argc, char *argv[]) {
  node **table;
  unsigned long size = TABLE_SIZE;
  // name of csv file
  char *filename;
  int info = 0;

  // Indicates days we want stats for/to remove
  char *date = NULL;
  char *del_date = NULL;

  // Parse options
  if (!parse_opts(argc, argv, &filename, &size, &info, &date, &del_date)) {
    return EXIT_FAILURE;
  }

  // Allocate space for table
  if ((table = calloc(size, sizeof(node *))) == NULL) {
    fprintf(stderr, "%s: Unable to allocate space for hash table\n", argv[0]);
    return EXIT_FAILURE;
  }

  // Load records from file
  if (load_table(table, size, filename)) {
    return EXIT_FAILURE;
  }
  // Delete data first
  if (del_date) {
    char *stripped_date = strip_date(del_date);
    if (stripped_date) { // no malloc fail
      delete_date(table, size, stripped_date);
      free(stripped_date);
    } else {
      return EXIT_FAILURE;
    }
  }

  // Produce data for a single date
  if (date) {
    char *stripped_date = strip_date(date);
    if (stripped_date) { // no malloc fail
      print_date_stats(table, size, stripped_date);
      free(stripped_date);
    } else {
      return EXIT_FAILURE;
    }
  }

  // Print metadata
  if (info)
    print_info(table, size);

  // Clean up
  delete_table(table, size);

  return EXIT_SUCCESS;
}

/*
 * !!! DO NOT EDIT THIS FUNCTION !!!
 * hash
 *
 * Arguments: a null terminated string
 *
 * Operation: calculates a hash value for the string
 *
 * returns:   the hash value
 * !!! DO NOT EDIT THIS FUNCTION !!!
 */
unsigned long hash(char *str) {
  unsigned long hash = 0;
  unsigned int c;
#ifdef C_HASH
  while ((c = (unsigned char)*str++) != '\0') {
    hash = c + (hash << 6) + (hash << 16) - hash;
  }
#else
  while ((c = (unsigned char)*str++) != '\0') {
    hash = hashFun((unsigned long)c, hash);
  }
#endif
  return hash;
}

/*
 * add_node
 *
 * Arguments: linked list pointer head, year, month, day, hour, pm25, temp
 */
node *add_node(node *front, int year, int month, int day, int hour, int pm25,
    int temp) {
  node * node1 =(node*)malloc(sizeof(node));
  if(node1==NULL){
    return NULL;
  }
  node *tmp=front;
  node1->year=year;
  node1->day=day;
  node1->month=month;
  node1->hour=hour;
  node1->pm25=pm25;
  node1->temp=temp;
  node1->next=NULL;
  if(tmp==NULL){
    tmp=node1;
    return tmp;


  }

    while(tmp->next!=NULL){
      tmp=tmp->next;

    }
    tmp->next=node1;
    return front;

}

/*
 * print_date_stats
 * Print the average stats for this date
 *
 * Arguments: pointer to hash table, hash table size, date as a string in the
 *form YYYY-MM-DD
 */
void print_date_stats(node **table, unsigned long size, char *datestr) {
  
  unsigned long index=hash(datestr)%size;
  node *chain = table[index];

  char temp[12];
  strncpy(temp, datestr, 12);

  int maxtemp=-1000000; int mintemp= 1000000; int maxpm=-1000000; int minpm= 1000000;
  int avpm=0; int avtemp=0; int count=0; int count2=0;

  char *token = strtok(datestr, "-");
  int cols[COL_DAY+1];
  int c = 0;

  while (token != NULL) {
    cols[c] = atoi(token);
    token = strtok(NULL, "-");
    c++;
  }
   while (chain != NULL)
   {
	   if (chain->year == cols[COL_YEAR] && chain->month == cols[COL_MONTH]
		   && chain->day == cols[COL_DAY]) {
		   count++;
		   if (chain->pm25 > maxpm) {
			   maxpm = chain->pm25;
		   }
		   if (chain->temp > maxtemp) {
			   maxtemp = chain->temp;
		   }
		   if (chain->pm25 < minpm) {
			   minpm = chain->pm25;
		   }
		   if (chain->temp < mintemp) {
			   mintemp = chain->temp;
		   }
		   avpm = avpm + chain->pm25;
		   avtemp = avtemp + chain->temp;
	   }
	   chain = chain->next;
   }
  // TODO: Implement print_data_stats
  // Use the following formatting strings to print messages.
  if(count==0){

    printf("Unable to find any data for the date %s.\n", temp);
  }
  else
  {
	  printf("Minimum pm2.5: %d\tMaximum pm2.5: %d\tAverage pm2.5: %d\n",
		  minpm, maxpm, avpm / count);
	  printf("Minimum temp: %d\tMaximum temp: %d\tAverage temp: %d\n",
		  mintemp, maxtemp, avtemp / count);
  }
 }


/*
 * load_table
 * Allocate memory for the hash table of node*s
 *
 * Arguments: pointer to hash table, hash table size, file name
 */
int load_table(node **table, unsigned long size, char *filename) {
  FILE * file1=fopen(filename,"r");
    if(file1==NULL){
        perror("load_table filename open");
        return 1;
    }

    int buf[6];
    char *abc=malloc(sizeof(char)*LINE_SIZE);
    fgets(abc,LINE_SIZE-1,file1);
    if(abc==NULL){
        perror("load_table malloc");
    }
    while(fgets(abc,LINE_SIZE-1,file1)!=NULL){

        char *store=strtok(abc,",");
        int count=0;
        while(store!=NULL){
            if(strcmp(store,"NA")==0){
                store="0";
            }
            buf[count]=atoi(store);
            store=strtok(NULL,",");
            count++;
        }

		

        char buf1[12];
        snprintf(buf1,11,"%d-%d-%d",buf[0],buf[1],buf[2]);

		char *stripped_date = strip_date(buf1);
        unsigned long value=hash(stripped_date)%size;
        /*
        if(node_lookup(head,buf[0],buf[1],buf[2],buf[3])!=NULL){
            fprintf(stderr,"load_table duplicate entry: %d-%d-%d %d\n",buf[0],buf[1],buf[2],buf[3]);
        }
        */

		table[value] = add_node(table[value],buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);
    }

  return 0;
}

/*
 * print_info
 *
 * Arguments: pointer to a hash table, number of elements
 */
void print_info(node **table, unsigned long size) {
  unsigned long entries=1;
  unsigned long lchain=0;
  unsigned long schain=10000000;
  unsigned long empty=0;


  for(int i=0;i<(int)size;i++){
    unsigned long count=0;
    node *chain=table[i];
    node * head=chain;

    if(head==NULL){
        empty++;
		continue;
    }
    else{
        while(head!=NULL){
            count++;
            head=head->next;
        }
    }
    entries=entries+count;
    if(count>lchain){
        lchain=count;
    }
    if(count<schain){
        schain=count;
    }
  }


  printf("Table size: %lu\n", size);
  printf("Total entries: %lu\n", entries);
  printf("Longest chain: %lu\n", lchain);
  printf("Shortest chain: %lu\n", schain);
  printf("Empty buckets: %lu\n", empty);
}

/*
 * delete_date
 * Delete all nodes associated with a given date of form YYYY-MM-DD
 * All leading zeros have been removed in the date string
 */
void delete_date(node **table, unsigned long size, char *datestr) {
  unsigned long hashval = hash(datestr) % size;
  node *chain = table[hashval];
  node *tmp, *prev = NULL;
  node* new_head = chain;

  const char split[] = "-";
  char *token = strtok(datestr, split);
  int cols[COL_DAY+1];
  int c = 0;
  while (token != NULL) {
    cols[c] = atoi(token);
    token = strtok(NULL, split);
    c++;
  }

  while (chain != NULL) {
    tmp = chain->next;
    // Delete if matching
    if (chain->year == cols[COL_YEAR] && chain->month == cols[COL_MONTH]
        && chain->day == cols[COL_DAY]) {
      // Only link previous if there was a previous
      if (prev) {
        prev->next = tmp;
      // No previous: this was the head, now new head is the one in front of us
      } else {
        new_head = tmp;
      }
      free(chain);
    // If not matching, don't delete and set prev as usual
    } else {
      prev = chain;
    }
    chain = tmp;
  }

  table[hashval] = new_head;
}


/*
 * delete_table
 *
 * Arguments: pointer to hash table, hash table array size
 */
void delete_table(node **table, unsigned long size) {
  unsigned int i;
  node *tmp;
  node *curr_tmp;

  for (i = 0; i < size; i++) {
    curr_tmp = table[i];
    while (curr_tmp != NULL) {
      tmp = curr_tmp->next;
      free(curr_tmp);
      curr_tmp = tmp;
    }
  }

  free(table);
}
