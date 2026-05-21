## Description
Making a C REST API, using basics like sockets, phtreads and such. For http headers parsing Im using picohttpparser. For JSON parsing Im using yyjson. For database: sqlite3. I'm trying to write most of the code myself without copying and pasting generated/searched code, just using documentations, `man`, etc. Still using AI for opinions and directions I should take with my code, but not to generate code, as this project is meant as a learning process. Also its one of my first projects using NVIM as my code editor, so I have nvim learning curve as well.


## Database intialization
For initializing db we will use `sqlite3 <db_name.db> < init.sql`
On my initial tests `<db_name.db>` will be jokes.db

## Running 
To compile and run it just type: `make ; ./bin/myserver`
