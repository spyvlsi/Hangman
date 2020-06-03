#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define LETTERS  26  /* synolo grammatwn latinikou alfavitou */
#define WORDLEN  30  /* megethos lexis */

/* orismos tis domis twn klasewn isodynamias twn lexewn (dipla syndedemeni lista) */
struct klasi_lexewn {
    char *repr;      /* antiproswpos klasis */
    int size;          /* megethos katigorias */
    char **pinakas;    /* periexei deiktes pros tis lexeis tis kaigorias */
    struct klasi_lexewn *next; /* deixnei ston epomeno    komvo */
    struct klasi_lexewn *prev; /* deixnei ston proigoumen komvo */
};
typedef struct klasi_lexewn ClassT;


/* orizw ti domi tou lexikou (apla syndedemeni lista) */
struct dict_list {
    char *word_dict;        /* lexi lexikou    */
    struct dict_list *next; /* epomenos komvo */
};
typedef struct dict_list DictionaryT;


/* synartiseis */
void main_game_loop(ClassT *initial_class, int user_turns);
void insert_used(char *letters, char letter );
void remove_not_used(char *letters, char letter);
void free_memory(DictionaryT *head);
void free_class_list(ClassT *head);
int argv_control(int argc, char *argv[], int *user_length, int *user_turns);
ClassT *create_class(DictionaryT *game_dict, int user_length, int total_words);
ClassT *calculate_max_class(char **pinakas, int size, char *repr, char curr_letter);
ClassT *get_max(ClassT *head);
DictionaryT *create_game_dict(char *filename, int user_length, int *total_words);
char *create_repr(int n);
char *create_new_repr(char *repr, char *word, char letter);
char *getWord(char *filename);

/******** typwnei ti lista me tis katigories gia elegxo na svistei *********/
void printCategories(ClassT *root, int analytical) {
    ClassT *current;
    int i;

    current=root;
    while (current != NULL) {
        /* ektypwsi komvou */
		printf("Pattern: %s, Size: %d\n",current->repr, current->size);
		if (analytical) {
			for (i=0; i< current->size; i++) {
				printf("\t%s", current->pinakas[i]);
				if ((i+1)%2==0) printf("\n");
			}
			printf("\n");
		}
		current=current->next;
    }
}

int main(int argc, char *argv[]) {
	ClassT *initial_class;   /* arxiki klasi */
	DictionaryT *game_dict;  /* lexiko       */

	int user_length;   /* mikos lexis poy thelei o xristis    */
	int user_turns;    /* arithmos gyrwn poy thelei o xristis */
	int total_words;   /* synolo lexewn me to swsto mikos     */

	char c;

	if (argv_control(argc, argv, &user_length, &user_turns) == 1) {
		game_dict=create_game_dict(argv[3], user_length, &total_words);  /* ftiaxnw ti lista tou lexikou */
		initial_class=create_class(game_dict,user_length, total_words);  /* ftiwxnw tin arxiki klasi     */
printCategories(initial_class, 1);	/******* mono gia elegxo na svistei *******/
		do {
		    main_game_loop(initial_class, user_turns);  /* trexw to kyrio loop */
		    do {
                printf("Play again? (y/n): ");  /* rwtaei to xristi an thelei na xanapaixei */
                scanf(" %c",&c);
                c=tolower(c);
                if ( c != 'y' && c != 'n')
                {
                    printf("Please enter y for yes or n for no.\n");
                }
            }  while (c!='y' && c != 'n');
		} while (c=='y');

		free_class_list(initial_class);  /* apeleutherwnw ti mnimi tis klasis  */
		free_memory(game_dict);          /* apeleutherwnw ti mnimi tou lexikou */
	}

	return 0;
}

