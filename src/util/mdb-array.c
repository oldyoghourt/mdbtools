/* MDB Tools - A library for reading MS Access database file
 * Copyright  (C) 2000 Brian Bruns
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/* this is inherited from mdb-export.c, modified to make arrays similar to
   those generated by parsecsv.c */

#include "mdbtools.h"

int
main (int argc, char **argv)
{
unsigned int j;
MdbHandle *mdb;
MdbTableDef *table;
MdbColumn *col;
/* doesn't handle tables > 256 columns.  Can that happen? */
char *bound_values [256]; 
char delimiter [] = ", ";
char quote_text = 1;
int count = 0;
int started;

 fputs("mdb-array is deprecated and will disappear in a future version of mdbtools.\n", stderr);
 fputs("Please drop us a line if you have any use of it.\n", stderr);
 fputs("See https://github.com/mdbtools/mdbtools/issues/197\n", stderr);
 fputs("\n", stderr);

 if  (argc < 3) 
   {
     fprintf (stderr, "Usage: %s <file> <table>\n", argv [0]);
     exit (1);
   }

 mdb = mdb_open (argv [1], MDB_NOFLAGS);
 if (!mdb)
	exit(1);
 
 table = mdb_read_table_by_name (mdb, argv[2], MDB_TABLE);
 if (table)
       {
	 mdb_read_columns (table);
	 mdb_rewind_table (table);
	 
	 for (j = 0; j < table->num_cols; j++) 
	   {
	     bound_values [j] =  (char *) g_malloc (MDB_BIND_SIZE);
	     bound_values [j] [0] = '\0';
	     mdb_bind_column (table, j+1, bound_values[j], NULL);
	   }

	 fprintf (stdout, "/******************************************************************/\n");
	 fprintf (stdout, "/* THIS IS AN AUTOMATICALLY GENERATED FILE.  DO NOT EDIT IT!!!!!! */\n");
	 fprintf (stdout, "/******************************************************************/\n");
	 fprintf (stdout, "\n");
	 fprintf (stdout, "#include <stdio.h>\n");
	 fprintf (stdout, "#include \"types.h\"\n");
	 fprintf (stdout, "#include \"dump.h\"\n");
	 fprintf (stdout, "\n");
	 fprintf (stdout, "const %s %s_array [] = {\n", argv [2], argv [2]);

	 count = 0;
	 started = 0;
	 while (mdb_fetch_row (table)) 
	   {
	     if (started != 0)
	       {
		 fprintf (stdout, ",\n");
	       }
	     started = 1;
	     fprintf (stdout, "{\t\t\t\t/* %6d */\n\t", count);
	     for  (j = 0; j < table->num_cols; j++) 
	       {
		 fprintf (stdout, "\t");
		 col = g_ptr_array_index (table->columns, j);
		 if  (quote_text && 
		      (col->col_type == MDB_TEXT ||
		       col->col_type == MDB_MEMO)) 
		   {
		     fprintf (stdout, "\"%s\"", bound_values [j]);
		   } 
		 else 
		   {
		     fprintf (stdout, "%s", bound_values [j]);
		   }
		 if (j != table->num_cols - 1)
		   {
		     fprintf (stdout, "%s\n", delimiter);
		   }
		 else
		   {
		     fprintf (stdout, "\n");
		   }
	       }
	     fprintf (stdout, "}");
	     count++;
	   }
	 fprintf (stdout, "\n};\n\n");

	 for  (j = 0; j < table->num_cols; j++) 
	   {
	     g_free (bound_values [j]);
	   }
	 mdb_free_tabledef(table);
       }
 
 mdb_close (mdb);

 fprintf (stdout, "const int %s_array_length = %d;\n", 
	  argv [2],
	  count);

 return 0;
}
