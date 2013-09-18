/* 
	Copyright © 2013 Reagan Lopez
	[This program is licensed under the "MIT License"]
*/

/*****************************************************************/
/* main.cpp: Program to parse the grammar described below.
/* Author: Reagan Lopez
/* Date: 07/13/2013
/*****************************************************************/
// GRAMMAR:
// expression	:	term [ + expression ]
//              |	term [ - expression ]
// term		    :	factor [ * term ]
//              |	factor [ / term ]
//              |	factor [ % term ]
// factor	    :	power [ ^ factor ]
// power		:	primary
//              |	( expression )
// primary	    : 	ident
//              | 	number
//              |	- expression
//              |	+ expression
// ident		: 	letter { let_git }
// let_git	    : 	letter | digit | ‘_’ letter | ‘_’ digit
// letter	    : 	‘a’ | ‘A’ | ‘b’ | ‘B’ | ...
// number	    : 	digit { un_digit }
// un_digit	:	digit | ‘_’ digit
// digit		: 	‘0’ | ‘1’ | ‘2’ | ... | ‘9’

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>

using namespace std;

#define 	BLANK	' '
#define 	EOL		'\n'
#define	    OPEN	'('
#define	    CLOSED	')'

const char* OUTPUTFILE = "parse.txt"; // output filename
const int ARRAY_SIZE = 1000; // max array size

char next_char;
char choice;
bool error;
string stringArray[ARRAY_SIZE];
string numArray[ARRAY_SIZE];
int ident_cnt;
int num_cnt;
int nest_level;
int max_nest_level;
ofstream outfile;


#define ASSERT( c )												                                    \
	if ( next_char != c ) {										                                    \
        if ( c == '\n' ) {                                                                          \
            printf( "\nSyntax Error: found '%c', expected EOF.\n", next_char );                     \
            outfile << "\nSyntax Error: found '" << next_char << "', expected EOF.\n";              \
        } else if ( next_char == '\n' ) {                                                           \
            printf( "\nSyntax Error, found EOF, expected '%c'.\n", c );                             \
            outfile << "\nSyntax Error: found EOF, expected '" << c << "'.\n";                      \
        } else {                                                                                    \
		  	printf( "\nSyntax Error, found '%c', expected '%c'\n", next_char, c );                  \
		  	outfile << "\nSyntax Error: found '" << next_char << "', expected '" << c << "'.\n";    \
        }	                                                                                        \
        error = true;                                                                               \
        exit( 0 );                                                                                  \
	} else {													                                    \
        if ( c != '\n' ) {                                                                          \
            scan();												                                    \
        }                                                                                           \
	}                                                                                               \

/*****************************************************************/
/* Function to read next char. Removes blank spaces.
/*****************************************************************/
void scan( )
{ // scan
	next_char = getchar();
	if ( BLANK == next_char ) {
			scan();
	}
	else {
	    outfile << next_char;
    }
} //end scan

/*****************************************************************/
/* Function to parse numbers. If not digit or if consecutive
/* underscores or if trailing underscore, then error.
/* Tracks nesting level.
/*****************************************************************/
void number()
{ // number
    char prev_char;
    char str[1000];
    int cnt = 0;

    nest_level++;
	if ( nest_level >= max_nest_level ) {
        max_nest_level = nest_level;
	} //end if

    do {
        prev_char = next_char;
        str[cnt++] = next_char;
        scan();
    } while ( ( isdigit( next_char ) ) || ( next_char == '_' && isdigit( prev_char ) ) );

	if ( ( isalpha ( next_char ) ) || ( ( next_char == '_' && prev_char == '_') ) || ( ( prev_char == '_' ) ) ) {
        error = true;
	} else {
        str[cnt] = '\0';
        numArray[num_cnt++].append( str );
	}//end if

	if ( error ) {
        printf( "\nSyntax error: found '%c', expected a digit.", next_char );
        outfile << "\nSyntax error: found '" << next_char << "', expected a digit.";
        exit( 0 );
	} //end if

	nest_level--;

} //end number