/* basiki synartisi poy trexei to kyrio loop toy paixnidiou */
void main_game_loop(ClassT *initial_class, int user_turns)
{
    ClassT *max_class, *old_class;
    char letters_used[2*LETTERS+1]="";
    char letters_not_used[2*LETTERS+1];
    char secret_word[WORDLEN], letter;
    int i,j, turns;

    old_class=NULL;
    max_class=initial_class;
    strcpy(secret_word,initial_class->repr); /* h lexi poy psaxnei o xristis */

    /* ftiaxnw ton pinaka me ta grammata poy den exoun xrhsimopoiithei akoma */
    for(i=0,j=0; i<LETTERS; i++, j=j+2)
    {
        letters_not_used[j]='a'+i;
        letters_not_used[j+1]=' ';
    }
    letters_not_used[j]='\0';

    /* vasiko loop */
	for(turns=user_turns; turns>0; )
	{
		printf("Remaining guesses: %d\n",turns);
		printf("Used letters: %s\n",letters_used);
		printf("Unused letters: %s\n",letters_not_used);
		printf("Word: %s\n",secret_word);
        do {
            printf("Guess a letter: ");   /* diavazw to epomeno gramma */
            scanf(" %c",&letter);
        } while ( !isalpha(letter) );
        if ( isupper(letter) )
        {
            letter=tolower(letter);   /* an einai kefalaio, to kanw mikro */
        }
        if (strchr(letters_not_used, letter) == NULL)
        {
            printf("You have already quessed this letter\n"); /* exei xrisimopoiithei xana */
        } else {
            /* neo gramma, prepei na vrw tin klasi me to megalytero plithos lexewn */
			max_class=calculate_max_class(max_class->pinakas, max_class->size, secret_word, letter);
			if (old_class != NULL)
			{
			    free_class_list(old_class);  /* apeleutherwnw thn prohgoymenh klasi */
			}
			old_class=max_class;  /* h palia klash pleon einai h nea */
			strcpy(secret_word,max_class->repr);
			printf("Category size: %d\n",max_class->size);
printCategories(max_class, 1);	/******* mono gia elegxo na svistei *******/
			if ( strchr(secret_word, '_') == NULL)    /* elegxos an o xristis ti bvrike */
			{
                printf("YOU WON! The word is %s\n", secret_word);
                if (old_class != NULL)
                {
                    free_class_list(old_class);
                }
                return;
			}
			/* ftiaxnw ta strings twn xrhsimopoihmenwn kai twn axrhsimopoihtwn grammatwn */
			insert_used(letters_used, letter);
			remove_not_used(letters_not_used, letter);
			turns--;
		}
	}
    strcpy(secret_word,max_class->pinakas[0]);  /* epilegw thn prwth lexh */
	printf("YOU LOST! The word was %s\n", secret_word);
    if (old_class != NULL)
    {
        free_class_list(old_class);
    }
}

/* synartisi poy kataskevazei tis klaseis isodynamias kai epistrefei ti megaluteri */
ClassT *calculate_max_class(char **pinakas, int size, char *repr, char curr_letter)
{
	ClassT *head_class, *curr, *new_node, *max_class_node;
	char *new_repr;
	int i, j, found;

	head_class=NULL;  /* arxikopoihsh tis listas */

    /* trexw ton pinaka me tis lexeis poy exei sti diathsi mexri twra kai
       kataskevi twn klasewn isodynamias */
	for (i=0; i<size; i++)
	{
		new_repr=create_new_repr(repr, pinakas[i], curr_letter); /* kataskevi neou antiproswpou */

        /* psaxnw na vrw mipws o antiproswpos exei idi mpei sti lista */
		found=0;
		for(curr=head_class; curr != NULL; curr=curr->next)
		{
			if ( strcmp(curr->repr, new_repr)==0 )
			{
			    found=1;
			    break;
			}
		}

		if ( !found )
		{   /* an den exei mpei, tote kataskevazw enan neo komvo kai ton vazw stin arxi */
			new_node=(ClassT *) malloc(sizeof(ClassT));
			new_node->repr=new_repr;
			new_node->size=1;
			new_node->pinakas=NULL;
			new_node->next=head_class;
			new_node->prev=NULL;
			if (head_class != NULL)
			{
			    head_class->prev=new_node;
			}
			head_class=new_node;
		} else {
		    curr->size++;  /* aliws, auxanw ton arithmo twn lexewn tis katigorias */
		}
	}

    /* kataskevazw ton komvo me to megalitero arithmo lexewn */
    max_class_node=get_max(head_class);
    if (max_class_node != NULL)
    {
        /* gemizw ton pinaka me tis lexeis poy tairiazoyn me ton antiproswpo poy vrika */
        max_class_node->pinakas=(char **) malloc(max_class_node->size*sizeof(char *));
        for (i=0,j=0; i<size; i++)
        {
			new_repr=create_new_repr(repr, pinakas[i], curr_letter);
            if( strcmp(new_repr , max_class_node->repr)==0)
            {
                max_class_node->pinakas[j]=pinakas[i];  /* prosthiki tis lexis ston pinaka */
                j++;
            }
        }
        max_class_node->next=NULL;
        max_class_node->prev=NULL;
    }

	free_class_list(head_class);  /* apeleutherwnei ti lista */

	return max_class_node;
}

