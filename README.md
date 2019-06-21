# Key-Sorting-Indexing-Tool
The key-sorting approach to create an index for a given data-file, and use them in an example application to show a search operation in action.

The application we developed will accepts some set up parameters from a JSON file. A typical JSON, file can be as follows:

```json
{
"dataFileName" : "data.dat",
"indexFileName" : "data.ndx",
"recordLenght" : 401,
"keyEncoding" : "UTF",
"keyStart" : 13,
"keyEnd" : 20,
"order" : "ASC"
}
```
In JSON file, dataFileName and indexFileName are the names of data file name and index file name, including full path, respectively. The datafile contains any type of records where records come in no particular order. By the way, datafile must be exist and given at the beginning. Value of recordLength shows the length of data file records in bytes. Normally, index file is not exist initially and our tool will create it when corresponding method is invoked. Index file includes only key and relative record numbers pointing to record in datafile, as in key-sorting approach of simple indexing. Index file is sorted in the order of order field value, where ASC and DESC are ascending and descending, relatively. Value of keyEncoding shows how key field is to be interpreted: CHR for one byte long text, BIN for binary, UTF for multi-byte character representation.