/*****************************************************************/
/* Function to parse identifiers. If not letter or if not digit or
/* if consecutive underscores or if trailing underscore,
/* then error.
/* Tracks nesting level.
/*****************************************************************/
void ident()
{ // ident
    char prev_char;
    char str[1000];
    int cnt = 0;

    nest_level++;
	if ( nest_level >= max_nest_level ) {
        max_nest_level = nest_level;
	} //end if

    do {
        prev_char = next_char;
        str[cnt++] = next_char;
        scan();
    } while ( ( isalpha( next_char ) ) || ( isdigit( next_char ) ) || ( next_char == '_' && isalpha( prev_char ) ) || ( next_char == '_' && isdigit( prev_char ) ) );

	if ( ( ( next_char == '_' && prev_char == '_' ) ) || ( ( prev_char == '_' ) ) ) {
        error = true;
	} else {
        str[cnt] = '\0';
        stringArray[ident_cnt++].append( str );
	}//end if

	if ( error ) {
        printf( "\nSyntax error: found '%c', expected a letter or a digit.", next_char );
        outfile << "\nSyntax error: found '" << next_char << "', expected a letter or a digit.";
        exit( 0 );
	} //end if

	nest_level--;

} //end ident

void expression();					// forward announcement!!

/*****************************************************************/
/* Function to parse the following section of the grammar:
/* primary	: 	ident
/*		    | 	number
/*		    |	- expression
/*		    |	+ expression
/* Tracks nesting level.
/*****************************************************************/
void primary()
{ // primary
    nest_level++;
	if ( nest_level >= max_nest_level ) {
        max_nest_level = nest_level;
	} //end if

	if ( isdigit( next_char ) ) {
		  number();
	} else if ( isalpha( next_char ) ) {
		  ident();
	} else if ( next_char == '-' || next_char == '+' ) {
	    scan();
	    expression();
	} else {
          printf( "\nSyntax error: found '%c', expected a digit or a letter or '+' or '-'.", next_char );
          outfile << "\nSyntax error: found '" << next_char << "', expected a digit or a letter or '+' or '-'.";
          exit( 0 );
	} //end if

	nest_level--;
} //end primary

/*****************************************************************/
/* Function to parse the following section of the grammar:
/* power		:	primary
/*      		|	( expression )
/* Tracks nesting level.
/*****************************************************************/
void power()
{ // power
    nest_level++;
	if ( nest_level >= max_nest_level ) {
        max_nest_level = nest_level;
	}

	if ( next_char == OPEN ) {
		  scan();
		  expression();
		  ASSERT( CLOSED );
	} else {
		  primary();
	} //end if

	nest_level--;
} //end power

/*****************************************************************/
/* Function to parse power operator.
/* Tracks nesting level.
/*****************************************************************/
void factor()
{ // factor
    nest_level++;
	if ( nest_level >= max_nest_level ) {
        max_nest_level = nest_level;
	}

	power();

	if ( next_char != EOL ) {
        if ( next_char == '^' ) {
              scan();
              factor();
        }
	} // end if

	nest_level--;
} //end factor

/*****************************************************************/
/* Function to parse the following section of the grammar:
/* term	:	factor [ * term ]
/*		|	factor [ / term ]
/*		|	factor [ % term ]
/* Tracks nesting level.
/*****************************************************************/
void term()
{ // term
    nest_level++;
	if ( nest_level >= max_nest_level ) {
        max_nest_level = nest_level;
	}

	factor();

    if ( next_char != EOL ) {
        if ( ( next_char == '*' ) || ( next_char == '/' ) || ( next_char == '%' ) ) {
              scan();
              term();
        }
    } // end if

	nest_level--;
} //end term