/* synartisi poy vriskei ton komvo me to megalitero arithmo, kai kataskevazei enan
   neo komvo poy periexei ta idia stoixeia kai ton epistrefei    */
ClassT *get_max(ClassT *head)
{
    ClassT *max, *curr, *new_node;

    if (head==NULL)
    {
        return NULL;  /* an h lista einai adeia, den epistrefei tipota */
    }

    /* entopismos teleutaiou megalyteroy komvou */
    max=head;
    for(curr=head; curr!=NULL; curr=curr->next)
    {
        if (curr->size > max->size)
        {
            max=curr;
        }
    }

    /* dhmiourgia neou komvou kai antigrafei twn stoixeiwn */
    new_node=(ClassT *) malloc(sizeof(ClassT));
    if (new_node==NULL)
    {
        printf("Memory allocation error.\n");
        exit(1);
    }
    new_node->repr=strdup(max->repr);
    new_node->size=max->size;
    new_node->pinakas=NULL;
    new_node->next=NULL;
    new_node->prev=NULL;

    return new_node;
}

/* synartisi poy vazei to gramma sto string twn xrhsimopoihmenwn */
void insert_used(char *letters, char letter )
{
    int i;
	char temp[2*LETTERS+1];

    for(i=0; letters[i] != '\0'; i++) /* entopismos ths theshs */
    {
        if (letters[i]>letter) {       /* h thesh poy prepei na mpei exei entopisthei */
			strcpy(temp,letters+i);    /* antigrafei twn xarakthrwn 2 theseis parakatw */
            strcpy(letters+i+2,temp);
            letters[i]=letter;
            return;
        }
    }
    /* to gramma mpainei sto telos */
    letters[i]=letter;
    letters[i+1]=' ';
    letters[i+2]='\0';
}

/* synartisi pou afairei ena gramma apo ton pinaka twn diathesimwn grammmatwn */
void remove_not_used(char *letters, char letter)
{
    char *ptr;

    ptr=strchr(letters, letter);  /* entopismos tou xarakatira */
    strcpy(ptr,ptr+2);            /* metafora twn ypoloipwn 2 theseis aristera */
}

/*  anadromiki synartisi pou apeleutherwnei th mnimi tou lexikou */
void free_memory(DictionaryT *head)
{
	if (head != NULL)
	{
		free_memory(head->next);
        free(head->word_dict);
        free(head);
    }
}

/* synartisi gia ton elegxo twn orismatwn */
int argv_control(int argc, char *argv[], int *user_length, int *user_turns)
{
    int number;

    /* elegxos arithmou twn orismatwn */
    if (argc != 4) {
        if (argc < 4) {
            printf("Insufficient arguments\n");
        } else {
            if (argc>4)
            {
                printf("Too many arguments\n");
            }
        }
        printf("./hangman LENGTH TURNS DICTIONARY\n");
        printf("\tLENGTH: requested length of mystery word. Must be >1, <30, !=26, !=27\n");
        printf("\tTURNS: requested number of turns. Must be >0\n");
        printf("\tDICTIONARY: name of dictionary file\n\n");
        return 0;
    }

	number=atoi(argv[1]);  /* plithos grammatwn */
    /* elegxei an to plithos twn grammatwn einai swsto */
	while (number <=1 || number>=30 || number == 26 || number ==27)
	{
		printf("There are no words of length %d in the dictionary.\n",number);
		printf("Specify a different length: ");
		scanf("%d", &number);
	}
	*user_length=number;

	number=atoi(argv[2]);  /* arithmos gyrwn    */
    /* elegxei an o arithmos twn gyrwn einai swstos */
	while (number <=0 )
	{
		printf("You may not have fewer than one turn.\n");
		printf("Specify a different number of turns: ");
		scanf("%d", &number);
	}
    *user_turns=number;

	return 1;
}

/* synartisi poy dimioyrgei to string me ton arxiko antiproswpo poy exei pavles */
char *create_repr(int n)
{
    char *repr;
    int i;

    repr=(char *) malloc((n+1)*sizeof(char));
    if (repr==NULL)
    {
        printf("Memory allocation error.\n");
        exit(1);
    }

	for (i=0; i<n; i++)
	{
		repr[i]='_';
	}
	repr[n]='\0';

	return repr;
}

/* synartisi poy dimioyrgei tin arxiki klasi (arxikos kombos) poy exei oles tis lexeis */
ClassT *create_class(DictionaryT *game_dict, int user_length, int total_words)
{
	ClassT *new_node;
	DictionaryT *curr;
	int i;

	new_node=(ClassT *) malloc(sizeof(ClassT));
	if (new_node==NULL) {
		printf("Memory allocation error.\n");
		exit(1);
	}

    new_node->repr=create_repr(user_length);  /* dimioyrgia antiproswpou */
    new_node->size=total_words;
	new_node->pinakas=(char **) malloc(total_words*sizeof(char *));
	if (new_node->pinakas==NULL)
	{
		printf("Memory allocation error.\n");
		exit(1);
	}

    /* trexw ti lista tou lexikou kai gemixw ton pinaka me kathe lexi tou lexikou */
	curr=game_dict;
	for(i=0; i<total_words; i++)
	{
		new_node->pinakas[i]=curr->word_dict;
		curr=curr->next;
	}

	new_node->next=NULL;
    new_node->prev=NULL;

	return new_node;
}

/* anadromiki synartisi poy apeleutherwnei ti lista twn katigoriwn */
void free_class_list(ClassT *head)
{
    if (head != NULL)
    {
        free_class_list(head->next);
        free(head->repr);
        free(head->pinakas);
        free(head);
    }
}

/* synartisi poy kataskevazei ton neo antiproswpo mia lexis basismeni ston
   palio antiproswpo kai to gramma pou exei dwsei o xristis */
char *create_new_repr(char *repr, char *word, char letter)
{
	char *new_repr, c;
	int i=0;

	new_repr=strdup(repr);  /* antigrafo tou paliou antiproswpou */
	while ( *word!='\0' )
    {
	    c=tolower(*word);
		if ( c == letter)  /* emfanisi tou grammatos pou edwse o xristis */
		{
			new_repr[i]=letter;
		}
		word++;
		i++;
	}

	return new_repr;
}

/* synartisi poy dimiourgei ti lista tou lexikou, episis metraei kai epistrefei
   mesw tis parametrou total_words to plithos twn lexewn poy exei brei   */
DictionaryT *create_game_dict(char *filename, int user_length, int *total_words)
{
	DictionaryT *head_dict, *new_node;
	char *new_word;
	int counter=0; /* plithos twn lexewn */

	head_dict=NULL;  /* arikopoihsh ths listas tou lexikou */

	/* diavazw tis lexeiw apo to arxeio kai an exoun to swsto mhkos, tis vazw sti lista */
	new_word=getWord(filename);
	while (new_word != NULL)
	{
		if ( user_length == strlen(new_word) )
		{
		    /* dhmiourgia neou kombou */
			new_node=(DictionaryT *) malloc(sizeof(DictionaryT));
			if (new_node==NULL)
			{
			    printf("Memory allocation error.\n");
			    exit(1);
			}
			new_node->word_dict=new_word;
			/* prosthiki tou komvou sthn arxi tis listas */
			new_node->next=head_dict;
			head_dict=new_node;
			counter++;  /* ayxanw ton arithmo twn lexewn */
		}
		new_word=getWord(filename);
	}

    *total_words=counter;
	return head_dict;
}

/* Given the name of a file, read and return the next word from it,
or NULL if there are no more words */

char *getWord(char *filename)  {
	char formatstr[15], *word;
	static FILE *input;
	static int firstTime = 1;
	if (firstTime) {
		input = fopen(filename, "r");
		if (input == NULL) {
			printf("ERROR: Could not open file \"%s\"\n", filename);
			exit(1);
		}
		firstTime = 0;
	}
	word = (char*)malloc(sizeof(char)*WORDLEN);
	if (word == NULL) {
		printf("ERROR: Memory allocation error in getWord\n");
		exit(1);
	}
	sprintf(formatstr, "%%%ds", WORDLEN-1);
	fscanf(input, formatstr, word);
	if (feof(input)) {
		fclose(input);
		return NULL;
	}
	return word;
}