/*****************************************************************/
/* Function to parse the following section of the grammar:
/* expression	:	term [ + expression ]
/*		        |	term [ - expression ]
/* Tracks nesting level.
/*****************************************************************/
void expression()
{ // expression
    nest_level++;
	if ( nest_level >= max_nest_level ) {
        max_nest_level = nest_level;
	}

	term();

	if ( next_char != EOL ) {
        if ( ( next_char == '+' ) || ( next_char == '-' ) ) {
              scan();
              expression();
        }
	} //end if

	nest_level--;
} //end expression

/*****************************************************************/
/* Function to compare two strings without case sensitivity.
/*****************************************************************/
bool compareNoCase (string first, string second)
{
  int i = 0;
  while ( ( i < first.length() ) && ( i < second.length() ) ) {
    if ( tolower ( first[i] ) < tolower ( second[i] ) ) return true;
    else if ( tolower (first[i] ) > tolower ( second[i] ) ) return false;
    i++;
  }

  if ( first.length() < second.length() ) return true;
  else return false;
}


/*****************************************************************/
/* Main function.
/* Get first token.
/* Then parse complete expression.
/* Assert no more source after expression.
/* Prints identifiers and numbers in ascending order.
/* Prints max nesting level.
/* Loop till user chooses to end.
/*****************************************************************/
int main()
{ // main
    bool first = true;
    outfile.open( OUTPUTFILE );
    do {
        next_char = BLANK;
        error = false;
        ident_cnt = 0;
        num_cnt = 0;
        nest_level = 0;
        max_nest_level = 0;
        // initialize string arrays
        for ( int i = 0; i < ARRAY_SIZE; i++ )  {
            stringArray[i] = "";
            numArray[i] = "";
        } // end for

        printf( "\nEnter arithmetic expression: " );
        outfile << "\n\n\nEnter arithmetic expression: ";
        scan();
        expression();
        ASSERT( EOL );

        if ( !error ) {
            printf( "\nThe input expression is correct." );
            outfile << "The input expression is correct.";
            if ( ident_cnt ) {
                first = true;
                vector<string> identListVector ( stringArray, stringArray + ident_cnt );
                vector<string>::iterator it1;
                sort( identListVector.begin(), identListVector.end(), compareNoCase );
                cout << "\nIdentifiers: ";
                cout << "\n------------ ";
                outfile << "\nIdentifiers: ";
                outfile << "\n------------ ";
                for ( it1 = identListVector.begin(); it1 != identListVector.end(); it1++ ) {
                    if ( !first ) {
                        if ( *it1 != *( it1 - 1 ) ) {
                            cout << '\n' << *it1;
                            outfile << '\n' << *it1;
                        }
                    } else {
                        cout << '\n' << *it1;
                        outfile << '\n' << *it1;
                        first = false;
                    }
                } // end if
            } // end if

            if ( num_cnt ) {
                first = true;
                vector<string> numListVector ( numArray, numArray + num_cnt );
                vector<string>::iterator it2;
                sort( numListVector.begin(), numListVector.end(), compareNoCase );
                cout << "\nNumbers: ";
                cout << "\n--------";
                outfile << "\nNumbers: ";
                outfile << "\n--------";
                for ( it2 = numListVector.begin(); it2 != numListVector.end(); it2++ ) {
                    if ( !first ) {
                        if ( *it2 != *( it2 - 1 ) ) {
                            cout << '\n' << *it2;
                            outfile << '\n' << *it2;
                        }
                    } else {
                        cout << '\n' << *it2;
                        outfile << '\n' << *it2;
                        first = false;
                    }
                }
            }

            cout << "\nNesting level of calls: ";
            cout << "\n-----------------------";
            cout << "\n" << max_nest_level;
            outfile << "\nNesting level of calls: ";
            outfile << "\n-----------------------";
            outfile << "\n" << max_nest_level;
        }
        cout << "\nPress 'y' to continue: ";
        choice = getchar();
        getchar();
    } while (choice == 'y'); // end while

    outfile.close();
	return 0;
} //end main
